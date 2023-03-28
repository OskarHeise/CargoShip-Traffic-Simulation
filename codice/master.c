#include "header.h"

volatile int num_child_ready;

void handle_child_ready(int sig) {
    num_child_ready++;
}

int main() {
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    pid_t pid_processi;
    sem_t *semaforo_master;
    pid_t *pid_figli; /*salvo i pid dei processi figli in un array*/
    int i;
    char c;
    char **args;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_navi, so_porti;

    /*"cattura" delle variabili*/
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
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
    }

    fclose(config_file);

    /*inizializzazione della variabile necessaria per la chiusura di tutti i processi nave*/
    num_child_ready = 0;
    pid_figli = realloc(pid_figli, so_navi * sizeof(pid_t));
    
    /*creazione semaforo e generazione merci*/
    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0666, 1);

    /*apertura di entrambe le memorie condivise che mi saranno utili successivamente*/
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_creazione(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto) * so_porti);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_creazione(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche));
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_creazione(SHM_KEY_GIORNO,  sizeof(struct struct_giorni));

    printf("\n\n\n");    

    /*creazione processi porto*/
    for(i = 0; i < so_porti; i++){  
        sem_wait(semaforo_master);
        switch (pid_processi = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() del Porto");
                exit(EXIT_FAILURE);
                break;    
            case 0:
                execvp("./porti", args);
                break;            
            default:
                signal(SIGCHLD, handle_child);
                pause();
                break;
        } 
    }

    printf("\n\n\n");

    /*creazione processi nave*/
    for(i = 0; i < so_navi; i++){
        sem_wait(semaforo_master); 
        switch (pid_figli[i] = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;    
            case 0: 
                execvp("./navi", args);
                break;            
            default: 
                signal(SIGUSR1, handle_child_ready);
                break;
        }
    }

    /*gestione delle "fermate" e delle "ripartenze"*/
    while (num_child_ready < so_navi) {
        pause();
    }
    for (i = 0; i < so_navi; i++){
        kill(pid_figli[i], SIGUSR1);
    }

    /*apro il processo ouput.c*/
    switch (pid_processi = fork()){
        case -1:
            fprintf(stderr, "Errore nella fork() della Nave");
            exit(EXIT_FAILURE);
            break;    
        case 0: 
            execvp("./output", args);
            break;            
        default: 
            break;
    }


    /* aspetta che tutti i processi figli terminino */
    for (i = 0; i < so_navi + 1; i++) {
        wait(NULL);
    }


    /*chiusura delle risorse IPC*/
    sem_close(semaforo_master);
    sem_unlink(semaforo_nome);
    sem_unlink(semaforo_nave_nome);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_giorni);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_nave);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_porto);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_scadenze_statistiche);

    return 0;
}  