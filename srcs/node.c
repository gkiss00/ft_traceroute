#include "./../ft_traceroute.h"

t_node *new_node(char *ip, char *domain) {
    t_node *node;

    node = malloc(sizeof(t_node));
    node->ip = strdup(ip);
    node->domain = strdup(domain);
    node->time = NULL;
    node->next = NULL;
    return (node);
}

t_node *node_good(t_node **node, char *ip, char *domain) {
    t_node *tmp = *node;

    while(tmp) {
        if (strcmp(tmp->ip, ip) == 0)
            return (tmp);
        tmp = tmp->next;
    }
    t_node *nw = new_node(ip, domain);
    node_add_back(node, nw);
    return (nw);
}

t_node *node_last(t_node *node) {
    while (node && node->next) {
        node = node->next;
    }
    return (node);
}

void node_add_back(t_node **head, t_node *new) {
    if (head != 0)
	{
		if (*head && new != 0)
		{
			node_last(*head)->next = new;
		}
		else
		{
			*head = new;
		}
	}
}

t_time *new_time(double t) {
    t_time *node;

    node = malloc(sizeof(t_time));
    node->time = t;
    node->next = NULL;
    return (node);
}

t_time *time_last(t_time *node) {
    while (node && node->next) {
        node = node->next;
    }
    return (node);
}

void time_add_back(t_time **head, t_time *new) {
    if (head != 0)
	{
		if (*head && new != 0)
		{
			time_last(*head)->next = new;
		}
		else
		{
			*head = new;
		}
	}
}

void add_time(t_node *node, t_time * time) {
    time_add_back(&node->time, time);
}

void time_free(t_time *node) {
    t_time *tmp = node;
    while(node) {
        tmp = node->next;
        free(node);
        node = tmp;
    }
}

void node_free(t_node *node) {
    t_node *tmp = node;
    while(node) {
        tmp = node->next;
        free(node->domain);
        free(node->ip);
        time_free(node->time);
        free(node);
        node = tmp;
    }
}