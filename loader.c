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

    printf("\n=== PROGRAMA KARGATZEN: %s (PID: %d) ===\n", fitxategia, pcb->pid);

    char lerro[256];
    uint32_t code_fisikoa = userMemoriaEskatu(ORRI_TAMAINA);
    uint32_t data_fisikoa = userMemoriaEskatu(ORRI_TAMAINA);
    uint32_t code_instructions = 0;
    uint32_t data_count = 0;

    int segmentua = -1;

    orriTaulaSortu(pcb);

    while (fgets(lerro, sizeof(lerro), f)) {
        // .text goiburua
        if (strncmp(lerro, ".text", 5) == 0) {
            sscanf(lerro + 6, "%X", &pcb->mm.code);
            segmentua = 0;
            continue;
        }
        // .data goiburua
        if (strncmp(lerro, ".data", 5) == 0) {
            sscanf(lerro + 6, "%X", &pcb->mm.data);
            segmentua = 1;
            continue;
        }
        // Balio hexadezimala irakurri
        uint32_t balioa;
        if (sscanf(lerro, "%X", &balioa) == 1) {
            if (segmentua == 0) {
                memoriaIdatzi(code_fisikoa + code_instructions * HITZ_TAMAINA, balioa);
                code_instructions++;
            }
            else if (segmentua == 1) {
                memoriaIdatzi(data_fisikoa + data_count * HITZ_TAMAINA, balioa);
                data_count++;
            }
        }
    }
    printf("Code start: 0x%X (fisikoa: 0x%X)\n", pcb->mm.code, code_fisikoa);
    printf("Data start: 0x%X (fisikoa: 0x%X)\n", pcb->mm.data, data_fisikoa);

    fclose(f);

    uint32_t code_orri = pcb->mm.code / ORRI_TAMAINA;
    OrriTaulaSarrera code_sarrera = {code_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera)], &code_sarrera, sizeof(OrriTaulaSarrera));

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    OrriTaulaSarrera data_sarrera = {data_fisikoa, 1, 0, 0};
    memcpy(&memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)], &data_sarrera, sizeof(OrriTaulaSarrera));

    debugMemoria(pcb);
    prozesuaPush(pcb);
    printf("\nPrograma kargatuta: %d agindu, %d datu\n", code_instructions, data_count);
    printf("===================================\n\n");
}

void debugMemoria(PCB* pcb) {
    if (pcb == NULL) return;

    printf("\n=== MEMORIA DUMP (PID: %d) ===\n", pcb->pid);

    // ============ .CODE SEGMENTUA ============
    printf("--- .CODE SEGMENTUA ---\n");

    uint32_t code_orri = pcb->mm.code / ORRI_TAMAINA;
    OrriTaulaSarrera code_sarrera;
    memcpy(&code_sarrera, &memoria.memoria[pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera)],
           sizeof(OrriTaulaSarrera));

    printf("  Helbide birtuala: 0x%06X\n", pcb->mm.code);
    printf("  Helbide fisikoa:  0x%06X\n", code_sarrera.fisikoa);
    printf("  Aginduak:\n");

    for (uint32_t i = 0; i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t agindua = memoriaIrakurri(code_sarrera.fisikoa + i);

        // EXIT agindua aurkitu arte inprimatu
        if (agindua == 0xF0000000) {
            printf("    [0x%06X] = 0x%08X  (EXIT)\n", pcb->mm.code + i, agindua);
            break;  // EXIT-en ondoren gelditu
        }

        // Bakarrik 0 ez diren aginduak erakutsi
        if (agindua != 0) {
            uint8_t opcode = (agindua >> 28) & 0xF;
            const char* agindu_izena = "";

            switch(opcode) {
                case 0x0: agindu_izena = "LD"; break;
                case 0x1: agindu_izena = "ST"; break;
                case 0x2: agindu_izena = "ADD"; break;
                case 0xF: agindu_izena = "EXIT"; break;
                default: agindu_izena = "???"; break;
            }

            printf("    [0x%06X] = 0x%08X  (%s)\n",
                   pcb->mm.code + i, agindua, agindu_izena);
        }
    }

    // ============ .DATA SEGMENTUA ============
    printf("\n--- .DATA SEGMENTUA ---\n");

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    OrriTaulaSarrera data_sarrera;
    memcpy(&data_sarrera, &memoria.memoria[pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera)],
           sizeof(OrriTaulaSarrera));

    printf("  Helbide birtuala: 0x%06X\n", pcb->mm.data);
    printf("  Helbide fisikoa:  0x%06X\n", data_sarrera.fisikoa);
    printf("  Datuak:\n");

    for (uint32_t i = 0; i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t balioa = memoriaIrakurri(data_sarrera.fisikoa + i);

        // Bakarrik 0 ez diren balioak erakutsi
        if (balioa != 0) {
            printf("    [0x%06X] = 0x%08X (%d)\n",
                   pcb->mm.data + i, balioa, (int32_t)balioa);
        }
    }

    printf("=========================\n\n");
}
