#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct{
    int id;
    int maiztasuna;
}TempArgs;

extern int sistemaAmaitu;
extern int TempCont;
extern int Done;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;
extern pthread_mutex_t mutexS;
extern pthread_cond_t condS;

void *erlojua(void *arg);
void *temporizadorea(void *arg);

