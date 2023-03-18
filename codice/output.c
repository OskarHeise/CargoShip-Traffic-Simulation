#include "header.h"

/*  FARE LA SHARED MEMORY   */

void stampa_risultati_giornalieri(void);
void stampa_risultati_finali(void);

/*thread per la stampa dei report*/
/*void stampa_risultati_giornalieri(){
    int i;

    /*aggiornamento della data di scadenza*/
    /*merce_nella_nave[indice_nave].tempo_vita_merce--;
    informazioni_porto[porto_piu_vicino].merce_offerta_tempo_vita--;
    sleep(1);
    if(pid_di_stampa == 1 && numero_giorno < SO_DAYS+1){
        /*print_report_giornaliero(conteggio_nave, merce_nella_nave, numero_giorno, informazioni_porto, somma_merci_disponibili, conteggio_merce_consegnata); /*print report*/
    /*}
    /*numero_giorno++;
}

/*void stampa_risultati_finali(){
    int i;

    /*print report finale*/
    /*print_report_finale(conteggio_nave, merce_nella_nave, numero_giorno, informazioni_porto, somma_merci_disponibili, conteggio_merce_consegnata, statistiche.totale_merce_generata_inizialmente, statistiche.merce_scaduta_in_nave, statistiche.merce_scaduta_in_porto, tappe_nei_porti);

    sleep(1);

    /*ricevo messaggio dal porto che è tutto ok*/
    /*messaggio_id = coda_messaggi_get_id(MSG_KEY);
    msgrcv(messaggio_id, &messaggio, sizeof(messaggio), 1, 0);
    printf("%s\n\n", messaggio.messaggio_testo);
    msgctl(messaggio_id, IPC_RMID, NULL);


    sleep(1);
}*/


main(){
    /*printf("\nsto facendo un output!\n");*/
}