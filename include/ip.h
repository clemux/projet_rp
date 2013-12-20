/**
* \file ip.h
* \brief Contient les definitions pour ip.c et la structure ip_header
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef IP_H
#define IP_H

#include <stdint.h>

/**
 * \struct ip_header
 * \brief objet header ip
 * Permet de contenir les informations du header ip du paquet
 */
struct ip_header {
    uint8_t version_hdl; /** version du paquet */
    uint8_t service; /** service du paquet */
    uint16_t length; /**longueur du paquet */
    uint16_t id; /** id du paquet */
    uint8_t flags; /** flags du paquet */
    uint8_t offset; /** offset du paquet */
    uint8_t ttl; /** ttl du paquet */
    uint8_t protocol; /** protocole du paquet */
    uint16_t checksum; /** checksum du paquet */
    uint32_t ip_src; /** ip de la source du paquet */
    uint32_t ip_dest; /**ip de la destination du paquet */
};

uint16_t checksum(uint16_t *buffer, uint32_t size);



#endif
