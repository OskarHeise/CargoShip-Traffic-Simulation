#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(int argc, char **argv){
    struct struct_merce *vettore_risultato;
    int result;
    sem_t *semaforo_porto;
    srand(getpid());

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);   
    vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    /*genero tutte le informazioni del porto*/
    porto.merce_offerta_richiesta = generatore_merce_offerta_richiesta();
    porto.posizione_porto = generatore_posizione_iniziale_porto(getpid());

    /*stampo per vedere se le merci sono state portate in maniera corretta*/
    printf("PID PORTO: %d\n", getpid());
    printf("ID merce: %d\n", vettore_risultato->id_merce);
    printf("Dimensione merce: %d\n", vettore_risultato->dimensione_merce);
    printf("Tempo vita merce: %d\n\n", vettore_risultato->tempo_vita_merce);

    /*stampo per vedere se tutto funziona*/
    /*printf("PID PORTO: %d\n", getpid());
    printf("POSIZIONE PORTO: (%f, %f)\n", porto.posizione_porto[0], porto.posizione_porto[1]);
    printf("Merce offerta -> tipo: %d, quantita: %d\n", porto.merce_offerta_richiesta[0], porto.merce_offerta_richiesta[1]);
    printf("Merce richiesta -> tipo: %d, quantita: %d\n", porto.merce_offerta_richiesta[2], porto.merce_offerta_richiesta[3]);
    printf("\n");*/

    return 0;
}