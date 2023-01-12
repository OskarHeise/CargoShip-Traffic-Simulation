#include "header.h"
struct struct_merce *merce_nella_nave;
struct struct_porto *informazioni_porto;
struct struct_conteggio_nave *conteggio_nave;
sem_t *semaforo_master;
sem_t *semaforo_nave;
int shared_memory_id_porti;
pid_t pid_di_stampa;
pid_t pid_kill;
int indice_nave;
int porto_piu_vicino;
int tappe_nei_porti;
int messaggio_id;


int numero_offerta(){
    int i;
    int numero_offerta;

    for(i = 0; i < SO_PORTI; i++){
        if(informazioni_porto[i].merce_offerta_quantita == 0){
            numero_offerta++;
        }
    }
    return numero_offerta;
}

int numero_richiesta(){
    int i;
    int numero_offerta;

    for(i = 0; i < SO_PORTI; i++){
        if(informazioni_porto[i].merce_richiesta_quantita == 0){
            numero_offerta++;
        }
    }
    return numero_offerta;
}

void stampa_risultati_giornalieri(void);
void stampa_risultati_finali(void);

/*thread per la stampa dei report*/
void stampa_risultati_giornalieri(){
    int i;

    /*printf("pid processo: %d, numero giorno: %d / %d infine pid di stampa: %d\n", getpid(), numero_giorno, SO_DAYS+1, pid_di_stampa);
    /*aggiornamento della data di scadenza*/
    merce_nella_nave[indice_nave].tempo_vita_merce--;
    informazioni_porto[porto_piu_vicino].merce_offerta_tempo_vita--;
    sleep(1);
    if(pid_di_stampa == 0 && numero_giorno < SO_DAYS+1){
        /*print_report_giornaliero(conteggio_nave, merce_nella_nave, numero_giorno, informazioni_porto, somma_merci_disponibili, conteggio_merce_consegnata); /*print report*/
    }
    numero_giorno++;
}

void stampa_risultati_finali(){
    int i;

    /*print report finale*/
    printf("prova prova sempre fuori: %d\n", totale_merce_generata_inizialmente[0]);
    print_report_finale(conteggio_nave, merce_nella_nave, numero_giorno, informazioni_porto, somma_merci_disponibili, conteggio_merce_consegnata, totale_merce_generata_inizialmente, merce_scaduta_in_nave, merce_scaduta_in_porto, tappe_nei_porti);

    sleep(1);

    /*ricevo messaggio dal porto che è tutto ok*/
    messaggio_id = coda_messaggi_get_id(MSG_KEY);
    msgrcv(messaggio_id, &messaggio, sizeof(messaggio), 1, 0);
    printf("%s\n\n", messaggio.messaggio_testo);
    msgctl(messaggio_id, IPC_RMID, NULL);


    sleep(1);
}


int main(int argc, char **argv){   
    float distanza_minima_temporanea;
    int porto_visitato_in_precedenza;
    int merce_richiesta_id_precedente;
    int id_merce_iniziale;
    pid_t pid_processo_stampa;
    pthread_t tid;
    key_t messaggio_key;
    clock_t tempo_precedente;
    int i;
    int j;
    srand(getpid());
    distanza_minima_temporanea = SO_LATO+1;
    porto_visitato_in_precedenza = SO_PORTI+1;
    tappe_nei_porti = 0;
    numero_giorno = 1;

    /*inizializzazioni variabili per statistiche finali*/
    for(i = 0; i < SO_MERCI; i++){
        somma_merci_disponibili[i] = 0;
        conteggio_merce_consegnata[i] = 0;
        totale_merce_generata_inizialmente[i] = 0; 
    }
    for(i = 0; i < SO_NAVI; i++){
        merce_scaduta_in_nave[i] = 0;
    }
    for(i = 0; i < SO_PORTI; i++){
        merce_scaduta_in_porto[i] = 0;
    }

    /*gestione semafori*/
    semaforo_master = sem_open(semaforo_nome, 0);
    sem_post(semaforo_master);
    semaforo_nave = sem_open(semaforo_nave_nome, O_CREAT, 0644, 0);
    
    /*ricevo l'array dalla memoria condivisa*/
    shared_memory_id_merce = memoria_condivisa_get(SHM_KEY_MERCE, sizeof(struct struct_merce)*(SO_NAVI+SO_PORTI), SHM_RDONLY);   
    merce_nella_nave = (struct struct_merce*)malloc(sizeof(struct struct_merce)); 
    merce_nella_nave = (struct struct_merce*)shmat(shared_memory_id_merce, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_merce); 

    /*setto indici*/
    pid_di_stampa = ((getppid()) + (SO_PORTI+1) + (SO_NAVI-3)) / (getpid());
    indice_nave = merce_nella_nave->index_nave;

    /*ricevo la roba dalla memoria condivisa*/
    shared_memory_id_conteggio_nave = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_conteggio_nave)*SO_NAVI, SHM_RDONLY);
    conteggio_nave = (struct struct_conteggio_nave*)malloc(sizeof(struct struct_conteggio_nave)*SO_NAVI);
    conteggio_nave = (struct struct_conteggio_nave*)shmat(shared_memory_id_conteggio_nave, NULL, 0666|IPC_EXCL);
    shmdt(&shared_memory_id_conteggio_nave);

    /*generazione tutte le informazioni della nave*/
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.capacita_nave = SO_CAPACITY;
    nave.velocita_nave = SO_SPEED;
    
    /*ricevo l'array delle informazioni dei porti*/
    shared_memory_id_porti = memoria_condivisa_get(SHM_KEY_PORTO, sizeof(struct struct_porto), SHM_RDONLY);  
    informazioni_porto = (struct struct_porto*)malloc(sizeof(struct struct_porto));
    informazioni_porto = (struct struct_porto*)shmat(shared_memory_id_porti, NULL, 0);

    /*imposto il timer*/
    sleep(1);
    if(pid_di_stampa == 0){
        switch(pid_processo_stampa){
            case -1: 
                fprintf(stderr, "Errore nella fork() del Porto");
                exit(EXIT_FAILURE);
                break; 
            case 0:
                while((numero_giorno < SO_DAYS+1) || (numero_offerta() == 0 && numero_richiesta() == 0)){stampa_risultati_giornalieri();}
                stampa_risultati_finali();
            default:
                for(i = (getpid() - SO_NAVI); i <= getpid(); i++){
                    pid_kill = i;
                    kill(pid_kill, SIGKILL);
                }
        }
    }
    /*pthread_create(&tid, NULL, &threadproc, NULL);*/

    /*resetto gli indici*/
    merce_nella_nave[indice_nave].dimensione_merce = -1;
    merce_nella_nave[indice_nave].id_merce = -1;
    merce_nella_nave[indice_nave].tempo_vita_merce = -1;

    conteggio_nave[indice_nave].conteggio_navi_con_carico = 0;
    conteggio_nave[indice_nave].conteggio_navi_senza_carico = 1;
    conteggio_nave[indice_nave].conteggio_navi_nel_porto = 0;

    /*inizio simulazione vera e propria*/
    while(numero_giorno < SO_DAYS+1){

        /*controllo la scadenza della merce nelle navi*/
        if(merce_nella_nave[indice_nave].tempo_vita_merce < 0){
            for(i = 0; i < SO_PORTI; i++){
                for(j = 0; j < SO_MERCI; j++){
                    if(merce_nella_nave[indice_nave].id_merce == j){
                        merce_scaduta_in_nave[j] = merce_scaduta_in_nave[j] + merce_nella_nave[indice_nave].dimensione_merce * informazioni_porto[i].numero_lotti_merce;
                    }
                }
            }
            merce_nella_nave[indice_nave].id_merce = -1;
            merce_nella_nave[indice_nave].dimensione_merce = -1;
        }

        /*controllo la scadenza della merce nei porti*/
        for(i = 0; i < SO_PORTI; i++){
            if(informazioni_porto[i].merce_offerta_tempo_vita < 0){
                for(i = 0; i < SO_PORTI; i++){
                    for(j = 0; j < SO_MERCI; j++){
                        if(merce_nella_nave[i].id_merce == j){
                            merce_scaduta_in_porto[j] = merce_scaduta_in_porto[j] + informazioni_porto[i].merce_offerta_quantita * informazioni_porto[i].numero_lotti_merce;
                        }   
                    }
                }
                informazioni_porto[i].merce_offerta_id = -1;
                informazioni_porto[i].merce_offerta_quantita = 0;
            }
        }
        
        /*scelgo il porto in cui sbarcare*/
        for(i = 0; i < SO_PORTI; i++){
            if((informazioni_porto[i].merce_richiesta_id == merce_nella_nave[indice_nave].id_merce || tappe_nei_porti == 0 || merce_nella_nave[indice_nave].tempo_vita_merce < 0) && distanza_minima_temporanea > distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y) && informazioni_porto[i].numero_banchine_libere != 0  && informazioni_porto[i].numero_lotti_merce > 0){
                if(porto_visitato_in_precedenza != i){
                    distanza_minima_temporanea = distanza_nave_porto(nave.posizione_nave, informazioni_porto[i].posizione_porto_X, informazioni_porto[i].posizione_porto_Y);
                    porto_piu_vicino = i;
                    porto_visitato_in_precedenza = i;
                }
            }
        }
        tappe_nei_porti++;

        /*memorizzazione valori iniziali porto per la statistica finale*/ /*sia nei porti che nelle navi*/
        if(tappe_nei_porti == 1){
            for(i = 0; i < SO_PORTI; i++){
                for(j = 0; j < SO_MERCI; j++){
                    if(informazioni_porto[i].merce_offerta_id == j){
                        totale_merce_generata_inizialmente[j] = totale_merce_generata_inizialmente[j] + informazioni_porto[i].merce_offerta_quantita * informazioni_porto[i].numero_lotti_merce;
                    }
                } 
            }
            for(j = 0; j < SO_MERCI; j++){
                if(merce_nella_nave[indice_nave].id_merce == j){
                    totale_merce_generata_inizialmente[j] = totale_merce_generata_inizialmente[j] + merce_nella_nave[indice_nave].dimensione_merce * informazioni_porto[i].numero_lotti_merce;
                }
            } 
        }

        if(merce_nella_nave[indice_nave].dimensione_merce > 1){ 
            conteggio_nave[indice_nave].conteggio_navi_con_carico = 1;
        }else if(merce_nella_nave[indice_nave].dimensione_merce < 1){
            conteggio_nave[indice_nave].conteggio_navi_senza_carico = 1;
        }  
        
        
        if(distanza_minima_temporanea != SO_LATO+1){                   
            /*spostamento della nave*/
            tempo_spostamento_nave(distanza_minima_temporanea);

            /*aggiorno la posizione delle navi*/
            conteggio_nave[indice_nave].conteggio_navi_con_carico = 0;
            conteggio_nave[indice_nave].conteggio_navi_senza_carico = 0;
            conteggio_nave[indice_nave].conteggio_navi_nel_porto = 1;


            sem_post(semaforo_nave);
            /*scarico le navi*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ 
                if(merce_nella_nave[indice_nave].dimensione_merce > 0){
                    informazioni_porto[porto_piu_vicino].numero_banchine_libere--;
                    tempo_sosta_porto(merce_nella_nave[indice_nave].dimensione_merce);
                    informazioni_porto[porto_piu_vicino].conteggio_merce_ricevuta_porto = informazioni_porto[porto_piu_vicino].conteggio_merce_ricevuta_porto + merce_nella_nave->dimensione_merce;
                    merce_nella_nave[indice_nave].dimensione_merce = 0;
                    merce_nella_nave[indice_nave].id_merce = -1;
                    merce_nella_nave[indice_nave].tempo_vita_merce = 0;
                    merce_richiesta_id_precedente = informazioni_porto[porto_piu_vicino].merce_richiesta_id;
                }
            }

            /*carico le navi*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){
                tempo_sosta_porto(merce_nella_nave[indice_nave].dimensione_merce);
                informazioni_porto[porto_piu_vicino].conteggio_merce_spedita_porto = informazioni_porto[porto_piu_vicino].conteggio_merce_spedita_porto + (informazioni_porto[porto_piu_vicino].merce_offerta_quantita / informazioni_porto[porto_piu_vicino].numero_lotti_merce);
                merce_nella_nave[indice_nave].dimensione_merce = informazioni_porto[porto_piu_vicino].merce_offerta_quantita / informazioni_porto[porto_piu_vicino].numero_lotti_merce;
                informazioni_porto[porto_piu_vicino].numero_lotti_merce--;
                merce_nella_nave[indice_nave].id_merce = informazioni_porto[porto_piu_vicino].merce_offerta_id;
                merce_nella_nave[indice_nave].tempo_vita_merce = generatore_tempo_vita_merce_offerta(merce_nella_nave[indice_nave].id_merce, informazioni_porto[porto_piu_vicino].pid_porto);
            }

            /*ripopolo il porto*/
            if(informazioni_porto[porto_piu_vicino].numero_banchine_libere != 0){ 
                tempo_sosta_porto(merce_nella_nave[indice_nave].dimensione_merce);
                do{
                    informazioni_porto[porto_piu_vicino].merce_richiesta_id = generatore_merce_richiesta_id();
                }while(informazioni_porto[porto_piu_vicino].merce_richiesta_id != merce_richiesta_id_precedente);
                informazioni_porto[porto_piu_vicino].merce_richiesta_quantita = generatore_merce_richiesta_quantita();
                informazioni_porto[porto_piu_vicino].merce_offerta_quantita = informazioni_porto[porto_piu_vicino].merce_offerta_quantita - (informazioni_porto[porto_piu_vicino].merce_offerta_quantita / informazioni_porto[porto_piu_vicino].numero_lotti_merce);
                informazioni_porto[porto_piu_vicino].numero_banchine_libere++;
            }
            /*faccio ripartire la nave*/
            
            /***************************************************/
            sem_wait(semaforo_nave);
        }
        
        /*reset dei vari parametri*/
        distanza_minima_temporanea = SO_LATO+1;
        conteggio_nave[indice_nave].conteggio_navi_nel_porto = 0;
        if(merce_nella_nave[indice_nave].dimensione_merce != 0){ 
            conteggio_nave[indice_nave].conteggio_navi_con_carico = 1;
        }else if(merce_nella_nave[indice_nave].dimensione_merce < 1){
            conteggio_nave[indice_nave].conteggio_navi_senza_carico = 1;
        }
        
        for(i = 0; i < SO_MERCI; i++){
            somma_merci_disponibili[i] = 0;
            conteggio_merce_consegnata[i] = 0;
        }
    }    

    shmdt(&shared_memory_id_porti);
    shmdt(&shared_memory_id_merce);
    sem_close(semaforo_master);
    sem_close(semaforo_nave);
    return 0;
}