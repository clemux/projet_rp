#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

#define ICMP_HDR_LEN 8
#define ECHO 0
#define ECHO_REQUEST 8
#define ECHO_REPLY 0
#define TIME_EXCEEDED 11

#include <netinet/in.h>

struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq_num;	
};

struct icmp_header* prepare_icmp_header(uint8_t code, uint8_t type,
                                        unsigned int seq_num);
struct icmp_header* send_echo_request(int sockfd, struct sockaddr_in *destination);

#endif
