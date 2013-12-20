/**
* \file traceroute.c
* \brief Contient la fonction de traceroute
* \author Arnaud Petit
* \author Clement Schreiner
*/

#include "traceroute.h"
#include "probe.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop
#include <stdio.h>

/**
 * \def TTLMAX Nombre de saut pour maximal
 */
#define TTLMAX 25
/**
 * \def TIMEOUT Secondes avant le timeout
 */
#define TIMEOUT 5

/**
 * \def NB_PROBES nombre de sondes envoyer par saut
 */
#define NB_PROBES 3

/**
 * \fn do_traceroute(int sockfd, struct sockaddr_in *destination, unsigned int nb_probes, unsigned int ttlmax)
 * \brief lance le traceroute
 * \param sockfd, socket a utilisé pour le traceroute
 * \param destination, pointeur vers la structure sockaddr_in contenant les informations de la destination des paquets
 * \param nb_probes, nombre de sondes envoyé par incrémentation de ttl
 * \param ttlmax, nombre de saut maximal du traceroute
 */
void do_traceroute(int sockfd, struct sockaddr_in *destination,
                   unsigned int nb_probes, unsigned int ttlmax) {
    struct probe_response *probe_responses = malloc(nb_probes*sizeof(struct probe_response));
    unsigned int ttl, i;
        
    probe_responses[0].icmp.type = 11; // on pourrait mettre n'importe quelle valeur != 0
    for(ttl=1; ttl < ttlmax && probe_responses[0].icmp.type != ECHO_REPLY; ttl++)
    {
        // incrémentation du ttl dans le paquet
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        for (i=0; i < nb_probes; i++)
            probe_responses[i] = probe_icmp(sockfd, destination);
        
        printf("%d - (%s) ", ttl, probe_responses[0].source_ip);

        for (i=0; i < nb_probes; i++)
            if (probe_responses[i].status == PROBE_OK)
                printf("(%ld ms)", probe_responses[i].time);
            else
                printf("(***)");
        printf("\n");
    }
}
