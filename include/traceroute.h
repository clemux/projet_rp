/**
* \file traceroute.h
* \brief Contient la definition de la fonction traceroute
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <netinet/in.h>

void do_traceroute(int sockfd, struct sockaddr_in *destination,
                   unsigned int nb_probes, unsigned int ttlmax);

#endif
