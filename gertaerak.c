#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gertaerak.h"
#include "prozesuak.h"
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
