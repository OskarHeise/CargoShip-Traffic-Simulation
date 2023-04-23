#include "header.h"

void stampa_report_giornaliero(int giorni_simulazione, int so_merci, int so_porti, struct struct_porto *shared_memory_porto, struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche, int so_navi){
    int somma_merci_disponibili;
    int navi_con_carico, navi_senza_carico, navi_nel_porto, totale_merce_disponibile, lotti_totali;
    int i,j,k;
    FILE *fp;

    /*reset giornalieri*/
    somma_merci_disponibili = 0;
    navi_con_carico = 0;
    navi_senza_carico = 0;
    navi_nel_porto = 0;
    totale_merce_disponibile = 0;
    lotti_totali = 0;

    /*conta stats navi*/
    for(i = 0; i < so_navi; i++){
        if(shared_memory_scadenze_statistiche->navi_con_carico[i] == 1) navi_con_carico++;
        if(shared_memory_scadenze_statistiche->navi_senza_carico[i] == 1) navi_senza_carico++;
        if(shared_memory_scadenze_statistiche->navi_nel_porto[i] == 1) navi_nel_porto++;
    }

    /*LA MERCE CONSEGNATA VA SUDDIVISA IN TIPOLOGIA*/
    fp = fopen("report.txt", "a");

    fprintf(fp, "\n----------------------------------------\n");
    fprintf(fp, "REPORT GIORNO %d\n", giorni_simulazione); 
    fprintf(fp, "----------------------------------------\n"); 

    fprintf(fp, "\nMerci:\n");  

    for(i = 0; i < so_merci; i++){
        for(j = 0; j < so_porti; j++){
            if(shared_memory_porto[j].numero_lotti_merce[i] > 0) somma_merci_disponibili +=  shared_memory_porto[j].merce_offerta_quantita[i];
        } 
        fprintf(fp, "\t- Tipologia %d:\n", i); 
        fprintf(fp, "\t\tDisponibile: %10d tonnellate\n", somma_merci_disponibili);
        fprintf(fp, "\t\tConsegnata:  %10d tonnellate\n", shared_memory_scadenze_statistiche->merce_consegnata[i]); 
        somma_merci_disponibili = 0;
    }
      
    fprintf(fp, "\nNavi:\n");
    fprintf(fp, "\t- Con un carico a bordo: %10d\n", navi_con_carico);
    fprintf(fp, "\t- Senza un carico a bordo: %10d\n", navi_senza_carico);
    fprintf(fp, "\t- Nel porto: %10d\n", navi_nel_porto);                                                                              
                                                                        
    fprintf(fp, "\nPorti:\n");
    for(i = 0; i < so_porti; i++){
        for(j = 0; j < so_merci; j++){
            if(shared_memory_porto[i].numero_lotti_merce[j] > 0) {totale_merce_disponibile += shared_memory_porto[i].merce_offerta_quantita[j];} 
            lotti_totali += shared_memory_porto[i].numero_lotti_merce[j];
        }
        fprintf(fp, "\t- Numero porto: %d\n", i); 
        fprintf(fp, "\t\tMerce disponibile:         %10d tonnellate\n", totale_merce_disponibile);
        fprintf(fp, "\t\tMerce spedita e ricevuta:  %10d tonnellate, %10d tonnellate\n", shared_memory_porto[i].conteggio_merce_spedita_porto, shared_memory_porto[i].conteggio_merce_ricevuta_porto);
        fprintf(fp, "\t\tNumero di banchine libere: %10d\n", shared_memory_porto[i].numero_banchine_libere);
        fprintf(fp, "\t\tLotti rimanenti:           %10d\n", lotti_totali);

        totale_merce_disponibile = 0;
        lotti_totali = 0;
    }

    fclose(fp); 
}


void stampa_report_finale(int giorni_simulazione, int so_merci, int so_porti, struct struct_porto *shared_memory_porto, struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche, int so_navi, int so_days) {
    int somma_merci_disponibili, lotti_totali;
    int porto_offerto_maggiore_conto, porto_offerto_maggiore;
    int porto_richiesto_maggiore_conto, porto_richiesto_maggiore;
    int navi_con_carico, navi_senza_carico, navi_nel_porto;
    int i, j;
    FILE *f;

    porto_offerto_maggiore_conto = 0;
    porto_richiesto_maggiore_conto = 0;
    somma_merci_disponibili = 0;
    navi_con_carico = 0;
    navi_senza_carico = 0;
    navi_nel_porto = 0;
    lotti_totali = 0;

    /* Conta stats navi */
    for(i = 0; i < so_navi; i++) {
        if(shared_memory_scadenze_statistiche->navi_con_carico[i] == 1) navi_con_carico++;
        if(shared_memory_scadenze_statistiche->navi_senza_carico[i] == 1) navi_senza_carico++;
        if(shared_memory_scadenze_statistiche->navi_nel_porto[i] == 1) navi_nel_porto++;
    }

    f = fopen("report.txt", "a");
    if(f == NULL) {
        printf("Errore nell'apertura del file report.txt\n");
        return;
    }

    fprintf(f, "\n\n\n\n\n------------------------------------\n");
    fprintf(f, "REPORT FINALE\n");
    fprintf(f, "------------------------------------\n");

    fprintf(f, "Navi:\n");
    fprintf(f, "\tNumero di navi in mare con un carico a bordo: %d\n", navi_con_carico);
    fprintf(f, "\tNumero di navi in mare senza un carico: %d\n", navi_senza_carico);
    fprintf(f, "\tNumero di navi che occupano una banchina: %d\n", navi_nel_porto);
    fprintf(f, "Meteo:\n");
    fprintf(f, "\tNumero navi rallentate dalla tempesta: %d\n", shared_memory_scadenze_statistiche->navi_rallentate_tempesta);
    fprintf(f, "\tNumero navi affondate dalla maelstrom: %d\n", shared_memory_scadenze_statistiche->navi_affondate_maelstrom);
    fprintf(f, "\tId porti interessati dalla mareggiata:");
    for(i = 1; i <= so_days; i++) {
        fprintf(f, " %d", shared_memory_scadenze_statistiche->porti_interessati_mareggiata[i]);
    }
    fprintf(f, "\n");

    fprintf(f, "Merci:\n");
    for(i = 0; i < so_merci; i++){
        for(j = 0; j < so_porti; j++){
            if(shared_memory_porto[j].numero_lotti_merce[i] > 0) somma_merci_disponibili +=  shared_memory_porto[j].merce_offerta_quantita[i];
        } 
        fprintf(f, "\t- Tipologia %d:\n", i); 
        fprintf(f, "\t\tDisponibile: %10d tonnellate\n", somma_merci_disponibili);
        fprintf(f, "\t\tConsegnata:  %10d tonnellate\n", shared_memory_scadenze_statistiche->merce_consegnata[i]); 
        somma_merci_disponibili = 0;
    }

    fprintf(f, "Porti:\n");
    for(i = 0 ; i < so_porti; i++) {
        for(j = 0; j < so_merci; j++){
            lotti_totali += shared_memory_porto[i].numero_lotti_merce[j];
        }
        fprintf(f, "\tNumero porto: %d - Merce totale spedita e ricevuta in tonnellate: %d, %d - Numero di banchine libere: %d - Lotti rimanenti: %d\n", i, shared_memory_porto[i].conteggio_merce_spedita_porto, shared_memory_porto[i].conteggio_merce_ricevuta_porto, shared_memory_porto[i].numero_banchine_libere, lotti_totali);
        lotti_totali = 0;
    }

    for(i = 0; i < so_porti; i++) {
        if(porto_offerto_maggiore_conto < shared_memory_porto[i].conteggio_merce_spedita_porto) {
            porto_offerto_maggiore_conto = shared_memory_porto[i].conteggio_merce_spedita_porto;
            porto_offerto_maggiore = i;
        }
        if(porto_richiesto_maggiore_conto < shared_memory_porto[i].conteggio_merce_ricevuta_porto) {
            porto_richiesto_maggiore_conto = shared_memory_porto[i].conteggio_merce_ricevuta_porto;
            porto_richiesto_maggiore = i;
        }
    }

    fprintf(f, "\nIl porto con il maggior conteggio di merce spedita e': %d con %d tonnellate di merce spedita\n", porto_offerto_maggiore, porto_offerto_maggiore_conto);
    fprintf(f, "\nIl porto con il maggior conteggio di merce ricevuta e': %d con %d tonnellate di merce ricevuta\n", porto_richiesto_maggiore, porto_richiesto_maggiore_conto);

    fclose(f);
}





















int main() {
    int giorni_simulazione;
    int indirizzo_attachment_shared_memory_giorni;
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    int indirizzo_attachment_shared_memory_nave;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    struct struct_giorni *shared_memory_giorni;
    struct struct_porto *shared_memory_porto;
    struct struct_nave *shared_memory_nave;
    int i, j;
    int messaggio_id;
    int numero_merci_richieste, indice_merci_richieste;
    int so_fill_inverso, aggiunta_parziale, numero_merci_divisione, valore_richiesto;


    /*cattura delle variabili*/
    FILE* config_file;
    FILE *fp;
    int so_days;
    int so_merci;
    int so_porti;
    int so_navi;
    int so_fill, max_vita, min_vita;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        return 1;
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
        if (strcmp(name, "SO_FILL") == 0) {
            so_fill = value;
        }
        if (strcmp(name, "SO_MAX_VITA") == 0) {
            max_vita = value;
        }
        if (strcmp(name, "SO_MIN_VITA") == 0) {
            min_vita = value;
        }
    }

    fclose(config_file);







    /*voglio scrivere sul file*/
    fp = fopen("report.txt", "w");

    /*srand*/
    srand(getpid());

    /*inizializzo i giorni della simulazione a 0*/
    giorni_simulazione = 1;

    printf("\n\n\n");

    /*apertura shared memory per il conteggio dei giorni*/
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_get(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi, SHM_W);
    shared_memory_nave = (struct struct_nave*)shmat(indirizzo_attachment_shared_memory_nave, NULL, 0);
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO,  sizeof(struct struct_porto) * so_porti, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);

    shared_memory_giorni->giorni = giorni_simulazione;
    printf("Simulazione... 0 %%\n");







    /*gestione del tempo e del dump simulazione*/
    while (shared_memory_giorni->giorni <= so_days) {
        stampa_report_giornaliero(shared_memory_giorni->giorni, so_merci, so_porti, shared_memory_porto, shared_memory_scadenze_statistiche, so_navi); /*stampa report giornaliero*/
        sleep(1);
        printf("Simulazione... %d %%\n", (shared_memory_giorni->giorni * 100) / so_days);

        /*aggiornamento scadenze porto*/
        for(i = 0; i < so_porti; i++){
            for(j = 0; j < so_merci; j++){
                if(shared_memory_porto[i].merce_offerta_tempo_vita[j] <= 0){
                    shared_memory_scadenze_statistiche->merce_scaduta_porto[j] += shared_memory_porto[i].merce_offerta_quantita[j];
                    shared_memory_porto[i].merce_offerta_quantita[j] = 0;
                }else{
                    shared_memory_porto[i].merce_offerta_tempo_vita[j] -= 1;
                }
            }
        }

        /*aggiornamento generazione merce nei porti*/
        numero_merci_richieste = rand()%(so_merci); 
        aggiunta_parziale = so_fill / so_days; numero_merci_divisione = 0;
        for(i = 0; i < numero_merci_richieste  && aggiunta_parziale > 0; i++){
            if(i == numero_merci_richieste-1){
                indice_merci_richieste = rand()%(so_merci);
                porto.merce_richiesta_quantita[indice_merci_richieste] += aggiunta_parziale;
            }else{
                indice_merci_richieste = rand()%(so_merci);
                valore_richiesto = rand() % aggiunta_parziale;
                aggiunta_parziale -= valore_richiesto;
                porto.merce_richiesta_quantita[indice_merci_richieste] += (valore_richiesto);
            }
        }
        for(i = 0; i < so_merci; i++){
            if(porto.merce_richiesta_quantita[i] < 1) numero_merci_divisione++;
        }
        aggiunta_parziale = so_fill / so_days; 
        for(i = 0; i < so_merci; i++){
            if(porto.merce_richiesta_quantita[i] < 1){
                valore_richiesto = rand() % aggiunta_parziale; 
                aggiunta_parziale -= valore_richiesto;
                if(valore_richiesto == 0) porto.merce_offerta_quantita[i] = 1;
                else porto.merce_offerta_quantita[i] = valore_richiesto; 
                porto.merce_offerta_tempo_vita[i] = rand()%(max_vita - min_vita +1) + min_vita;
                porto.numero_lotti_merce[i] = rand()%10+2;
            }else{
                porto.merce_offerta_quantita[i] = 0;
                porto.merce_offerta_tempo_vita[i] = 0;
                porto.numero_lotti_merce[i] = 0;
            }
        }


        /*aggiornamento scadenze nave*/
        for(i = 0; i < so_navi; i++){
            for(j = 0; j < so_merci; j++){
                if(shared_memory_nave[i].merce_nave.tempo_vita_merce[j] <= 0){
                    shared_memory_scadenze_statistiche->merce_scaduta_nave[j] += shared_memory_nave[i].merce_nave.dimensione_merce[j];
                    shared_memory_nave[i].merce_nave.dimensione_merce[j] = 0;
                }else{
                    shared_memory_nave[i].merce_nave.tempo_vita_merce[j]--;
                }
            }
        }

        shared_memory_giorni->giorni++;
    }




    /*stampo il report finale*/
    stampa_report_finale(shared_memory_giorni->giorni, so_merci, so_porti, shared_memory_porto, shared_memory_scadenze_statistiche, so_navi, so_days);

    /*stampo il messaggio finale*/
    printf("\nFine caricamento, report presente nel file \"report.txt\" \n");



    /*creazione coda di messaggi, mando il messaggio FINE al master così da terminare il programma*/
    messaggio_id = coda_messaggi_get_id(MSG_KEY);
    strcpy(messaggio.messaggio_testo, "FINE");
    messaggio.messaggio_tipo = 1;
    msgsnd(messaggio_id, &messaggio, sizeof(messaggio), 0);




    /*termino il processo ouput*/
    exit(EXIT_SUCCESS);
}