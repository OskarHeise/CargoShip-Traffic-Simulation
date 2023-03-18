#include "header.h"

int main() {
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze;
    int i;
    int j;
    int k;
    int fd;
    char **args;
    pid_t pid_processi;
    sem_t *semaforo_master;
    int *value;
    j = 0;
    srand(time(NULL));
    
    /*creazione semaforo e generazione merci*/
    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0644, 1);

    /*apertura di entrambe le memorie condivise che mi saranno utili successivamente*/
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
                exit(EXIT_SUCCESS);
                break;            
            default:
                break;
        } 
    } j = 0;

    /*creazione processi nave*/
    for(i = 0; i < (SO_NAVI); i++){
        sem_wait(semaforo_master); /*da modificare poi*/
        switch (pid_processi = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;    
            case 0:
                execvp("./navi", args);
                exit(EXIT_SUCCESS);
                break;            
            default:
                break;
        }
    } j = 0;


    sleep(0.5);
    /*chiusura delle risorse IPC*/
    sem_unlink(semaforo_nome);
    sem_unlink(semaforo_nave_nome);
    sem_close(semaforo_master);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_porto);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_scadenze);

    return 0;
}  