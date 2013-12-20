#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>
#include "packet.h"

int ip_to_sinaddr(char *ip, struct in_addr* saddr);
int receive_packet(int sockfd, struct packet *to_packet);

#endif
