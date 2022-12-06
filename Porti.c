#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(int argc, char **argv){
    srand(time(NULL));
    struct struct_porto *porto;
    porto = (struct struct_porto*)malloc(sizeof(struct struct_porto));
    pid_t pid = getpid();

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);   
    struct struct_merce *vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NO_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    /*genero la posizione del porto*/
    porto = generatore_array_porti(pid);
    double *posizione_porto = porto[0].posizione_porto;

    printf("PID: %d e Posizione: %f - %f\n", pid, *posizione_porto, *(posizione_porto++));

    return 0;
}