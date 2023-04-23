#include "header.h"

int main(int argc, char **argv){
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    double *temp_posizione_porto;
    sem_t *semaforo_master;
    int i, numero_merci_richieste, indice_merci_richieste, so_fill_inverso, aggiunta_parziale, numero_merci_divisione, so_fill_porto, valore_richiesto;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_porti, so_days, so_merci, so_fill, max_vita, min_vita, so_banchine;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        return 1;
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
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
        if (strcmp(name, "SO_BANCHINE") == 0) {
            so_banchine = value;
        }
    }

    fclose(config_file);















    /*srand*/
    srand(getpid());

    /*gestione semafori*/
    semaforo_master = sem_open(semaforo_nome, O_RDWR);     

    /*accedo alla memoria condivisa*/
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto) * so_porti, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0); 
   

    /*genero tutte le informazioni del porto*/
    if(shared_memory_scadenze_statistiche->conto_indice_porto == 0){shared_memory_scadenze_statistiche->conto_so_fill = so_fill;}
    so_fill_porto = rand() % (shared_memory_scadenze_statistiche->conto_so_fill / so_porti) + 1;
    shared_memory_scadenze_statistiche->conto_so_fill -= so_fill_porto;
    

    if(so_merci == 1) numero_merci_richieste = rand() % 2;
    else numero_merci_richieste = rand()%(so_merci)+1; 

    aggiunta_parziale = so_fill_porto; numero_merci_divisione = 0;
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

    aggiunta_parziale = so_fill_porto; 
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

    temp_posizione_porto = generatore_posizione_iniziale_porto(getpid(), getppid());
    porto.posizione_porto_X = temp_posizione_porto[0];
    porto.posizione_porto_Y = temp_posizione_porto[1];
    porto.numero_banchine_libere = rand()%so_banchine+2;
    generatore_semaforo_banchine_nome(getpid(), porto.semaforo_banchine_nome);
    porto.semaforo_banchine = sem_open(porto.semaforo_banchine_nome, O_CREAT, 0666, porto.numero_banchine_libere);
    porto.conteggio_merce_ricevuta_porto = 0;
    porto.conteggio_merce_spedita_porto = 0;
    porto.pid_porto = getpid();    











    

    /*caricamento delle informazioni del porto in memoria condivisa*/
    shared_memory_porto[shared_memory_scadenze_statistiche->conto_indice_porto] = porto;

    /*aggiornamento statistica*/
    for(i = 0; i < so_merci; i++){
        shared_memory_scadenze_statistiche->merce_generata_inizialmente[i] += porto.merce_offerta_quantita[i];
    }
    shared_memory_scadenze_statistiche->conto_indice_porto++;






    /*aggiorno il semaforo e lo chiudo*/
    sem_post(semaforo_master);
    sem_close(semaforo_master);
    exit(EXIT_SUCCESS);
    return 0;
}