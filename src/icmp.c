#include "icmp.h"

icmp_header_t* prepare_icmp_header(uint8_t code, uint8_t type) {
    icmp_header_t *header = malloc(sizeof(struct icmp_header));
    header->type = type;
    header->code = code;
    header->checksum = htons(0x0000);
    header->id = htons(0x0001);
    header->seq_num = htons(0x0001);

    header->checksum = checksum((uint16_t*) header, ICMP_HDR_LEN);

    return header;
}
