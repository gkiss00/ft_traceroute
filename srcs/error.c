#include "./../ft_traceroute.h"

// static bool isBooleanOption(char *arg) {
//     return (strchr("n", arg[1]) != NULL);
// }

static bool isNum(char *str) {
    int i = 0;
    while(str[i]) {
        if (str[i] > '9' || str[i] < '0')
            return false;
        ++i;
    }
    return true;
}

static bool isIntegerOption(char *arg) {
    return (strchr("hfmqw", arg[1]) != NULL);
}

static bool isKnownOption(char *arg) {
    return (strchr("hfmqwn", arg[1]) != NULL);
}

static bool isOption(char *arg) {
    return (arg[0] == '-');
}

static void print_error_packet_length_value(char *str) {
    printf("traceroute: \"%s\" bad value for packet length\n", str);
}

static void print_error_packet_length_size_min() {
    printf("traceroute: packet length must be > 51\n");
}

static void print_error_packet_length_size_max() {
    printf("traceroute: packet length must be <= 32768\n");
}

static void print_error_waittime() {
    printf("traceroute: wait time must be > 0\n");
}

static void print_error_first_max_ttl(t_error *error) {
    printf("traceroute: first ttl (%d) may not be greater than max ttl (%d)\n", error->f_value, error->m_value);
}

static void print_error_n_probe() {
    printf("traceroute: nprobes must be > 0\n");
}

static void print_error_max_ttl_max() {
    printf("traceroute: max ttl must be <= 255\n");
}

static void print_error_max_ttl_min() {
    printf("traceroute: max ttl must be > 0\n");
}

static void print_error_first_ttl_max() {
    printf("traceroute: first ttl must be <= 255\n");
}

static void print_error_first_ttl_min() {
    printf("traceroute: first ttl must be > 0\n");
}

static void print_error_argument_required(char opt) {
    printf("traceroute: option requires an argument -- %c\n", opt);
}

static void print_error_option(char opt) {
    printf("traceroute: invalid option -- %c\n", opt);
}

static void print_error_usage() {
    printf("usage: traceroute\n");
    printf("\t[-h ???]\n");
    printf("\t[-f first_ttl]\n");
    printf("\t[-m max_ttl]\n");
    printf("\t[-q nqueries]\n");
    printf("\t[-w waittime]\n");
    printf("\t[-n]\n");
}

static void check_option_format(char *opt) {
    if (strlen(opt) != 2) {
        print_error_usage();
        exit(EXIT_FAILURE);
    }
    if (isKnownOption(opt) == false) {
        print_error_option(opt[1]);
        print_error_usage();
        exit(EXIT_FAILURE);
    }
}

static void check_numeric_option_value(char **argv, t_error *error) {
    int i = 1;
    while (argv[i]) {
        if(isOption(argv[i])) {
            if (isIntegerOption(argv[i])) {
                int value = atoi(argv[i + 1]);
                if (argv[i][1] == 'h') {
                    error->h_value = value;
                } else if (argv[i][1] == 'f') {
                    error->f_value = value;
                } else if (argv[i][1] == 'm') {
                    error->m_value = value;
                } else if (argv[i][1] == 'q') {
                    error->q_value = value;
                } else if (argv[i][1] == 'w') {
                    error->w_value = value;
                }
                ++i;
            }
        }
        ++i;
    }
    // if (h_value == 0) {
    //     printf("ping: invalid increment size: `0'\n");
    //     exit(EXIT_FAILURE);
    // }
    if (error->f_value <= 0) {
        print_error_first_ttl_min();
        exit(EXIT_FAILURE);
    } else if (error->f_value > 255) {
        print_error_first_ttl_max();
        exit(EXIT_FAILURE);
    } else if (error->m_value <= 0) {
        print_error_max_ttl_min();
        exit(EXIT_FAILURE);
    } else if (error->m_value > 255) {
        print_error_max_ttl_max();
        exit(EXIT_FAILURE);
    } else if (error->q_value <= 0){
        print_error_n_probe();
        exit(EXIT_FAILURE);
    } else if (error->m_value < error->f_value) {
        print_error_first_max_ttl(error);
        exit(EXIT_FAILURE);
    } else if (error->w_value <= 0) {
        print_error_waittime();
        exit(EXIT_FAILURE);
    }
}

static void check_options(char **argv, t_error *error) {
    int i = 1;
    while (argv[i]) {
        if(isOption(argv[i])) {
            check_option_format(argv[i]);
            if (isIntegerOption(argv[i])) {
                ++i;
                if (argv[i] == NULL) {
                    print_error_argument_required(argv[i - 1][1]);
                    print_error_usage();
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            ++error->nb_target;
            if (argv[i + 1] != NULL && argv[i + 2] != NULL) {
                print_error_usage();
                exit(EXIT_FAILURE);
            }
            if (argv[i + 1]) {
                if (!isNum(argv[i + 1])) {
                    print_error_packet_length_value(argv[i + 1]);
                    exit(EXIT_FAILURE);
                }
                if (atoi(argv[i + 1]) > 32768) {
                    print_error_packet_length_size_max();
                    exit(EXIT_FAILURE);
                } else if (atoi(argv[i + 1]) < 52) {
                    print_error_packet_length_size_min();
                    exit(EXIT_FAILURE);
                }
            }
            check_numeric_option_value(argv, error);
            return ;
        }
        ++i;
    }
}

static void check_args(t_error *error) {
    if (error->nb_target != 1) {
        print_error_usage();
        exit(EXIT_FAILURE);
    }
}

static void check_nb_args(int argc) {
    if (argc == 1) {
        print_error_usage();
        exit(EXIT_FAILURE);
    }
}

void init_error(t_error *error) {
    error->nb_target = 0;
    error->h_value = 0;
    error->f_value = 1;
    error->m_value = 30;
    error->q_value = 3;
    error->w_value = 5;
}

void check_error(int argc, char **argv) {
    t_error error;
    init_error(&error);
    check_nb_args(argc);
    check_options(argv, &error);
    check_args(&error);
}