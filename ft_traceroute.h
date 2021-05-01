#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <assert.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <errno.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <math.h>
# include <stdbool.h>

#define HEADER_SIZE 12

typedef struct          s_time
{
    double              time;
    struct s_time       *next;
}                       t_time;

typedef struct          s_node
{
    char                *ip;
    char                *domain;
    struct s_time       *time;
    struct s_node       *next;
}                       t_node;

typedef struct          s_option
{
    int                 h; // ???
    int                 f; // ttl to start > 0
    int                 m; // max ttl > 0
    int                 q; // nb prob > 0
    int                 w; // waittime for a prob > 0
    bool                n; // display hostname
}                       t_option;

typedef struct		    s_data
{
    char                *target;
    char                address[100];
    void                *ptr;
    int                 type;
    int                 fd;
    int                 ttl;
    int                 packet_size;
    int                 probe;
    struct timeval      timeout;
    struct timeval      sending_time;
    struct timeval      receiving_time;
    struct addrinfo     hints;
    struct addrinfo     *res;
    struct s_node       *node;
    struct s_option     opts;
}                       t_data;

void check_error(int argc, char **argv);

void parsing(t_data *data, uint8_t **argv);

t_node *new_node(char *ip, char *domain);
t_node *node_last(t_node *node);
t_node *node_good(t_node **node, char *ip, char *domain);
void node_add_back(t_node **head, t_node *new);
void node_free(t_node *node);

t_time *new_time(double t);
t_time *time_last(t_time *node);
void time_add_back(t_time **head, t_time *new);
void add_time(t_node *node, t_time * time);
void time_free(t_time *node);

void init_data(t_data *data);
void init_socket(t_data *data);

#endif