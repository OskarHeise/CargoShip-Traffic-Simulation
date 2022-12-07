#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(){
    srand(time(NULL));
    struct struct_nave *nave;
    nave = (struct struct_nave*)malloc(sizeof(struct struct_nave));
    pid_t pid = getpid();
    sem_t semaforo_navi;

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);   
    struct struct_merce *vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    sem_init(&semaforo_navi, 1, 1);
    nave = generatore_array_navi();

    /*for(int i = 0; i < NUMERO_TOTALE_MERCI; i++){
        sem_wait(&semaforo_navi);
        while(vettore_risultato[i].tempo_vita_merce != 0){
            printf("Valore merce: %d, e vita: %d\n", vettore_risultato[i].dimensione_merce, vettore_risultato[i].tempo_vita_merce);
            sleep(1);
            vettore_risultato[i].tempo_vita_merce = vettore_risultato[i].tempo_vita_merce-1;
        }
        sem_post(&semaforo_navi);
    }*/
    
    /*gestisco i semafori e permetto ad un solo processo alla volta di agire*/

    return 0;
}


