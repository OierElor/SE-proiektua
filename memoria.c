#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "memoria.h"

MemoriaFisikoa memoria;

void memoriaHasieratu() {
    memoria.tamaina = MEMORIA_TAMAINA;
    memoria.kernelAmaieraHelbidea = KERNEL_MEMORIA;
    memoria.hurrengoKernelMemoriaAskea = 0;
    memoria.hurrengoUserMemoriaAskea = KERNEL_MEMORIA;

    memoria.memoria = (uint8_t*)calloc(MEMORIA_TAMAINA, sizeof(uint8_t));
    if (memoria.memoria == NULL) {
        perror("Errorea memoria fisikoa sortzean");
        exit(EXIT_FAILURE);
    }
    printf("Memoria fisikoa hasieratuta: %d MB\n", MEMORIA_TAMAINA/(1024*1024));
    printf("Kernel memoria: 0x0 - 0x%X\n", KERNEL_MEMORIA);
    printf("User memoria: 0x%X - 0x%X\n", KERNEL_MEMORIA, MEMORIA_TAMAINA);
}

uint32_t kernelMemoriaEskatu(uint32_t tamaina) {
    if (memoria.hurrengoKernelMemoriaAskea + tamaina > memoria.kernelAmaieraHelbidea) {
        printf("Errorea: Ez dago nahikoa kernel memoria\n");
        return NULL;
    }
    uint32_t helbidea = memoria.hurrengoKernelMemoriaAskea;
    memoria.hurrengoKernelMemoriaAskea += tamaina;
    return helbidea;
}
hurrengoUserMemoriaAskea
uint32_t userMemoriaEskatu(uint32_t tamaina) {
    if (memoria.hurrengoUserMemoriaAskea + tamaina > memoria.tamaina) {
        printf("Errorea: Ez dago nahikoa erabiltzaile memoria\n");
        return NULL;
    }
    uint32_t helbidea = memoria.hurrengoUserMemoriaAskea;
    memoria.hurrengoUserMemoriaAskea += tamaina;
    return helbidea;
}

void memoriaIdatzi(uint32_t helbidea, uint32_t balioa) {
    if (helbidea + HITZ_TAMAINA > memoria.tamaina) {
        printf("Errorea: Helbidea memoriatik kanpo\n");
        return;
    }
    // Little-endian formatuan idatzi
    memoria.memoria[helbidea] = balioa & 0xFF;
    memoria.memoria[helbidea + 1] = (balioa >> 8) & 0xFF;
    memoria.memoria[helbidea + 2] = (balioa >> 16) & 0xFF;
    memoria.memoria[helbidea + 3] = (balioa >> 24) & 0xFF;
}

uint32_t memoriaIrakurri(uint32_t helbidea) {
    if (helbidea + HITZ_TAMAINA > memoria.tamaina) {
        printf("Errorea: Helbidea memoriatik kanpo\n");
        return 0;
    }
    // Little-endian formatuan irakurri
    uint32_t balioa = memoria.memoria[helbidea] |
    (memoria.memoria[helbidea + 1] << 8) |
    (memoria.memoria[helbidea + 2] << 16) |
    (memoria.memoria[helbidea + 3] << 24);
    return balioa;
}

void memoriaLibratu() {
    if (memoria.memoria != NULL) {
        free(memoria.memoria);
        memoria.memoria = NULL;
    }
}
