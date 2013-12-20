#include "probe.h"
#include "utils.h"
#include "traceroute.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define NB_PROBES 3 // pour traceroute

int main(int argc, char *argv[])
{
    int sockfd;
    int res;
    struct sockaddr_in destination; // hote cible
    struct probe_response probe;
    int i;

    if(argc != 2) {
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


    probe.status = PROBE_NO_RESPONSE;
    for (i=0; i < 5 && probe.status != PROBE_OK; i++)
        probe = probe_icmp(sockfd, &destination);

    if (probe.status == PROBE_OK)
        printf("%s atteint (essai %d) (%ld ms)\n", argv[1], i, probe.time);

    do_traceroute(sockfd, &destination, NB_PROBES);


    return 0;
}
