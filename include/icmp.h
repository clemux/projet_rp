/**
* \file icmp.h
* \brief Contient les definitions pour icmp.c et la structure icmp_header
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef ICMP_H
#define ICMP_H

#include <stdint.h>

/**
 * \def ICMP_HDR_LEN taille du header icmp
 */
#define ICMP_HDR_LEN 8
/**
 * \def ECHO code pour echo request
 */
#define ECHO 0
/**
 * \def ECHO_REQUEST type pour echo request
 */
#define ECHO_REQUEST 8
/**
 * \def ECHO REPLY type pour echo reply
 */
#define ECHO_REPLY 0
/**
 * \def TIME_EXCEEDED type pour Time exceeded
 */
#define TIME_EXCEEDED 11

#include <netinet/in.h>

/**
 * \struct icmp_header
 * \brief objet header icmp
 * Permet de contenir les informations du header icmp du paquet
 */
struct icmp_header {
    uint8_t type; /** type du paquet */
    uint8_t code; /** code du paquet */
    uint16_t checksum; /** checksum du paquet */
    uint16_t id; /** id du paquet */
    uint16_t seq_num;	/** numero de sequence du paquet */
};

struct icmp_header* prepare_icmp_header(uint8_t code, uint8_t type,
                                        unsigned int seq_num);
struct icmp_header* send_echo_request(int sockfd, struct sockaddr_in *destination);

#endif
