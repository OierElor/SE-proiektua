#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gertaerak.h"
#include "prozesuak.h"
#include "CPU.h"


//temporizadorek batek ruleta aktibatuko du eta ruleta ausaz gertaera bat exekutatuko du
void jokoaAukeratu(PCB* pcb){
    if(pcb->garrantzia<=0){
        diruaZero(pcb);
    }
    int gertaeraKantitatea = 1;
    int jokoa = rand() % gertaeraKantitatea;
    switch(jokoa){
        case 0:
            printf("\n=== RULETA JOKOA ===\n");
            ruleta(pcb);
            break;
        default:
            break;
    }
    printf("\n====================\n");
    if(pcb->garrantzia>100){
        BihurtuVIP(pcb);
    }
    else if(pcb->blokeatuta==1){
        GalduVIP(pcb);
    }
}

void ruleta(PCB* pcb){
    if(pcb==NULL){
        printf("Ruletak jaso duen pcb-a NULL da");
    }
    int apustutakoDirua = (rand() % (pcb->garrantzia)) + 1;
    int emaitza = rand() % 37;
    int apustuMota =rand() % 7;
    switch(apustuMota){
        case 0: //Jokoa: Zenbaki bakarra
            if(rand() % 37 == emaitza){
                pcb->garrantzia+=apustutakoDirua*35;
                printf("Ruleta (Zenbakia) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*35, pcb->garrantzia, pcb->pid);
            }
            else{
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Zenbakia) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            break;
        case 1: //Jokoak: Par/Impar
            if(emaitza % 2){
                pcb->garrantzia+=apustutakoDirua;
                printf("Ruleta (Par/Impar) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            else{
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Par/Impar) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            break;
        case 2: //Jokoak: Gorri/Beltz
            if(gorriaDa(emaitza)){
                pcb->garrantzia+=apustutakoDirua;
                printf("Ruleta (Gorri/Beltz) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            else{
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Gorri/Beltz) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            break;
        case 3: //Jokoak: 1-18 edo 19-36
            if(emaitza >= 19){
                pcb->garrantzia+=apustutakoDirua;
                printf("Ruleta (Txiki/Haundi) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            else{
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Txiki/Handi) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            break;
        case 4: //Jokoak: Zutabea
            if(emaitza==0){
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Zutabea) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            else{
                switch(rand() % 3){//Zutabea aukeratu
                    case 0:
                        if(emaitza%3==0){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Zutabea) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);
                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Zutabea) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    case 1:
                        if(emaitza%3==1){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Zutabea) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);
                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Zutabea) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    case 2:
                        if(emaitza%3==2){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Zutabea) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);
                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Zutabea) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case 5: //Jokoak: Dozena
            if(emaitza==0){
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Dozena) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
            else{
                switch(rand() % 3){//Dozena aukeratu
                    case 0:
                        if(emaitza<12){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Dozena) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);
                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Dozena) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    case 1:
                        if(emaitza>12 && emaitza<=24){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Dozena) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);
                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Dozena) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    case 2:
                        if(emaitza>24){
                            pcb->garrantzia+=apustutakoDirua*2;
                            printf("Ruleta (Dozena) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid);

                        }
                        else{
                            pcb->garrantzia-=apustutakoDirua;
                            printf("Ruleta (Dozena) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        case 6: //Jokoak: Errenkada
            if(emaitza!= 0 && emaitza/3== (rand()%12) ){
                pcb->garrantzia+=apustutakoDirua*11;
                printf("Ruleta (Errenkada) irabazi da (%d -> %d) (pid:%d)", pcb->garrantzia - apustutakoDirua*11, pcb->garrantzia, pcb->pid);
            }
            else{
                pcb->garrantzia-=apustutakoDirua;
                printf("Ruleta (Errenkada) galdu da (%d -> %d) (pid:%d)", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
            }
        break;
        default:
            break;
    }
}

int gorriaDa(int n) {
    if (n == 0) return 0; // 0 berdea da

    // 1-10 eta 19-28 tarteetan: Bakoitiak gorriak dira
    if ((n >= 1 && n <= 10) || (n >= 19 && n <= 28)) {
        return n % 2;
    }
    // 11-18 eta 29-36 tarteetan: Bikoitiak gorriak dira
    if ((n >= 11 && n <= 18) || (n >= 29 && n <= 36)) {
        return n % 2;
    }

    return 0;
}
void diruaZero(PCB* pcb){
    pcb->garrantzia = (rand() % (HASIERAKO_DIRU_MAX - HASIERAKO_DIRU_MIN + 1)) + HASIERAKO_DIRU_MIN;
}

void BihurtuVIP(PCB* pcb){
    printf("\n pid:%d VIP bihurtu da, orain inork ezin dio bota CPUtik\n", pcb->pid);
    pcb->blokeatuta=1;
}

void GalduVIP(PCB* pcb){
    printf("\n pid:%d VIP abantaila galdu du\n", pcb->pid);
    pcb->blokeatuta=0;
}
