#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "scheduler.h"
#include "prozesuak.h"
#include "dispatcher.h"
#include "CPU.h"

void *scheduler(void *arg){
    printf("Scheduler-a exekutatzen hasi da\n");
    while(1){
         pthread_mutex_lock(&mutexS);
        // Scheduler-aren kondizio-aldagaia itxaron
        pthread_cond_wait(&condS, &mutexS);
        //Nodoko PCB emandako CPUko malloc-ean sartu
        PCB *sarrera = prozesuaPoll();
        if(sarrera->pid!=-1){
            Dispatcher(sarrera);
        }
        else{
            printf(" <-- Ez dago PCBrik ready ilaran.");
            int amaitu=1;
            for(int i=0; i<(cpu.harikopCoreko*cpu.corekop); i++){
                if(cpu.hariakIlara[i]->libre==0){
                    amaitu=0;
                    break;
                }
            }
            if(amaitu){
                printf("\n========================================================================");
                printf("\nOrain 30 segundu duzu aurretik exekutatu diren aginduak guztiak ikusteko");
                printf("\n========================================================================");
                for(int i=0; i<cpu.corekop*cpu.harikopCoreko; i++){
                    mmuEstadistikakErakutsi(cpu.hariakIlara[i]);
                }
                sleep(30);
            }
        }
        pthread_mutex_unlock(&mutexS);
    }
    return NULL;
}

void schedulerdeitu(){
    pthread_mutex_lock(&mutexS);
    pthread_cond_signal(&condS);
    pthread_mutex_unlock(&mutexS);
}
