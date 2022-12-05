#include "header.h"

struct struct_porto{
    double posizione_porto;
};

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(int argc, char **argv){
    srand(time(NULL));

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);   
    struct struct_merce *vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NO_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    return 0;
}