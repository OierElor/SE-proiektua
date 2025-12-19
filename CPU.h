#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesuak.h"

#define TLB_TAMAINA 16

typedef struct {
    uint32_t orri_birtuala;
    uint32_t frame_fisikoa;
    uint8_t baliozkoa;
    uint8_t dirty;
} TLBSarrera;

typedef struct {
    TLBSarrera sarrerak[TLB_TAMAINA];
    uint32_t hits;
    uint32_t misses;
    uint32_t next_replace;
} TLB;

typedef struct {
    uint8_t gaituta;
    uint32_t itzulpenak;
    uint32_t page_faults;
} MMU;

typedef struct{
    int libre;
    int coreID; //Jakiteko haria ze coretakoa den
    PCB *pcb;

    MMU mmu;
    uint32_t PTBR;
    uint32_t IR;
    uint32_t PC;
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
