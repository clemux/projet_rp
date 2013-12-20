#include "traceroute.h"
#include "probe.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop
#include <stdio.h>

#define TTLMAX 25
#define TIMEOUT 5

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
