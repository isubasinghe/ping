#ifndef PING_H
#define PING_H

#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <netinet/ip.h>
#include <netinet/ip_icmp.h> 
#include <time.h> 
#include <fcntl.h> 
#include <signal.h> 

#define RECV_TIMEOUT 1
#define PORT_NO 0  
#define PING_SLEEP_RATE 1000000

typedef struct {
    int ttl;
    int size;
} ping_opts;

int get_ipv4_addr(const char *hostname, char *ip);

int ping(const char *hostname, ping_opts *opts);

#endif // PING_H
