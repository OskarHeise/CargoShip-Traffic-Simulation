#include "header.h"

int main(int argc, char **argv){   
    struct struct_merce *merce_nella_nave;
    struct struct_porto *informazioni_porto;
    float distanza_minima_temporanea;
    int porto_piu_vicino;
    int tappe_nei_porti;
    sem_t *semaforo_master;
    sem_t *semaforo_nave;
    key_t messaggio_key;
    int shared_memory_id_porti;
    int i;
    srand(getpid());
    distanza_minima_temporanea = SO_LATO+1;
    tappe_nei_porti = 0;

    /*gestione semafori*/
    semaforo_master = sem_open(semaforo_nome, 0);
    sem_post(semaforo_master);
    semaforo_nave = sem_open(semaforo_nave_nome, O_CREAT, 0644, 0);
    sem_post(semaforo_nave);
    
    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id_merce = memoria_condivisa_get(SHM_KEY_MERCE, sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI, SHM_RDONLY);   
    merce_nella_nave = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI); 
    merce_nella_nave = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_merce); 

    merce_nella_nave->dimensione_merce = 0;
    merce_nella_nave->id_merce = 0;
    merce_nella_nave->tempo_vita_merce = 999;

    /*printf("PID nave prima: %d\n", getpid());
    printf("Merce nella nave quantita: %d\n", merce_nella_nave->dimensione_merce);
    printf("Merce nella nave id: %d\n", merce_nella_nave->id_merce);
    printf("Merce nella nave tempo vita: %d\n", merce_nella_nave->tempo_vita_merce);*/

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
        if(((informazioni_porto[i].merce_richiesta_id == merce_nella_nave->id_merce || tappe_nei_porti == 0) && distanza_minima_temporanea > distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y))&& informazioni_porto[i].numero_banchine_libere != 0){
            distanza_minima_temporanea = distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y);
            porto_piu_vicino = i;
        }
    }
    if(distanza_minima_temporanea != SO_LATO+1){
        tempo_spostamento_nave(distanza_minima_temporanea);
    }

    /*gestione dell'arrivo nei porti e lo scambio della merce*/
    if(distanza_minima_temporanea != SO_LATO+1 && informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ /*controllo se c'è posto*/
        informazioni_porto[porto_piu_vicino].numero_banchine_libere--;
        merce_nella_nave->dimensione_merce = informazioni_porto[porto_piu_vicino].merce_offerta_quantita;
        merce_nella_nave->id_merce = informazioni_porto[porto_piu_vicino].merce_offerta_id;
        merce_nella_nave->tempo_vita_merce = generatore_tempo_vita_merce(); /*fixare questo che non è giusto*/
    }

    printf("\nPID nave dopo: %d\n", getpid());
    printf("Merce nella nave quantita: %d\n", merce_nella_nave->dimensione_merce);
    printf("Merce nella nave id: %d\n", merce_nella_nave->id_merce);
    printf("Merce nella nave tempo vita: %d\n", merce_nella_nave->tempo_vita_merce);
    printf("Nel porto numero %d le banchine libere sono %d\n\n", porto_piu_vicino, informazioni_porto[porto_piu_vicino].numero_banchine_libere);




    sem_wait(semaforo_nave);
    shmdt(&shared_memory_id_porti);
    shmdt(&shared_memory_id_merce);
    sem_close(semaforo_master);
    sem_close(semaforo_nave);
    return 0;
}


