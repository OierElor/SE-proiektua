#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "exekuzioMotorra.h"
#include "memoria.h"
#include "CPU.h"

int DispacherZikloa = 0;

uint32_t helbideBirtualatikFisikora(haria *h, uint32_t helbide_birtuala) {
    if (!h->mmu.gaituta) {
        return helbide_birtuala; // MMU gaituta ez badago
    }

    // Helbide birtuala zatitu: orria eta offset-a
    uint32_t orri_birtuala = helbide_birtuala / ORRI_TAMAINA;
    uint32_t offset = helbide_birtuala % ORRI_TAMAINA;

    // Lehenik TLB-n bilatu
    uint32_t frame_fisikoa = tlbBilatu(&h->mmu.tlb, orri_birtuala);

    if (frame_fisikoa != 0xFFFFFFFF) {
        h->mmu.itzulpenak++;
        printf("Hitzulpena eginda: %X -> %X (pid:%d)", helbide_birtuala, frame_fisikoa + offset, h->pcb->pid);
        return frame_fisikoa + offset;
    }

    // TLB barruan ez dago, ondorioz orri taula begiratu
    h->mmu.itzulpenak++;

    OrriTaulaSarrera sarrera;
    memcpy(&sarrera, &memoria.memoria[h->pcb->mm.pgb + (sizeof(OrriTaulaSarrera) * orri_birtuala)], sizeof(OrriTaulaSarrera));

    if (!sarrera.baliozkoa) {
        printf("Arazo bat gertatu da sarrera ez da baliozkoa");
        return 0;
    }

    tlbSartu(&h->mmu.tlb, orri_birtuala, sarrera.fisikoa);

    return sarrera.fisikoa + offset;
}

void hurrengoAgindua(haria *h) {
    uint32_t pc_fisikoa = helbideBirtualatikFisikora(h, h->PC);
    h->IR = memoriaIrakurri(pc_fisikoa);
    h->PC += 4;
}

// Decode & Execute: Agindua dekodifikatu eta exekutatu
void decode_execute(haria *h) {
    uint32_t opcode = (h->IR >> 28) & 0xF;

    switch(opcode) {
        case OP_LD: {
            uint8_t reg = (h->IR >> 24) & 0xF;
            uint32_t addr = h->IR & 0xFFFFFF;
            uint32_t addr_fisikoa = helbideBirtualatikFisikora(h, addr);
            h->regs[reg] = memoriaIrakurri(addr_fisikoa);
            printf("      LD R%d, [0x%06X] -> %d (0x%08X)\n", reg, addr, (int32_t)h->regs[reg], h->regs[reg]);
            break;
        }

        case OP_ST: {
            uint8_t reg = (h->IR >> 24) & 0xF;
            uint32_t addr = h->IR & 0xFFFFFF;
            uint32_t addr_fisikoa = helbideBirtualatikFisikora(h, addr);
            memoriaIdatzi(addr_fisikoa, h->regs[reg]);
            printf("      ST [0x%06X], R%d <- %d (0x%08X)\n", addr, reg, (int32_t)h->regs[reg], h->regs[reg]);
            break;
        }

        case OP_ADD: {
            uint8_t r1 = (h->IR >> 20) & 0xF;
            uint8_t r2 = (h->IR >> 16) & 0xF;
            uint8_t r3 = (h->IR >> 24) & 0xF;
            h->regs[r3] = h->regs[r1] + h->regs[r2];
            printf("      ADD R%d, R%d, R%d -> %d (0x%08X)\n", r3, r1, r2, (int32_t)h->regs[r3], h->regs[r3]);
            break;
        }

        case OP_EXIT: {
            printf("      EXIT - Prozesua amaitu da\n");
            h->pcb->running = 0;
            h->libre = 1;

            // Memoria dump erakutsi amaieran
            debugData(h->pcb);

            pcbAmaituMarkatu(h->pcb);
            h->pcb = NULL;
            break;
        }

        default:
            printf("      Agindu ezezaguna: 0x%08X\n", h->IR);
            break;
    }
}

// Prozesuaren exekuzio osoa
void exekutatuProzesua(haria *h) {
    if (h == NULL || h->pcb == NULL || !h->pcb->running) {
        return;
    }
    printf("\n>>> EXEKUTATZEN: PID %d (Core: %d)\n", h->pcb->pid, h->coreID);

    printf("PC=0x%06X: ", h->PC);

    hurrengoAgindua(h);
    printf("IR=0x%08X ", h->IR);

    decode_execute(h);
}

void hariDenakExekutatu(){
    DispacherZikloa++;
    if(DispacherZikloa<10){
        printf("\n=== Exekuzio Motorra ===");
        for(int j = 0; j < hariTotalak; j++){
            haria *h = cpu.hariakIlara[j];
            if(h->pcb != NULL && h->pcb->running){
                exekutatuProzesua(h);
            }
        }
    }
    else{
        DispacherZikloa=0;
    }
}

