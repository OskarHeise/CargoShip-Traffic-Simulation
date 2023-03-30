#include "header.h"

int main(int argc, char **argv){
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_semaforo;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    double *temp_posizione_porto;
    sem_t *semaforo_master;
    int messaggio_id;
    int i;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_porti, so_days;

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
    }

    fclose(config_file);

    /*srand*/
    srand(getpid());

    /*gestione semafori*/
    semaforo_master = sem_open(semaforo_nome, O_RDWR);

    /*genero tutte le informazioni del porto*/
    do{
        porto.merce_richiesta_id = generatore_merce_richiesta_id();
        porto.merce_richiesta_quantita = generatore_merce_richiesta_quantita() / so_days;
        porto.merce_offerta_id = generatore_merce_offerta_id();
        porto.merce_offerta_quantita = generatore_merce_offerta_quantita(porto.merce_richiesta_quantita) / so_days;
        porto.merce_offerta_tempo_vita = generatore_tempo_vita_merce();
    }while(porto.merce_richiesta_id == porto.merce_offerta_id);
    temp_posizione_porto = generatore_posizione_iniziale_porto(getpid(), getppid());
    porto.posizione_porto_X = temp_posizione_porto[0];
    porto.posizione_porto_Y = temp_posizione_porto[1];
    porto.numero_banchine_libere = generatore_banchine_porto();
    porto.numero_lotti_merce = generatore_lotti_merce();
    porto.conteggio_merce_ricevuta_porto = 0;
    porto.conteggio_merce_spedita_porto = 0;
    porto.pid_porto = getpid();    

    /*inserisco le informazioni nella memoria condivisa, nella posizione giusta*/
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto) * so_porti, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    shared_memory_porto[(getpid() - getppid())-1] = porto;
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);

    /*aggiornamento statistica*/
    shared_memory_scadenze_statistiche->merce_generata_inizialmente[porto.merce_offerta_id] += porto.merce_offerta_quantita;

    sem_post(semaforo_master);
    sem_close(semaforo_master);
    exit(EXIT_SUCCESS);
    return 0;
}