#include <stdio.h>
#include <stdlib.h>
#include "dispatcher.h"
#include "prozesuak.h"
#include "CPU.h"

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
        esleitua->PTBR = jasotakopcb->mm.pgb;
        esleitua->PC = jasotakopcb->mm.code;

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
        tlbGarbitu(&esleitua->mmu.tlb);

        printf(" <-- Corea:%d; Haria:%d (PTBR: 0x%X)\n", azkenHaria/cpu.harikopCoreko, azkenHaria%cpu.harikopCoreko, esleitua->PTBR);
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
