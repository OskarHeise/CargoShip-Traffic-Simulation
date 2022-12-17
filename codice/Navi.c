#include "header.h"

int main(int argc, char **argv){   
    struct struct_merce *merce_nella_nave;
    struct struct_porto *informazioni_porto;
    struct struct_conteggio_nave conteggio_nave;
    float distanza_minima_temporanea;
    int porto_visitato_in_precedenza;
    int merce_richiesta_id_precedente;
    int porto_piu_vicino;
    int tappe_nei_porti;
    sem_t *semaforo_master;
    sem_t *semaforo_nave;
    key_t messaggio_key;
    int shared_memory_id_porti;
    int i;
    int j;
    srand(getpid());
    distanza_minima_temporanea = SO_LATO+1;
    tappe_nei_porti = 0;
    numero_giorno = 1;
    porto_visitato_in_precedenza = NO_PORTI+1;

    for(i = 0; i < SO_MERCI; i++){
        somma_merci_disponibili[i] = 0;
        conteggio_merce_consegnata[i] = 0;
    }

    /*imposto il timer per la simulazione*/
    signal(SIGALRM, termina_programma);
    alarm(SO_DAYS);

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

    /*generazione tutte le informazioni della nave*/
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.capacita_nave = generatore_capacita_nave();
    nave.velocita_nave = SO_SPEED;
    
    /*ricevo l'array delle informazioni dei porti*/
    shared_memory_id_porti = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto), SHM_RDONLY);  
    informazioni_porto = (struct struct_porto*)malloc(sizeof(struct struct_porto));
    informazioni_porto = (struct struct_porto*)shmat(shared_memory_id_porti, NULL, 0);
    
    while(1){
        /*faccio muovere la nave fino al porto, calcolando prima la distanza e la richiesta*/
        for(i = 0; i < NO_PORTI; i++){
            if((informazioni_porto[i].merce_richiesta_id == merce_nella_nave->id_merce || tappe_nei_porti == 0) && distanza_minima_temporanea > distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y) && informazioni_porto[i].numero_banchine_libere != 0){
                if(porto_visitato_in_precedenza != i){
                    distanza_minima_temporanea = distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y);
                    porto_piu_vicino = i;
                    tappe_nei_porti++;
                    porto_visitato_in_precedenza = i;
                }
            }
        }
        if(distanza_minima_temporanea != SO_LATO+1){
            tempo_spostamento_nave(distanza_minima_temporanea);

            /*arrivo al porto e scarico la merce*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ /*controllo se c'è posto*/
                informazioni_porto[porto_piu_vicino].numero_banchine_libere--;
                informazioni_porto[porto_piu_vicino].conteggio_merce_ricevuta_porto = informazioni_porto[porto_piu_vicino].conteggio_merce_ricevuta_porto + merce_nella_nave->dimensione_merce;
                merce_nella_nave->dimensione_merce = 0;
                merce_nella_nave->id_merce = 0;
                merce_nella_nave->tempo_vita_merce = 0;
                merce_richiesta_id_precedente = informazioni_porto[porto_piu_vicino].merce_richiesta_id;
            }

            /*incremento contatori per poi stampare informazioni corrette alla fine del giorno*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ /*se si trova nel porto*/
                conteggio_nave.conteggio_navi_nel_porto++;
            }else if(merce_nella_nave->dimensione_merce != 0){ /*ricordarsi di resettare questo valore ad ogni giorno*/
                conteggio_nave.conteggio_navi_con_carico++;
            }else if(merce_nella_nave->dimensione_merce == 0){
                conteggio_nave.conteggio_navi_senza_carico++;
            }

            /*carico le navi*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ /*se si trova nel porto*/
                informazioni_porto[porto_piu_vicino].conteggio_merce_spedita_porto = informazioni_porto[porto_piu_vicino].conteggio_merce_spedita_porto + (informazioni_porto[porto_piu_vicino].merce_offerta_quantita / informazioni_porto[porto_piu_vicino].numero_lotti_merce);
                merce_nella_nave->dimensione_merce = informazioni_porto[porto_piu_vicino].merce_offerta_quantita / informazioni_porto[porto_piu_vicino].numero_lotti_merce;
                informazioni_porto[porto_piu_vicino].numero_lotti_merce--;
                merce_nella_nave->id_merce = informazioni_porto[porto_piu_vicino].merce_offerta_id;
                merce_nella_nave->tempo_vita_merce = generatore_tempo_vita_merce_offerta(merce_nella_nave->id_merce, informazioni_porto[porto_piu_vicino].pid_porto);
            }

            /*ripopolo il porto*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ /*se la nave era effettivamente nel porto*/
                do{
                    informazioni_porto[porto_piu_vicino].merce_richiesta_id = generatore_merce_richiesta_id();
                }while(informazioni_porto[porto_piu_vicino].merce_richiesta_id != merce_richiesta_id_precedente);
                informazioni_porto[porto_piu_vicino].merce_richiesta_quantita = generatore_merce_richiesta_quantita();
                informazioni_porto[porto_piu_vicino].merce_offerta_quantita = informazioni_porto[porto_piu_vicino].merce_offerta_quantita * informazioni_porto[porto_piu_vicino].numero_lotti_merce;
                informazioni_porto[porto_piu_vicino].numero_banchine_libere++;
            }
            /*faccio ripartire la nave*/

            /*gestire il problema delle somme - FATTO (in teoria)*/
            /*gestire la cosa dei lotti - FATTO */
            /*gestire scadenza merce*/
            /*fixare numero totale merci*/
            /*gestire accesso banchina con semafori, quindi usare questi ultimi anzichè il contatore semplice*/
            /*ricordarsi di aggiungere roba nella nave e di aggiornare il contatore delle spedizioni per la print*/
            /*usare piu di una nave e sincronizzarle tutte per benino*/
            /*aggiungere il timer globale in qualche modo*/


            /***************************************************/
        } /*quanto va all'infinito è perchè non trova nessun porto con la sua merce*/

        print_report_giornaliero(conteggio_nave, merce_nella_nave, numero_giorno, informazioni_porto, somma_merci_disponibili, conteggio_merce_consegnata);




        /*incremento giornata*/
        numero_giorno++;


        /*reset dei vari parametri*/
        distanza_minima_temporanea = SO_LATO+1;
        conteggio_nave.conteggio_navi_nel_porto = 0;
        conteggio_nave.conteggio_navi_con_carico = 0;
        conteggio_nave.conteggio_navi_senza_carico = 0;

        for(i = 0; i < SO_MERCI; i++){
            somma_merci_disponibili[i] = 0;
            conteggio_merce_consegnata[i] = 0;
        }
    }


    sem_wait(semaforo_nave);
    shmdt(&shared_memory_id_porti);
    shmdt(&shared_memory_id_merce);
    sem_close(semaforo_master);
    sem_close(semaforo_nave);
    return 0;
}


