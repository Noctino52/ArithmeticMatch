#ifndef RACCOLTA_CONNECTION_H
#define RACCOLTA_CONNECTION_H

#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "DatiServer.h"

#define true 1
#define false 0

#include "Connection.c"
int         setSocket();
void        setServAdress(struct sockaddr_in *srv_addr);
int         setConn(int sd,int *conn,struct sockaddr_in srv_addr);
int         syncro();
int         startConnection(int* firstTime);
void        closeConnection();

#endif //RACCOLTA_CONNECTION_H
