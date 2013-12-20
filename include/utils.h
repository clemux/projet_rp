/**
* \file utils.h
* \brief Contient les definition des fonctions necessaire a utils.c
* \author Arnaud Petit
* \author Clement Schreiner
*/

#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>
#include "packet.h"

int ip_to_sinaddr(char *ip, struct in_addr* saddr);
int receive_packet(int sockfd, struct packet *to_packet, struct timeval *interval);

#endif
