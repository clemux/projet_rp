#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define TTLMAX 20


typedef struct icmp_header
{
	u_int8_t type;
	u_int8_t code;
	u_int16_t checksum;
	u_int16_t id;
	u_int16_t seq_num;	
}ICMP_H;

typedef struct ip_header
{
	u_int8_t version_hdl;
	u_int8_t service;
	u_int16_t length;
	u_int16_t id;
	u_int8_t flags;
	u_int8_t offset;
	u_int8_t ttl;
	u_int8_t protocol;
	u_int16_t checksum;
	u_int32_t ip_src;
	u_int32_t ip_dest;
}IP_H;

typedef struct packet
{
	IP_H ip;
	ICMP_H icmp;
	
}PACK;

//~ int ip_hex_to_dec(const char *input, char *output, size_t outlen)
//~ {
    //~ unsigned int a, b, c, d;
    //~ 
//~ 
    //~ if (sscanf(input, "%2x%2x%2x%2x", &a, &b, &c, &d) != 4)
        //~ return -1;
//~ 
    //~ snprintf(output, outlen, "%u.%u.%u.%u", a, b, c, d);
    //~ return 0;
//~ }

int main (int argc, char **argv)
{
	int sockfd, ttl;
	fd_set rfd;
	ICMP_H pkt_s;
	PACK pkt_r;
	char *ip_retour, *ip_retour_dns;
	socklen_t addrlen;
	struct sockaddr_in server, dns;
	struct timeval timeInterval;
	
	// check the number of args on command line
	if(argc != 2)
	{
		printf("USAGE: %s @adress\n", argv[0]);
		exit(-1);
	}
   
    
	// socket factory && try to connect
	if((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		perror("socket choucroute ");
		close(sockfd);
        exit(EXIT_FAILURE);
	}
	
	addrlen = sizeof(struct sockaddr_in);
	server.sin_family = AF_INET;
	
	if(inet_pton(AF_INET, argv[1], &server.sin_addr) != 1)
	{
		perror("inet_pton merde ");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
    
	//Preparation du paquet ICMP request
	pkt_s.type = 0x08;
	pkt_s.code = 0x00;
	pkt_s.checksum = htons(0x0000);
	pkt_s.id = htons(0x0001);
	pkt_s.seq_num = htons(0x0001);
    
	if (sendto(sockfd, &pkt_s, sizeof(pkt_s), 0, (struct sockaddr*)&server, addrlen ) == -1)
	{
		perror("Envoi échoué ");
		close(sockfd);
		exit(-1);
	}
	printf("Paquet envoyé\n");
	
	
	if(recv (sockfd, &pkt_r, sizeof(pkt_r), 0) == -1)
	{
		perror("Rien recu ");
		close(sockfd);
		exit(-1);
	}
	
	if((pkt_r.icmp.type == 0) && (pkt_r.icmp.code == 0) && (pkt_r.icmp.id == pkt_s.id) && (pkt_r.icmp.seq_num == pkt_s.seq_num))
		printf("Reponse recue OK, destination atteignable.\nChecksum = %x\n", pkt_r.icmp.checksum);
	else
	{
		perror("Reponse mauvaise, destination non atteignable");
		close(sockfd);
		exit(-1);
	}


	
	//TRACEROUTE
	pkt_r.icmp.type = 0x0B;
	ttl = 1;
	ip_retour = malloc(sizeof(uint32_t));
	ip_retour_dns = malloc(100*sizeof(char));
	printf("Lancement de traceroute vers %s, nombre de saut : %d\n", argv[1], TTLMAX);
	while (ttl < TTLMAX && pkt_r.icmp.type != 0)
	{
		setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); //Met a jour le ttl sur le socket
		if (sendto(sockfd, &pkt_s, sizeof(pkt_s), 0, (struct sockaddr*)&server, addrlen ) == -1) //Envoi le paquet ICMP
		{
			perror("Envoi échoué ");
			close(sockfd);
			exit(-1);
		}
		//Réinitialisation des valeurs necessaire a select
		FD_ZERO (&rfd);
		FD_SET (sockfd, &rfd);
		timeInterval.tv_sec = 5; //Timeout a 5 secondes
		timeInterval.tv_usec = 0;
		if( select (sockfd+1, &rfd, NULL, NULL, &timeInterval) > 0 ) //Ecoute le socket jusqu au timeout, si > 0, il a recu quelque chose
		{
			if(recv (sockfd, &pkt_r, sizeof(pkt_r), 0) == -1) //Recuperation de l'information
			{
				perror("Rien recu ");
				close(sockfd);
				exit(-1);
			}
			
			dns.sin_family = AF_INET;
			if(inet_ntop(AF_INET, &pkt_r.ip.ip_src, ip_retour, sizeof(argv[1])*2) == NULL)
			{
				perror("inet_ntop merde ");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
			inet_pton(AF_INET, ip_retour, &dns.sin_addr);
			getnameinfo((struct sockaddr*)&dns, sizeof(dns), ip_retour_dns, sizeof(ip_retour_dns), NULL, 0, 0);
			printf("%d routeur : %s (%s)\n", ttl, ip_retour_dns, ip_retour);
		}
		else
		{
			printf("%d Pas de reponse\n", ttl);
		}
		ttl++;
	}
	
	
	close(sockfd);
	return 0;
}


//~ char *getDomaine(char *ip) { 
	//~ char *domaine = malloc(sizeof(char) * NI_MAXHOST); 
	//~ struct sockaddr_in sa; 
	//~ sa.sin_family = AF_INET; 
	//~ inet_pton(AF_INET, ip, &sa.sin_addr); 
	//~ getnameinfo((struct sockaddr*)&sa, sizeof(sa), domaine, sizeof(char)*NI_MAXHOST, NULL, 0, 0); 
	//~ return domaine; 
//~ }
