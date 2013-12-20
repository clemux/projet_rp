#ifndef IP_H
#define IP_H

typedef struct ip_header
{
    u_int8_t version_hdl;
    u_int8_t service;
    u_int16_t length;
    u_int16_t id;
    u_int8_t flags;
    u_int8_t offset;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t checksum;
    u_int32_t ip_src;
    u_int32_t ip_dest;
} ip_header_t;

uint16_t checksum(uint16_t *buffer, uint32_t size) 
{
    unsigned long sum = 0;

    while(size > 1) 
    {
        sum += *buffer++;
        size -= sizeof(uint16_t);
    }

    if(size == 1) 
    {
        sum += *(uint8_t*)buffer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >>16);

    return (uint16_t)(~sum); /* on retourne le complément à 1 de la somme */
}

#endif
