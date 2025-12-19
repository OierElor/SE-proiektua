#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"
#include "memoria.h"
#include "prozesuak.h"

void orriTaulaSortu(PCB* pcb) {
    uint32_t taula_tamaina = SARRERA_KOPURUA * sizeof(OrriTaulaSarrera);
    pcb->mm.pgb = (uint32_t)(uintptr_t)kernelMemoriaEskatu(taula_tamaina);

    if (pcb->mm.pgb == 0) {
        printf("Errorea: Ezin da orri-taula sortu\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SARRERA_KOPURUA; i++) {
        uint32_t sarrera_helbidea = pcb->mm.pgb + i * sizeof(OrriTaulaSarrera);
        OrriTaulaSarrera sarrera = {0, 0, 0, 0};
        memcpy(&memoria.memoria[sarrera_helbidea], &sarrera, sizeof(OrriTaulaSarrera));
    }

    printf("<-- Orri-taula sortuta: 0x%X\n", pcb->mm.pgb);
}

PCB* programaKargatu(const char* fitxategia, int garrantzia) {
    FILE* f = fopen(fitxategia, "r");
    if (f == NULL) {
        printf("Errorea: Ezin da %s fitxategia ireki\n", fitxategia);
        return NULL;
    }

    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    if (pcb == NULL) {
        fclose(f);
        return NULL;
    }

    pcb->pid = last_ID++;
    pcb->running = 0;
    pcb->blokeatuta = 0;
    pcb->preferentziaCPU = -1;
    pcb->garrantzia = garrantzia;

    fscanf(f, "%X", &pcb->mm.code_start);
    fscanf(f, "%X", &pcb->mm.data_start);

    printf("\n=== PROGRAMA KARGATZEN: %s (PID: %d) ===\n", fitxategia, pcb->pid);
    printf("Code start: 0x%X\n", pcb->mm.code_start);
    printf("Data start: 0x%X\n", pcb->mm.data_start);

    orriTaulaSortu(pcb);

    char lerro[256];
    uint32_t code_instructions = 0;
    uint32_t code_fisikoa = (uint32_t)(uintptr_t)userMemoriaEskatu(ORRI_TAMAINA * 10); // 10 orri koderentzat

    printf("\n--- .TEXT SEGMENTUA ---\n");
    while (fgets(lerro, sizeof(lerro), f)) {
        uint32_t agindua;
        if (sscanf(lerro, "%X", &agindua) == 1) {
            memoriaIdatzi(code_fisikoa + code_instructions * HITZ_TAMAINA, agindua);
            printf("  [0x%06X] -> 0x%08X (fis: 0x%X)\n",
                   pcb->mm.code_start + code_instructions * HITZ_TAMAINA,
                   agindua,
                   code_fisikoa + code_instructions * HITZ_TAMAINA);
            code_instructions++;

            if ((agindua & 0xF0000000) == 0x40000000) {
                break;
            }
        }
    }
    pcb->mm.code_size = code_instructions * HITZ_TAMAINA;

    uint32_t data_count = 0;
    uint32_t data_fisikoa = (uint32_t)(uintptr_t)userMemoriaEskatu(ORRI_TAMAINA * 5); // 5 orri datuentzat

    printf("\n--- .DATA SEGMENTUA ---\n");
    while (fgets(lerro, sizeof(lerro), f)) {
        uint32_t datua;
        if (sscanf(lerro, "%X", &datua) == 1) {
            memoriaIdatzi(data_fisikoa + data_count * HITZ_TAMAINA, datua);
            printf("  [0x%06X] -> 0x%08X (fis: 0x%X)\n",
                   pcb->mm.data_start + data_count * HITZ_TAMAINA,
                   datua,
                   data_fisikoa + data_count * HITZ_TAMAINA);
            data_count++;
        }
    }
    pcb->mm.data_size = data_count * HITZ_TAMAINA;

    fclose(f);

    uint32_t code_orri = pcb->mm.code_start / ORRI_TAMAINA;
    OrriTaulaSarrera code_sarrera = {code_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera)],
           &code_sarrera, sizeof(OrriTaulaSarrera));

    uint32_t data_orri = pcb->mm.data_start / ORRI_TAMAINA;
    OrriTaulaSarrera data_sarrera = {data_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)],
           &data_sarrera, sizeof(OrriTaulaSarrera));

    printf("\nPrograma kargatuta: %d agindu, %d datu\n", code_instructions, data_count);
    printf("===================================\n\n");

    return pcb;
}

void debugMemoria(PCB* pcb) {
    if (pcb == NULL) return;

    printf("\n=== MEMORIA DUMP (PID: %d) ===\n", pcb->pid);
    printf("--- .DATA SEGMENTUA ---\n");

    uint32_t data_orri = pcb->mm.data_start / ORRI_TAMAINA;
    OrriTaulaSarrera sarrera;
    memcpy(&sarrera, &memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)],
           sizeof(OrriTaulaSarrera));

    for (uint32_t i = 0; i < pcb->mm.data_size; i += HITZ_TAMAINA) {
        uint32_t balioa = memoriaIrakurri(sarrera.fisikoa + i);
        printf("  [0x%06X] = 0x%08X (%d)\n",
               pcb->mm.data_start + i, balioa, (int32_t)balioa);
    }
    printf("=========================\n\n");
}
