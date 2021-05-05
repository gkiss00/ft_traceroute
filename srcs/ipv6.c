#include "./../ft_traceroute.h"

void send_ping_6(t_data *data) {

    uint8_t buff[data->packet_size];
    memset(buff, 0, data->packet_size);

    if (setsockopt(data->fd, IPPROTO_IPV6, IP_TTL, &data->opts.f, sizeof data->opts.f) < 0) {
        printf("Can't set ttl option : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // SIZE = 28
    struct icmp icmp;
    icmp.icmp_type = 128;
    icmp.icmp_code = 0;
    icmp.icmp_cksum = 0;
    icmp.icmp_id = getpid();
    icmp.icmp_seq = data->ttl;

    memcpy(buff, &icmp, data->packet_size);
    icmp.icmp_cksum = checksum(buff, data->packet_size);
    memcpy(buff, &icmp, data->packet_size);

    gettimeofday(&data->sending_time, NULL); // stock the sending time
    int x = sendto(data->fd, buff, data->packet_size, 0, (struct sockaddr*)data->ptr, sizeof(struct sockaddr_in6));
    if (x < 0) {
        printf("Can't send : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

const char *get_data_6(t_data *data, uint8_t *buffer, const char *address_string) {
    void *ptr = NULL;
    uint16_t *response;   
    response = (uint16_t *)buffer;
    // print_tab_in_hex("tab", buffer, 200);
    // char t[256];
    // char adr[256];
    // memcpy(adr, &buffer[22], 16);
    // print_tab_in_hex("tab", (uint8_t *)adr, 16);
    //inet_pton(AF_INET6, adr, t);
    //const char *address_string = inet_ntop(AF_INET6, adr, t, 256);
    //printf("t: %s %s\n", t, address_string);
    struct addrinfo hints;
    struct addrinfo *res = malloc(sizeof(struct addrinfo));
        
    int type = 0;
    hints.ai_flags = 0;
    hints.ai_family = AF_INET6; //either IPV4 or IPV6
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMPV6;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    
    int g = getaddrinfo(address_string, NULL, &hints, &res);
    if (g < 0)
        printf("Can't get addrinfo");

    char tmp[100];
    //printf("get addrinfo: %s : %p\n", address_string, res);
    if (res) {
        //printf("RES\n");
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
            //printf("4: %s\n", tmp);
        } else {
            inet_ntop (res->ai_family, &((struct sockaddr_in6 *)ptr)->sin6_addr, tmp, 100);
            //printf("6: %s\n", tmp);
        }
        get_name(res->ai_addr);
        t_node *node = node_good(&data->node, tmp, get_name(res->ai_addr));
        add_time(node, new_time(get_time_diff()));
        //printf (" (%s)\n", tmp);
        //res = res->ai_next;
    }
    free(res);
    if (response[2] == getpid()) {
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

void receive_ping_6(t_data *data) {
    uint8_t    msg_buffer[200];
    memset(msg_buffer, 0, sizeof(msg_buffer));
    struct sockaddr_in6 *ptr = NULL;
    ptr = malloc(sizeof(struct sockaddr_in6));

    int len = sizeof(struct sockaddr_in6);
    int z = recvfrom(data->fd, msg_buffer, sizeof(msg_buffer), MSG_WAITALL, (struct sockaddr*)ptr, (socklen_t *)&len); // try to receive a message before time out
    if (z < 0) {
        printf("* ");
        fflush(stdout);
        if (data->probe == data->opts.q)
            printf("\n");
    } else {
        char t[256];
        const char *address_string = inet_ntop(AF_INET6, (uint8_t*)&ptr->sin6_addr, t, 256);
        // printf("ptr sin6_addr: %s\n", (uint8_t*)&ptr->sin6_addr);
        // printf("ptr sin6_family: %d\n", ptr->sin6_family);
        //printf("ptr sin6_addr: %s\n", );
        gettimeofday(&data->receiving_time, NULL); // stock the receiving time
        get_data_6(data, msg_buffer, address_string);
        free(ptr);
    }
}