#include "header.h"

int main(){   
    sem_t *semaforo_master;
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze *shared_memory_scadenze;
    char **args;
    int pid_di_stampa;
    int tappe_nei_porti;
    int prossima_tappa; /*indice del porto in cui recarsi*/
    int i;

    srand(getpid());

    /*aprertura semaforo*/
    semaforo_master = sem_open(semaforo_nome, 0);
    sem_post(semaforo_master);

    /*generazione del pid di stampa*/
    pid_di_stampa = (getppid() + SO_PORTI + 1) / getpid();

    /*generazione delle informazioni della nave*/
    nave.capacita_nave = generatore_capacita_nave();
    nave.posizione_nave = generatore_posizione_iniziale_nave();
    nave.velocita_nave = SO_SPEED;

    /*creo un processo che gestisce l'ouput*/
    if(pid_di_stampa == 1){
        switch(fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;
            case 0:
                execvp("./output", args);
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    /*creo un processo che controlla se il programma termina prima del pervisto*/
    if(pid_di_stampa == 1){
        switch(fork()){
            case -1:
                fprintf(stderr, "Errore nella fork() della Nave");
                exit(EXIT_FAILURE);
                break;
            case 0:
                execvp("./controlli", args);
                exit(EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    indirizzo_attachment_shared_memory_scadenze = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze), SHM_W);
    shared_memory_scadenze = (struct struct_controllo_scadenze*)shmat(indirizzo_attachment_shared_memory_scadenze, NULL, 0);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO,  sizeof(struct struct_porto) * SO_PORTI, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    
    /*inizio la simulzione vera e propria*/
    while(1){
        prossima_tappa = -1;
        
        /*controllo se sono finite le merci nelle navi e/o nei porti*/
        if(nave.capacita_nave == 0 && tappe_nei_porti > 0){
            shared_memory_scadenze->numero_navi_senza_merce++;
        }
        if(porto.merce_offerta_quantita == 0 && tappe_nei_porti > 0){
            shared_memory_scadenze->numero_porti_senza_merce++;
        }

        /*scelgo il porto in cui sbarcare*/
        if(tappe_nei_porti == 0 || nave.merce_nave.dimensione_merce == 0){ /*caso in cui non ha la merce*/
            prossima_tappa = calcolo_porto_piu_vicino(nave.posizione_nave); /*controllare se ci sono banchine libere*/
            tappe_nei_porti++;
        }else{
            for(i = 0; i < SO_PORTI; i++){
                if(nave.merce_nave.id_merce == shared_memory_porto[i].merce_richiesta_id && shared_memory_porto[i].numero_banchine_libere > 0){ /*caso in cui cerca la roba*/
                    prossima_tappa = i;
                }
            }
        }
        
        if(prossima_tappa >= SO_PORTI){ /*non ha trovato alcun porto*/
            prossima_tappa = -1;
        }

        /*ora distinguo i casi in cui trova il porto rispetto a quando non lo trova*/
        if(prossima_tappa == -1){
            /*significa che non ho trovato il porto, quindi non faccio niente*/
            printf("Sto ferma con nave numero %d che va al porto %d........", getpid(), prossima_tappa);
        }else if(prossima_tappa > -1){
            /*significa che ho trovato un porto disponibile, e mi dirigo verso di esso*/
            /*per prima cosa aggiorno il numero di tappe e il numero di banchine disponibili*/
            tappe_nei_porti++;
            if(shared_memory_porto[prossima_tappa].numero_banchine_libere > 0){
                shared_memory_porto[prossima_tappa].numero_banchine_libere--;
            }
            /*aspetto che la navi vada al porto*/
            tempo_spostamento_nave(distanza_nave_porto(nave.posizione_nave, shared_memory_porto[prossima_tappa].posizione_porto_X , shared_memory_porto[prossima_tappa].posizione_porto_Y));
            printf("\nOra faccio spostamento con nave numero %d che va al porto %d........\n", getpid(), prossima_tappa);

            /*ora sono arrivato al porto e posso iniziare le operazioni di carico e di scarico delle merci*/

            /*aggiorno le coordinate delle navi*/
            printf("PRIMA *nave.posizione_nave %f - %f\n", *nave.posizione_nave, *(nave.posizione_nave++));
            *nave.posizione_nave = shared_memory_porto[prossima_tappa].posizione_porto_Y;
            nave.posizione_nave++;
            *nave.posizione_nave = shared_memory_porto[prossima_tappa].posizione_porto_X;
            nave.posizione_nave--;

            printf("DOPO *nave.posizione_nave %f - %f\n", *nave.posizione_nave, *(nave.posizione_nave++));

            /*scarico le navi*/
            printf("PRIMA merce nella nave: %d\n", nave.merce_nave.dimensione_merce );
            if(nave.merce_nave.dimensione_merce != 0){
                nave.merce_nave.dimensione_merce = 0;
            }        
            printf("DOPO merce nella nave: %d\n", nave.merce_nave.dimensione_merce );

            /*carico le navi*/
            printf("PRIMA nave.merce_nave.id_merce: %d e nave.merce_nave.dimensione_merce: %d\n", nave.merce_nave.id_merce, nave.merce_nave.dimensione_merce);
            printf("PRIMA shared_memory_porto[prossima_tappa].numero_lotti_merce: %d\n", shared_memory_porto[prossima_tappa].numero_lotti_merce);
            nave.merce_nave.id_merce = shared_memory_porto[prossima_tappa].merce_offerta_id;
            nave.merce_nave.dimensione_merce = shared_memory_porto[prossima_tappa].merce_offerta_quantita / shared_memory_porto[prossima_tappa].numero_lotti_merce;
            shared_memory_porto[prossima_tappa].numero_lotti_merce--;
            printf("DOPO nave.merce_nave.id_merce: %d e nave.merce_nave.dimensione_merce: %d\n", nave.merce_nave.id_merce, nave.merce_nave.dimensione_merce);
            printf("DOPO shared_memory_porto[prossima_tappa].numero_lotti_merce: %d\n", shared_memory_porto[prossima_tappa].numero_lotti_merce);
        }

        if(tappe_nei_porti > 2){
            break;
        }

    }



    sem_close(semaforo_master);
    return 0;
}