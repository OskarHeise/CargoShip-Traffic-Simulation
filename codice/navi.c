#include "header.h"

int main(){   
    sem_t *semaforo_master;
    sem_t *semaforo_banchine;
    struct timespec timeout; /*per evitare che il processo rimanga fermo all'infinito a causa del semaforo delle banchine*/
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    struct struct_nave *shared_memory_nave;
    struct struct_giorni *shared_memory_giorni;
    double *coordinate_temporanee;
    char **semaforo_banchine_array_nome;
    int tappe_nei_porti;
    int prossima_tappa; /*indice del porto in cui recarsi*/
    int tappa_precedente;
    int i;
    int value;
    int dimensione_nave_totale;
    char nome_semaforo[10000];

    /*cattura delle variabili*/
    FILE* config_file;
    int so_navi, so_porti, so_days, so_speed, so_merci, merce_caricata;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        return 1;
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_SPEED") == 0) {
            so_speed = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    fclose(config_file);

    /*strand*/
    srand(getpid());
    
    /*segnale*/
    signal(SIGUSR1, handle_ready);

    /*aprertura semaforo generale*/
    semaforo_master = sem_open(semaforo_nome, O_RDWR);

    /*setto la tappa precedente, in questo caso -1 perchè non abbiamo ancora iniziato*/
    tappa_precedente = -1;
    tappe_nei_porti = 0;

    /*generazione delle informazioni della nave*/
    nave.pid_nave = getpid();
    nave.capacita_nave = generatore_capacita_nave();
    coordinate_temporanee = generatore_posizione_iniziale_nave();
    nave.posizione_nave_X = coordinate_temporanee[0];
    nave.posizione_nave_Y = coordinate_temporanee[1];
    nave.velocita_nave = so_speed;

    for(i = 0; i < so_merci; i++){
        nave.merce_nave.dimensione_merce[i] = 0;
        nave.merce_nave.id_merce[i] = 0;
        nave.merce_nave.tempo_vita_merce[i] = 0;
    }
    
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO,  sizeof(struct struct_porto) * so_porti, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_get(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi, SHM_W);
    shared_memory_nave = (struct struct_nave*)shmat(indirizzo_attachment_shared_memory_nave, NULL, 0);
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_get(SHM_KEY_GIORNO,  sizeof(struct struct_giorni), SHM_W);
    shared_memory_giorni = (struct struct_giorni*)shmat(indirizzo_attachment_shared_memory_giorni, NULL, 0);

    /*salvo lo status della nave*/
    shared_memory_nave[getpid() - getppid() - so_porti - 1] = nave; 
    sem_post(semaforo_master); 

    /*gestione ripartenza*/
    kill(getppid(), SIGUSR1);
    pause();  


    while(shared_memory_giorni->giorni <= so_days){
        prossima_tappa = -1;
        dimensione_nave_totale = 0;
        merce_caricata = 0;

        for(i = 0; i < so_merci; i++){
            dimensione_nave_totale += nave.merce_nave.dimensione_merce[i];
        }

        /*aggiornamento statistiche*/
        if(dimensione_nave_totale < 1){
            shared_memory_scadenze_statistiche->navi_senza_carico[getpid() - getppid() - so_porti - 1] = 1;
            shared_memory_scadenze_statistiche->navi_con_carico[getpid() - getppid() - so_porti - 1] = 0;
        }else{
            shared_memory_scadenze_statistiche->navi_con_carico[getpid() - getppid() - so_porti - 1] = 1;
            shared_memory_scadenze_statistiche->navi_senza_carico[getpid() - getppid() - so_porti - 1] = 0;
        }

        /*scelgo il porto in cui sbarcare*/
        if(tappe_nei_porti == 0 || dimensione_nave_totale == 0){ /*caso in cui non ha la merce*/
            prossima_tappa = ricerca_binaria(shared_memory_porto, 0, so_porti - 1, nave.posizione_nave_X, nave.posizione_nave_Y, so_porti);
            tappe_nei_porti++;
        }else {
            prossima_tappa = ricerca_binaria_porto(nave, shared_memory_porto, so_porti, tappa_precedente);
            tappe_nei_porti++;
        }
        /*gestire il caso in cui ha più di una tappa e non trova mai un porto in cui sbarcare, provare con 3 navi e 1 porto*/
        tappa_precedente = prossima_tappa;

        /*forse questo è da rimuovere*/
        if(prossima_tappa >= so_porti){ /*non ha trovato alcun porto*/
            prossima_tappa = -1;
        }

        /*ora distinguo i casi in cui trova il porto rispetto a quando non lo trova*/
        if(prossima_tappa <= -1){
            /*significa che non ho trovato il porto, quindi non faccio niente*/
            sleep(1);
        }else if(prossima_tappa > -1){
            /*significa che ho trovato un porto disponibile, e mi dirigo verso di esso*/

            /*per prima cosa aggiorno il numero di banchine disponibili*/
            if(shared_memory_porto[prossima_tappa].numero_banchine_libere > 0){
                shared_memory_porto[prossima_tappa].numero_banchine_libere--;
            }
            /*aspetto che la nave arrivi al porto*/
            tempo_spostamento_nave(distanza_nave_porto(nave.posizione_nave_X, nave.posizione_nave_Y, shared_memory_porto[prossima_tappa].posizione_porto_X , shared_memory_porto[prossima_tappa].posizione_porto_Y));
            /*ora sono arrivato al porto e posso iniziare le operazioni di carico e di scarico delle merci*/

            /*aggiorno il semaforo che si occupa delle banchine*/
            semaforo_banchine = sem_open(shared_memory_porto[prossima_tappa].semaforo_banchine_nome, O_RDWR);
            sem_wait(semaforo_banchine);

            /*aggiorno la posizione*/
            shared_memory_scadenze_statistiche->navi_con_carico[getpid() - getppid() - so_porti - 1] = 0;
            shared_memory_scadenze_statistiche->navi_senza_carico[getpid() - getppid() - so_porti - 1] = 0;
            shared_memory_scadenze_statistiche->navi_nel_porto[getpid() - getppid() - so_porti - 1] = 1;


            /*aggiorno le coordinate delle navi*/
            nave.posizione_nave_Y = shared_memory_porto[prossima_tappa].posizione_porto_Y;
            nave.posizione_nave_X = shared_memory_porto[prossima_tappa].posizione_porto_X;

            /*libero il semaforo che si occupa delle banchine*/
            sem_post(semaforo_banchine);
            sem_close(semaforo_banchine);

            /*tempo in cui sto fermo nel porto*/
            tempo_sosta_porto(nave.merce_nave.dimensione_merce);

            /*scarico la nave*/
            for(i = 0; i < so_merci; i++){
                shared_memory_porto[prossima_tappa].conteggio_merce_ricevuta_porto += nave.merce_nave.dimensione_merce[i];
                shared_memory_scadenze_statistiche->merce_consegnata[i] += nave.merce_nave.dimensione_merce[i];
                nave.merce_nave.dimensione_merce[i] = 0;
            }

            /*carico la nave*/
            for(i = 0; i < so_merci; i++){
                if(shared_memory_porto[prossima_tappa].numero_lotti_merce[i] > 0){
                    if(merce_caricata > nave.capacita_nave){ break;}
                    nave.merce_nave.dimensione_merce[i] = shared_memory_porto[prossima_tappa].merce_offerta_quantita[i];
                    merce_caricata += (shared_memory_porto[prossima_tappa].merce_offerta_quantita[i]);
                    
                    shared_memory_porto[prossima_tappa].numero_lotti_merce[i]--;

                    shared_memory_porto[prossima_tappa].conteggio_merce_spedita_porto += nave.merce_nave.dimensione_merce[i];  
                }  
            }


            /*lascio virtualmente il porto e libero una banchina*/
            shared_memory_porto[prossima_tappa].numero_banchine_libere++;
            /*aggiorno la posizione*/
            shared_memory_scadenze_statistiche->navi_con_carico[getpid() - getppid() - so_porti - 1] = 1;
            shared_memory_scadenze_statistiche->navi_senza_carico[getpid() - getppid() - so_porti - 1] = 0;
            shared_memory_scadenze_statistiche->navi_nel_porto[getpid() - getppid() - so_porti - 1] = 0;


            /*salvo nuovamente lo status della nave*/
            shared_memory_nave[getpid() - getppid() - so_porti - 1] = nave;
        }
    }


    fflush(stdout);
    for(i = 0; i < so_porti; i++){
        sem_close(shared_memory_porto[i].semaforo_banchine);
    }
    sem_close(semaforo_master);
    exit(EXIT_SUCCESS);
    
    return 0;
}