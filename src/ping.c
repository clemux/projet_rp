#include "packet.h"
#include "utils.h"
#include "dbg.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>



#define NB_PING 10
#define TIMEOUT 5
#define DEFAULT_TTL 64

int main(int argc, char *argv[]) {
    struct packet pkt_r;
    struct icmp_header *pkt_s;
    unsigned int ttl;
    int sockfd;
    char *ip_string = malloc(sizeof(uint32_t)); // adresse ip à afficher
    struct sockaddr_in destination; // hote vers lequel on veut la liste des routes
    struct timeval interval;
    int res;
    int i;
    long int time, mean = 0;
    unsigned int lost_packets = 0;
    

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


    
    ttl = DEFAULT_TTL;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    printf("Lancement de Ping, envois de %d paquet(s)\n", NB_PING);
    for (i = 0; i < NB_PING; i++)
    {
        //Envoi le paquet ICMP
        if ((pkt_s = send_echo_request(sockfd, &destination)) == NULL)
        {
            perror("Envoi échoué ");
            close(sockfd);
            exit(-1);
        }
        //Réinitialisation des valeurs necessaire a select
        interval.tv_sec = TIMEOUT;
        interval.tv_usec = 0;
        if (receive_packet(sockfd, &pkt_r, &interval) == -1) {
            lost_packets++;
        }
        
        // Stock l'ip du routeur dans ip_string en format texte
        if(inet_ntop(AF_INET, &(pkt_r.ip.ip_src), ip_string,
                     sizeof(argv[1])*2) == NULL)
        {
            perror("inet_ntop merde ");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        
        if(pkt_r.icmp.type == 0 &&
           pkt_r.icmp.id == pkt_s->id &&
           pkt_r.icmp.seq_num == pkt_s->seq_num)
        {
            time = (1000000 - (long int) interval.tv_usec) / 1000;
            if (TIMEOUT-1-interval.tv_sec > 0)
            {
                printf("%d time =  %ld sec et %ld ms\n", i+1,
                       (long int)TIMEOUT-1-interval.tv_sec, time);
                mean += 1000 * (long int)TIMEOUT-1-interval.tv_sec;
                mean += time;
            }
            else
            {
                printf("%d time = %ld ms\n", i+1, time);
                mean += time;
            }
        }
    }

    free(pkt_s);
    pkt_s = NULL;
    
    printf("Moyenne : %ld ms\nNombre de Paquet perdu : %d\n", mean/i,
           lost_packets);

    close(sockfd);


return 0;
}
