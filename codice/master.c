#include "header.h"

volatile int num_child_ready;

void handle_child_ready(int sig) {
    num_child_ready++;
}

int main() {
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze;
    int indirizzo_attachment_shared_memory_nave;
    int i;
    char c;
    char **args;
    pid_t pid_processi;
    sem_t *semaforo_master;
    pid_t pid_figli[SO_NAVI]; /*salvo i pid dei processi figli in un array*/
    
    num_child_ready = 0;
    
    /*creazione semaforo e generazione merci*/
    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0666, 1);

    /*apertura di entrambe le memorie condivise che mi saranno utili successivamente*/
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_creazione(SHM_KEY_NAVE,  sizeof(struct struct_nave) * SO_NAVI);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto) * SO_PORTI);
    indirizzo_attachment_shared_memory_scadenze = memoria_condivisa_creazione(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze));

    printf("\n\n\n");    


    /*creazione processi porto*/
    for(i = 0; i < SO_PORTI; i++){  
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
    for(i = 0; i < SO_NAVI; i++){
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
    while (num_child_ready < SO_NAVI) {
        pause();
    }
    for (i = 0; i < SO_NAVI; i++){
        kill(pid_figli[i], SIGUSR1);
    }

    /* aspetta che tutti i processi figli terminino */
    for (i = 0; i < SO_NAVI; i++) {
        wait(NULL);
    }

    /*chiusura delle risorse IPC*/
    sem_close(semaforo_master);
    sem_unlink(semaforo_nome);
    sem_unlink(semaforo_nave_nome);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_nave);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_porto);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_scadenze);

    return 0;
}  