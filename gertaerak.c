#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gertaerak.h"
#include "prozesuak.h"
#include "CPU.h"

//temporizadorek batek ruleta aktibatuko du eta ruleta ausaz gertaera bat exekutatuko du
void ruleta(){
    int gertaeraKantitatea = 6;
    int gertaera = (rand() % gertaeraKantitatea) + 1;
    switch(gertaera){
        case 1:
            printf("<-- Garrantzia RESET!");
            garrantziaReset();
            break;
        case 2:
            printf("<-- Ilarako garrantziak igo");
            garrantziaIgo();
            break;
        case 3:
            printf("<-- Ilarako lehena HIL!");
            ilarakoProzesuaHil();
            break;
        case 4:
            printf("<-- Prosezu berri bat, HURRA!");
            prozesuaSortuGertaera();
            break;
        case 5:
            printf("<-- Ilarako lehenak blokeatzeko gaitazuna lortzen du");
            blokeatuLehena();
            break;
        case 6:
            printf("<-- Blokeoak askatu");
            askatuBlokeoDenak();
            break;
        default:
            break;
    }
}

void garrantziaIgo(){
    if(ready.lehena!=NULL&&ready.lehena->pcb!=NULL){
        prozesu_ilara_zatia *momentukoa = ready.lehena;
        momentukoa->pcb->garrantzia++;
        while(momentukoa->hurrengoa!=NULL){
            momentukoa = momentukoa->hurrengoa;
            momentukoa->pcb->garrantzia++;
        }
    }
}

void garrantziaReset(){
    if(ready.lehena!=NULL&&ready.lehena->pcb!=NULL){
        prozesu_ilara_zatia *momentukoa = ready.lehena;
        momentukoa->pcb->garrantzia=0;
        while(momentukoa->hurrengoa!=NULL){
            momentukoa = momentukoa->hurrengoa;
            momentukoa->pcb->garrantzia=0;
        }
    }
    for(int i=0; i<hariTotalak; i++){
        if(cpu.hariakIlara[i]->pcb!=NULL){
            cpu.hariakIlara[i]->pcb->garrantzia=0;
        }
    }
}

void blokeatuLehena(){
    if(ready.lehena!=NULL&&ready.lehena->pcb!=NULL){
        ready.lehena->pcb->blokeatuta=1;
    }
}

void askatuBlokeoDenak(){
    if(ready.lehena!=NULL&&ready.lehena->pcb!=NULL){
        prozesu_ilara_zatia *momentukoa = ready.lehena;
        momentukoa->pcb->blokeatuta=0;
        while(momentukoa->hurrengoa!=NULL){
            momentukoa = momentukoa->hurrengoa;
            momentukoa->pcb->blokeatuta=0;
        }
    }
    for(int i=0; i<hariTotalak; i++){
        if(cpu.hariakIlara[i]->pcb!=NULL){
            cpu.hariakIlara[i]->pcb->blokeatuta=0;
        }
    }
}

void prozesuaSortuGertaera(){
    int garrantziMin = 0;
    int garrantziMax = 10;
    int garrantzia = (rand() % (garrantziMax - garrantziMin + 1)) + garrantziMin;
    prozesuakSortu(garrantzia);
}

void ilarakoProzesuaHil(){
    if(ready.lehena==NULL){
        printf("Ezin da hilarako prozesurik hil hilara hutsik dago");
    }
    else{
        PCB* hildakoa = prozesuaPoll();
        printf("<-- Hil da:%d",hildakoa->pid);
        free(hildakoa);
    }
}
