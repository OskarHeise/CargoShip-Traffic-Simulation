#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(){
    struct struct_porto porto;
    struct struct_merce *vettore_risultato;
    pid_t pid;
    sem_t semaforo_porti;
    int result;
    srand(getpid());
    
    pid = getpid();

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);   
    vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    /*genero tutte le informazioni del porto*/
    porto.merce_offerta_richiesta = generatore_merce_offerta_richiesta();
    porto.posizione_porto = generatore_posizione_iniziale_porto(pid);

    /*creazione della coda di messaggi*/

    /*stampo per vedere se tutto funziona*/
    printf("PID porto: %d\n", getpid());
    printf("Resto PID -> %d\n", pid % 4);
    printf("Posizione porto: (%f, %f)\n", porto.posizione_porto[0], porto.posizione_porto[1]);
    printf("Merce offerta -> tipo: %d, quantita: %d\n", porto.merce_offerta_richiesta[0], porto.merce_offerta_richiesta[1]);
    printf("Merce richiesta -> tipo: %d, quantita: %d\n", porto.merce_offerta_richiesta[2], porto.merce_offerta_richiesta[3]);
    printf("\n");


    /*coda_messaggi_deallocazione(coda_messaggi_id);*/

    return 0;
}