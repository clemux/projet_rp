#ifndef NETANALYZER_H
#define NETANALYZER_H

#include <stdio.h>

void end_analysis(void);

void handle_sigint(int sig);

void do_log(FILE *fp, char *msg, ...);

#endif
