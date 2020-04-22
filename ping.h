#ifndef PING_H
#define PING_H

typedef struct {
    int ttl;
    int size;
} ping_opts;



int ping(const char *hostname, ping_opts *opts);

#endif // PING_H
