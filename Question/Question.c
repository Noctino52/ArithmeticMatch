#include "Question.h"

Question* createQuestion(){
    Question* domanda=malloc(sizeof(Question));
    domanda->domanda=malloc(sizeof(char)*(MAX_QUESTION_LENGHT+1));
    domanda->N1=-1;
    domanda->N2=-1;
    domanda->N3=-1;
    domanda->N4=-1;
    domanda->N_OK=-1;
    domanda->num=-1;
    return domanda;
}

void destroyQuestion(Question* domanda){
    free(domanda->domanda);
    free(domanda);
}

void printQuestion(Question* domanda){
    printf("\n --------------------------------------------------\n");
    printf("   Domanda N.%d : %s \n",domanda->num,domanda->domanda);
    printf("            1) %d \n",domanda->N1);
    printf("            2) %d \n",domanda->N2);
    printf("            3) %d \n",domanda->N3);
    printf("            4) %d \n",domanda->N4);
    printf("--------------------------------------------------\n");
}

void waitToStart(int attesa){
    int sec = 0;
    do{
        sleep(1);
        sec+=1;
        printf("\n%d",sec);
        fflush(stdout);
    } while (sec<attesa);
}

void waitQuestion(int time,int attesa,int firstTime){
    time = attesa - time;
    if(firstTime){
        printf("\nAttesa Inizio Partita: %d sec",time);
    }else{
        printf("\nAttesa prossima domanda: %d sec",time);
    }
    
    waitToStart(time);
}