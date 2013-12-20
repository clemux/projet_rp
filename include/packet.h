/**
* \file packet.h
* \brief Contient les structure packet ainsi que la definition de l'initialisation de la structure packet
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef PACKET_H
#define PACKET_H

#include "ip.h"
#include "icmp.h"

/**
 * \struct packet
 * \brief objet paquet ip
 * Permet de contenir les informations du paquet,
 * a savoir le header icmp et le header ip
 */
struct packet {
    struct ip_header ip; /** header ip */
    struct icmp_header icmp; /** header icmp */
};

struct packet* new_packet(void);

#endif
