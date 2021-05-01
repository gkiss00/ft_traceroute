#include "./../ft_traceroute.h"

void init_data(t_data *data) {
    data->target = NULL;
    data->res = NULL;
    data->node = NULL;
    data->ptr = NULL;
    data->fd = 0;
    data->ttl = 0;
    data->packet_size = 52;
    data->type = 0;
    data->probe = 0;
    data->timeout.tv_sec = 5;
    data->timeout.tv_usec = 0;
    data->opts.h = 0;
    data->opts.f = 1;
    data->opts.m = 30;
    data->opts.q = 3;
    data->opts.w = 5;
    data->opts.n = true;
}

void init_socket(t_data *data) {
    data->res = malloc(sizeof(struct addrinfo));
    
    data->hints.ai_flags = AI_CANONNAME;
    data->hints.ai_family = AF_UNSPEC; //either IPV4 or IPV6
    data->hints.ai_socktype = SOCK_RAW;
    data->hints.ai_protocol = IPPROTO_ICMP;
    data->hints.ai_addrlen = 0;
    data->hints.ai_addr = NULL;
    data->hints.ai_canonname = NULL;
    data->hints.ai_next = NULL;
    
    int errcode = getaddrinfo ((char*)data->target, NULL, &data->hints, &data->res);
    if (errcode < 0) {
        printf("Getaddrinfo failed\n");
        exit(EXIT_FAILURE);
    }
    while(data->res) {
        inet_ntop (data->res->ai_family, data->res->ai_addr->sa_data, data->address, 100);
        switch(data->res->ai_family) {
            case AF_INET:
                data->ptr = data->res->ai_addr;
                data->type = AF_INET;
                break;
            case AF_INET6:
                data->ptr = data->res->ai_addr;
                data->type = AF_INET6;
                break;
        }
        if (data->type == AF_INET) {
            inet_ntop (data->res->ai_family, &((struct sockaddr_in *)data->ptr)->sin_addr, data->address, 100);
        } else {
            inet_ntop (data->res->ai_family, &((struct sockaddr_in6 *)data->ptr)->sin6_addr, data->address, 100);
        }
        
        //printf ("IPv%d address: %s (%s)\n", data->res->ai_family == PF_INET6 ? 6 : 4, data->address, data->res->ai_canonname);
        data->res = data->res->ai_next;
    }
    
    data->fd = socket(data->type, SOCK_RAW, IPPROTO_ICMP);
    if (data->fd < 0) {
        printf("ping: cannot resolve %s: Unknown host", (char*)data->target);
        exit(EXIT_FAILURE);
    }

    // int option = 1;
    // if (setsockopt(data->fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
    //     exit(EXIT_FAILURE);
    // if (setsockopt(data->fd, IPPROTO_IP, IP_TTL, &data->ttl, sizeof data->ttl) < 0)
    //     exit(EXIT_FAILURE);
    data->timeout.tv_sec = data->opts.w;
    if (setsockopt(data->fd, SOL_SOCKET, SO_RCVTIMEO, &data->timeout, sizeof data->timeout) < 0)
        exit(EXIT_FAILURE);
}