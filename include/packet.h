#ifndef PACKET_H
#define PACKET_H

#include "ip.h"
#include "icmp.h"

struct packet {
    struct ip_header ip;
    struct icmp_header icmp;
    
};

struct packet* new_packet(void);

#endif
