#include "./../ft_traceroute.h"

static void settarget(t_data *data, uint8_t *target) {
    data->target = strdup((char*)target);
}

static void setIntegerOption(t_data *data, uint8_t opt, uint8_t *arg) {
    if (opt == 'h') {
        data->opts.h = atoi((char*)arg);
    } else if (opt == 'f') {
        data->opts.f = atoi((char*)arg);
    } else if (opt == 'm') {
        data->opts.m = atoi((char*)arg);
    } else if (opt == 'q') {
        data->opts.q = atoi((char*)arg); 
    } else if (opt == 'w') {
        data->opts.w = atoi((char*)arg); 
    }
}

static void addOption(t_data *data, uint8_t *opt) {
    uint32_t i = 1;

    while(opt[i]) {
        if (opt[i] == 'h') {
            data->opts.h = 1;
        } else if (opt[i] == 'f') {
            data->opts.f = 1;
        } else if (opt[i] == 'm') {
            data->opts.m = 1;
        } else if (opt[i] == 'q') {
            data->opts.q = 1; 
        } else if (opt[i] == 'w') {
            data->opts.w = 1; 
        } else if (opt[i] == 'n') {
            data->opts.n = false;
        }
        ++i;
    }
}

static bool isIntegerOption(uint8_t *arg) {
    return (strchr("hfmq", arg[1]) != NULL);
}

static bool isOption(uint8_t *arg) {
    return (arg[0] == '-');
}

void parsing(t_data *data, uint8_t **argv){
    uint32_t i = 1;

    while(argv[i]) {
        if (isOption(argv[i])) {
            addOption(data, argv[i]);
            if (isIntegerOption(argv[i])) {
                setIntegerOption(data, argv[i][1], argv[i + 1]);
                ++i;
            }
        } else {
            settarget(data, argv[i]);
            if (argv[i + 1])
                data->packet_size = atoi((char*)argv[i + 1]);
            return ;
        }
        ++i;
    }
}