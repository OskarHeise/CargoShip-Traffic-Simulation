#include "header.h"

#define NUM_THREADS 3

void *thread_function_tempesta(void *arg) {
    int indice_tempesta;
    FILE* config_file;
    int so_navi, so_days, so_storm_duration;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_nave *shared_memory_nave;
    struct struct_giorni *shared_memory_giorni;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    float tempo_di_stop1;
    float tempo_di_stop2;

    srand(getpid());

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_STORM_DURATION") == 0) {
            so_storm_duration = value;
        }
    }
    fclose(config_file);

    /*apertura shared memory */
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_get(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi, SHM_W);
    shared_memory_nave = (struct struct_nave*)shmat(indirizzo_attachment_shared_memory_nave, NULL, 0);
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);

    tempo_di_stop1 = ((float)so_storm_duration / 24)*1000000;
    tempo_di_stop2 = 1000000 - tempo_di_stop1;

    while (shared_memory_giorni->giorni <= so_days){
        /*gestione effettiva della tempesta*/
        indice_tempesta = rand()%so_navi;
        shared_memory_scadenze_statistiche->navi_rallentate_tempesta++;
        kill(shared_memory_nave[indice_tempesta].pid_nave, SIGSTOP);
        usleep(tempo_di_stop1);
        kill(shared_memory_nave[indice_tempesta].pid_nave, SIGCONT);
        /*fine gestione effettiva della tempesta*/
        usleep(tempo_di_stop2);
    }

    shared_memory_scadenze_statistiche->navi_rallentate_tempesta = (so_days * 24) / so_storm_duration;

    pthread_exit(NULL);
}




void *thread_function_mareggiata(void *arg) {
    int indice_mareggiata;
    FILE* config_file;
    int so_porti, so_days, so_swell_duration;
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_porto *shared_memory_porto;
    struct struct_giorni *shared_memory_giorni;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    float tempo_di_stop1;
    float tempo_di_stop2;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
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
        if (strcmp(name, "SO_SWELL_DURATION") == 0) {
            so_swell_duration = value;
        }
    }
    fclose(config_file);

    /*apertura shared memory */
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO,  sizeof(struct struct_porto) * so_porti, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);

    tempo_di_stop1 = ((float)so_swell_duration / 24)*1000000;
    tempo_di_stop2 = 1000000 - tempo_di_stop1;

    while (shared_memory_giorni->giorni <= so_days) {
        indice_mareggiata = rand()%so_porti;
        shared_memory_scadenze_statistiche->porti_interessati_mareggiata[shared_memory_giorni->giorni] = shared_memory_porto[indice_mareggiata].pid_porto - getppid() - 1; /*il porto interessato, ricordarsi di fare la divisione dell'indice*/
        kill(shared_memory_porto[indice_mareggiata].pid_porto, SIGSTOP);
        usleep(tempo_di_stop1);
        kill(shared_memory_porto[indice_mareggiata].pid_porto, SIGCONT);
        usleep(tempo_di_stop2);
    }

    pthread_exit(NULL);
}





void *thread_function_maelstrom(void *arg) {
    int indice_maelstrom;
    FILE* config_file;
    int so_navi, so_days, so_maelstrom;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_nave *shared_memory_nave;
    struct struct_giorni *shared_memory_giorni;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    float tempo_di_stop;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_MAELSTROM") == 0) {
            so_maelstrom = value;
        }
    }
    fclose(config_file);

    /*apertura shared memory */
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_get(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi, SHM_W);
    shared_memory_nave = (struct struct_nave*)shmat(indirizzo_attachment_shared_memory_nave, NULL, 0);
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);

    shared_memory_scadenze_statistiche->navi_affondate_maelstrom = (so_days * 24) / so_maelstrom;
    tempo_di_stop = ((float)so_maelstrom / 24)*1000000;

    if((so_navi - ((so_days * 24) / so_maelstrom)) > 12){
        while (shared_memory_giorni->giorni <= so_days){
            indice_maelstrom = rand()%so_navi;
            kill(shared_memory_nave[indice_maelstrom].pid_nave, SIGKILL);
            so_navi--;
            usleep(tempo_di_stop);
        }
    }

    pthread_exit(NULL);
}

























int main() {
    pthread_t threads[NUM_THREADS];
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_giorni *shared_memory_giorni;
    int i;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_days;

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
    }
    fclose(config_file);

    /*apertura shared memory */
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);

    /* creazione dei thread */
    pthread_create(&threads[0], NULL, thread_function_tempesta, NULL);
    pthread_create(&threads[1], NULL, thread_function_mareggiata, NULL);
    pthread_create(&threads[2], NULL, thread_function_maelstrom, NULL);

    /*esecuzione delle attività per so_days secondi*/
    while (shared_memory_giorni->giorni <= so_days) {
        sleep(0.5);
    }


    /* attesa della terminazione dei thread*/
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }    


    exit(EXIT_SUCCESS);

    return 0;
}