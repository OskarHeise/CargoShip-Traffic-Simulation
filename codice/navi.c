#include "header.h"

int main(){   
    sem_t *semaforo_master;
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze;
    int indirizzo_attachment_shared_memory_nave;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze *shared_memory_scadenze;
    struct struct_nave *shared_memory_nave;
    char **args;
    int pid_di_stampa;
    double *coordinate_temporanee;
    int tappe_nei_porti;
    int prossima_tappa; /*indice del porto in cui recarsi*/
    int i;
    srand(getpid());

    /*aprertura semaforo*/
    semaforo_master = sem_open(semaforo_nome, O_RDWR);

    /*generazione del pid di stampa*/
    pid_di_stampa = (getppid() + SO_PORTI + 1) / getpid();

    /*generazione delle informazioni della nave*/
    nave.capacita_nave = generatore_capacita_nave();
    coordinate_temporanee = generatore_posizione_iniziale_nave();
    nave.posizione_nave_X = coordinate_temporanee[0];
    nave.posizione_nave_Y = coordinate_temporanee[1];
    nave.velocita_nave = SO_SPEED;
    
    printf("NAVE pid: %d pid padre %d, indice nave: %d, coordinate nave: %f  - %f\n", getpid(), getppid(), getpid() - getppid() - SO_PORTI - 1, nave.posizione_nave_X, nave.posizione_nave_Y);

    indirizzo_attachment_shared_memory_scadenze = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze), SHM_W);
    shared_memory_scadenze = (struct struct_controllo_scadenze*)shmat(indirizzo_attachment_shared_memory_scadenze, NULL, 0);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_get(SHM_KEY_PORTO,  sizeof(struct struct_porto) * SO_PORTI, SHM_W);
    shared_memory_porto = (struct struct_porto*)shmat(indirizzo_attachment_shared_memory_porto, NULL, 0);
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_get(SHM_KEY_NAVE,  sizeof(struct struct_nave) * SO_NAVI, SHM_W);
    shared_memory_nave = (struct struct_nave*)shmat(indirizzo_attachment_shared_memory_nave, NULL, 0);


    sem_post(semaforo_master);
    
    /*salvo lo status della nave*/
    shared_memory_nave[getpid() - getppid() - SO_PORTI - 1] = nave; 
    
    /*inizio la simulzione vera e propria*/
    while(1){
        printf("\n\n..........................\naltro giro altra corsa con PID: %d e indice %d \n", getpid(), getpid() - getppid() - SO_PORTI - 1);
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
            prossima_tappa = calcolo_porto_piu_vicino(nave.posizione_nave_X, nave.posizione_nave_Y); /*controllare se ci sono banchine libere*/
            tappe_nei_porti++;
        }else{
            for(i = 0; i < SO_PORTI; i++){
                if(nave.merce_nave.id_merce == shared_memory_porto[i].merce_richiesta_id && shared_memory_porto[i].numero_banchine_libere > 0){ /*caso in cui cerca la roba*/
                    prossima_tappa = i;
                }
            }
        }
        
        /*forse questo è da rimuovere*/
        if(prossima_tappa >= SO_PORTI){ /*non ha trovato alcun porto*/
            prossima_tappa = -1;
        }

        /*ora distinguo i casi in cui trova il porto rispetto a quando non lo trova*/
        if(prossima_tappa <= -1){
            /*significa che non ho trovato il porto, quindi non faccio niente*/
            printf("PID: %d, PROSSIMA TAPPA: %d\n", getpid(), prossima_tappa);
            printf("Sto fermo con nave numero %d che va al porto %d........\n", getpid(), prossima_tappa);
            sleep(1);
        }else if(prossima_tappa > -1){
            printf("PID: %d, PROSSIMA TAPPA: %d\n", getpid(), prossima_tappa);
            /*significa che ho trovato un porto disponibile, e mi dirigo verso di esso*/

            /*per prima cosa aggiorno il numero di banchine disponibili*/
            tappe_nei_porti++;
            if(shared_memory_porto[prossima_tappa].numero_banchine_libere > 0){
                shared_memory_porto[prossima_tappa].numero_banchine_libere--;
            }

            /*aspetto che la nave arrivi al porto*/
            printf("Ora faccio spostamento con nave numero %d che va al porto %d........\n", getpid(), prossima_tappa);
            printf("%f %f //// %f %f\n", nave.posizione_nave_X, nave.posizione_nave_Y, shared_memory_porto[prossima_tappa].posizione_porto_X , shared_memory_porto[prossima_tappa].posizione_porto_Y);
            tempo_spostamento_nave(distanza_nave_porto(nave.posizione_nave_X, nave.posizione_nave_Y, shared_memory_porto[prossima_tappa].posizione_porto_X , shared_memory_porto[prossima_tappa].posizione_porto_Y));

            /*ora sono arrivato al porto e posso iniziare le operazioni di carico e di scarico delle merci*/

            /*aggiorno le coordinate delle navi*/
            printf("PRIMA *nave.posizione_nave %f - %f\n", nave.posizione_nave_X, nave.posizione_nave_Y);
            nave.posizione_nave_Y = shared_memory_porto[prossima_tappa].posizione_porto_Y;
            nave.posizione_nave_X = shared_memory_porto[prossima_tappa].posizione_porto_X;
            printf("DOPO *nave.posizione_nave %f - %f\n", nave.posizione_nave_X, nave.posizione_nave_Y);

            /*scarico la nave*/
            printf("PRIMA merce nella nave: %d\n", nave.merce_nave.dimensione_merce );
            nave.merce_nave.dimensione_merce = 0;     
            printf("DOPO merce nella nave: %d\n", nave.merce_nave.dimensione_merce );

            /*carico la nave*/
            printf("PRIMA nave.merce_nave.id_merce: %d e nave.merce_nave.dimensione_merce: %d\n", nave.merce_nave.id_merce, nave.merce_nave.dimensione_merce);
            printf("PRIMA shared_memory_porto[prossima_tappa].numero_lotti_merce: %d\n", shared_memory_porto[prossima_tappa].numero_lotti_merce);
            nave.merce_nave.id_merce = shared_memory_porto[prossima_tappa].merce_offerta_id;
            nave.merce_nave.dimensione_merce = shared_memory_porto[prossima_tappa].merce_offerta_quantita / shared_memory_porto[prossima_tappa].numero_lotti_merce;
            shared_memory_porto[prossima_tappa].numero_lotti_merce--;
            printf("DOPO nave.merce_nave.id_merce: %d e nave.merce_nave.dimensione_merce: %d\n", nave.merce_nave.id_merce, nave.merce_nave.dimensione_merce);
            printf("DOPO shared_memory_porto[prossima_tappa].numero_lotti_merce: %d\n", shared_memory_porto[prossima_tappa].numero_lotti_merce);

            /*lascio virtualmente il porto e libero una banchina*/
            shared_memory_porto[prossima_tappa].numero_banchine_libere++;
            /*salvo nuovamente lo status della nave*/
            shared_memory_nave[getpid() - getppid() - SO_PORTI - 1] = nave; 
        }

    }

    sem_close(semaforo_master);
    exit(EXIT_SUCCESS);
    
    return 0;
}