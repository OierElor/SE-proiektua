#include <stdio.h>
#include <stdlib.h>
#include "CPU.h"
#include <pthread.h>
#include "prozesuak.h"

CPU cpu;
int hariTotalak;
int azkenCorea;
int azkenHaria;

void cpuHasieratu(int ckop, int hkop){
    int i;
    int j;
    cpu.corekop = ckop;
    cpu.coreak = (Core*)malloc(ckop * sizeof(Core));
    if (cpu.coreak == NULL) {
        perror("Errorea malloc egitean Corentzat");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < ckop; i++){
        cpu.coreak[i].harikop = hkop;
        cpu.coreak[i].hariak = (haria*)malloc(hkop * sizeof(haria));
        for(j = 0; j < hkop; j++){
            cpu.coreak[i].hariak[j].libre=1;
        }
        if (cpu.coreak[i].hariak == NULL) {
            perror("Errorea malloc egitean Harientzat");
            for(j = 0; j < i; j++){
                free(cpu.coreak[j].hariak);
            }
            free(cpu.coreak);
            exit(EXIT_FAILURE);
        }
    }
}

//Dispacher
void pcbCPUariEsleitu(PCB *jasotakopcb){
    jasotakopcb->running=1;
    haria *esleitua = lortuHariAskea();
    if(esleitua->libre==0){
        printf(" <-- %d prozesua kanporatu da eta %d prozesua hasieratu da.", esleitua->pcb->pid, jasotakopcb->garrantzia);
        esleitua->pcb->running=0;
        prozesuaPush((esleitua->pcb));
    }
    else{
        printf(" <-- %d prozesua hasieratu da.", jasotakopcb->pid);
    }
    esleitua->pcb = jasotakopcb;
    esleitua->libre = 0;
}

haria* lortuHariAskea(){
    int i;
    int j;
    for(i = 0; i < cpu.corekop; i++){
        for(j = 0; j < cpu.coreak[i].harikop; j++){
            if(cpu.coreak[i].hariak[j].libre == 1){
                azkenHaria=j;
                azkenCorea=i;
                return &cpu.coreak[i].hariak[j];
            }
        }
    }
    if(azkenHaria<cpu.coreak[azkenCorea].harikop-1){
        azkenHaria++;
    }
    else{
        azkenHaria=0;
        if(azkenCorea<cpu.corekop-1){
            azkenCorea++;
        }
        else{
            azkenCorea=0;
        }
    }
    return &cpu.coreak[azkenCorea].hariak[azkenHaria];
}
