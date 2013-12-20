/**
* \file icmp.c
* \brief Contient les implémentations des fonctions permettant de preparer et d'envoyer un paquet par protocole icmp
* \author Arnaud Petit
* \author Clement Schreiner
*/

#include "icmp.h"
#include "ip.h"
#include <stdlib.h> // malloc
#include <arpa/inet.h> // htons
#include <stdio.h> // perror


/**
 * \fn struct icmp_header* prepare_icmp_header(uint8_t code, uint8_t type, unsigned int seq_num)
 * \brief prepare le header icmp
 * \param uint8_t code, le code du paquet
 * \param uint8_t type, le type du paquet
 * \param unsigned int seq_num, le numero de sequence du paquet
 * \return un pointeur vers une structure icmp_header
 */
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

/**
 * \fn struct icmp_header* send_echo_request(int sockfd, struct sockaddr_in *destination)
 * \brief prepare et envoi un icmp echo request
 * \param int sockfd, le socket par lequel le paquet envoyé
 * \param struct sockaddr_in, la structure contenant notamment l'adresse de destination
 * \return un pointeur vers une structure icmp_header, NULL si l'envoi a échoué
 */
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
