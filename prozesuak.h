#ifndef PROZESUAK_H
#define PROZESUAK_H

#include <stdio.h>
#include <stdlib.h>

type struct{
    void* pgb;
    void* code;
    void* data;
}mmStruct;

typedef struct{
    int pid;
    int running;
    int garrantzia;
    int blokeatuta;
    int preferentziaCPU;
    mmStruct mm;
}PCB;

typedef struct prozesu_ilara_zatia prozesu_ilara_zatia;

struct prozesu_ilara_zatia{
    PCB *pcb;
    prozesu_ilara_zatia* hurrengoa;
};

typedef struct {
    prozesu_ilara_zatia* lehena;
    prozesu_ilara_zatia* azkena;
}prozesu_ilara;

extern prozesu_ilara ready;

extern int last_ID;

void prozesuakSortu(int garrantzi);

PCB* prozesuaPoll();

PCB* prozesuaPick();

void prozesuaPush(PCB *sartzenDena);

#endif
