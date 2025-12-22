#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "prozesuak.h"

#define TLB_TAMAINA 16

typedef struct {
    uint32_t helbideBirtuala;
    uint32_t helbideFisikoa;
    uint8_t baliozkoa;
    uint8_t aldatu;
} TLBSarrera;

typedef struct {
    TLBSarrera sarrerak[TLB_TAMAINA];
    uint32_t hits;
    uint32_t misses;
    uint32_t HurrengoSarrera;
} TLB;

typedef struct {
    uint8_t gaituta;
    uint32_t itzulpenak;
    uint32_t orriHutsigitea;
    TLB tlb;
} MMU;

typedef struct{
    int libre;
    int coreID; //Jakiteko haria ze coretakoa den
    PCB *pcb;

    MMU mmu;
    uint32_t PTBR; //Helbide fisikoa
    uint32_t IR;
    uint32_t PC;
    uint32_t regs[16];
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

//MMU eta TLB funtzioak
void mmuHasieratu(haria *h);
void tlbHasieratu(haria *h);
uint32_t tlbBilatu(TLB *tlb, uint32_t orri_birtuala);
void tlbSartu(TLB *tlb, uint32_t orri_birtuala, uint32_t frame_fisikoa);
void tlbGarbitu(TLB *tlb);

#endif
