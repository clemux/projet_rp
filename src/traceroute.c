
#include "icmp.h"
#include "ip.h"
#include "packet.h"
#include "dbg.h"
#include "utils.h"
#include "probe.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop

#define TTLMAX 25
#define TIMEOUT 5

#define NB_PROBES 3

int main(int argc, char *argv[]) {
    struct packet pkt_r;
    unsigned int ttl, i;
    int sockfd;
    struct sockaddr_in destination; // hote vers lequel on veut la liste des routes
    struct probe_response probe_responses[NB_PROBES];

    int res; // valeur de retour utilisée par différentes fonctions

    // check the number of args on command line
    if(argc != 2)
    {
        printf("USAGE: %s @adress\n", argv[0]);
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("impossible d'ouvrir la socket ");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    destination.sin_family = AF_INET;

    // conversion de l'adresse donnée en sockaddr_in
    res = ip_to_sinaddr(argv[1], &destination.sin_addr);
    if (res == -1)
        exit(1);

    //TRACEROUTE

    printf("Lancement de traceroute vers %s, nombre de saut maximum : %d\n",
           argv[1], TTLMAX);
    probe_responses[0].icmp.type = 11; // on pourrait mettre n'importe quelle valeur != 0
    for(ttl=1; ttl < TTLMAX && probe_responses[0].icmp.type != ECHO_REPLY; ttl++)
    {
        // incrémentation du ttl dans le paquet
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); 

        for (i=0; i < NB_PROBES; i++)
            probe_responses[i] = probe_icmp(sockfd, &destination);
        printf("%d - (%s) ", ttl, probe_responses[0].source_ip);
        for (i=0; i < NB_PROBES; i++)
            if (probe_responses[i].status == PROBE_OK)
                printf("(%ld ms)", probe_responses[i].time);
            else
                printf("(***)");
        printf("\n");
    }
    return 0;
}
