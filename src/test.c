#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "icmp.h"
#include "ip.h"
#include "packet.h"
#include "dbg.h"

#define TTLMAX 25
#define PING 10
#define TIMEOUT 5

/*getnameinfo*/

int main (int argc, char **argv)
{
    int sockfd, ttl, i, paquet_perdu = 0;
    unsigned int try;
    long int temps, moyenne = 0;
    fd_set rfd;
    struct icmp_header *pkt_s;
    struct packet pkt_r;
    char *ip_retour;
    socklen_t addrlen;
    struct sockaddr_in server;
    struct timeval timeInterval;

    // check the number of args on command line
    if(argc != 2)
    {
        printf("USAGE: %s @adress\n", argv[0]);
        exit(-1);
    }
    
    
    if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        perror("impossible d'ouvrir la socket ");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    addrlen = sizeof(struct sockaddr_in);
    server.sin_family = AF_INET;

    // conversion de l'adresse donnée en sockaddr_in
    if(inet_pton(AF_INET, argv[1], &server.sin_addr) != 1)
    {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    /* Preparation du paquet ICMP request */
    pkt_s = prepare_icmp_header(0, 8);
    
    for (try=0; try < 5; try++) {
        if (sendto(sockfd, pkt_s, sizeof(struct icmp_header), 0, (struct sockaddr *)&server, addrlen ) == -1) {
            perror("Envoi échoué ");
            close(sockfd);
            exit(1);
        }
        debug("paquet envoyé, essai %d", try);
        
        /* Réinitialisation des valeurs necessaire a select */
        FD_ZERO (&rfd);
        FD_SET (sockfd, &rfd);
        timeInterval.tv_sec = TIMEOUT; //Timeout a 5 secondes
        timeInterval.tv_usec = 0; // FIXME
        
        // Ecoute le socket jusqu au timeout, si > 0, il a recu quelque chose
        if( select (sockfd + 1, &rfd, NULL, NULL, &timeInterval) > 0 ) {
            printf("select ok\n");
            try = 5;
            //Recuperation de l'information
            if(recv (sockfd, &pkt_r, sizeof(struct packet), 0) == -1) {
                perror("Rien recu ");
                close(sockfd);
                exit(-1);
            }
            else
                printf("paquet reçu: %d\n", pkt_r.ip.protocol);
        }
        else
            perror("erreur select");
    }

    printf("PROUTTT\n");
    
    if(((pkt_r.icmp.type == 0) || (pkt_r.icmp.type == 8)) &&
       (pkt_r.icmp.code == 0) &&
       (pkt_r.icmp.id == pkt_s->id) &&
       (pkt_r.icmp.seq_num == pkt_s->seq_num)) {
        printf("Reponse recue OK, destination atteignable.\nChecksum = %x\n",
               pkt_r.icmp.checksum);
    }
    else
        printf("Reponse mauvaise\n");
    
//TRACEROUTE
    pkt_r.icmp.type = 0x0B;
    ttl = 1;
    ip_retour = malloc(sizeof(uint32_t));
    printf("Lancement de traceroute vers %s, nombre de saut : %d\n", argv[1], TTLMAX);
    while (ttl < TTLMAX && strcmp(ip_retour, argv[1]) != 0)
    {
        pkt_s->id = pkt_s->id + htons(0x0001);
        pkt_s->seq_num = pkt_s->seq_num + htons(0x0001);
        pkt_s->checksum = 0;
        pkt_s->checksum = checksum((uint16_t*)&pkt_s, ICMP_HDR_LEN);
        setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); //Met a jour le ttl sur le socket
        if (sendto(sockfd, &pkt_s, sizeof(pkt_s), 0, (struct sockaddr*)&server, addrlen ) == -1) //Envoi le paquet ICMP
        {
            perror("Envoi échoué ");
            close(sockfd);
            exit(-1);
        }
        //Réinitialisation des valeurs necessaire a select
        FD_ZERO (&rfd);
        FD_SET (sockfd, &rfd);
        timeInterval.tv_sec = TIMEOUT; //Timeout a 5 secondes
        timeInterval.tv_usec = 0;
        if( select (sockfd+1, &rfd, NULL, NULL, &timeInterval) > 0 ) //Ecoute le socket jusqu au timeout, si > 0, il a recu quelque chose
        {
            if (recv (sockfd, &pkt_r, sizeof(pkt_r), 0) == -1) //Recuperation de l'information
            {
                perror("Rien recu ");
                close(sockfd);
                exit(-1);
            }
            
            if(inet_ntop(AF_INET, &(pkt_r.ip.ip_src), ip_retour, sizeof(argv[1])*2) == NULL) //Stock l'ip du routeur dans ip_retour en format texte
            {
                perror("inet_ntop merde ");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            printf("%d routeur : (%s)\n", ttl, ip_retour);
        }
        else
        {
            printf("%d Pas de reponse\n", ttl);
        }
        ttl++;
    }
    
//PING
    ttl = 64;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); //Met a jour le ttl sur le socket
    printf("Lancement de Ping, envois de %d paquet(s)\n", PING);
    for (i = 0; i < PING; i++)
    {
        pkt_s->id = pkt_s->id + htons(0x0001);
        pkt_s->seq_num = pkt_s->seq_num + htons(0x0001);
        pkt_s->checksum = 0;
        pkt_s->checksum = checksum((uint16_t*)&pkt_s, ICMP_HDR_LEN);
        if (sendto(sockfd, &pkt_s, sizeof(pkt_s), 0, (struct sockaddr*)&server, addrlen ) == -1) //Envoi le paquet ICMP
        {
            perror("Envoi échoué ");
            close(sockfd);
            exit(-1);
        }
        //Réinitialisation des valeurs necessaire a select
        FD_ZERO (&rfd);
        FD_SET (sockfd, &rfd);
        timeInterval.tv_sec = TIMEOUT; //Timeout a 5 secondes
        timeInterval.tv_usec = 0;
        if( select (sockfd+1, &rfd, NULL, NULL, &timeInterval) > 0 ) //Ecoute le socket jusqu au timeout, si > 0, il a recu quelque chose
        {
            if (recv (sockfd, &pkt_r, sizeof(pkt_r), 0) == -1) //Recuperation de l'information
             {
                 perror("Rien recu ");
                 close(sockfd);
                 exit(-1);
             }
            
            
            if(inet_ntop(AF_INET, &(pkt_r.ip.ip_src), ip_retour, sizeof(argv[1])*2) == NULL) //Stock l'ip du routeur dans ip_retour en format texte
            {
                perror("inet_ntop merde ");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            
            if(pkt_r.icmp.type == 0 &&
               pkt_r.icmp.id == pkt_s->id &&
               pkt_r.icmp.seq_num == pkt_s->seq_num)
            {
                temps = (1000000 - (long int)timeInterval.tv_usec) / 1000;
                if (TIMEOUT-1-timeInterval.tv_sec > 0)
                {
                    printf("%d time =  %ld sec et %ld ms\n", i+1, (long int)TIMEOUT-1-timeInterval.tv_sec, temps);
                    moyenne += 1000 * (long int)TIMEOUT-1-timeInterval.tv_sec;
                    moyenne += temps;
                }
                else
                {
                    printf("%d time = %ld ms\n", i+1, temps);
                    moyenne += temps;
                }
            }
        }
        else
        {
            paquet_perdu++;
        }
    }
    
    printf("Moyenne : %ld ms\nNombre de Paquet perdu : %d\n", moyenne/i, paquet_perdu);
    
    close(sockfd);
    return 0;
}
