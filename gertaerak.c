#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gertaerak.h"
#include "prozesuak.h"
#include "CPU.h"

PCB *VIPLista[VIP_MAXIMO] = {NULL};
int AzkenVIP = 0;
//temporizadorek batek ruleta aktibatuko du eta ruleta ausaz gertaera bat exekutatuko du
void jokoaAukeratu(PCB* pcb){
    if(pcb->garrantzia<=0){
        diruaZero(pcb);
    }
    int gertaeraKantitatea = 3;
    int jokoa = rand() % gertaeraKantitatea;
    switch(jokoa){
        case 0:
            printf("\n======= RULETA =======\n");
            ruleta(pcb);
            break;
        case 1:
            printf("\n===== BLACKJACK =====\n");
            blackJack(pcb);
            break;
        case 2:
            printf("\n=== TXANPON-MAKINA ===\n");
            txanpon_Makina(pcb);
            break;
        default:
            break;
    }
    printf("\n======================\n");
    if(pcb->garrantzia>VIP_IZATEKO_GARRANTZIA){
        BihurtuVIP(pcb);
    }
    else if(pcb->blokeatuta==1){
        GalduVIP(pcb);
    }
}

void txanpon_Makina(PCB* pcb){
    if(pcb==NULL){
        printf("Txanpon-makinak jaso duen pcb-a NULL da");
    }
    int apustutakoDirua = (rand() % (pcb->garrantzia)) + 1;

    int gurpila1 = rand() % 10;
    int gurpila2 = rand() % 10;
    int gurpila3 = rand() % 10;

    printf("Txanpon-makina emaitza: [%d | %d | %d] ", gurpila1, gurpila2, gurpila3);

    if (gurpila1 == gurpila2 && gurpila2 == gurpila3) {
        int irabazia = apustutakoDirua * 10;
        pcb->garrantzia += irabazia;
        printf(" JACKPOT !!! (%d -> %d) (pid:%d)", pcb->garrantzia - irabazia, pcb->garrantzia, pcb->pid);
    }
    else if (gurpila1 == gurpila2 || gurpila1 == gurpila3 || gurpila2 == gurpila3) {
        int irabazia = apustutakoDirua * 2;
        pcb->garrantzia += irabazia;
        printf(" Bikotea! Irabazi duzu (%d -> %d) (pid:%d)", pcb->garrantzia - irabazia, pcb->garrantzia, pcb->pid);
    }
    else {
        pcb->garrantzia -= apustutakoDirua;
        printf(" Galdu duzu (%d -> %d) (pid:%d)",
               pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid);
    }
}

void blackJack(PCB* pcb){
    if(pcb==NULL){
        printf("blackJackak jaso duen pcb-a NULL da");
    }
    int apustutakoDirua = (rand() % (pcb->garrantzia)) + 1;

    int jokalaria = 0;
    int dealer = 0;
    int momentukoKarta = 0;

    for(int i=0;i<2;i++){
        momentukoKarta=blackJackKartaJaso();
        jokalaria+=momentukoKarta;
        if(momentukoKarta==1 && jokalaria+10<=21){
            jokalaria+=10; //AS kartak 1 edo 11 balioa eduki ahal du
        }
    }
    for(int i=0;i<2;i++){
        momentukoKarta=blackJackKartaJaso();
        dealer+=momentukoKarta;
        if(momentukoKarta==1 && dealer+10<=21){
            dealer+=10; //AS kartak 1 edo 11 balioa eduki ahal du
        }
    }
    // Jokalariaren BlackJack!
    if(jokalaria == 21){
        pcb->garrantzia += apustutakoDirua * 2;
        printf("BlackJack (BlackJack!) irabazi da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia - apustutakoDirua*2, pcb->garrantzia, pcb->pid, jokalaria, dealer);
        return;
    }

    while(jokalaria<=16){
        momentukoKarta=blackJackKartaJaso();
        jokalaria+=momentukoKarta;
        if(momentukoKarta==1 && jokalaria+10<=21){
            jokalaria+=10; //AS kartak 1 edo 11 balioa eduki ahal du
        }
    }
    // Jokalaria BUST (21 gainditu)
    if(jokalaria > 21){
        pcb->garrantzia -= apustutakoDirua;
        printf("BlackJack (BUST!) galdu da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid, jokalaria, dealer);
        return;
    }

    while(dealer<=16){
        momentukoKarta=blackJackKartaJaso();
        dealer+=momentukoKarta;
        if(momentukoKarta==1 && dealer+10<=21){
            dealer+=10; //AS kartak 1 edo 11 balioa eduki ahal du
        }
    }
    //Emaitzak
    if(dealer > 21){
        pcb->garrantzia += apustutakoDirua;
        printf("BlackJack (Dealer BUST) irabazi da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia - apustutakoDirua, pcb->garrantzia, pcb->pid, jokalaria, dealer);
    }
    else if(jokalaria > dealer){
        pcb->garrantzia += apustutakoDirua;
        printf("BlackJack irabazi da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia - apustutakoDirua, pcb->garrantzia, pcb->pid, jokalaria, dealer);
    }
    else if(jokalaria == dealer){
        printf("BlackJack berdinketa da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia, pcb->garrantzia, pcb->pid, jokalaria, dealer);
    }
    else{
        pcb->garrantzia -= apustutakoDirua;
        printf("BlackJack galdu da (%d -> %d) (pid:%d) [J:%d D:%d]", pcb->garrantzia + apustutakoDirua, pcb->garrantzia, pcb->pid, jokalaria, dealer);
    }


}

int blackJackKartaJaso(){
    int karta = (rand() % 13) + 1;
    if(karta>10){
        karta=10; //J,Q,K
    }
    return karta;
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
    for(int i=0; i<VIP_MAXIMO; i++){
        if(VIPLista[i]==NULL){
            VIPLista[i]=pcb;
            AzkenVIP=i;
            return;
        }
    }
    if(AzkenVIP+1<VIP_MAXIMO){
        AzkenVIP++;
    }
    else{
        AzkenVIP=0;
    }
    VIPLista[AzkenVIP]->blokeatuta=0;
    VIPLista[AzkenVIP]=pcb;

}

void GalduVIP(PCB* pcb){
    printf("\n pid:%d VIP abantaila galdu du\n", pcb->pid);
    pcb->blokeatuta=0;
    for(int i=0; i<VIP_MAXIMO;i++){
        if(VIPLista[i]->pid==pcb->pid){
            VIPLista[i]=NULL;
            return;
        }
    }
}
