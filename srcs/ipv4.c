#include "./../ft_traceroute.h"

void send_ping(t_data *data) {

    uint8_t buff[data->packet_size];
    memset(buff, 0, data->packet_size);

    if (setsockopt(data->fd, IPPROTO_IP, IP_TTL, &data->opts.f, sizeof data->opts.f) < 0) {
        printf("Can't set ttl option : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // SIZE = 28
    struct icmp icmp;
    icmp.icmp_type = 8;
    icmp.icmp_code = 0;
    icmp.icmp_cksum = 0;
    icmp.icmp_id = getpid();
    icmp.icmp_seq = data->ttl;

    memcpy(buff, &icmp, data->packet_size);
    icmp.icmp_cksum = checksum(buff, data->packet_size);
    memcpy(buff, &icmp, data->packet_size);

    gettimeofday(&data->sending_time, NULL); // stock the sending time
    int x = sendto(data->fd, buff, data->packet_size, 0, (struct sockaddr*)data->ptr, sizeof(struct sockaddr));
    if (x < 0) {
        printf("Can't send : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static char *get_data(t_data *data, uint8_t *buffer) {
    void *ptr = NULL;
    struct icmp *response;   
    response = (struct icmp *)&buffer[20];

    struct in_addr *address = (struct in_addr *)&buffer[HEADER_SIZE];
    char *address_string = inet_ntoa(*address);
    struct addrinfo hints;
    struct addrinfo *res = malloc(sizeof(struct addrinfo));
        
    int type = 0;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET; //either IPV4 or IPV6
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    getaddrinfo(address_string, NULL, &hints, &res);

    char tmp[100];
    if (res) {
        inet_ntop (res->ai_family, res->ai_addr->sa_data, tmp, 100);
        switch(res->ai_family) {
            case AF_INET:
                ptr = res->ai_addr;
                type = AF_INET;
                break;
            case AF_INET6:
                ptr = res->ai_addr;
                type = AF_INET6;
                break;
        }
        if (type == AF_INET) {
            inet_ntop (res->ai_family, &((struct sockaddr_in *)ptr)->sin_addr, tmp, 100);
        } else {
            inet_ntop (res->ai_family, &((struct sockaddr_in6 *)ptr)->sin6_addr, tmp, 100);
        }
        get_name(res->ai_addr);
        t_node *node = node_good(&data->node, tmp, get_name(res->ai_addr));
        add_time(node, new_time(get_time_diff()));
        //printf (" (%s)\n", tmp);
        //res = res->ai_next;
    }
    if (response->icmp_type == ICMP_ECHOREPLY && response->icmp_id == getpid()) {
        //printf("rec1\n");
    } else {
        //printf("rec2\n");
    }
    if (strcmp(address_string, data->address) == 0 && data->probe == data->opts.q) {
        data->success = true;
        //printf("SUCCESS");
    }
    return address_string;
}

void receive_ping(t_data *data) {
    uint8_t    msg_buffer[256];
    memset(msg_buffer, 0, sizeof(msg_buffer));
    void *ptr = NULL;

    int len = sizeof(struct sockaddr);
    int z = recvfrom(data->fd, msg_buffer, sizeof(msg_buffer), 0, (struct sockaddr*)ptr, (socklen_t *)&len); // try to receive a message before time out
    if (z < 0) {
        printf("* ");
        fflush(stdout);
        if (data->probe == data->opts.q)
            printf("\n");
    } else {
        gettimeofday(&data->receiving_time, NULL); // stock the receiving time
        get_data(data, msg_buffer);
    }
}