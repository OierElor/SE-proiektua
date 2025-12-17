#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesuak.h"

typedef struct{
    int libre;
    int coreID; //Jakiteko haria ze coretakoa den
    PCB *pcb;
}haria;

typedef struct{
    haria *hariak;
}Core;

typedef struct{
    haria **hariakIlara;
    int corekop;
    int harikopCoreko;
    Core *coreak;
}CPU;

void cpuHasieratu(int ckop, int hkop);

extern CPU cpu;

extern int hariTotalak;
extern int azkenHaria;

void Dispatcher(PCB *pcb);

haria* lortuHariAskea(int preferentzia);

#endif
