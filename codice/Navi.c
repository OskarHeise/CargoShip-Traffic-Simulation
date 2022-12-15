#include "header.h"

int main(int argc, char **argv){   
    struct struct_merce *merce_nella_nave;
    struct struct_porto *informazioni_porto;
    float distanza_minima_temporanea;
    int porto_piu_vicino;
    sem_t *semaforo_nave;
    key_t messaggio_key;
    int shared_memory_id_porti;
    int i;
    srand(getpid());
    distanza_minima_temporanea = SO_LATO+1;

    /*gestione semafori*/
    semaforo_nave = sem_open(semaforo_nome, 0);
    sem_post(semaforo_nave);
    
    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id_merce = memoria_condivisa_get(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI, SHM_RDONLY);   
    merce_nella_nave = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    merce_nella_nave = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_merce); 

    /*generazione tutte le informazioni della nave*/
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.capacita_nave = generatore_capacita_nave();
    nave.velocita_nave = SO_SPEED;
    
    /*ricevo l'array delle informazioni dei porti*/
    shared_memory_id_porti = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto), SHM_RDONLY);  
    informazioni_porto = (struct struct_porto*)malloc(sizeof(struct struct_porto));
    informazioni_porto = (struct struct_porto*)shmat(shared_memory_id_porti, NULL, 0);
    

    /*faccio muovere la nave fino al porto, calcolando prima la distanza e la richiesta*/
    for(i = 0; i < NO_PORTI; i++){
        if(informazioni_porto[i].merce_richiesta_id == merce_nella_nave->id_merce && distanza_minima_temporanea > distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y)){
            distanza_minima_temporanea = distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y);
            porto_piu_vicino = i;
        }
    }
    if(distanza_minima_temporanea != SO_LATO+1){
        tempo_spostamento_nave(distanza_minima_temporanea);
    }




    shmdt(&shared_memory_id_porti);
    shmdt(&shared_memory_id_merce);
    sem_close(semaforo_nave);
    return 0;
}


