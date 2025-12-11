#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "scheduler.h"
#include "prozesuak.h"
#include "CPU.h"

void *scheduler(void *arg){
    printf("Scheduler-a exekutatzen hasi da\n");
    pthread_mutex_lock(&mutexS);
    while(1){
        // Scheduler-aren kondizio-aldagaia itxaron
        pthread_cond_wait(&condS, &mutexS);
        //Nodoko PCB emandako CPUko malloc-ean sartu
        PCB *sarrera = prozesuaPoll();
        if(sarrera->pid!=-1){
            pcbCPUariEsleitu(sarrera);
        }
        else{
            printf(" <-- Ez dago PCBrik ready ilaran.");
        }
        printf(" <-- Corea:%d; Haria:%d", azkenCorea, azkenHaria);
    }
    pthread_mutex_unlock(&mutexS);

    return NULL;
}

void schedulerdeitu(){
    pthread_mutex_lock(&mutexS);
    pthread_cond_signal(&condS);
    pthread_mutex_unlock(&mutexS);
}
