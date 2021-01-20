#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include "Server.h"
#include "Question/Question.h"
#define MIN_LENGHT_OF_A_QUESTION 24
#define PENDING_CONNECTION_QUEUE_LENGHT 10
#define MAX_PLAYER 10
#define TIME_FOR_BEGIN 60
#define TIME_FOR_NEXT_QUESTION 60
pthread_t tid1,tid2;
siginfo_t signalInfo;
sigset_t signal1,signal2;
int num_giocatori=0;
bool isStarted=0;
int domanda_attuale=-1;
int sec=0;
int fin_par=0;

char *getNextAnswer(int i, int num);

int strToInt(char* str){
    int i, intValue, result, posResult = 0;

    for (i = 1; str[i] >= '0' && str[i] <= '9'; ++i){
        intValue = str[i] - '0';
        posResult = posResult * 10 + intValue;
        result = posResult * (-1);
    }

    return result;
}
int toInt(char* str){
    int i;int dim = strlen(str);
    char str2[dim];
    int j = 0;
    int start = 0;
    for(i=0;i<dim;i++){
        if(str[i]=='-'){
            j = 1;
        }
        if(j == 1){
            str2[start] = str[i];
            start+=1;
        }
    }
    return j == 1 ? strToInt(str2) : atoi(str);
}
void print_signal(int id_sig){
    //fprintf(stdout,"ID_SIG:%d",id_sig);
    //fflush(stdout);
}
void* timer(){
    do{
        sleep(1);
        sec+=1;
    }while(sec<TIME_FOR_BEGIN);
    return NULL;
}
void* checkLimitTime(pthread_t tid){
    sleep(TIME_FOR_BEGIN-1);
    pthread_cancel(tid);
    isStarted=1;
    while(1){
        sec=0;
        domanda_attuale+=1;
        pthread_create(&tid,NULL,timer,NULL);
        sleep(TIME_FOR_NEXT_QUESTION);
        pthread_cancel(tid);
    }
    return NULL;
}

char* str_replace(char* search, char* replace, char* subject) {
    int i, j, k;

    int searchSize = strlen(search);
    int replaceSize = strlen(replace);
    int size = strlen(subject);

    char* ret;

    if (!searchSize) {
        ret = malloc(size + 1);
        for (i = 0; i <= size; i++) {
            ret[i] = subject[i];
        }
        return ret;
    }

    int retAllocSize = (strlen(subject) + 1) * 2; // Allocation size of the return string.
    // let the allocation size be twice as that of the subject initially
    ret = malloc(retAllocSize);

    int bufferSize = 0; // Found characters buffer counter
    char* foundBuffer = malloc(searchSize); // Found character bugger

    for (i = 0, j = 0; i <= size; i++) {
        /**
         * Double the size of the allocated space if it's possible for us to surpass it
         **/
        if (retAllocSize <= j + replaceSize) {
            retAllocSize *= 2;
            ret = (char*) realloc(ret, retAllocSize);
        }
            /**
             * If there is a hit in characters of the substring, let's add it to the
             * character buffer
             **/
        else if (subject[i] == search[bufferSize]) {
            foundBuffer[bufferSize] = subject[i];
            bufferSize++;

            /**
             * If the found character's bugger's counter has reached the searched substring's
             * length, then there's a hit. Let's copy the replace substring's characters
             * onto the return string.
             **/
            if (bufferSize == searchSize) {
                bufferSize = 0;
                for (k = 0; k < replaceSize; k++) {
                    ret[j++] = replace[k];
                }
            }
        }
            /**
             * If the character is a miss, let's put everything back from the buffer
             * to the return string, and set the found character buffer counter to 0.
             **/
        else {
            for (k = 0; k < bufferSize; k++) {
                ret[j++] = foundBuffer[k];
            }
            bufferSize = 0;
            /**
             * Add the current character in the subject string to the return string.
             **/
            ret[j++] = subject[i];
        }
    }

    /**
     * Free memory
     **/
    free(foundBuffer);

    return ret;
}
int num_character(int numero){
    int count=0;
    if(numero<0)count++;
    else if(numero==0)return 1;
    while (numero != 0) {
        numero /= 10;
        ++count;
    }
    return count;
}
char* int_Str(int num){
    int count_riga=num_character(num);
    char* stringa=malloc(sizeof(char)*(count_riga+1));
    sprintf(stringa,"%d",num);
    return stringa;
}
char* addZero10(char* string){
    if(strlen(string)<9){
        int i; int diff;
        int MAX_LEN = 9; int size = strlen(string);
        char* converted = malloc(sizeof(char)*10);

        diff = MAX_LEN - size ;
        for(i = 0; i<diff;i++){
            converted[i]='0';
        }
        converted[i] = '\0';
        strcat(converted,string);

        return converted;
    }
    return NULL;
}
char* addZero5(char* string){
    if(strlen(string)<4){
        int i; int diff;
        int MAX_LEN = 4; int size = strlen(string);
        char* converted = malloc(sizeof(char)*5);

        diff = MAX_LEN - size ;

        for(i = 0; i<diff;i++){
            converted[i]='0';
        }
        converted[i] = '\0';
        strcat(converted,string);
        return converted;
    }
    return NULL;
}

void writeToPipe(char** argv,int link,int n){
    int* lunghezza_p=malloc(sizeof(int)*n);
    for(int i=0;i<n;i++)lunghezza_p[i]=(strlen(argv[i]))+1;

    for(int i=0;i<n;i++)write(link,&lunghezza_p[i],sizeof(int));
    for(int i=0;i<n;i++)write(link,argv[i],lunghezza_p[i]);
    free(lunghezza_p);
}
char **recFromPipe(int link,int n) {
    int* lunghezza=malloc(sizeof(int)*n);
    char *buff = malloc(sizeof(char*));
    char** argom=malloc(sizeof(char*)*(n+1));

    for(int i=0;i<n;i++) {
        read(link, &lunghezza[i], sizeof(int));
    }

    for(int i=0;i<n;i++) {
        buff = realloc(buff, lunghezza[i]);
        read(link, buff, lunghezza[i]);
        argom[i] = strdup(buff);
    }
    argom[n]=NULL;
    return argom;
}


int getScriptCommandAndExitStatus(char**argv,int n){
    //argv{nome_comando,arg_1,arg_2....,NULL}
    int link[2];
    pid_t pid;

    char* foo=malloc(sizeof(char)*4096); //Limiter of output bytes

    if (pipe(link)==-1) {
        perror("Non sono uscito a creare la pipe:");
        return NULL;
    }
    if ((pid = fork()) == -1) {
        perror("Non sono riuscito a fare la fork:");
        return NULL;
    }
    if(pid == 0) {
        char** argom=recFromPipe(link[0],n);
        close(link[0]);
        close(link[1]);
        execvp(argom[0],argom);
        perror("Errore nell'execvp:");
        return NULL;

    } else {
        writeToPipe(argv,link[1],n);
        close(link[1]);
        close(link[0]);
        int status;
        waitpid(pid,&status,0);

        if(WIFEXITED(status)){
            const int es=WEXITSTATUS(status);
            return es;
        }
        return -1;
    }

}
char* getScriptCommand2(char**argv1,char**argv2,int n1, int n2){
    //argv{nome_comando,arg_1,arg_2....,NULL}
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    signal(SIGUSR1,print_signal);
    int link[2];
    int pipee[2];
    pid_t pid;

    if (pipe(link)==-1 || pipe(pipee)==-1) {
        perror("Non sono uscito a creare la pipe:");
        return NULL;
    }

    if ((pid = fork()) == -1) {
        perror("Non sono riuscito a fare la fork:");
        return NULL;
    }
    if(pid == 0) {
        //Primo figlio
        char** argom1=recFromPipe(link[0],n1);

        dup2 (pipee[1], STDOUT_FILENO);
        close(pipee[0]);
        close(pipee[1]);

        execvp(argom1[0],argom1);
        perror("Errore nell'execvp:");
        return NULL;

    } else {
        int pid2=fork();
        if(pid2==0){
            //Secondo figlio
            int sig;
            sigwait(&sigset,&sig);
            char** argom2=recFromPipe(link[0],n2);
            dup2 (pipee[0], STDIN_FILENO);
            dup2 (link[1], STDOUT_FILENO);
            close(pipee[1]);
            close(pipee[0]);

            execvp(argom2[0],argom2);
            perror("Errore nell'execvp:");
            return NULL;
        }
        else{
            //Padre
            char* foo=malloc(sizeof(char)*4086);
            //SERVONO SOLO PER FAR PASSARE ROBBA TRAI FRATELLI
            close(pipee[1]);
            close(pipee[0]);
            //Padre
            int sig;
            writeToPipe(argv1, link[1], n1);
            waitpid(pid, &sig, NULL);
            kill(pid2,SIGUSR1);
            writeToPipe(argv2, link[1], n2);
            waitpid(pid2, &sig, NULL);
            int nbytes = read(link[0], foo,4086);
            foo[nbytes]='\0';
            return foo;
        }
    }
}
char* getScriptCommand3(char**argv1,char**argv2,char**argv3,int n1,int n2, int n3){
    //argv{nome_comando,arg_1,arg_2....,NULL}
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    signal(SIGUSR1,print_signal);

    int link[2];
    int pipee[2];
    pid_t pid;

    if (pipe(link)==-1 || pipe(pipee)==-1) {
        perror("Non sono uscito a creare la pipe:");
        return NULL;
    }

    if ((pid = fork()) == -1) {
        perror("Non sono riuscito a fare la fork:");
        return NULL;
    }
    if(pid == 0) {
        //Primo figlio
        char** argom1=recFromPipe(link[0],n1);

        dup2 (pipee[1], STDOUT_FILENO);
        close(pipee[0]);
        close(pipee[1]);

        execvp(argom1[0],argom1);
        perror("Errore nell'execvp:");

    } else {
        int pid2=fork();
        if(pid2==0){
            //Secondo figlio
            int sig;
            sigwait(&sigset,&sig);
            char** argom2=recFromPipe(link[0],n2);
            dup2 (pipee[0], STDIN_FILENO);
            dup2 (pipee[1], STDOUT_FILENO);
            close(pipee[1]);
            close(pipee[0]);

            execvp(argom2[0],argom2);
            perror("Errore nell'execvp:");
        }
        else{
            int pid3=fork();
            if(pid3==0){
                //Terzo Figlio
                int sig;
                sigwait(&sigset,&sig);
                char** argom3=recFromPipe(link[0],n3);
                dup2 (pipee[0], STDIN_FILENO);
                dup2 (link[1], STDOUT_FILENO);
                close(pipee[0]);
                close(pipee[1]);

                execvp(argom3[0],argom3);
                perror("Errore nell'execvp:");
            }
            else {
                char* foo=malloc(sizeof(char)*4086); //Limiter of output bytes
                //SERVONO SOLO PER FAR PASSARE ROBBA TRAI FRATELLI
                close(pipee[1]);
                close(pipee[0]);
                //Padre
                int sig;
                writeToPipe(argv1, link[1], n1);
                waitpid(pid, &sig, NULL);
                kill(pid2,SIGUSR1);
                writeToPipe(argv2, link[1], n2);
                waitpid(pid2, &sig, NULL);
                kill(pid3,SIGUSR1);
                writeToPipe(argv3, link[1], n3);
                waitpid(pid3, &sig, NULL);
                int nbytes = read(link[0], foo, 100);
                foo[nbytes-1]='\0';
                return foo;
            }
        }
    }
}

//OBSOLETE
void double_to_char(double f,char * buffer){
    gcvt(f,10,buffer);
}
void writeIntoFile(int fd,Question* domanda){
    //Conversione int top string da dare al file
    int count_num=num_character(domanda->num);
    char* str_num=malloc(sizeof(char) * (count_num + 1));
    int count_1=num_character(domanda->N1);
    char* str_1=malloc(sizeof(char)*(count_1+1));
    int count_2=num_character(domanda->N2);
    char* str_2=malloc(sizeof(char)*(count_2+1));
    int count_3=num_character(domanda->N3);
    char* str_3=malloc(sizeof(char)*(count_3+1));
    int count_4=num_character(domanda->N4);
    char* str_4=malloc(sizeof(char)*(count_4+1));
    sprintf(str_num, "%d", domanda->num);
    sprintf(str_1,"%d",domanda->N1);
    sprintf(str_2,"%d",domanda->N2);
    sprintf(str_3,"%d",domanda->N3);
    sprintf(str_4,"%d",domanda->N4);

    //Prima riga
    write(fd,"\n",sizeof(char));
    write(fd, str_num, sizeof(char)*count_num);
    write(fd,".",sizeof(char));
    write(fd,domanda->domanda,strlen(domanda->domanda)-1);
    write(fd,":",sizeof(char));
    write(fd,"\n",sizeof(char));

    //Seconda riga (1° risposta)
    write(fd,"1)",sizeof(char)*2);
    write(fd,str_1,sizeof(char)*count_1);
    if(domanda->N_OK==1)write(fd,"OK",sizeof(char)*2);
    write(fd,"\n",sizeof(char));

    //Terza riga (2° risposta)
    write(fd,"2)",sizeof(char)*2);
    write(fd,str_2,sizeof(char)*count_2);
    if(domanda->N_OK==2)write(fd,"OK",sizeof(char)*2);
    write(fd,"\n",sizeof(char));

    //Quarta riga (3° risposta)
    write(fd,"3)",sizeof(char)*2);
    write(fd,str_3,sizeof(char)*count_3);
    if(domanda->N_OK==3)write(fd,"OK",sizeof(char)*2);
    write(fd,"\n",sizeof(char));

    //Quinta riga (4° risposta)
    write(fd,"4)",sizeof(char)*2);
    write(fd,str_4,sizeof(char)*count_4);
    if(domanda->N_OK==4)write(fd,"OK",sizeof(char)*2);
    write(fd,"\n",sizeof(char));

    //free str_*
    free(str_num);
    free(str_1);
    free(str_2);
    free(str_3);
    free(str_4);
}
//OBSOLETE

int lastID(){
    int riga_cur=2;
    char* stringa_riga_cur;
    int num_cur=0;
    char* string_num_cur;

    char** argv1=malloc(sizeof(char*)*4);
    argv1[0]="sed";
    argv1[1]="$1s/\\([0-9]*\\)\\..*/\\1/g";
    argv1[2]="domande.txt";
    argv1[3]=NULL;
    char** argv2=malloc(sizeof(char*)*3);
    argv2[0]="head";
    argv2[1]="-$1";
    argv2[2]=NULL;
    char** argv3=malloc(sizeof(char*)*3);
    argv3[0]="tail";
    argv3[1]="-1";
    argv3[2]=NULL;
    char** argv4=malloc(sizeof(char*)*3);
    argv4[0]="cat";
    argv4[1]="domande.txt";
    argv4[2]=NULL;
    char** argv5=malloc(sizeof(char*)*3);
    argv5[0]="wc";
    argv5[1]="-l";
    argv5[2]=NULL;
    int ris=1;
    char* temp1=argv1[1];
    char* temp2=argv2[1];
    char* stringa_max_riga=getScriptCommand2(argv4,argv5,2,2);
    int max_riga=atoi(stringa_max_riga);
    free(stringa_max_riga);
    while(max_riga>riga_cur) {
        stringa_riga_cur = int_Str(riga_cur);
        argv1[1] = str_replace("$1", stringa_riga_cur, argv1[1]);
        argv2[1] = str_replace("$1", stringa_riga_cur, argv2[1]);
        string_num_cur=getScriptCommand3(argv1, argv2, argv3, 3, 2, 2);
        num_cur = atoi(string_num_cur);
        ris++;
        riga_cur+=6;
        free(argv1[1]);
        free(argv2[1]);
        free(stringa_riga_cur);
        free(string_num_cur);
        argv1[1]=temp1;
        argv2[1]=temp2;
    }
    free(argv1);
    free(argv2);
    free(argv3);
    free(argv4);
    free(argv5);

    return ris;
}
char* nextQue(int num){
    //Dato come input L'id corrente, restituisce la domanda (string) con l'id successivo.
    int next_record=(2+(6*(num-1)))+6;
    char** argv1=malloc(sizeof(char*)*7);
    argv1[0]="sed";
    argv1[1]="-e";
    argv1[2]="$1s/.\\.//g";
    argv1[3]="-e";
    argv1[4]="$1s/:$//";
    argv1[5]="domande.txt";
    argv1[6]=NULL;
    char** argv2=malloc(sizeof(char*)*4);
    argv2[0]="head";
    argv2[1]="-$1";
    argv2[2]=NULL;
    char** argv3=malloc(sizeof(char*)*3);
    argv3[0]="tail";
    argv3[1]="-1";
    argv3[2]=NULL;

    argv1[2] = str_replace("$1", int_Str(next_record), argv1[2]);
    argv1[4] = str_replace("$1", int_Str(next_record), argv1[4]);
    argv2[1] = str_replace("$1", int_Str(next_record), argv2[1]);

    return getScriptCommand3(argv1,argv2,argv3,6,2,2);
}

char *getNextAnswer(int i, int num) {
    //Dato come input L'id corrente e il numero della risposta, restituisce la risposta dell'id successivo
    int next_record=(2+(6*(num-1)))+(6+i);
    char** argv1=malloc(sizeof(char*)*4);
    argv1[0]="sed";
    argv1[1]="$1s/.*)//";
    argv1[2]="domande.txt";
    argv1[3]=NULL;
    char** argv2=malloc(sizeof(char*)*3);
    argv2[0]="head";
    argv2[1]="-$1";
    argv2[2]=NULL;
    char** argv3=malloc(sizeof(char*)*3);
    argv3[0]="tail";
    argv3[1]="-1";
    argv3[2]=NULL;

    argv1[1] = str_replace("$1", int_Str(next_record), argv1[1]);
    argv2[1] = str_replace("$1", int_Str(next_record), argv2[1]);

    return getScriptCommand3(argv1,argv2,argv3,3,2,2);
}

Question* getNextQuestion(int num){
        //La domanda è posta correttamente
        //L'applicativo dà per scontato che le domande inserite nel file siano inserite correttamente.
        Question* next_question=createQuestion();
        next_question->num=num+1;
        strcpy(next_question->domanda,nextQue(num));
        char* answer=getNextAnswer(1,num);
        char* corretta=strstr(answer,"OK");
        if(corretta!=NULL){
            next_question->N_OK=1;
            answer[strlen(answer)-2]=NULL;
        }
        next_question->N1=atoi(answer);
        answer=getNextAnswer(2,num);
        corretta=strstr(answer,"OK");
        if(corretta!=NULL){
            next_question->N_OK=2;
            answer[strlen(answer)-2]=NULL;
        }
        next_question->N2=atoi(answer);
        answer=getNextAnswer(3,num);
        corretta=strstr(answer,"OK");
        if(corretta!=NULL){
            next_question->N_OK=3;
            answer[strlen(answer)-2]=NULL;
        }
        next_question->N3=atoi(answer);
        answer=getNextAnswer(4,num);
        corretta=strstr(answer,"OK");
        if(corretta!=NULL){
            next_question->N_OK=4;
            answer[strlen(answer)-2]=NULL;
        }
        next_question->N4=atoi(answer);

    return next_question;
}
int setupConn(){
    struct sockaddr_in andress;
    andress.sin_family=AF_INET;
    andress.sin_port=htons(20000);
    andress.sin_addr.s_addr=htonl(INADDR_ANY);
    int sd;
    if((sd=socket(PF_INET,SOCK_STREAM,0))<0){
        fprintf(stderr,"Si e' verificato un'errore nella creazione della socket, riprova tra qualche secondo: %s \n ",strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(bind(sd,(struct sockaddr*)&andress,sizeof(andress)) <0){
        fprintf(stderr,"Si e' verificato un'errore nella creazione della socket, riprova tra qualche secondo: %s \n ",strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(listen(sd,PENDING_CONNECTION_QUEUE_LENGHT)<0){
        fprintf(stderr,"Si e' verificato un'errore nella creazione della socket, riprova tra qualche secondo: %s \n ",strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sd;
}
char* getIPAndress(int client_sd) {
    //[...]
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(client_sd, (struct sockaddr *)&addr, &addr_size);
    char *clientip = malloc(sizeof(char)*20);
    strcpy(clientip, inet_ntoa(addr.sin_addr));
    return clientip;
    //[...]
}

void writeAnswer(int client_sd){
    char* risposta=malloc(sizeof(char*));
    char* tempo=malloc(sizeof(char*));
    char* eptr;
    char* num=malloc(sizeof(char*)*6);
    char** argv1=malloc(sizeof(char**)+sizeof(char*)*5);
    char** argv2=malloc(sizeof(char**)+sizeof(char*)*5);
    char** argv3=malloc(sizeof(char**)+sizeof(char*)*3);
    char** argv4=malloc(sizeof(char**)+sizeof(char*)*3);
    char** argv5=malloc(sizeof(char**)+sizeof(char*)*3);
    char** argv6=malloc(sizeof(char**)+sizeof(char*)*5);
    char** argv7=malloc(sizeof(char**)+sizeof(char*)*5);
    char* IP_client=getIPAndress(client_sd);
    argv1[0]="grep";
    argv1[1]="-Fq";
    argv1[2]=IP_client;
    argv1[3]="classifica.txt";
    argv1[4]=NULL;
    //NUmero domande risposte
    argv2[0]="sed";
    argv2[1]="-n";
    argv2[2]="s/$1(//p";
    argv2[3]="classifica.txt";
    argv2[4]=NULL;
    argv3[0]="sed";
    argv3[1]="s/).*$//g";
    argv3[2]=NULL;
    //una riga
    argv4[0]="tail";
    argv4[1]="-1";
    argv4[2]=NULL;
    //Punteggio medio n-1
    argv5[0]="sed";
    argv5[1]="s/.*=//g";
    argv5[2]=NULL;
    //replace n
    argv6[0]="sed";
    argv6[1]="-i";
    argv6[2]="s/$1([0-9]*)/$1($2)/g";
    argv6[3]="classifica.txt";
    argv6[4]=NULL;
    //replace medio n
    argv7[0]="sed";
    argv7[1]="-i";
    argv7[2]="s/\\($1([0-9]*)\\)\\(=.*$\\)/\\1=$2/";
    argv7[3]="classifica.txt";
    argv7[4]=NULL;

    read(client_sd,risposta,2);
    read(client_sd,tempo,3);
    read(client_sd,num,5);
    //TEST
    //risposta="1";
    //tempo="03";
    //num="00001";
    int vnum=toInt(num);
    int tempo_in=toInt(tempo);

    Question* domanda=getNextQuestion(vnum-1);
    printQuestion(domanda);
    if(domanda->N_OK==atoi(risposta)){
        int fd;
        fd=open("classifica.txt",O_RDWR|O_CREAT|O_APPEND,S_IRWXU);
        int presente=getScriptCommandAndExitStatus(argv1,4);

        if(presente==0){
            char* temp=malloc(sizeof(char)*16);
            temp=strcpy(temp,IP_client);
            char media_s[10];
            argv2[2] = str_replace("$1",strcat(temp,"("), argv2[2]);
            int n=atoi(getScriptCommand2(argv2,argv3,4,2));
            double media_n=strtod(getScriptCommand2(argv2,argv5,4,2),&eptr);
            n=n+1;
            media_n=((media_n*(n-1))+tempo_in)/n;
            sprintf(media_s,"%.5f",media_n);

            argv6[2] = str_replace("$1", IP_client, argv6[2]);
            argv6[2] = str_replace("$2", int_Str(n), argv6[2]);
            argv7[2] = str_replace("$1", IP_client, argv7[2]);
            argv7[2]=str_replace("$2", media_s, argv7[2]);
            if(getScriptCommandAndExitStatus(argv6,4)!=0) perror("get n error:");
            if(getScriptCommandAndExitStatus(argv7,4)!=0) perror("set med error:");
        }
        else if(presente==1){
            lseek(fd,0,SEEK_END);
            write(fd,IP_client,sizeof(char)*strlen(IP_client));
            write(fd,"(1)=",4);
            write(fd,int_Str(tempo_in),sizeof(char)*strlen(int_Str(tempo_in)));//strlen(tempo)
        }
        else perror("erroree");
        close(fd);
    }
    free(argv1);
    free(argv2);
    free(argv3);
    free(argv4);
    free(argv5);
    free(argv6);
    free(argv7);
    free(risposta);
    free(tempo);
    free(num);
}

void sendScore(int client_sd){
    char** argv1=malloc(sizeof(char**)+sizeof(char*)*2);
    char** argv2=malloc(sizeof(char**)+sizeof(char*)*3);
    argv1[0]="echo";
    argv1[1]=NULL;

    argv2[0]="cat";
    argv2[1]="classifica.txt";
    argv2[2]=NULL;


    char* classifica=getScriptCommand2(argv1,argv2,1,2);
    char* temp=int_Str(strlen(classifica));
    char* num_char=addZero10(temp);
    write(client_sd,num_char,10);
    write(client_sd,classifica,strlen(classifica));

    free(argv2);
    free(argv1);
}
void writeDomanda(int client_sd,Question* domanda){
    //Protoccolo di comunicazione di una domanda:
    //Passare sequenzialmente i singoli campi della struttura. Ordine:
    //N1,N2,N3,N4,N_OK,num,lunghezza di domanda,domanda
    char* N1_zero=addZero10(int_Str(domanda->N1));
    char* N2_zero=addZero10(int_Str(domanda->N2));
    char* N3_zero=addZero10(int_Str(domanda->N3));
    char* N4_zero=addZero10(int_Str(domanda->N4));
    char* Num_zero=addZero5(int_Str(domanda->num));
    char* Ldomanda=addZero5(int_Str(strlen(domanda->domanda)+1));
    write(client_sd,N1_zero,10);
    write(client_sd,N2_zero,10);
    write(client_sd,N3_zero,10);
    write(client_sd,N4_zero,10);
    write(client_sd,int_Str(domanda->N_OK),2);
    write(client_sd,Num_zero,5);
    write(client_sd,Ldomanda,5);
    write(client_sd,domanda->domanda,strlen(domanda->domanda)+1);
}
void gestisciConnessione(int sd,int num_domande){
    int client_sd,err;
    Question* domanda=createQuestion();
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);
    char* fin_partita=malloc(sizeof(char));
    char* ultima_azione=malloc(sizeof(char));
    char* tmp=malloc(sizeof(char)*2);

    while(1){
        //Verifica di fine partita
        if(fin_par==num_giocatori && num_giocatori!=0 && tmp[0]=='c'){
        fprintf(stdout,"\n Partita finita! \n");
        fflush(stdout);
        break;
        }
        //In attesa di una richiesta da parte di un client
        if((client_sd=accept(sd,(struct sockaddr* )&client_addr,(socklen_t*)&client_len))<0) {
            fprintf(stderr,"Si e' verificato un'errore nella creazione della socket, riprova tra qualche secondo: %s \n ",strerror(errno));
            exit(EXIT_FAILURE);
        }
        int pid;
        int fd[2];
        char* buff;

        pipe(fd);
        if((pid=fork())<0){
            fprintf(stderr,"errore nella fork:",strerror(errno));
        }
        else if(pid==0) {
            //figlio
            close(sd);
            close(fd[0]);
            write(client_sd,"1",2);
            char* azione=malloc(sizeof(char*));
            read(client_sd,azione,2);
            write(fd[1],azione,2);
            if(strcmp(azione,"n")==0){
                if(domanda_attuale>=num_domande){
                    write(client_sd,"0",2);
                    write(fd[1],"3",2);
                    exit(EXIT_SUCCESS);
                }
                else write(client_sd,"1",2);
                domanda=getNextQuestion(domanda_attuale);
                writeDomanda(client_sd,domanda);
            }
            else if (strcmp(azione,"a")==0)
            {
                if(domanda_attuale>=num_domande)write(client_sd,"0",2);
                else write(client_sd,"1",2);
                writeAnswer(client_sd);
            }
            else if(strcmp(azione,"c")==0)
            {
                //if(Se non posso gestire la richiesta)write(client_sd,"0",2)
                //else
                write(client_sd,"1",2);

                sendScore(client_sd);
            }
            else if(strcmp(azione,"s")==0)
            {
                //if(Se non posso gestire la richiesta)write(client_sd,"0",2)
                //else
                write(client_sd,"1",2);
                if(!isStarted) {
                    if(num_giocatori==0){
                        write(fd[1],"2",2);
                    }
                }
                write(client_sd,int_Str(sec),3);
            }
            close(client_sd);
            close(fd[1]);
            shutdown(client_sd,SHUT_RDWR);
            exit(EXIT_SUCCESS);
        }
        close(fd[1]);
        read(fd[0],tmp,2);
        if(tmp[0]=='s')
        {
            num_giocatori+=1;
            read(fd[0], tmp, 2);
            if (tmp[0] == '2') {
                fprintf(stdout, "\n Il primo client si è connesso alla partita, la partità iniziera tra %d secondi!",TIME_FOR_BEGIN);
                fflush(stdout);
                err = pthread_create(&tid1, NULL, timer, NULL);
                if (err != 0)perror("Thread non creato correttamente");
                err = pthread_create(&tid2, NULL, checkLimitTime, tid1);
                if (err != 0)perror("Thread non creato correttamente");
                tmp[0]='\0';
            }
        }
        read(fd[0],tmp,2);
        if(tmp[0]=='3')fin_par+=1;
        close(client_sd);
        close(fd[0]);
    }
}
int main(void) {
    int sd;
    int num_domande;
    num_domande = lastID() - 1;
    sd = setupConn();
    fprintf(stdout,"Server avviato con successo, in attesa di un client...");
    fflush(stdout);
    gestisciConnessione(sd,num_domande);


}

