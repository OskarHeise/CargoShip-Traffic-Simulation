#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(int argc, char **argv){   
    struct struct_merce *vettore_risultato;
    int *result;
    sem_t *semaforo_nave;
    srand(getpid());
    
    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);   
    vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);    

    /*genero tutte le informazioni della nave*/
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.capacita_nave = generatore_capacita_nave();
    nave.velocita_nave = SO_SPEED;

    /*stampo per vedere se le merci sono state portate in maniera corretta*/
    printf("PID NAVE: %d\n", getpid());
    printf("ID merce: %d\n", vettore_risultato->id_merce);
    printf("Dimensione merce: %d\n", vettore_risultato->dimensione_merce);
    printf("Tempo vita merce: %d\n\n", vettore_risultato->tempo_vita_merce);

    /*stampo per vedere se tutto funziona correttamente*/
    /*printf("PID nave: %d\n", getpid());
    printf("Posizione nave -> %f, %f\n", nave.posizione_nave[0], nave.posizione_nave[1]);
    printf("Capacita nave -> %d\n", nave.capacita_nave);
    printf("Velocita nave -> %d\n", nave.velocita_nave);
    printf("\n");*/

    /*printf("PID nave: %d\n", getpid());
    printf("ID merce nave: %d\n",  vettore_risultato[0].id_merce);
    printf("Dimensione merce nave: %d\n",  vettore_risultato[0].dimensione_merce);

    
    /*gestisco i semafori e permetto ad un solo processo alla volta di agire*/

    /*ricevo il messaggio tramite la coda di messaggi*/
    

    return 0;
}


