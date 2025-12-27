#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "prozesuak.h"

#define HASIERAKO_DIRU_MIN 1
#define HASIERAKO_DIRU_MAX 10

void jokoaAukeratu(PCB* pcb);

void ruleta(PCB* pcb);

int gorriaDa(int n);

void diruaZero(PCB* pcb);

void BihurtuVIP(PCB* pcb);

void GalduVIP(PCB* pcb);
