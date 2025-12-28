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

    pcb->gordePC = 0;

    for(int i=0;i<16;i++){
        pcb->gordeErregistroak[i]=0;
    }

    orriTaulaSortu(pcb);

    printf("\n=== PROGRAMA KARGATZEN: %s (PID: %d) ===\n", fitxategia, pcb->pid);

    char lerro[256];
    uint32_t fisikoa[SARRERA_KOPURUA];

    fisikoa[0] = userMemoriaEskatu(ORRI_TAMAINA);

    uint32_t helbide_birtuala = 0;

    // .text goiburua
    fgets(lerro, sizeof(lerro), f);
    sscanf(lerro + 6, "%X", &pcb->mm.code);
    helbide_birtuala = pcb->mm.code;
    // .data goiburua
    fgets(lerro, sizeof(lerro), f);
    sscanf(lerro + 6, "%X", &pcb->mm.data);

    int lerroKopCode = (pcb->mm.data - pcb->mm.code)/HITZ_TAMAINA;

    int orriZenbakia = 0;
    uint32_t orriBarnekoHelbidea = 0;
    uint32_t dataHelbidea=0;

    while (fgets(lerro, sizeof(lerro), f)) {
        // Balio hexadezimala irakurri
        if(orriBarnekoHelbidea>=ORRI_TAMAINA){
            orriZenbakia++;
            if(orriZenbakia>=SARRERA_KOPURUA){
                printf("Sarrera kopurua gainditu da");
                exit(EXIT_FAILURE);
            }
            fisikoa[orriZenbakia]=userMemoriaEskatu(ORRI_TAMAINA);
        }
        uint32_t balioa;
        if (sscanf(lerro, "%X", &balioa) == 1) {
            memoriaIdatzi(fisikoa[orriZenbakia] + helbide_birtuala, balioa);
            helbide_birtuala += HITZ_TAMAINA;
            if(lerroKopCode==0){
                dataHelbidea=fisikoa[orriZenbakia] + helbide_birtuala;
            }
            lerroKopCode--;
        }
    }
    printf("Code start: 0x%X (fisikoa: 0x%X)\n", pcb->mm.code, fisikoa);
    printf("Data start: 0x%X (fisikoa: 0x%X)\n", pcb->mm.data, dataHelbidea);

    fclose(f);

    for(int i=0; i<=orriZenbakia;i++){
        uint32_t sarreraZenbakia = pcb->mm.pgb + (i * sizeof(OrriTaulaSarrera));
        OrriTaulaSarrera sarrera = {fisikoa[i], 1, 0, 0};
        memcpy(&memoria.memoria[sarreraZenbakia], &sarrera, sizeof(OrriTaulaSarrera));
    }

    debugMemoria(pcb);
    prozesuaPush(pcb);
    printf("===================================\n\n");
}

void debugCode(PCB* pcb) {
    if (pcb == NULL) {
        printf("   PCB NULL da\n");
        return;
    }

    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│             .CODE SEGMENTUA (PID: %d)                           │\n", pcb->pid);
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    uint32_t code_orri = pcb->mm.code / ORRI_TAMAINA;
    uint32_t code_sarrera_helbidea = pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera);

    if (code_sarrera_helbidea + sizeof(OrriTaulaSarrera) > memoria.tamaina) {
        printf("   ERROREA: Orri-taula helbide txarra (0x%X)\n", code_sarrera_helbidea);
        return;
    }

    OrriTaulaSarrera code_sarrera;
    memcpy(&code_sarrera, &memoria.memoria[code_sarrera_helbidea], sizeof(OrriTaulaSarrera));

    if (!code_sarrera.baliozkoa) {
        printf("   OHARRA: Code sarrera ez da baliozkoa\n");
        return;
    }

    if (code_sarrera.fisikoa + ORRI_TAMAINA > memoria.tamaina) {
        printf("   ERROREA: Frame fisikoa txarra (0x%X)\n", code_sarrera.fisikoa);
        return;
    }

    printf("   Helbide birtuala: 0x%06X\n", pcb->mm.code);
    printf("   Helbide fisikoa:  0x%06X\n", code_sarrera.fisikoa);
    printf("\n  Aginduak:\n");
    printf("  ─────────────────────────────────────────────────────────────────\n");

    int agindu_kopurua = 0;
    for (uint32_t i = 0; i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t agindua = memoriaIrakurri(code_sarrera.fisikoa + i);

        if (agindua == 0xF0000000) {
            printf("    [0x%06X] 0x%08X  → EXIT\n", pcb->mm.code + i, agindua);
            agindu_kopurua++;
            break;
        }

        if (agindua != 0) {
            uint8_t opcode = (agindua >> 28) & 0xF;
            uint8_t reg = (agindua >> 24) & 0xF;
            uint32_t addr = agindua & 0xFFFFFF;

            switch(opcode) {
                case 0x0: // LD
                    printf("    [0x%06X] 0x%08X  → LD   R%d, [0x%06X]\n",
                           pcb->mm.code + i, agindua, reg, addr);
                    break;
                case 0x1: // ST
                    printf("    [0x%06X] 0x%08X  → ST   [0x%06X], R%d\n",
                           pcb->mm.code + i, agindua, addr, reg);
                    break;
                case 0x2: { // ADD
                    uint8_t r3 = (agindua >> 24) & 0xF;
                    uint8_t r1 = (agindua >> 20) & 0xF;
                    uint8_t r2 = (agindua >> 16) & 0xF;
                    printf("    [0x%06X] 0x%08X  → ADD  R%d, R%d, R%d\n",
                           pcb->mm.code + i, agindua, r3, r1, r2);
                    break;
                }
                case 0xF: // EXIT
                    printf("    [0x%06X] 0x%08X  → EXIT\n",
                           pcb->mm.code + i, agindua);
                    break;
                default:
                    printf("    [0x%06X] 0x%08X  → ??? (Agindu ezezaguna)\n",
                           pcb->mm.code + i, agindua);
                    break;
            }
            agindu_kopurua++;
        }
    }

    printf("\n  Guztira %d agindu\n", agindu_kopurua);
    printf("  ─────────────────────────────────────────────────────────────────\n\n");
}

void debugData(PCB* pcb) {
    if (pcb == NULL) {
        printf("   PCB NULL da\n");
        return;
    }

    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│             .DATA SEGMENTUA (PID: %d)                           │\n", pcb->pid);
    printf("└─────────────────────────────────────────────────────────────────┘\n");

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    uint32_t data_sarrera_helbidea = pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera);

    if (data_sarrera_helbidea + sizeof(OrriTaulaSarrera) > memoria.tamaina) {
        printf("   ERROREA: Orri-taula helbide txarra (0x%X)\n", data_sarrera_helbidea);
        return;
    }

    OrriTaulaSarrera data_sarrera;
    memcpy(&data_sarrera, &memoria.memoria[data_sarrera_helbidea], sizeof(OrriTaulaSarrera));

    if (!data_sarrera.baliozkoa) {
        printf("   OHARRA: Data sarrera ez da baliozkoa\n");
        return;
    }

    if (data_sarrera.fisikoa + ORRI_TAMAINA > memoria.tamaina) {
        printf("   ERROREA: Frame fisikoa txarra (0x%X)\n", data_sarrera.fisikoa);
        return;
    }

    printf("   Helbide birtuala: 0x%06X\n", pcb->mm.data);
    printf("   Helbide fisikoa:  0x%06X\n", data_sarrera.fisikoa + (pcb->mm.data % ORRI_TAMAINA));
    printf("\n  Datuak:\n");
    printf("  ─────────────────────────────────────────────────────────────────\n");

    int datuKop = 0;

    for (uint32_t i = (pcb->mm.data % ORRI_TAMAINA); i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t balioa = memoriaIrakurri(data_sarrera.fisikoa + i);

        if (balioa != 0) {
            int32_t signed_val = (int32_t)balioa;
            printf("    [0x%06X]  0x%08X  →  %-11d \n", pcb->mm.data + (i - (pcb->mm.data % ORRI_TAMAINA)), balioa, signed_val);
            datuKop++;
        }
    }
    printf("\n   Guztira %d datu\n", datuKop);

    printf("  ─────────────────────────────────────────────────────────────────\n\n");
}

void debugMemoria(PCB* pcb) {
    if (pcb == NULL) return;

    printf("\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║           MEMORIA DUMP (PID: %d)                                 ║\n", pcb->pid);
    printf("╚══════════════════════════════════════════════════════════════════╝\n");

    debugCode(pcb);
    debugData(pcb);

}

void orriTaulaPantailaratu(PCB* pcb) {
    if (pcb == NULL) return;

    printf("\n=== ORRI-TAULA INFO (PID: %d) ===\n", pcb->pid);
    printf("PGB (Hasierako helbide fisikoa): 0x%08X\n", pcb->mm.pgb);
    printf("Taularen tamaina: %u byte (%d sarrera)\n", SARRERA_KOPURUA * sizeof(OrriTaulaSarrera), SARRERA_KOPURUA);
    printf("----------------------------------------------------------\n");
    printf("Indizea | Helb. Fisikoa | V (Valid) | A (Dirty) | R (Ref)\n");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < SARRERA_KOPURUA; i++) {
        OrriTaulaSarrera sarrera;
        uint32_t sarrera_helbidea = pcb->mm.pgb + (i * sizeof(OrriTaulaSarrera));

        memcpy(&sarrera, &memoria.memoria[sarrera_helbidea], sizeof(OrriTaulaSarrera));

        if (sarrera.baliozkoa) {
            printf("  %3d   |   0x%08X  |     %d     |     %d     |    %d\n", i, sarrera.fisikoa, sarrera.baliozkoa, sarrera.aldatua, sarrera.erreferentziatua);
        }
    }
    printf("==========================================================\n\n");
}

void orriTaulakGuztiakPantailaratu() {
    prozesu_ilara_zatia* momentukoa = ready.lehena;

    if (momentukoa == NULL) {
        printf("Ez dago prozesurik kargatuta ready hilaran.\n");
        return;
    }

    printf("\n**********************************************************\n");
    printf("      SISTEMAKO PROZESU GUZTIEN ORRI-TAULAK\n");
    printf("**********************************************************\n");

    while (momentukoa != NULL) {
        orriTaulaPantailaratu(momentukoa->pcb);
        momentukoa = momentukoa->hurrengoa;
    }

    printf("**********************************************************\n\n");
}

/*
 * EMAITZAK KANPOKO FITXATEGI BATEAN GORDETZEKO FUNTZIOAK
 */

void debugCodeToFile(PCB* pcb, FILE* f) {
    if (pcb == NULL || f == NULL) {
        return;
    }

    fprintf(f, "\n┌─────────────────────────────────────────────────────────────────┐\n");
    fprintf(f, "│             .CODE SEGMENTUA (PID: %d)                           │\n", pcb->pid);
    fprintf(f, "└─────────────────────────────────────────────────────────────────┘\n");

    uint32_t code_orri = pcb->mm.code / ORRI_TAMAINA;
    uint32_t code_sarrera_helbidea = pcb->mm.pgb + code_orri * sizeof(OrriTaulaSarrera);

    if (code_sarrera_helbidea + sizeof(OrriTaulaSarrera) > memoria.tamaina) {
        fprintf(f, "   ERROREA: Orri-taula helbide txarra (0x%X)\n", code_sarrera_helbidea);
        return;
    }

    OrriTaulaSarrera code_sarrera;
    memcpy(&code_sarrera, &memoria.memoria[code_sarrera_helbidea], sizeof(OrriTaulaSarrera));

    if (!code_sarrera.baliozkoa) {
        fprintf(f, "   OHARRA: Code sarrera ez da baliozkoa\n");
        return;
    }

    if (code_sarrera.fisikoa + ORRI_TAMAINA > memoria.tamaina) {
        fprintf(f, "   ERROREA: Frame fisikoa txarra (0x%X)\n", code_sarrera.fisikoa);
        return;
    }

    fprintf(f, "   Helbide birtuala: 0x%06X\n", pcb->mm.code);
    fprintf(f, "   Helbide fisikoa:  0x%06X\n", code_sarrera.fisikoa);
    fprintf(f, "\n  Aginduak:\n");
    fprintf(f, "  ─────────────────────────────────────────────────────────────────\n");

    int agindu_kopurua = 0;
    for (uint32_t i = 0; i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t agindua = memoriaIrakurri(code_sarrera.fisikoa + i);

        if (agindua == 0xF0000000) {
            fprintf(f, "    [0x%06X] 0x%08X  → EXIT\n", pcb->mm.code + i, agindua);
            agindu_kopurua++;
            break;
        }

        if (agindua != 0) {
            uint8_t opcode = (agindua >> 28) & 0xF;
            uint8_t reg = (agindua >> 24) & 0xF;
            uint32_t addr = agindua & 0xFFFFFF;

            switch(opcode) {
                case 0x0: // LD
                    fprintf(f, "    [0x%06X] 0x%08X  → LD   R%d, [0x%06X]\n",
                            pcb->mm.code + i, agindua, reg, addr);
                    break;
                case 0x1: // ST
                    fprintf(f, "    [0x%06X] 0x%08X  → ST   [0x%06X], R%d\n",
                            pcb->mm.code + i, agindua, addr, reg);
                    break;
                case 0x2: { // ADD
                    uint8_t r3 = (agindua >> 24) & 0xF;
                    uint8_t r1 = (agindua >> 20) & 0xF;
                    uint8_t r2 = (agindua >> 16) & 0xF;
                    fprintf(f, "    [0x%06X] 0x%08X  → ADD  R%d, R%d, R%d\n",
                            pcb->mm.code + i, agindua, r3, r1, r2);
                    break;
                }
                default:
                    fprintf(f, "    [0x%06X] 0x%08X  → ??? (Agindu ezezaguna)\n",
                            pcb->mm.code + i, agindua);
                    break;
            }
            agindu_kopurua++;
        }
    }

    fprintf(f, "\n  Guztira %d agindu\n", agindu_kopurua);
    fprintf(f, "  ─────────────────────────────────────────────────────────────────\n\n");
}

void debugDataToFile(PCB* pcb, FILE* f) {
    if (pcb == NULL || f == NULL) {
        return;
    }

    fprintf(f, "\n┌─────────────────────────────────────────────────────────────────┐\n");
    fprintf(f, "│             .DATA SEGMENTUA (PID: %d)                           │\n", pcb->pid);
    fprintf(f, "└─────────────────────────────────────────────────────────────────┘\n");

    uint32_t data_orri = pcb->mm.data / ORRI_TAMAINA;
    uint32_t data_sarrera_helbidea = pcb->mm.pgb + data_orri * sizeof(OrriTaulaSarrera);

    if (data_sarrera_helbidea + sizeof(OrriTaulaSarrera) > memoria.tamaina) {
        fprintf(f, "   ERROREA: Orri-taula helbide txarra (0x%X)\n", data_sarrera_helbidea);
        return;
    }

    OrriTaulaSarrera data_sarrera;
    memcpy(&data_sarrera, &memoria.memoria[data_sarrera_helbidea], sizeof(OrriTaulaSarrera));

    if (!data_sarrera.baliozkoa) {
        fprintf(f, "   OHARRA: Data sarrera ez da baliozkoa\n");
        return;
    }

    if (data_sarrera.fisikoa + ORRI_TAMAINA > memoria.tamaina) {
        fprintf(f, "   ERROREA: Frame fisikoa txarra (0x%X)\n", data_sarrera.fisikoa);
        return;
    }

    fprintf(f, "   Helbide birtuala: 0x%06X\n", pcb->mm.data);
    fprintf(f, "   Helbide fisikoa:  0x%06X\n", data_sarrera.fisikoa + (pcb->mm.data % ORRI_TAMAINA));
    fprintf(f, "\n  Datuak:\n");
    fprintf(f, "  ─────────────────────────────────────────────────────────────────\n");

    int datuKop = 0;

    for (uint32_t i = (pcb->mm.data % ORRI_TAMAINA); i < ORRI_TAMAINA; i += HITZ_TAMAINA) {
        uint32_t balioa = memoriaIrakurri(data_sarrera.fisikoa + i);

        if (balioa != 0) {
            int32_t signed_val = (int32_t)balioa;
            fprintf(f, "    [0x%06X]  0x%08X  →  %-11d \n",
                    pcb->mm.data + (i - (pcb->mm.data % ORRI_TAMAINA)), balioa, signed_val);
            datuKop++;
        }
    }
    fprintf(f, "\n   Guztira %d datu\n", datuKop);
    fprintf(f, "  ─────────────────────────────────────────────────────────────────\n\n");
}

void debugMemoriaToFile(PCB* pcb, FILE* f) {
    if (pcb == NULL || f == NULL) return;

    fprintf(f, "\n╔══════════════════════════════════════════════════════════════════╗\n");
    fprintf(f, "║           MEMORIA DUMP (PID: %d)                                 ║\n", pcb->pid);
    fprintf(f, "╚══════════════════════════════════════════════════════════════════╝\n");

    debugCodeToFile(pcb, f);
    debugDataToFile(pcb, f);
}
