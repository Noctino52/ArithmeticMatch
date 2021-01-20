#ifndef RACCOLTA_GAME_ARITMETICA_H
#define RACCOLTA_GAME_ARITMETICA_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "Connection/Connection.h"
#include "Answer/Answer.h"
#include "Question/Question.h"  

#define MENU 'm'
#define START 's'
#define NEXT 'n'
#define ANSWER 'a'
#define CLASSIFICA 'c'
#define EXIT 'x'

#include "Game_Aritmetica.c"
void        doQuestion(int* idQ,char* azione,int temp,int* flag);
void        startAnswer(int IdQ,int* temp);
void        startGame(int* temp);
void        getClassifica();
void        doClassifica();
void        Azione(char* azione,int* idQ,int* temp,int* flag);
void        setComunication(char azione);
void        printAction();
int         checkAction(char azione);
void        getAction(char* action);

#endif //RACCOLTA_GAME_ARITMETICA_H
