#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "scheduler.h"
#include "prozesuak.h"
#include "dispatcher.h"
#include "memoria.h"
#include "CPU.h"
#include "erlojua.h"

void *scheduler(void *arg){
    printf("Scheduler-a exekutatzen hasi da\n");
    while(!sistemaAmaitu){
         pthread_mutex_lock(&mutexS);
        // Scheduler-aren kondizio-aldagaia itxaron
        pthread_cond_wait(&condS, &mutexS);
        //Nodoko PCB emandako CPUko malloc-ean sartu

        if(sistemaAmaitu) {
            pthread_mutex_unlock(&mutexS);
            break;
        }
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
                sistemaAmaitu = 1;
                for(int i=0; i<cpu.corekop*cpu.harikopCoreko; i++){
                    mmuEstadistikakErakutsi(cpu.hariakIlara[i]);
                }
                printf("\n===============================");
                printf("\n Programa denak exekutatu dira");
                printf("\n===============================");

                pthread_cond_broadcast(&cond2);
                pthread_cond_signal(&condS);

                pthread_mutex_unlock(&mutexS);

                memoriaLibratu();

                for(int i = 0; i < cpu.corekop; i++){
                    free(cpu.coreak[i].hariak);
                }
                free(cpu.coreak);
                free(cpu.hariakIlara);

                // Programa amaitu
                printf("\nSistema itzaltzen...\n");
                exit(0);
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
