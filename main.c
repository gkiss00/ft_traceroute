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
    double t0, t1, diff;
    t0 = (data.sending_time.tv_sec * 1000000) + data.sending_time.tv_usec;
    t1 = (data.receiving_time.tv_sec * 1000000) + data.receiving_time.tv_usec;
    diff = (t1 - t0) / 1000;
    return diff;
}

char *get_name(struct sockaddr *addr) {
    char *host = malloc(200);
    char serv[200];
    getnameinfo(addr, (socklen_t)sizeof(struct sockaddr), host, 200, serv, 200, 0);
    return (host);
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

void traceroute() {
    while(data.opts.f <= data.opts.m) {
        data.ttl += 1;
        data.probe = 0;
        printf("%d ", data.opts.f);
        for (int i = 0; i < data.opts.q; ++i) {
            ++data.probe;
            send_ping(&data);
            receive_ping(&data);
        }
        output(&data);
        node_free(data.node);
        data.node = NULL;
        data.opts.f += 1;
        if (data.success)
            exit(EXIT_SUCCESS);
    }
}

void traceroute_6() {
    while(data.opts.f <= data.opts.m) {
        data.ttl += 1;
        data.probe = 0;
        printf("%d ", data.opts.f);
        for (int i = 0; i < data.opts.q; ++i) {
            ++data.probe;
            send_ping_6(&data);
            receive_ping_6(&data);
        }
        output(&data);
        node_free(data.node);
        data.node = NULL;
        data.opts.f += 1;
        if (data.success)
            exit(EXIT_SUCCESS);
    }
}

int     main(int argc, char **argv) {
    check_error(argc, argv);
    init_data(&data);
    parsing(&data, (uint8_t**) argv);
    init_socket(&data);
    print_start(&data);
    if(data.type == AF_INET6) {
        traceroute_6();
    } else {
        traceroute();
    }
    
}