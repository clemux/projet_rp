#include "probe.h"
#include "utils.h"
#include "icmp.h"
#include "packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>


    

struct probe_response probe_icmp(int sockfd, struct sockaddr_in *destination) {
    struct packet pkt_r;
    struct icmp_header *pkt_s;
    struct probe_response response;
    struct timeval interval;
    
    response.source_ip = malloc(sizeof(uint32_t)); //

    response.status = PROBE_NO_RESPONSE;
    response.integrity_error = INTEGRITY_OK;
    response.time = TIMEOUT*1000;

    //Envoi le paquet ICMP
    if ((pkt_s = send_echo_request(sockfd, destination)) == NULL)
    {
        perror("Envoi de la sonde échoué ");
        response.status = PROBE_SEND_FAIL;
    }
    //Réinitialisation des valeurs necessaire a select
    interval.tv_sec = TIMEOUT;
    interval.tv_usec = 0;

    if (receive_packet(sockfd, &pkt_r, &interval) > 0) {
        response.status = PROBE_OK; // on a reçu une réponse
    }
    else
        return response;
    
    // Stock l'ip du routeur dans ip_string en format texte
    if(inet_ntop(AF_INET, &(pkt_r.ip.ip_src), response.source_ip,
                 sizeof(char)*16) == NULL) {
        perror("echec inet_ntop");
        response.integrity_error = 1;
        response.source_ip = NULL;
    }

    response.icmp = pkt_r.icmp;

    if(!(pkt_r.icmp.type == 0 &&
         pkt_r.icmp.id == pkt_s->id &&
         pkt_r.icmp.seq_num == pkt_s->seq_num)) {
        response.integrity_error = INTEGRITY_ERROR;
    }
    
    response.time = (1000000 - (long int) interval.tv_usec) / 1000;
    if (TIMEOUT - 1 - interval.tv_sec > 0)
        response.time += (long int) (TIMEOUT - 1 - interval.tv_sec)*1000;

    return response;
}
