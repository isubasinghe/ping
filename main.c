#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "ping.h"


static const char *const usage[] = {
    "ping host [[--] args]",
    NULL,
    NULL,
};

ping_opts opts = { 30, 32 };


struct argparse_option options[] = {
    OPT_HELP(),
    OPT_GROUP("Basic Options"),
    OPT_INTEGER('T', "ttl", &opts.ttl, "Time To Live"),
    OPT_INTEGER('s', "size", &opts.size, "Size of data to send"),
    OPT_END()
};

int main(int argc, const char *argv[]) {
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nPing a host or IP", 
        "\nPing application developed for cloudflare internship position\n");
    argc = argparse_parse(&argparse, argc, argv);

    if (argc != 1) {
        argparse_usage(&argparse);
        return 0;
    }
    ping(argv[0], &opts);
}
