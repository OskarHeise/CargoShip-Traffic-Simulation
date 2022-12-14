#include "header.h"

int main(int argc, char **argv){
    struct struct_merce *merce_nel_porto;
    struct struct_porto *shared_memory_porto;
    int indirizzo_attachment_porto;
    sem_t *semaforo_porto;
    key_t messaggio_key;
    int messaggio_id;
    int i;
    srand(getpid());

    /*gestione semafori*/
    semaforo_porto = sem_open(semaforo_nome, 0);
    sem_post(semaforo_porto);

    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id_merce = memoria_condivisa_get(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI, SHM_RDONLY);   
    merce_nel_porto = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    merce_nel_porto = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_merce);

    /*genero tutte le informazioni del porto*/
    do{
        porto.merce_richiesta_id = generatore_merce_richiesta_id();
        porto.merce_richiesta_quantita = generatore_merce_richiesta_quantita();
        porto.merce_offerta_id = generatore_merce_offerta_id();
        porto.merce_offerta_quantita = generatore_merce_offerta_quantita();
    }while(porto.merce_richiesta_id == porto.merce_offerta_id);
    porto.posizione_porto = generatore_posizione_iniziale_porto(getpid(), getppid());

    /*inserisco le informazioni nella memoria condivisa*/
    indirizzo_attachment_porto = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto), SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_porto, NULL, 0);
    shared_memory_porto[(getpid() - getppid())-1] = porto;

    printf("PID PARENT: %d, PID PORTO: %d\n", getppid(), getpid());
    printf("Posizione porto: %f, %f\n", porto.posizione_porto[0], porto.posizione_porto[1]);
    printf("ID offerta: %d, offerta quantita: %d\n", porto.merce_offerta_id, porto.merce_offerta_quantita);
    printf("ID richiesta: %d, offerta quantita: %d\n\n", porto.merce_richiesta_id, porto.merce_richiesta_quantita);





    sem_close(semaforo_porto);
    return 0;
}