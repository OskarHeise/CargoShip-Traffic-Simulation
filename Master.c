#include "header.h"

int main() {
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
    srand(time(NULL));  

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
                struct struct_merce *indirizzo_attachment;
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                for(j = 0; j < NO_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
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
                shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);
                indirizzo_attachment = (struct struct_merce*)shmat(shared_memory_id, NULL, 0);
                for(j = 0; j < NO_MERCI; j++){
                    indirizzo_attachment[j] = vettore_di_merci[j];
                }
                waitpid(pid_porto, NULL, WUNTRACED);
                break;
            }  
    }

    memoria_condivisa_deallocazione(shared_memory_id);

    return 0;
}                                                       