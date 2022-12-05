#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(){
    srand(time(NULL));
    struct struct_nave *nave;
    nave = (struct struct_nave*)malloc(sizeof(struct struct_nave));
    pid_t pid = getpid();

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id = memoria_condivisa_creazione(SHM_KEY, sizeof(struct struct_merce)*NO_MERCI);   
    struct struct_merce *vettore_risultato = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NO_MERCI); 
    vettore_risultato = (struct struct_merce*)shmat(shared_memory_id, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id);

    nave = generatore_array_navi();

    return 0;
}


