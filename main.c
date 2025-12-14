#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "erlojua.h"
#include "CPU.h"
#include "prozesuak.h"
#include "scheduler.h"



int TempCont;
int Done;
pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condS = PTHREAD_COND_INITIALIZER;
//Main Funtzioa

int main(){
    //Prosezuen ilara hasieratzeko
    ready.lehena = NULL;
    ready.azkena = NULL;
    //cpu-hasieratu
    int harikop = 2;
    int corekop = 2;
    azkenHaria=0;
    cpuHasieratu(corekop, harikop);
    printf("Hari kopurua: %d \n", cpu.harikopCoreko);
    printf("Core kopurua: %d \n", cpu.corekop);
    Done = 0;
    //Prozesuak sortu
    for(int i=0; i<10; i++){
         prozesuakSortu(i);
    }

    //Temporizadore kantitatea ezaretzeko
    TempCont = 3;
    //Erlojua hasieratu
    pthread_t clock;
    //Scheduler hasieratu
    pthread_t sched;

    pthread_create(&sched, NULL, scheduler, NULL);
    pthread_create(&clock, NULL, erlojua, NULL);
    pthread_t *threads = calloc(TempCont, sizeof(pthread_t));
    TempArgs *args = calloc(TempCont, sizeof(TempArgs));

    // Maiztasun desberdinak definitu
    int maiztasunak[] = {2, 3, 20};

    for(int i=0; i<TempCont; i++){
        args[i].id = i;
        args[i].maiztasuna = maiztasunak[i];
        pthread_create(&threads[i], NULL, temporizadorea, &args[i]);
    }

    pthread_exit(NULL);

    free(threads);
    free(args);

    return 0;
}
