#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loader.h"
#include "memoria.h"
#include "prozesuak.h"

void orriTaulaSortu(PCB* pcb) {
    uint32_t taula_tamaina = SARRERA_KOPURUA * sizeof(OrriTaulaSarrera);
    pcb->mm.pgb = kernelMemoriaEskatu(taula_tamaina);

    for (int i = 0; i < SARRERA_KOPURUA; i++) {
        uint32_t sarrera_helbidea = pcb->mm.pgb + i * sizeof(OrriTaulaSarrera);
        OrriTaulaSarrera sarrera = {0, 0, 0, 0};
        memcpy(&memoria.memoria[sarrera_helbidea], &sarrera, sizeof(OrriTaulaSarrera));
    }

    printf("<-- Orri-taula sortuta: 0x%X\n", pcb->mm.pgb);
}

void programaKargatu(const char* fitxategia, int garrantzia) {
    FILE* f = fopen(fitxategia, "r");
    if (f == NULL) {
        printf("Errorea: Ezin da %s fitxategia ireki\n", fitxategia);
        return;
    }

    PCB* pcb = prozesuakSortu(garrantzia);

    fscanf(f, "%X", &pcb->mm.code);
    fscanf(f, "%X", &pcb->mm.data);

    printf("\n=== PROGRAMA KARGATZEN: %s (PID: %d) ===\n", fitxategia, pcb->pid);

    char lerro[256];
    uint32_t code_fisikoa = userMemoriaEskatu(ORRI_TAMAINA);
    uint32_t data_fisikoa = userMemoriaEskatu(ORRI_TAMAINA);
    uint32_t code_instructions = 0;
    uint32_t data_count = 0;

    orriTaulaSortu(pcb);

    while (fgets(lerro, sizeof(lerro), f)) {
        //Irakurri goiburuak
        if (strncmp(lerro, ".text", 5) == 0) {
            sscanf(lerro + 6, "%X", &pcb->mm.code);
            continue;
        }
        if (strncmp(lerro, ".data", 5) == 0) {
            sscanf(lerro + 6, "%X", &pcb->mm.data);
            continue;
        }

        //Irakurri balio hexadezimalak (aginduak edo datuak)
        uint32_t balioa;
        if (sscanf(lerro, "%X", &balioa) == 1) {
            if (data_count == 0 && (balioa & 0xF0000000) != 0xF0000000) {
                memoriaIdatzi(code_fisikoa + code_instructions * HITZ_TAMAINA, balioa);
                code_instructions++;
            } else {
                memoriaIdatzi(data_fisikoa + data_count * HITZ_TAMAINA, balioa);
                data_count++;
            }
        }
    }
    printf("Code start: 0x%X\n", pcb->mm.code);
    printf("Data start: 0x%X\n", pcb->mm.data);

    fclose(f);

    uint32_t code_orri = pcb->mm.code / ORRI_TAMAINA;
    OrriTaulaSarrera code_sarrera = {code_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera)],
           &code_sarrera, sizeof(OrriTaulaSarrera));

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    OrriTaulaSarrera data_sarrera = {data_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)],
           &data_sarrera, sizeof(OrriTaulaSarrera));

    prozesuaPush(pcb);
    printf("\nPrograma kargatuta: %d agindu, %d datu\n", code_instructions, data_count);
    printf("===================================\n\n");
}

void debugMemoria(PCB* pcb) {
    if (pcb == NULL) return;

    printf("\n=== MEMORIA DUMP (PID: %d) ===\n", pcb->pid);
    printf("--- .DATA SEGMENTUA ---\n");

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    OrriTaulaSarrera sarrera;
    memcpy(&sarrera, &memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)],
           sizeof(OrriTaulaSarrera));

    for (uint32_t i = 0; i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t balioa = memoriaIrakurri(sarrera.fisikoa + i);
        if (balioa != 0) {
            printf("  [0x%06X] = 0x%08X (%d)\n", pcb->mm.data + i, balioa, (int32_t)balioa);
        }
    }
    printf("=========================\n\n");
}
