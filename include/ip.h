#ifndef IP_H
#define IP_H

#include <stdint.h>

struct ip_header {
    uint8_t version_hdl;
    uint8_t service;
    uint16_t length;
    uint16_t id;
    uint8_t flags;
    uint8_t offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t ip_src;
    uint32_t ip_dest;
};

uint16_t checksum(uint16_t *buffer, uint32_t size);



#endif
