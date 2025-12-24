#include <stdint.h>
#include "prozesuak.h"

#define SARRERA_KOPURUA 256

typedef struct {
    uint32_t fisikoa;
    uint8_t baliozkoa;
    uint8_t aldatua;
    uint8_t erreferentziatua;
} OrriTaulaSarrera;

void programaKargatu(const char* fitxategia, int garrantzia);
void orriTaulaSortu(PCB* pcb);
void debugMemoria(PCB* pcb);
void orriTaulaPantailaratu(PCB* pcb);
void orriTaulakGuztiakPantailaratu();
