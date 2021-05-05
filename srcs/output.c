#include "./../ft_traceroute.h"

static void output_without_hostname(t_data *data) {
    t_node *tmp = data->node;

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

static void output_with_hostname(t_data *data) {
    t_node *tmp = data->node;

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

void    output(t_data *data) {
    if(data->opts.n)
        output_with_hostname(data);
    else
        output_without_hostname(data);
}

void    print_start(t_data *data) {
    printf("traceroute to %s (%s), %d hops max, %d byte packets\n", data->target, data->address, 64, data->packet_size);
}