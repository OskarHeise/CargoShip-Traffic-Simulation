#include "header.h"

int main(int argc, char **argv){
    int indirizzo_attachment_shared_memory_porto;
    struct struct_porto *shared_memory_porto;
    double *temp_posizione_porto;
    sem_t *semaforo_master;
    key_t messaggio_key;
    int messaggio_id;
    int i;
    srand(getpid());

    /*gestione semafori*/
    semaforo_master = sem_open(semaforo_nome, 0);
    sem_post(semaforo_master);
    
    /*genero tutte le informazioni del porto*/
    do{
        porto.merce_richiesta_id = generatore_merce_richiesta_id();
        porto.merce_richiesta_quantita = generatore_merce_richiesta_quantita();
        porto.merce_offerta_id = generatore_merce_offerta_id();
        porto.merce_offerta_quantita = generatore_merce_offerta_quantita(porto.merce_richiesta_quantita);
        porto.merce_offerta_tempo_vita = generatore_tempo_vita_merce();
    }while(porto.merce_richiesta_id == porto.merce_offerta_id);
    temp_posizione_porto = generatore_posizione_iniziale_porto(getpid(), getppid());
    porto.posizione_porto_X = temp_posizione_porto[0];
    porto.posizione_porto_Y = temp_posizione_porto[1];
    porto.numero_banchine_libere = generatore_banchine_porto();
    porto.numero_lotti_merce = generatore_lotti_merce();
    porto.conteggio_merce_ricevuta_porto = 0;
    porto.conteggio_merce_spedita_porto = 0;
    porto.pid_porto = getpid();

    /*inserisco le informazioni nella memoria condivisa, nella posizione giusta*/
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto) * SO_PORTI, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    shared_memory_porto[(getpid() - getppid())-1] = porto;

    /*visualizzo e provo a modificare il contenuto*/
    printf("PORTO pid: %d, indice porto: %d, coordinate porto: %f  - %f\n", getpid(), getpid() - getppid()-1, porto.posizione_porto_X, porto.posizione_porto_Y);

    sem_close(semaforo_master);
    return 0;
}