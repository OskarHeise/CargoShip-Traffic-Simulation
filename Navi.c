#include "header.h"

struct struct_nave{
    int velocita_nave; /*velocità nave*/
    double *posizione_nave; /*posizione della nave con coordinate*/
    int capacita_nave; /*capacità della nave*/
};

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(){
    srand(time(NULL));

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);   
    struct struct_merce *vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NO_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    return 0;
}


