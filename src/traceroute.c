
#include "icmp.h"
#include "ip.h"
#include "packet.h"
#include "dbg.h"
#include "utils.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop

#define TTLMAX 25
#define TIMEOUT 5

int main(int argc, char *argv[]) {
    struct packet pkt_r;
    unsigned int ttl;
    int sockfd;
    char *ip_string;
    struct sockaddr_in destination; // hote vers lequel on veut la liste des routes


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
    ip_string = malloc(sizeof(uint32_t)); // pour stocker l'adresse ip avant
                                          // de l'afficher
    printf("Lancement de traceroute vers %s, nombre de saut : %d\n",
           argv[1], TTLMAX);
    pkt_r.icmp.type = 11; // on pourrait mettre n'importe quelle valeur != 0
    for(ttl=1; ttl < TTLMAX && pkt_r.icmp.type != ECHO_REPLY; ttl++)
    {
        // incrémentation du ttl dans le paquet
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); 

        if (send_echo_request(sockfd, &destination) == NULL) {
            perror("Envoi échoué ");
            close(sockfd);
            exit(-1);
        }

        if (receive_packet(sockfd, &pkt_r) == -1) {
            close(sockfd);
            exit(1);
        }
        
        // Stock l'ip du routeur dans ip_string en format texte
        if(inet_ntop(AF_INET, &(pkt_r.ip.ip_src), ip_string,
                     sizeof(argv[1])*2) == NULL) 
        {
            perror("inet_ntop ");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("%d routeur : (%s)\n", ttl, ip_string);
        
    }

    return 0;
}
