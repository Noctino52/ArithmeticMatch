#ifndef RACCOLTA_QUESTION_H
#define RACCOLTA_QUESTION_H

#define MAX_QUESTION_LENGHT 100
typedef struct Question{
    int num;
    char* domanda;
    int N1;
    int N2;
    int N3;
    int N4;
    int N_OK;
}Question;

#include "Question.c"
Question*   createQuestion();
void        destroyQuestion(Question* domanda);
void        printQuestion(Question* domanda);
void        waitToStart(int attesa);
void        waitQuestion(int time,int attesa,int firstTime);
#endif //RACCOLTA_QUESTION_H
