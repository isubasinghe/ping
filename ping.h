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
#include <signal.h>

#define RECV_TIMEOUT 1
#define PORT_NO 0  
#define PACKET_SIZE 64


typedef struct {
    struct icmphdr hdr;
    char msg[PACKET_SIZE-sizeof(struct icmphdr)];
} ping_pkt;

typedef struct {
    int ttl;
    int size;
    unsigned int sleep_time;
} ping_opts;

unsigned short checksum(void *b, int len);

int get_ipv4_addr(const char *hostname, char *ip, struct sockaddr_in *con);

int ping(const char *hostname, ping_opts *opts);

#endif // PING_H
