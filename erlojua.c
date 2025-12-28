#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "erlojua.h"
#include "scheduler.h"
#include "gertaerak.h"
#include "CPU.h"
#include "prozesuak.h"
#include "exekuzioMotorra.h"

void *erlojua(void *arg){
    printf("Erlojua exekutatzen hasi da\n");
    while(1){
        pthread_mutex_lock(&mutex);
        while (Done < TempCont) {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("\nTick!");

        Done = 0;
        pthread_cond_broadcast(&cond2);
        pcbakGarbitu();
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *temporizadorea(void *arg){
    TempArgs *args = (TempArgs *)arg;
    printf("Temporizadorea %d exekutatzen hasi da (maiztasuna: %d )\n",
           args->id, args->maiztasuna);

    int i=0;
    pthread_mutex_lock(&mutex);
    while(1){
        Done++;
        i++;
        if(i==args->maiztasuna){
            i=0;
            //printf(" <--Temp %d", args->id);
            if(args->id==1){
                //Exekutatu prozesagailuko hari guztiak
                hariDenakExekutatu();
            }
            // Scheduler-ari seinalea bidali
            if(args->id == 0){
                schedulerdeitu();
            }
        }
        pthread_cond_signal(&cond);
        pthread_cond_wait(&cond2, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
