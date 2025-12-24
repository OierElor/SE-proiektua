#ifndef EXEKUZIO_MOTORRA_H
#define EXEKUZIO_MOTORRA_H

#include "CPU.h"
#include "prozesuak.h"
#include "loader.h"
#include <stdint.h>

// Agindu kodeak
#define OP_LD   0x0
#define OP_ST   0x1
#define OP_ADD  0x2
#define OP_EXIT 0xF

// Funtzioak
void exekutatuProzesua(haria *h);
uint32_t helbideBirtualatikFisikora(haria *h, uint32_t helbide_birtuala);
void hurrengoAgindua(haria *h);
void decode_execute(haria *h);

#endif
