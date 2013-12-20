/**
* \file ping.c
* \brief Contient la fonction de ping
* \author Arnaud Petit
* \author Clement Schreiner
*/

#include "packet.h"
#include "utils.h"
#include "dbg.h"
#include "probe.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>


/**
 * \def NB_PING Nombre de saut pour ping
 */
#define NB_PING 20
/**
 * \def TIMEOUT Secondes avant le timeout
 */
#define TIMEOUT 5
/**
 * \def DEFAULT_TTL Time to live par defaut
 */
#define DEFAULT_TTL 64


/**
 * \fn ping()
 * \brief lance le ping
 * \param 
 * \param 
 * \return
 */

int main(int argc, char *argv[]) {
    struct probe_response probe_response;
    unsigned int ttl;
    int sockfd;
    struct sockaddr_in destination; // hote vers lequel on veut la liste des routes
    int i, res;
    long int sum_time = 0;
    unsigned int lost_packets = 0;
    

    if(argc != 2)
    {
        printf("USAGE: %s @adress\n", argv[0]);
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        if (errno == EPERM)
            debug("Le programme doit être lancé en root.\n");
        else
            perror("Impossible d'ouvrir la socket ");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    destination.sin_family = AF_INET;

    // conversion de l'adresse donnée en sockaddr_in
    res = ip_to_sinaddr(argv[1], &destination.sin_addr);
    if (res == -1)
        exit(1);


    
    ttl = DEFAULT_TTL;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    printf("Lancement de Ping, envois de %d paquet(s)\n", NB_PING);
    sum_time = 0;
    for (i = 0; i < NB_PING; i++)
    {
        probe_response = probe_icmp(sockfd, &destination);
        
        if (probe_response.status != PROBE_OK)
            lost_packets++;
        else {
            sum_time += probe_response.time;
            printf("icmp_req=%d on %s, time=%ld ms, ttl=%d\n",
                   i + 1, probe_response.source_ip, probe_response.time,
                   probe_response.ttl);
        }
                
    }

    printf("Moyenne : %ld ms\nNombre de Paquet perdu : %d\n", sum_time/i,
           lost_packets);
    
    close(sockfd);
    
    
    return 0;
}
