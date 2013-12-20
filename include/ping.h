#ifndef PING_H
#define PING_H

#include <netinet/in.h>


void do_ping(int sockfd, struct sockaddr_in *destination,
             unsigned int packet_count, unsigned int interval);

#endif
