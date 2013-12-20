#include "icmp.h"
#include "ip.h"
#include <stdlib.h> // malloc
#include <arpa/inet.h> // htons
#include <stdio.h> // perror

struct icmp_header* prepare_icmp_header(uint8_t code, uint8_t type,
                                        unsigned int seq_num) {
    struct icmp_header *header = malloc(sizeof(struct icmp_header));
    header->type = type;
    header->code = code;
    header->checksum = 0;
    header->id = htons(42);
    header->seq_num = htons(seq_num);

    header->checksum = checksum((uint16_t*) header, ICMP_HDR_LEN);

    return header;
}

struct icmp_header* send_echo_request(int sockfd, struct sockaddr_in *destination) {
    static unsigned int echo_seq_num = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    struct icmp_header *pkt_s = prepare_icmp_header(ECHO, ECHO_REQUEST, echo_seq_num);
    if (sendto(sockfd, pkt_s, sizeof(struct icmp_header),
               0, (struct sockaddr *) destination, addrlen ) == -1) {
        perror("Envoi échoué ");
        return NULL;
    }
    else {
        echo_seq_num++;
        return pkt_s;
    }
}
