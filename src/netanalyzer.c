/**
* \file netanalyzer.c
* \brief contient le main ainsi que la structure permettant l'analyse statistiques
* \author Arnaud Petit
* \author Clement Schreiner
*/

#include "probe.h"
#include "utils.h"
#include "traceroute.h"
#include "netanalyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#define NB_PROBES 3 // pour traceroute
#define TTLMAX 25
#define INTERVAL 1

struct sigaction action;

/**
 * \struct statistics
 * \brief structure contenant les informations calculant les statistiques du programme
 */
struct statistics {
    unsigned int nb_probes; /** nombre de sonde envoyees */
    unsigned int nb_failed_probes; /** nombre de sondes perdues */
    unsigned int nb_route_changes; /** nombre de fois où la route a du etre recalculee */
};

struct statistics stats;

/**
 * \fn void end_analysis(void)
 * \brief affiche les statistiques de programme
 */
void end_analysis(void) {
    printf("----------------\n");
    printf("FIN DE L'ANALYSE\n");
    printf("----------------\n");

    printf("Nombre de sondes envoyées: %d\n", stats.nb_probes);
    printf("Nombre de sondes sans réponse: %d\n", stats.nb_failed_probes);
    printf("Taux d'erreur: %f %% \n",
           (float) ((float) stats.nb_failed_probes / (float) stats.nb_probes)*100);
    printf("La route vers l'hote a changé %d fois.\n", stats.nb_route_changes);
    exit(0);
}

/**
 * \fn void handle_sigint(int sig
 * \brief handler de la structure sigaction,
 * lance la fonction end_analysis
 */
void handle_sigint(int sig) {
    (void) sig; // ceci est complètement inutile, mais on nous demande de compiler
               // en -Werror... C'est absurde d'être obligé de faire ça.
    end_analysis();
}




int main(int argc, char *argv[])
{
    int sockfd;
    int res;
    struct sockaddr_in destination; // hote cible
    struct probe_response probe;
    unsigned int i, ttl, num_probe;

    action.sa_handler = handle_sigint;
    sigaction(SIGINT, &action, NULL);

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

    printf("Découverte des routes vers %s\n", argv[1]);
    do_traceroute(sockfd, &destination, NB_PROBES, TTLMAX);

    ttl = 0;
    stats.nb_probes = 0;
    stats.nb_failed_probes = 0;
    for(num_probe=0; ; num_probe++) {
        struct probe_response probe_response = probe_icmp(sockfd, &destination);
        if (probe_response.status != PROBE_OK) {
            fprintf(stderr, "Sonde %d non reçue.\n", num_probe);
            stats.nb_failed_probes++;
        }
        
        else {
            fprintf(stderr, "Sonde %d reçue. RTT: %ld ms\n", num_probe,
                    probe_response.time);
            stats.nb_probes++;
            if (ttl != 0 && probe_response.ttl != ttl) {
                fprintf(stderr, "Le TTL a changé: %d->%d. Découverte des routes :\n",
                        ttl, probe_response.ttl);
                stats.nb_route_changes++;
                do_traceroute(sockfd, &destination, NB_PROBES, TTLMAX);
                ttl = probe_response.ttl;
            }
        }
        sleep(INTERVAL);
    }

    return 0;
}
