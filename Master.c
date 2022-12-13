#include "header.h"

int main() {
    struct struct_merce *vettore_di_merci;
    struct struct_merce *indirizzo_attachment;
    int message_queue_id;
    int navi;
    int porti;
    int i;
    int j;
    int fd;
    char **args;
    pid_t pid_processi;
    sem_t *semaforo;
    int *value;
    srand(time(NULL)); 

    printf("\n\n\n\n");

    vettore_di_merci = generatore_array_merci();
    j = 0;
    
    
    /*creazione processi nave*/
    for(i = 0; i < NO_NAVI; i++){
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
                indirizzo_attachment = NULL;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                indirizzo_attachment[0] = vettore_di_merci[j];
                j++;
                waitpid(pid_processi, NULL, WUNTRACED);
                break;
        }
    }

    printf("\n-------------------\n");
    j = 0;
    
    /*creazione processi porto*/
    for(i = 0; i < NO_PORTI; i++){
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
                indirizzo_attachment = NULL;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                indirizzo_attachment[0] = vettore_di_merci[j];
                j++;
                waitpid(pid_processi, NULL, WUNTRACED);
                break;
        } 
    }

    memoria_condivisa_deallocazione(shared_memory_id);
    return 0;
}                                                       