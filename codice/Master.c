#include "header.h"

int main() {
    struct struct_merce *indirizzo_attachment_merce;
    struct struct_merce *vettore_di_merci;
    struct struct_porto *indirizzo_attachment_porto;
    struct struct_porto *vettore_di_porti;
    struct struct_conteggio_nave *indirizzo_attachment_conteggio_nave;
    struct struct_conteggio_nave *conteggio_nave;
    int shared_memory_id_porto;
    int navi;
    int porti;
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
    vettore_di_merci = generatore_array_merci();

    /*apertura memoria condivisa per il passaggio delle informazioni dai porti alle navi*/
    indirizzo_attachment_porto = NULL;
    vettore_di_porti = (struct struct_porto*)malloc(sizeof(struct struct_porto) * SO_PORTI);
    shared_memory_id_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto) * SO_PORTI);
    indirizzo_attachment_porto = (struct struct_porto*)shmat(shared_memory_id_porto, NULL, 0);
    for(i = 0; i < SO_PORTI; i++){
        indirizzo_attachment_porto[i] = vettore_di_porti[i];
    }

    /*creazione vettore per il conteggio delle navi*/
    conteggio_nave = (struct struct_conteggio_nave*)malloc(sizeof(struct struct_conteggio_nave) * SO_NAVI);

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
                execvp("./Porti", args);
                exit(EXIT_SUCCESS);
                break;            
            default:
                /*inserisco l'array di strutture contenente la merce nella memoria condivisa*/
                indirizzo_attachment_merce = NULL;
                shared_memory_id_merce = memoria_condivisa_creazione(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment_merce = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0);
                indirizzo_attachment_merce[0] = vettore_di_merci[j];
                j++;
                /*waitpid(pid_processi, NULL, WUNTRACED);*/
                break;
        } 
    } j = 0;

    /*creazione processi nave*/
    for(i = 0; i < SO_NAVI; i++){
        sem_wait(semaforo_master);
        switch (pid_processi = fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;    
            case 0:
                execvp("./Navi", args);
                exit(EXIT_SUCCESS);
                break;            
            default:
                /*inserisco l'array di strutture contenente la merce nella memoria condivisa*/
                indirizzo_attachment_merce = NULL;
                shared_memory_id_merce = memoria_condivisa_creazione(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment_merce = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0);
                indirizzo_attachment_merce[0] = vettore_di_merci[j];
                j++;
                
                /*waitpid(pid_processi, NULL, WUNTRACED);*/
                indirizzo_attachment_conteggio_nave = NULL;
                shared_memory_id_conteggio_nave = memoria_condivisa_creazione(SHM_KEY_CONTEGGIO, sizeof(struct struct_conteggio_nave)*SO_NAVI);
                indirizzo_attachment_conteggio_nave = (struct struct_conteggio_nave*)shmat(shared_memory_id_conteggio_nave, NULL, 0);
                for(k = 0; k < SO_NAVI; k++){
                    indirizzo_attachment_conteggio_nave[k] = conteggio_nave[k];
                }
                break;
        }
    } j = 0;


    sem_unlink(semaforo_nome);
    sem_unlink(semaforo_nave_nome);
    sem_close(semaforo_master);
    memoria_condivisa_deallocazione(shared_memory_id_conteggio_nave);
    memoria_condivisa_deallocazione(shared_memory_id_porto);
    memoria_condivisa_deallocazione(shared_memory_id_merce);

    return 0;
}  