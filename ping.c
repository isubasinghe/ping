#include <stdio.h>
#include <stdlib.h>
#include "ping.h"


int ping(const char *hostname, ping_opts *opts) {
    printf("Pinging %s with %d bytes with ttl of %d\n", hostname, opts->size, opts->ttl);
    return 0;
}
