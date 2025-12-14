#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gertaerak.h"
#include "prozesuak.h"
#include "CPU.h"

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
        if(cpu.hariakIlara[i].pcb!=NULL){
            cpu.hariakIlara[i].pcb->garrantzia=0;
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
        if(cpu.hariakIlara[i].pcb!=NULL){
            cpu.hariakIlara[i].pcb->blokeatuta=0;
        }
    }
}
