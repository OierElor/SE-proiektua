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
    hariTotalak = ckop * hkop;
    cpu.corekop = ckop;
    cpu.harikopCoreko = hkop;
    cpu.hariakIlara = (haria**)malloc(hariTotalak * sizeof(haria*));
    if (cpu.hariakIlara == NULL) {
        perror("Errorea malloc egitean hariakIlara-rentzat");
        exit(EXIT_FAILURE);
    }
    cpu.coreak = (Core*)malloc(ckop * sizeof(Core));
    if (cpu.coreak == NULL) {
        perror("Errorea malloc egitean Corentzat");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < ckop; i++){
        cpu.coreak[i].hariak = (haria*)malloc(hkop * sizeof(haria));
        for(j = 0; j < hkop; j++){
            cpu.coreak[i].hariak[j].libre=1;
            cpu.coreak[i].hariak[j].coreID=i;
            int indizea = i * hkop + j;
            if (indizea < hariTotalak) {
                cpu.hariakIlara[indizea] = &cpu.coreak[i].hariak[j];
            } else {

            }
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

void Dispatcher(PCB *jasotakopcb){
    jasotakopcb->running=1;
    haria *esleitua = lortuHariAskea(jasotakopcb->preferentziaCPU);
    if (esleitua==NULL){
        printf("<-- CPU osoa okupatuta dago ezin da haririk sartu CPUan. ");
        prozesuaPush(jasotakopcb);
        return;
    }
    else{
        jasotakopcb->running=1;
        jasotakopcb->preferentziaCPU=esleitua->coreID;
        if(esleitua->libre==0){
            printf(" <-- %d prozesua kanporatu da eta %d prozesua hasieratu da.", esleitua->pcb->pid, jasotakopcb->pid);
            esleitua->pcb->running=0;
            prozesuaPush((esleitua->pcb));
        }
        else{
            printf(" <-- %d prozesua hasieratu da.", jasotakopcb->pid);
        }
        esleitua->pcb = jasotakopcb;
        esleitua->libre = 0;
        printf(" <-- Corea:%d; Haria:%d", azkenHaria/cpu.harikopCoreko, azkenHaria%cpu.harikopCoreko);
    }
}

haria* lortuHariAskea(int preferentzia){
    if(preferentzia!=-1){
        for(int i=0; i<cpu.harikopCoreko; i++){
            //Ez bada egoten libre preferentzia duen corean haririk, preferentzia ez da kontutan hartuko
            if(cpu.coreak[preferentzia].hariak[i].libre == 1){
                //Ez da azken haria aldatzen
                return &cpu.coreak[preferentzia].hariak[i];
            }
        }
    }
    for(int i = 0; i < hariTotalak; i++){
        if(cpu.hariakIlara[i]->libre == 1){
            azkenHaria=i;
            return cpu.hariakIlara[i];
        }
    }
    if(azkenHaria<hariTotalak-1){
        azkenHaria++;
    }
    else{
        azkenHaria=0;
    }
    int buelta=0;
    while(buelta<hariTotalak && cpu.hariakIlara[azkenHaria]->pcb != NULL && cpu.hariakIlara[azkenHaria]->pcb->blokeatuta==1){
        if(azkenHaria<hariTotalak-1){
            azkenHaria++;
        }
        else{
            azkenHaria=0;
        }
        buelta++;
    }
    if(buelta==hariTotalak){
        return NULL;
    }
    return cpu.hariakIlara[azkenHaria];
}
