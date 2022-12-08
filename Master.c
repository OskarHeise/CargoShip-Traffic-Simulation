#include "header.h"

int main() {
    struct struct_merce* vettore_di_merci;
    struct struct_merce *indirizzo_attachment;
    int message_queue_id;
    int navi;
    int porti;
    int i;
    int j;
    char **args;
    pid_t pid_nave, pid_porto;
    sem_t *semaforo_navi;
    srand(time(NULL)); 

    /*generazione del vettore di merci, creazione e inizializzazione dei semafori*/
    /*sem_init(semaforo_navi, NO_NAVI, 1); /*secondo argomento è 0 thread, non zero quando viene usato con i processi; ultimo argomento è il valore iniziale del semaforo_navi*/

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
                indirizzo_attachment = NULL;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
                break;
            } 
    } i = 0;    

    /*creazione processi porto*/

    for(i = 0; i < NO_PORTI; i++){
        pid_porto = fork();

        if(i == 0){pid_porto_iniziale = getpid() + NO_NAVI + 1;}
        
        switch (pid_porto){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
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
                for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
                break;
            } 
    } i = 0;  

    memoria_condivisa_deallocazione(shared_memory_id);

    printf("\n\nPID PORTO INIZIALE: %d\n", pid_porto_iniziale);

    return 0;
}                                                       