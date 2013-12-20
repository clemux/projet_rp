#ifndef PROBE_H
#define PROBE_H

#include "icmp.h"

#include <netinet/in.h>

#define TIMEOUT 5

#define PROBE_SEND_FAIL 2;
#define PROBE_NO_RESPONSE 1
#define PROBE_OK 0
#define INTEGRITY_OK 0
#define INTEGRITY_ERROR 1

#include "icmp.h"


struct probe_response {
    unsigned int status; // FIXME: enum
    long int time; // ^
    unsigned int ttl; // ttl du echo reply, si reçu
    unsigned int integrity_error; // 0 si reponse ok, sinon 1
    char *source_ip; // source de la réponse
    struct icmp_header icmp;
};

struct probe_response probe_icmp(int sockfd, struct sockaddr_in *destination);

#endif
