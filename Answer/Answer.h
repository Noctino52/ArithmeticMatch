#ifndef RACCOLTA_ANSWER_H
#define RACCOLTA_ANSWER_H

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#include "../Question/Question.h"

#define WAIT_TIME 60

#include "Answer.c"
int         strToInt(char* str);
int         toInt(char* string);
char*       addZero5(char* string);
void        conversione(int t2);
void        reciveQuestion(int sd,Question* question);
void        *askAnswer();
void        *checkLimitTime();
void        doAnswer(int IdQ,int* tempo);
void        getAnswer();
void        sendAnswer();

#endif //RACCOLTA_ANSWER_H
