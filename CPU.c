#include <stdio.h>
#include <stdlib.h>
#include "CPU.h"
#include <pthread.h>
#include "prozesuak.h"

CPU cpu;
int hariTotalak;
int azkenCorea;
int azkenHaria;

void cpuHasieratu(int ckop, int hkop){
    int i;
    int j;
    hariTotalak = ckop * hkop;
    cpu.corekop = ckop;
    cpu.harikopCoreko = hkop;
    cpu.hariakIlara = (haria**)malloc(hariTotalak * sizeof(haria*));
    if (cpu.hariakIlara == NULL) {
        perror("Errorea malloc egitean hariakIlara-rentzat");
        exit(EXIT_FAILURE);
    }
    cpu.coreak = (Core*)malloc(ckop * sizeof(Core));
    if (cpu.coreak == NULL) {
        perror("Errorea malloc egitean Corentzat");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < ckop; i++){
        cpu.coreak[i].hariak = (haria*)malloc(hkop * sizeof(haria));
        for(j = 0; j < hkop; j++){
            cpu.coreak[i].hariak[j].libre=1;
            cpu.coreak[i].hariak[j].coreID=i;
            cpu.coreak[i].hariak[j].pcb = NULL;

            //Erregistroak hasieratu
            cpu.coreak[i].hariak[j].PC = 0;
            cpu.coreak[i].hariak[j].IR = 0;
            cpu.coreak[i].hariak[j].PTBR = 0;
            for(int r = 0; r < 16; r++) {
                cpu.coreak[i].hariak[j].regs[r] = 0;
            }
            mmuHasieratu(&cpu.coreak[i].hariak[j]);

            int indizea = i * hkop + j;
            if (indizea < hariTotalak) {
                cpu.hariakIlara[indizea] = &cpu.coreak[i].hariak[j];
            } else {

            }
        }
        if (cpu.coreak[i].hariak == NULL) {
            perror("Errorea malloc egitean Harientzat");
            for(j = 0; j < i; j++){
                free(cpu.coreak[j].hariak);
            }
            free(cpu.coreak);
            exit(EXIT_FAILURE);
        }
    }
}

//MMU eta TLB funtzioak
void mmuHasieratu(haria *h) {
    h->mmu.gaituta = 1;
    h->mmu.itzulpenak = 0;
    h->mmu.orriHutsigitea = 0;
    tlbHasieratu(h);
}

void tlbHasieratu(haria *h) {
    for (int i = 0; i < TLB_TAMAINA; i++) {
        h->mmu.tlb.sarrerak[i].helbideBirtuala = 0;
        h->mmu.tlb.sarrerak[i].helbideFisikoa = 0;
        h->mmu.tlb.sarrerak[i].baliozkoa = 0;
        h->mmu.tlb.sarrerak[i].aldatu = 0;
    }
    h->mmu.tlb.hits = 0;
    h->mmu.tlb.misses = 0;
    h->mmu.tlb.HurrengoSarrera = 0;
}

uint32_t tlbBilatu(TLB *tlb, uint32_t helbideBirtuala) {
    for (int i = 0; i < TLB_TAMAINA; i++) {
        if (tlb->sarrerak[i].baliozkoa && tlb->sarrerak[i].helbideBirtuala == helbideBirtuala) {
            tlb->hits++;
            return tlb->sarrerak[i].helbideFisikoa;
        }
    }
    tlb->misses++;
    return 0xFFFFFFFF;
}

void tlbSartu(TLB *tlb, uint32_t helbideBirtuala, uint32_t helbideFisikoa) {
    int idx = tlb->HurrengoSarrera;
    tlb->sarrerak[idx].helbideBirtuala = helbideBirtuala;
    tlb->sarrerak[idx].helbideFisikoa = helbideFisikoa;
    tlb->sarrerak[idx].baliozkoa = 1;
    tlb->sarrerak[idx].aldatu = 0;

    tlb->HurrengoSarrera = (tlb->HurrengoSarrera + 1) % TLB_TAMAINA;
}

void tlbGarbitu(TLB *tlb) {
    for (int i = 0; i < TLB_TAMAINA; i++) {
        tlb->sarrerak[i].baliozkoa = 0;
    }
    tlb->HurrengoSarrera = 0;
}

void mmuEstadistikakErakutsi(haria* h){
    printf("\n=== MMU ESTATISTIKAK ===\n");
    printf("Itzulpenak: %u\n", h->mmu.itzulpenak);
    printf("TLB Hits: %u\n", h->mmu.tlb.hits);
    printf("TLB Misses: %u\n", h->mmu.tlb.misses);
    if (h->mmu.itzulpenak > 0) {
        float hit_rate = (float)h->mmu.tlb.hits / h->mmu.itzulpenak * 100;
        printf("TLB Hit Rate: %.2f%%\n", hit_rate);
    }
    printf("Page Faults: %u\n", h->mmu.orriHutsigitea);
    printf("================================\n\n");
}
