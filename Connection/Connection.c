#include "Connection.h"

int setSocket(int *sd){
    *sd = socket(AF_INET, SOCK_STREAM, 0);
    return (*sd != -1 )? true : false ;
}

void setServAdress(struct sockaddr_in *srv_addr){
    *srv_addr = (struct sockaddr_in){  
        .sin_addr.s_addr = inet_addr(IPANDRESS), //indirizzo server
	    .sin_family = AF_INET, //tipologia famiglia
	    .sin_port = htons(20000) //porta del server
    };
}

//connessione con il srv_addr specificato e la socket aperta 
int setConn(int sd,int *conn,struct sockaddr_in srv_addr){
    *conn =connect(sd,(struct sockaddr *) &srv_addr,sizeof(srv_addr));
    return (*conn == 0)? true : false;
}

//sincronizazzione con Server
int syncro(){
    char* ack = malloc(sizeof (char*));
    read(sd,ack,2);
    //printf(" |%s| ",ack);
    return ( strcmp(ack,"1") != 0)? false: true;
}

//Comunicazione con Server pronta
int startConnection(int* firstTime){
    int flag = false;

    if(*firstTime){
        printf("\nTentativo Connessione... ");
        fflush(stdout); 
    }

    setServAdress(&srv_addr); // seetting dati del Server

    //creazione socket
    if(!setSocket(&sd)){
        printf(" Socket non create ");
        return flag; //esci con flag -->false
    }

    //collegamento con Server
    if(setConn(sd,&conn,srv_addr)){
        if(*firstTime){
            printf("Connessione eseguita");
            printf("\nRicerca Partita... ");
        }
        
        flag = syncro(); //sincronizzazione con Server
        if(*firstTime){
            (flag) ? printf("Partita disponibile\n"): printf("Nessuna Partita in corso\n");    
        }
        
    }else{
        printf("Connessione fallita\n");
        fflush(stdout);
    }

    *firstTime = 0;
    return flag;
}

//ciusura socketDescriptor
void closeConnection(){
    close(sd);
}
