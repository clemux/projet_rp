#ifndef PACKET_H
#define PACKET_H

typedef struct packet
{
    struct ip_header ip;
    struct icmp_header icmp;
    
} packet_t;


#endif
