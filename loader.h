#include <stdint.h>
#include "prozesuak.h"

#define ORRI_TAMAINA 4096
#define SARRERA_KOPURUA 256

typedef struct {
    uint32_t fisikoa;
    uint8_t baliozkoa;
    uint8_t aldatua;
    uint8_t erreferentziatua;
} OrriTaulaSarrera;

PCB* programaKargatu(const char* fitxategia, int garrantzia);
void orriTaulaSortu(PCB* pcb);
void debugMemoria(PCB* pcb);
