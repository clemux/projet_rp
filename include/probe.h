/**
* \file probe.h
* \brief Contient la structure probe_response ainsi que la definition de l'envoi de sonde
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef PROBE_H
#define PROBE_H

#include "icmp.h"

#include <netinet/in.h>

/**
 * \def TIMEOUT seconde avant le timeout
 */
#define TIMEOUT 5
/**
 * \def PROBE_SEND_FAIL code de retour d'une sonde non envoyé
 */
#define PROBE_SEND_FAIL 2;
/**
 * \def PROBE_NO_RESPONSE code de retour d'une absence d'echo reply
 */
#define PROBE_NO_RESPONSE 1
/**
 * \def PROBE_OK code de retour quand tout se passe bien
 */
#define PROBE_OK 0
/**
 * \def INTEGRITY_OK code de paquet correct
 */
#define INTEGRITY_OK 0
/**
 * \def INTEGRITY_ERROR code d'erreur sur paquet
 */
#define INTEGRITY_ERROR 1

#include "icmp.h"

/**
 * \struct probe_response
 * \brief structure comprenant les informations qui nous interessent de la reponse
 */
struct probe_response {
    unsigned int status;  /** status de la reponse */ // FIXME: enum
    long int time; /** valeur du ping en ms */ // ^
    unsigned int ttl; /** ttl du echo reply, si reçu */
    unsigned int integrity_error; /** code d'integrite de la reponse */ // 0 si reponse ok, sinon 1
    char *source_ip; /** ip de la reponse */ // source de la réponse
    struct icmp_header icmp; /** header icmp de la reponse */
};

struct probe_response probe_icmp(int sockfd, struct sockaddr_in *destination);

#endif
