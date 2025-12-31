#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prozesuak.h"

#define HASIERAKO_DIRU_MIN 1
#define HASIERAKO_DIRU_MAX 10
#define VIP_MAXIMO 2
#define VIP_IZATEKO_GARRANTZIA 50

extern int AzkenVIP;
extern PCB* VIPLista[VIP_MAXIMO];

void jokoaAukeratu(PCB* pcb);

void blackJack(PCB* pcb);

int blackJackKartaJaso();

void txanpon_Makina(PCB* pcb);

void ruleta(PCB* pcb);

int gorriaDa(int n);

void diruaZero(PCB* pcb);

void BihurtuVIP(PCB* pcb);

void GalduVIP(PCB* pcb);
