#include <stdio.h>
#include <stdlib.h>
#include "prozesuak.h"

int last_ID = 1;

prozesu_ilara ready;
//Prozesuak sortzeko funtzioa
PCB* prozesuakSortu(int garrantzi){
    //PCB egitura dinamikoki alokatu
    PCB *pcb_berria = (PCB*)malloc(sizeof(PCB));

    pcb_berria->running = 0;
    pcb_berria->blokeatuta = 0;
    pcb_berria->preferentziaCPU = -1;
    pcb_berria->garrantzia = garrantzi;
    pcb_berria->pid = last_ID;
    last_ID++;
    return pcb_berria;
}

//Prozesu hilaran dagoen leheneko pcb lortzeko
PCB* prozesuaPick(){
    if(ready.lehena != NULL){
        return ready.lehena->pcb;
    }
    // Ilara hutsik badago, PCB huts bat itzuli
    PCB *hutsik = (PCB*)malloc(sizeof(PCB));
    if(hutsik == NULL) exit(EXIT_FAILURE);
    hutsik->pid = -1;
    hutsik->running = 0;
    hutsik->garrantzia = 0;
    return hutsik;
}

//Prozesu hilaran dagoen leheneko pcb lortzeko eta ezabatzeko
PCB* prozesuaPoll(){
    if(ready.lehena != NULL){
        prozesu_ilara_zatia* lehen = ready.lehena;
        PCB *pcb_adierazlea = lehen->pcb;
        ready.lehena = ready.lehena->hurrengoa;

        if (ready.lehena == NULL) {
            ready.azkena = NULL;
        }
        free(lehen);
        return pcb_adierazlea;
    }
    // Ilara hutsik badago, PCB huts bat itzuli
    PCB *hutsik = (PCB*)malloc(sizeof(PCB));
    if(hutsik == NULL) exit(EXIT_FAILURE);
    hutsik->pid = -1;
    hutsik->running = 0;
    hutsik->garrantzia = 0;
    return hutsik;
}

void prozesuaPush(PCB *sartzenDena){
    prozesu_ilara_zatia* nodoa = (prozesu_ilara_zatia*)malloc(sizeof(prozesu_ilara_zatia));
    if (nodoa == NULL) exit(EXIT_FAILURE);
    nodoa->pcb = sartzenDena;
    nodoa->pcb->running = 0;
    if(ready.azkena!=NULL){
        prozesu_ilara_zatia* momentukoa = ready.lehena;
        while(momentukoa->hurrengoa != NULL && momentukoa->hurrengoa->pcb->garrantzia > nodoa->pcb->garrantzia){
            momentukoa = momentukoa->hurrengoa;
        }
        if(momentukoa == ready.lehena && momentukoa->pcb->garrantzia <= nodoa->pcb->garrantzia){
            nodoa->hurrengoa = ready.lehena;
            ready.lehena = nodoa;
        }
        else if (momentukoa->hurrengoa != NULL && momentukoa->hurrengoa->pcb->garrantzia <= nodoa->pcb->garrantzia) {
            nodoa->hurrengoa = momentukoa->hurrengoa;
            momentukoa->hurrengoa = nodoa;
        }
        else{
            ready.azkena->hurrengoa = nodoa;
            ready.azkena = nodoa;
            nodoa->hurrengoa = NULL;
        }
    }
    else{
        ready.lehena=nodoa;
        ready.azkena=nodoa;
        nodoa->hurrengoa = NULL;
    }
}
