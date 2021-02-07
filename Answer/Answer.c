#include "Answer.h"
#include "DatiRisposte.h"

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
    char str2[dim]; int j = 0; int start = 0;
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

void conversione(int t2){
    if(t2<10) snprintf(timeTaken, 3, "0%d", t2);
    else snprintf(timeTaken, 3, "%d", t2);     
    snprintf(answer, 2, "%d", num); 
}

void reciveQuestion(int sd,Question* question){
    char* N1_zero = malloc(sizeof (char*));
    char* N2_zero = malloc(sizeof (char*));
    char* N3_zero = malloc(sizeof (char*));
    char* N4_zero = malloc(sizeof (char*));
    char* Num_zero = malloc(sizeof (char*));
    char* Ldomanda = malloc(sizeof (char*));
    char* N_OK = malloc(sizeof (char*));

    read(sd,N1_zero,10);
    //printf("N1_zero: %s\n",N1_zero);
    question->N1 = toInt(N1_zero);
    //printf("N1 zero: %d\n",question->N1);
    
    read(sd,N2_zero,10);
    //printf("N2_zero: %s\n",N2_zero);
    question->N2 = toInt(N2_zero);
    //printf("N2 zero: %d\n",question->N2);

    read(sd,N3_zero,10);
    //printf("N3_zero: %s\n",N3_zero);
    question->N3 = toInt(N3_zero);
    //printf("N3 zero: %d\n",question->N3);

    read(sd,N4_zero,10);
    //printf("N4_zero: %s\n",N4_zero);
    question->N4 = toInt(N4_zero);
    //printf("N4 zero: %d\n",question->N4);

    read(sd,N_OK,2);
    //printf("N_OK: %s\n",N_OK);
    question->N_OK = toInt(N_OK);
    //printf("N_OK: %d\n",question->N_OK);

    read(sd,Num_zero,5);
    //printf("IdD: %s\n",Num_zero);
    question->num = toInt(Num_zero);
    //printf("IdD: %d\n",question->num);

    read(sd,Ldomanda,5);
    int x = toInt(Ldomanda);
    //printf("Dimensione domanda: %d\n",x);

    read(sd,question->domanda,x);
    //printf("Domanda ricevuta: \n%s\n",question->domanda);
}

//THREAD 1 
void *askAnswer(){
    clockT = time(NULL);//tempo partito
    do{
        //printf("\n0 per uscire---");
        printf("\nRISPOSTA \n  Scrivi il numero della risposta(1-4):");  
        scanf("%d",&num);
    }while(num>4 || num<0);

    clockT = time(NULL)- clockT;//fine tempo
    pthread_cancel(tid_checkLimitTime);//chiusura checkLimitTime
}
//THREAD 2  
void *checkLimitTime (){
    sleep(WAIT_TIME);//tempo massimo per ogni domanda
    clockT = WAIT_TIME;//fine tempo
    pthread_cancel(tid_askAnswer);//chiusura askAnswer
}

void getAnswer(){
    int en; int tempo;
    if (en = pthread_create(&tid_askAnswer, NULL, askAnswer, NULL)!=0)
        errno = en , perror("create Thread") , exit(1);
    
    if (en = pthread_create(&tid_checkLimitTime, NULL, checkLimitTime, NULL)!=0)
        errno = en , perror("create Thread") , exit(1);

    pthread_join(tid_askAnswer,NULL);
    pthread_join(tid_checkLimitTime,NULL);
    tempo = (int) clockT;   
    conversione(tempo);
    temp = tempo;
}

void sendAnswer(){
    char idQuestion[5];
    snprintf(idQuestion, 4, "%d", idQ);
    fflush(stdout);
    strcpy(idQuestion,addZero5(idQuestion));

    write(sd,answer,2); //rispostaData
    write(sd,timeTaken,3); //tempo impiegato
    write(sd,idQuestion,5); //id Question
    printf("\nRisposta Inviata : %s  (tempo: %s sec)\n",answer,timeTaken);

    waitQuestion(temp,WAIT_TIME,0);
    fflush(stdout);
}

void doAnswer(int IdQ,int* tempo){
    idQ = IdQ;
    if(temp >= WAIT_TIME){
        strcpy(answer,"-");
    }
    sendAnswer();
    *tempo = temp;
}

