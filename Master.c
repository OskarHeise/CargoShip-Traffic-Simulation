#include "header.h"

int main() {
    srand(time(NULL));  
    int navi;
    int porti;
    int i;
    int j;
    int id_merce_temporaneo;
    int dimensione_merce_temporaneo;
    int tempo_vita_merce_temporaneo;
    char **args;
    struct struct_merce* vettore_di_merci;
    pid_t pid_nave, pid_porto;
    int id;
    sem_t semaforo_navi;

    /*generazione del vettore di merci, creazione e inizializzazione dei semafori*/
    sem_init(&semaforo_navi, NO_NAVI, 1); /*secondo argomento è 0 thread, non zero quando viene usato con i processi; ultimo argomento è il valore iniziale del semaforo_navi*/

    vettore_di_merci = generatore_array_merci();
   
    /*creazione processi nave*/
    for(i = 0; i < NO_NAVI; i++){
        pid_nave = fork();
        
        switch (pid_nave){
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
                sem_wait(&semaforo_navi);
                struct struct_merce *indirizzo_attachment;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
                sem_post(&semaforo_navi);
                break;
            } 
    } i = 0;    

    /*creazione processi porto*/

    for(i = 0; i < NO_PORTI; i++){ 
        pid_porto = fork();
        
        switch (pid_porto){
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
                struct struct_merce *indirizzo_attachment;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
                break;
            }  
    }

    memoria_condivisa_deallocazione(shared_memory_id);

    /*aspetto che tutti i processi siano pronti per lanciare il timer*/


    /*faccio partire il tempo*/
    alarm(SO_DAYS);
    printf("\n\n\n\n\n\nTempo della simulazione partito: %d sec\n", SO_DAYS);


    return 0;
}                                                       