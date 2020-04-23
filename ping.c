#include "ping.h"


volatile sig_atomic_t loop = 1;

void sig_handle() {
    loop = 0;
}

int get_ipv4_addr(const char *hostname, char *ip, struct sockaddr_in *con) {
    struct hostent *he;
    if ( (he = gethostbyname( hostname ) ) == NULL) {
        printf("ERROR\n");
        herror("gethostbyname");
        return -1;
    }

    strcpy(ip, inet_ntoa(*(struct in_addr *) 
                          he->h_addr)); 
    
    if(con == NULL) {
        return -1;
    }

    con->sin_family = he->h_addrtype;
    con->sin_port = htons( PORT_NO );
    con->sin_addr.s_addr = *(long *)he->h_addr;

    return 1;

}


unsigned short checksum(void *b, int len) {    
    unsigned short *buf = b; 
    unsigned int sum=0; 
    unsigned short result; 
  
    for ( sum = 0; len > 1; len -= 2 ) 
        sum += *buf++; 
    if ( len == 1 ) 
        sum += *(unsigned char*)buf; 
    sum = (sum >> 16) + (sum & 0xFFFF); 
    sum += (sum >> 16); 
    result = ~sum; 
    return result; 
} 

int ping(const char *hostname, ping_opts *opts) {

    // Stop looping and display results
    signal(SIGINT, sig_handle);

    struct sockaddr_in con;
    memset(&con, 0, sizeof(struct sockaddr_in));
    struct sockaddr_in r_addr;
    memset(&r_addr, 0, sizeof(struct sockaddr_in));

    int sent_count = 0;
    int replies = 0;

    // Ideally we should be doing something more safe, given we are strcpy-ing in the above function.
    // but even ipv6 will be under 1024 characters. 
    // This imo is acceptable for the purposes of the coding tech test.
    char ip[1024];
    memset(ip, 0, 1024*sizeof(char));

    if(get_ipv4_addr(hostname, ip, &con) < 0) {
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

    struct timeval time_rcv;
    memset(&time_rcv, 0, sizeof(struct timeval));
    time_rcv.tv_sec = RECV_TIMEOUT;
    time_rcv.tv_usec = 0;

    

    if (setsockopt(sock_fd, SOL_IP, IP_TTL, &opts->ttl, sizeof(opts->ttl)) != 0) {
        perror("Unable to set ttl\n");
        return -1;
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_rcv, sizeof(struct timeval));

    
    ping_pkt pkt;
    


    while(loop) {
        memset(&pkt, 0, sizeof(ping_pkt));

        pkt.hdr.type = ICMP_ECHO; 
        pkt.hdr.un.echo.id = getpid();
        
        int i;
        for(i = 0; i < sizeof(pkt.msg); i++) {
            // http://www.asciitable.com/ +97 for 'a'
            // we wrap over to ensure we are between [97, 122] or ['a', 'z']
            pkt.msg[i] = (i%26) + 97;
        } 

        pkt.msg[i] = 0;
        pkt.hdr.un.echo.sequence = sent_count;
        pkt.hdr.checksum = checksum(&pkt, sizeof(ping_pkt));

        struct timespec start_time;
        struct timespec end_time;
        memset(&start_time, 0, sizeof(struct timespec));
        memset(&end_time, 0, sizeof(struct timespec));


        // MONOTONIC clock needed to ensure time diff checks are more accurate
        // The value from end-time-start_time in CLOCK_REALTIME may be negative. 
        clock_gettime(CLOCK_MONOTONIC, &start_time);

        if ( sendto(sock_fd, &pkt, sizeof(ping_pkt), 0, (struct sockaddr*)&con, sizeof(struct sockaddr_in)) <= 0) {
            printf("Failed to send\n");
            sent_count++;
            continue;
        }

        socklen_t addrlen  = sizeof(r_addr);

        if( recvfrom(sock_fd, &pkt, sizeof(ping_pkt), 0, (struct sockaddr*)&r_addr, &addrlen) <= 0) {
            printf("Packet recv failed\n");
            sent_count++;
            continue;
        }
            
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        double time_elapsed = ((double)(end_time.tv_nsec -  
                                 start_time.tv_nsec))/1000000.0;

        double rtt_msec = (end_time.tv_sec- 
                          start_time.tv_sec) * 1000.0 
                        + time_elapsed; 
        
        // type 69, code 0 is a success, 
        // branch if we failed.
        if(!(pkt.hdr.type ==69 && pkt.hdr.code==0)) {
            sent_count++;

            // TTL exceeded check 
            if(pkt.hdr.type == 11) {
                printf("Time Out: TTL exceeded\n");
            }else {
                printf("Error: Received ICMP packet with type %d code %d\n", pkt.hdr.type, pkt.hdr.code);
            }
            
            
            continue;
        }

        printf("Received %d bytes from %s | seq=%d | ttl=%d | rtt=%lf ms\n", 
            PACKET_SIZE, hostname, sent_count, opts->ttl, rtt_msec);


        sent_count++;
        replies++;

        usleep(opts->sleep_time  * 1000);


    }
    printf("%d packets sent, %d packets received, %f packet_loss\n", 
        sent_count, replies,  ( (sent_count-replies) / (sent_count) )*100.0);

    return 0;
}
