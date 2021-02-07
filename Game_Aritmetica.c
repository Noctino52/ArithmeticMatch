#include "Game_Aritmetica.h"

void getClassifica(){
    char DimClassifica[10];
    int Dim;
    read(sd,DimClassifica,10);
    Dim = toInt(DimClassifica);
    char* Classifica = malloc(sizeof(char)*Dim);
    read(sd,Classifica,Dim);
    printf("%s",Classifica);
    printf("\n----------------------------\n");
}

void printAction(){
    printf("\n\n  Cosa vuoi fare ?\n");
    printf("   s)Start Game\n");
    printf("   c)visualizza classifica\n"); 
    printf("   x)Esci\n"); 
    printf("Tua azione:  ");
}

int checkAction(char action){
    return (action == START || action == CLASSIFICA || action == EXIT)? 1:0;
}

//AZIONI********
//MENU
void getAction(char* action){
    int flag = 0;
    do{
        printAction();
        fflush(stdin);
        scanf(" %c",action);
        flag = checkAction(*action);
    }while(flag == 0);
}

//START
void startGame(int* temp){
    write(sd,"s",2);
    if(syncro()){
        printf("\nSTARTED GAME");
        char *tempo = malloc(sizeof(char)*3);
        read(sd,tempo,3);
        int a = atoi(tempo);
        waitQuestion(a,WAIT_TIME,1);
    }
}

//NEXT
void doQuestion(int* idQ,char* azione,int temp,int* flag){
    write(sd,"n",2);
    if(syncro()){
        Question* Question = createQuestion();
        reciveQuestion(sd,Question);
        printQuestion(Question);
        *idQ = Question->num;   
        *azione = ANSWER;
    }else{
        printf("\n Partita Finita\n");
        *flag = 0;
        *azione = CLASSIFICA;
    }      
}

//ANSWER
void startAnswer(int idQ,int* temp){
    write(sd,"a",2); //asnwer
    if(syncro()){
        doAnswer(idQ,temp);
    }
}

//CLASSIFICA
void doClassifica(){
    printf("\n--------Classifica----------\n");
    write(sd,"c",2);
    if(syncro()){
        getClassifica();                  
    } 
}
//FINE AZIONI***

void Azione(char* azione,int* idQ,int* temp,int* flag){
    switch (*azione){
        case MENU :
            getAction(azione);
            break;
        case START :
            startGame(temp);
            *azione = NEXT;
            break;
        case NEXT :
            doQuestion(idQ, azione,*temp,flag);
            break;
        case ANSWER:
            startAnswer(*idQ, temp);
            *azione = NEXT;
            break;
        case CLASSIFICA :
            doClassifica();
            if(*flag) *azione = MENU;
            else *azione = EXIT;
            break;
    }
}

void setComunication(char action){
    int pid; 
    if((pid=fork())<0){
        perror("\nFork error\n");
        exit(EXIT_FAILURE);
    }

    if(pid != 0){
        //PADRE
        wait(NULL);
    }else{ 
        //FIGLIO
        int temp = 0;
        int flag = 1;
        int start = 1;
        int idQ;

        do{
            if(action=='a'){
                getAnswer();
            }
            if (startConnection(&start)) {
                Azione(&action, &idQ, &temp, &flag);
            }
            else {
                printf("Connessione fallita\n");
                action = EXIT;
            } 
            closeConnection();
        }while(action != EXIT);
        sleep(5);
    }   
}

