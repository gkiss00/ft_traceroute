#include "./ft_traceroute.h"

t_data data;

uint16_t    checksum(uint8_t *msg, uint32_t size) {
    uint32_t    new_size = size % 2 == 0 ? size : size + 1;
    uint16_t    tmp[new_size / 2];
    memset(tmp, 0, new_size);
    memcpy(tmp, msg, size);
    uint32_t sum = 0;
    uint16_t check = 0;
    for (uint32_t i = 0; i < new_size / 2; ++i) {
        sum += tmp[i];
    }
    check = sum % UINT16_MAX;
    return ~check;
}

double get_time_diff() {
    //double diff = ((data.receiving_time.tv_sec - data.sending_time.tv_sec) * UINT32_MAX + (data.receiving_time.tv_usec - data.sending_time.tv_usec)) / 1000;
    long long t0 = (long long)(((long long)data.sending_time.tv_sec) * 1000000 + data.sending_time.tv_usec);
    long long t1 = (long long)(((long long)data.receiving_time.tv_sec) * 1000000 + data.receiving_time.tv_usec);
    double diff = (double)(t1 - t0);
    return diff;
}

void print_tab_in_hex(char *name, uint8_t *tab, int len) {     
    printf("%s: {\n", name);     
    for (int i = 0; i < len; ++i)     
    {         
        if (i % 8 == 0)
            printf(" ");         
        if (i % 16 == 0)             
            printf("\n    ");         
        printf("%.02x ", tab[i]);     
    }     
    printf("\n}\n"); 
}

static void send_ping() {

    uint8_t buff[data.packet_size];
    memset(buff, 0, data.packet_size);

    if (setsockopt(data.fd, IPPROTO_IP, IP_TTL, &data.opts.f, sizeof data.opts.f) < 0) {
        printf("Can't set ttl option : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    // SIZE = 28
    struct icmp icmp;
    icmp.icmp_type = 8;
    icmp.icmp_code = 0;
    icmp.icmp_cksum = 0;
    icmp.icmp_id = getpid();
    icmp.icmp_seq = data.ttl;

    memcpy(buff, &icmp, data.packet_size);
    icmp.icmp_cksum = checksum(buff, data.packet_size);
    memcpy(buff, &icmp, data.packet_size);

    gettimeofday(&data.sending_time, NULL); // stock the sending time
    int x = sendto(data.fd, buff, data.packet_size, 0, (struct sockaddr*)data.ptr, sizeof(struct sockaddr));
    if (x < 0) {
        printf("Can't send : %d : %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static char *get_name(struct sockaddr *addr) {
    char *host = malloc(200);
    char serv[200];
    getnameinfo(addr, (socklen_t)sizeof(struct sockaddr), host, 200, serv, 200, 0);
    return (host);
}

static char *get_data(uint8_t *buffer) {
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
        t_node *node = node_good(&data.node, tmp, get_name(res->ai_addr));
        add_time(node, new_time(get_time_diff()));
        //printf (" (%s)\n", tmp);
        //res = res->ai_next;
    }
    if (response->icmp_type == ICMP_ECHOREPLY && response->icmp_id == getpid()) {
        //printf("rec1\n");
    } else {
        //printf("rec2\n");
    }
    if (strcmp(address_string, data.address) == 0 && data.probe == data.opts.q) {
        //printf("SUCCESS");
        exit(EXIT_SUCCESS);
    }
    return address_string;
}

static void receive_ping() {
    uint8_t    msg_buffer[256];
    memset(msg_buffer, 0, sizeof(msg_buffer));
    void *ptr = NULL;

    int len = sizeof(struct sockaddr);
    int z = recvfrom(data.fd, msg_buffer, sizeof(msg_buffer), 0, (struct sockaddr*)ptr, (socklen_t *)&len); // try to receive a message before time out
    if (z < 0) {
        printf("* ");
        fflush(stdout);
        if (data.probe == data.opts.q)
            printf("\n");
    } else {
        gettimeofday(&data.receiving_time, NULL); // stock the receiving time
        get_data(msg_buffer);
    }
}

static void output_without_hostname() {
    t_node *tmp = data.node;

    while(tmp) {
        printf("%s", tmp->ip);
        t_time *t = tmp->time;
        while(t) {
            printf("  %.03f ms", t->time / 100);
            t = t->next;
        }
        printf("\n");
        tmp = tmp->next;
    }
}

static void output_with_hostname() {
    t_node *tmp = data.node;

    while(tmp) {
        printf("%s (%s)", tmp->domain, tmp->ip);
        t_time *t = tmp->time;
        while(t) {
            printf("  %.03f ms", t->time / 100);
            t = t->next;
        }
        printf("\n");
        tmp = tmp->next;
    }
}

static void    output() {
    if(data.opts.n)
        output_with_hostname();
    else
        output_without_hostname();
}

static void    print_start() {
    printf("traceroute to %s (%s), %d hops max, %d byte packets\n", data.target, data.address, 64, 52);
}

int     main(int argc, char **argv) {
    check_error(argc, argv);

    init_data(&data);
    parsing(&data, (uint8_t**) argv);

    init_socket(&data);

    print_start();
    while(data.opts.f <= data.opts.m) {
        data.ttl += 1;
        data.probe = 0;
        printf("%d ", data.opts.f);
        for (int i = 0; i < data.opts.q; ++i) {
            ++data.probe;
            send_ping();
            receive_ping();
        }
        output();
        node_free(data.node);
        data.node = NULL;
        data.opts.f += 1;
    }
}