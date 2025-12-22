#include <stdint.h>

#define MEMORIA_TAMAINA (1 << 24)
#define HITZ_TAMAINA 4
#define KERNEL_MEMORIA 0x100000

typedef struct {
    uint8_t *memoria;
    uint32_t tamaina;
    uint32_t kernelAmaieraHelbidea;
    uint32_t hurrengoKernelMemoriaAskea;
    uint32_t hurrengoUserMemoriaAskea;
} MemoriaFisikoa;

extern MemoriaFisikoa memoria;

void memoriaHasieratu();
void* kernelMemoriaEskatu(uint32_t tamaina);
void* userMemoriaEskatu(uint32_t tamaina);
void memoriaIdatzi(uint32_t helbidea, uint32_t balioa);
uint32_t memoriaIrakurri(uint32_t helbidea);
void memoriaLibratu();
