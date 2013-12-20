#include "icmp.h"
#include "ip.h"
#include "packet.h"
#include "dbg.h"
#include "utils.h"
#include "probe.h"
#include "traceroute.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop

#define TTLMAX 25
#define TIMEOUT 5

#define NB_PROBES 3


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in destination; // hote vers lequel on veut la liste des routes

    
    int res; // valeur de retour de ip_to_sinaddr
    
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

    printf("Lancement de traceroute vers %s, nombre de saut maximum : %d\n",
           argv[1], TTLMAX);
    do_traceroute(sockfd, &destination, NB_PROBES, TTLMAX);


    return 0;
}
