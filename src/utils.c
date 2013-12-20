#include "utils.h"
#include "probe.h"

#include <arpa/inet.h> // inet_pton
#include <stdio.h> // perror
#include <stdlib.h> // malloc

#define TIMEOUT 5

int ip_to_sinaddr(char *ip, struct in_addr *saddr) {
    if(inet_pton(AF_INET, ip, (struct sockaddr *) saddr) != 1)
    {
        perror("inet_pton");
        return -1;

    }
    return 0;
}

int receive_packet(int sockfd, struct packet *to_packet,
                   struct timeval *interval) {
    fd_set rfd;
    int res;
    // Initialisation des valeurs necessaire a select
    FD_ZERO (&rfd);
    FD_SET (sockfd, &rfd);
    if (interval == NULL) {
        interval = malloc(sizeof(struct timeval));
        interval->tv_sec = TIMEOUT; //Timeout a 5 secondes
        interval->tv_usec = 0;
    }
    
    // Ecoute le socket jusqu au timeout, si > 0, il a recu quelque chose
    if((res = select (sockfd+1, &rfd, NULL, NULL, interval)) > 0)
    {
        //Recuperation du paquet
        if (recv (sockfd, to_packet, sizeof(struct packet), 0) == -1)
        {
            perror("Rien recu ");
            return -1;
        }
        return 1;
    }
    else if (res == 0)
        return 0;
    else {
        perror("select");
        return -1;
    }
}
