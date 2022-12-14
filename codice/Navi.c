#include "header.h"

/*ATTENZIONE STAI PER ENTRARE NEL MAIN*/

int main(int argc, char **argv){   
    struct struct_merce *merce_nella_nave;
    struct struct_porto *informazioni_porto;
    float *posizione_porto;
    sem_t *semaforo_nave;
    key_t messaggio_key;
    int shared_memory_id_porti;
    srand(getpid());

    /*gestione semafori*/
    semaforo_nave = sem_open(semaforo_nome, 0);
    sem_post(semaforo_nave);
    
    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id_merce = memoria_condivisa_get(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI, SHM_RDONLY);   
    merce_nella_nave = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    merce_nella_nave = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_merce);    

    

    /*genero tutte le informazioni della nave*/
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.capacita_nave = generatore_capacita_nave();
    nave.velocita_nave = SO_SPEED;

    /*stampo per vedere se le merci sono state portate in maniera corretta*/
    /*printf("PID NAVE: %d\n", getpid());
    printf("ID merce: %d\n", merce_nella_nave->id_merce);
    printf("Dimensione merce: %d\n", merce_nella_nave->dimensione_merce);
    printf("Tempo vita merce: %d\n\n", merce_nella_nave->tempo_vita_merce);

    /*stampo per vedere se tutto funziona correttamente*/
    /*printf("PID nave: %d\n", getpid());
    printf("Posizione nave -> %f, %f\n", nave.posizione_nave[0], nave.posizione_nave[1]);
    printf("Capacita nave -> %d\n", nave.capacita_nave);
    printf("Velocita nave -> %d\n", nave.velocita_nave);
    printf("\n");*/

    
    shared_memory_id_porti = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto), SHM_RDONLY);  
    informazioni_porto = (struct struct_porto*)malloc(sizeof(struct struct_porto));
    informazioni_porto = (struct struct_porto*)shmat(shared_memory_id_porti, NULL, 0);
    printf("quantita merce richiesta dalla nave %d\n", informazioni_porto->merce_richiesta_quantita);







    shmdt(&shared_memory_id_porti);
    shmdt(&shared_memory_id_merce);
    sem_close(semaforo_nave);
    return 0;
}


