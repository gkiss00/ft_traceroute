#include "./../ft_traceroute.h"

int nb_target;
int h_value = 0;
int f_value = 1;
int m_value = 30;
int q_value = 3;
int w_value = 5;

// static bool isBooleanOption(char *arg) {
//     return (strchr("n", arg[1]) != NULL);
// }

static bool isIntegerOption(char *arg) {
    return (strchr("hfmqw", arg[1]) != NULL);
}

static bool isKnownOption(char *arg) {
    return (strchr("hfmqwn", arg[1]) != NULL);
}

static bool isOption(char *arg) {
    return (arg[0] == '-');
}


static void print_error_waittime() {
    printf("traceroute: wait time must be > 0");
}

static void print_error_first_max_ttl() {
    printf("traceroute: first ttl (%d) may not be greater than max ttl (%d)", f_value, m_value);
}

static void print_error_n_probe() {
    printf("traceroute: nprobes must be > 0");
}

static void print_error_max_ttl() {
    printf("traceroute: max ttl must be > 0");
}

static void print_error_first_ttl() {
    printf("traceroute: first ttl must be > 0");
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

static void check_numeric_option_value(char **argv) {
    int i = 1;
    while (argv[i]) {
        if(isOption(argv[i])) {
            if (isIntegerOption(argv[i])) {
                int value = atoi(argv[i + 1]);
                if (argv[i][1] == 'h') {
                    h_value = value;
                } else if (argv[i][1] == 'f') {
                    f_value = value;
                } else if (argv[i][1] == 'm') {
                    m_value = value;
                } else if (argv[i][1] == 'q') {
                    q_value = value;
                } else if (argv[i][1] == 'w') {
                    w_value = value;
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
    if (f_value <= 0) {
        print_error_first_ttl();
        exit(EXIT_FAILURE);
    } else if (m_value <= 0) {
        print_error_max_ttl();
        exit(EXIT_FAILURE);
    }else if (q_value <= 0){
        print_error_n_probe();
        exit(EXIT_FAILURE);
    } else if (m_value < f_value) {
        print_error_first_max_ttl();
        exit(EXIT_FAILURE);
    } else if (w_value <= 0) {
        print_error_waittime();
        exit(EXIT_FAILURE);
    }
}

static void check_options(char **argv) {
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
            ++nb_target;
        }
        ++i;
    }
    check_numeric_option_value(argv);
}

static void check_args() {
    if (nb_target != 1) {
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

void check_error(int argc, char **argv) {
    check_nb_args(argc);
    check_options(argv);
    check_args();
}