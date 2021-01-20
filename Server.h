#include "Question/Question.h"
#include <stdbool.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>


void gestisciConnessione(int sd,int num_domande);
int setupConn();
int lastID();
