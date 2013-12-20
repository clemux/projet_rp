#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>


#define ICMP_HDR_LEN 8

struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq_num;	
};

struct icmp_header* prepare_icmp_header(uint8_t code, uint8_t type);

#endif
