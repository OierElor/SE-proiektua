#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "erlojua.h"
#include "CPU.h"
#include "prozesuak.h"
#include "scheduler.h"
#include "gertaerak.h"
#include "memoria.h"
#include "loader.h"

int TempCont;
int Done;
pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condS = PTHREAD_COND_INITIALIZER;
//Main Funtzioa

int main(){
    srand(time(NULL)); //Zenbaki ausazko sortzailea hasieratu uneko denborarekin

    printf("====== CasUnix hasieratzen da =====\n\n");

    printf("====== MEMORIA KONFIGURAZIOA ======\n");
    memoriaHasieratu();
    printf("===================================\n\n");
    //Prosezuen ilara hasieratzeko
    ready.lehena = NULL;
    ready.azkena = NULL;
    //cpu-hasieratu
    int harikop = 2;
    int corekop = 2;
    azkenHaria=0;
    cpuHasieratu(corekop, harikop);
    printf("=== CPU KONFIGURAZIOA ===\n");
    printf("Hari kopurua: %d \n", cpu.harikopCoreko);
    printf("Core kopurua: %d \n", cpu.corekop);
    printf("=========================\n\n");
    Done = 0;

    printf("=== PROGRAMAK KARGATZEN ===\n");
    int programaKop = 50;  // Zenbat programa kargatu nahi ditugun
    int kargatuak = 0;
    char fitxategia[50];

    for(int i = 0; i < programaKop; i++){
        sprintf(fitxategia, "prometheus/prog%03d.elf", i);
        int garrantzia = (rand() % 10) + 1;  //Ausazko garrantzia

        programaKargatu(fitxategia, garrantzia);
    }

    printf("Programa denak kargatuta\n");
    printf("===========================\n\n");

    orriTaulakGuztiakPantailaratu();

    //Denbora hasierako konfigurazioa ikusteko
    printf("\n === Orain 3 segundu duzu programa gelditzeko eta hasierako konfigurazioa ikusteko ===\n");
    sleep(3);

    //Temporizadore kantitatea ezaretzeko
    TempCont = 2;
    //Erlojua hasieratu
    pthread_t clock;
    //Scheduler hasieratu
    pthread_t sched;

    pthread_create(&sched, NULL, scheduler, NULL);
    pthread_create(&clock, NULL, erlojua, NULL);
    pthread_t *threads = calloc(TempCont, sizeof(pthread_t));
    TempArgs *args = calloc(TempCont, sizeof(TempArgs));

    // Maiztasun desberdinak definitu
    int maiztasunak[] = {3, 1};

    for(int i=0; i<TempCont; i++){
        args[i].id = i;
        args[i].maiztasuna = maiztasunak[i];
        pthread_create(&threads[i], NULL, temporizadorea, &args[i]);
    }

    pthread_exit(NULL);

    memoriaLibratu();

    free(threads);
    free(args);

    return 0;
}
