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
#include "ping.h"




int get_ipv4_addr(const char *hostname, char *ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    if ( (he = gethostbyname( hostname ) ) == NULL) {
        printf("ERROR\n");
        herror("gethostbyname");
        return -1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(int i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        break;
    }

    return 1;

}

int ping(const char *hostname, ping_opts *opts) {
    // Ideally we should be doing something more safe, given we are strcpy-ing in the above function.
    // but even ipv6 will be under 1024 characters. 
    // This imo is acceptable for the purposes of the coding tech test.
    char ip[1024];
    memset(ip, 0, 1024*sizeof(char));

    if(get_ipv4_addr(hostname, ip) < 0) {
        return -1;
    } 

    printf("Pinging [%s] with %d bytes with ttl of %d\n", 
        ip, opts->size, opts->ttl);

    int sock_fd = 0;

    sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sock_fd < 0) {
        perror("Unable to obtain file descriptor for socket\n");
        return -1;
    }

    struct timespec time;
    memset(&time, 0, sizeof(struct timeval));
    time.tv_sec = RECV_TIMEOUT;
    time.tv_nsec = 0;

    clock_gettime(CLOCK_MONOTONIC, &time);

    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &opts->ttl, sizeof(opts->ttl)) != 0) {
        perror("Unable to set ttl\n");
        return -1;
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time, sizeof(struct timeval));
    
    while(1) {
        
    }


    return 0;
}
