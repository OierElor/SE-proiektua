#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesuak.h"

typedef struct{
    int libre;
    PCB *pcb;
}haria;

typedef struct{
    int harikop;
    haria *hariak;
}Core;

typedef struct{
    int corekop;
    Core *coreak;
}CPU;

void cpuHasieratu(int ckop, int hkop);

extern CPU cpu;

extern int hariTotalak;
extern int azkenCorea;
extern int azkenHaria;

void pcbCPUariEsleitu(PCB *pcb);

haria* lortuHariAskea();

#endif
