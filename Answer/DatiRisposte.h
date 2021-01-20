#ifndef RACCOLTA_RISPOSTE_H
#define RACCOLTA_RISPOSTE_H

#include <pthread.h>
#include <unistd.h>

pthread_t tid_askAnswer; pthread_t tid_checkLimitTime; clock_t clockT;
double t2; int num = -1;
char answer[2];char timeTaken[3];int idQ; int temp;
#endif //RACCOLTA_RISPOSTE_H    