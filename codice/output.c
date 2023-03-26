#include "header.h"

void stampa_report_giornaliero(int giorni_simulazione){
    printf("GIORNO:  %d\n", giorni_simulazione);
}

void stampa_report_finale(int giorni_simulazione){
    printf("GIORNO FINALE:  %d\n", giorni_simulazione);
}

int main() {
    int giorni_simulazione;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_giorni *shared_memory_giorni;

    giorni_simulazione = 0;

    /*apertura shared memory per il conteggio dei giorni*/
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);
    shared_memory_giorni->giorni = giorni_simulazione;

    while (shared_memory_giorni->giorni < SO_DAYS) {
        stampa_report_giornaliero(shared_memory_giorni->giorni);
        sleep(1);
        shared_memory_giorni->giorni++;
    }

    stampa_report_finale(shared_memory_giorni->giorni);
    printf("Messaggio di terminazione\n");

    /*termino il processo ouput*/
    exit(0);
}