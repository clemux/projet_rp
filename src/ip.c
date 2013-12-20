#include "ip.h"

uint16_t checksum(uint16_t *buffer, uint32_t size) {
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

    /* on retourne le complément à 1 de la somme */
    return (uint16_t)(~sum);
}
