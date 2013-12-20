#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>
#include <stdlib.h>

#define ICMP_HDR_LEN 8

typedef struct icmp_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq_num;	
} icmp_header_t;

icmp_header_t* prepare_icmp_header(uint8_t code, uint8_t type);

#endif
