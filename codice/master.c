#include "header.h"

int num_child_ready;

/*incrementa la variabile  num_child_ready quando viene ricevuto il segnale SIGUSR1 dal figlio.*/
void handle_child_ready(int sig) {
    if (sig == SIGUSR1) {
        num_child_ready++;
    } else {
        perror("Errore nella gestione del segnale SIGUSR1");
        exit(EXIT_FAILURE);
    }
}


int main() {
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    struct struct_controllo_scadenze_statistiche *shared_memory_scadenze_statistiche;
    pid_t pid_processi;
    sem_t *semaforo_master;
    pid_t *pid_figli; /*salvo i pid dei processi figli in un array*/
    int i,j;
    int messaggio_id, value;
    char **args;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_navi, so_porti, so_maelstrom, so_days, so_merci;

    /*"cattura" delle variabili*/
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
        if (strcmp(name, "SO_MAELSTROM") == 0) {
            so_maelstrom = value;
        }
        if (strcmp(name, "SO_DAYS") == 0) {
            so_days = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    if(so_porti == 0 || so_navi == 0 || so_days == 0 || so_merci == 0){
        perror("Errore, una delle variabili non permette il corretto funzionamento del programma!!");
        exit(EXIT_FAILURE);
    }

    fclose(config_file);










    /*inizializzazione della variabile necessaria per la chiusura di tutti i processi nave*/
    num_child_ready = 0;
    pid_figli = realloc(pid_figli, so_navi * sizeof(pid_t));
    
    /*creazione semaforo e generazione merci*/
    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0666, 1);
    value = 0;

    /*apertura di tutte le memorie condivise e la coda di messaggi che mi saranno utili successivamente*/
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_creazione(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto) * so_porti);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_creazione(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche));
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_creazione(SHM_KEY_GIORNO,  sizeof(struct struct_giorni));
    messaggio_id = coda_messaggi_creazione(MSG_KEY);

    /*lascio qualche spazio per facilitare la lettura*/
    printf("\n\n\n");    










    /*creazione processi porto*/
    for(i = 0; i < so_porti; i += 256){  /*incremento di 256 ad ogni blocco*/
        for(j = i; j < i + 256 && j < so_porti; j++){
            signal(SIGCHLD, handle_child);
            sem_wait(semaforo_master);
            switch (pid_processi = fork()){
                case -1:
                    fprintf(stderr, "Errore nella fork() del Porto");
                    exit(EXIT_FAILURE);
                    break;
                case 0:
                    execvp("./porti", args);
                    break;
                default:
                    break;
            }
        }
        printf("Creazione processi porto: %d/%d \n", j, so_porti);
    }
    waitpid(pid_processi, NULL, 0);

    printf("\n");

    /*apro la memoria condivisa per resettare il valore di conto_indice_porto*/
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche), SHM_W);
    shared_memory_scadenze_statistiche = (struct struct_controllo_scadenze_statistiche*)shmat(indirizzo_attachment_shared_memory_scadenze_statistiche, NULL, 0);
    shared_memory_scadenze_statistiche->conto_indice_porto = 0;

    for(i = 0; i < so_navi; i += 256){ /*incremento di 256 ad ogni blocco*/
        for(j = i; j < i + 256 && j < so_navi; j++){
            signal(SIGUSR1, handle_child_ready);
            sem_wait(semaforo_master);
            switch (pid_figli[j] = fork()){
                case -1:
                    fprintf(stderr, "Errore nella fork() della Nave");
                    exit(EXIT_FAILURE);
                    break;
                case 0:
                    execvp("./navi", args);
                    break;
                default:
                    break;
            }
        }
        printf("Creazione processi nave: %d/%d \n", j, so_navi);
    }    
    
    /*attendo che tutti i processi figli (navi) inviino il segnale SIGUSR1 al padre per indicare che sono pronti.*/
    /*fermo l'esecuzione del processo padre finché non riceve un segnale, utilizzando la funzione "pause()" */
    while (num_child_ready < so_navi) {
        usleep(100);
    }

    /*invio il segnale SIGUSR1 a ciascun processo figlio (nave) per far partire la simulazione, utilizzando la funzione kill*/
    /*la chiamata "usleep(100)" serve a garantire che il processo padre invia il segnale SIGUSR1 a ciascun processo figlio in modo ordinato, con una piccola pausa tra un invio e l'altro.*/
    for (i = 0; i < so_navi; i++){
        kill(pid_figli[i], SIGUSR1);
        usleep(100);
    }


    /*apro il processo print.c*/
    switch (pid_processi = fork()){
        case -1:
            fprintf(stderr, "Errore nella fork() del Print");
            exit(EXIT_FAILURE);
            break;    
        case 0: 
            execvp("./print", args);
            break;            
        default: 
            break;
    }

    /*apro il processo meteo.c*/
    switch (pid_processi = fork()){
        case -1:
            fprintf(stderr, "Errore nella fork() del Meteo");
            exit(EXIT_FAILURE);
            break;    
        case 0: 
            execvp("./meteo", args);
            break;            
        default: 
            break;
    }







    /*aspetto il messaggio per terminare l'esecuzione del programma*/
    while(strcmp(messaggio.messaggio_testo, "FINE") != 0){
        messaggio_id = coda_messaggi_get_id(MSG_KEY);
        msgrcv(messaggio_id, &messaggio, sizeof(messaggio), 1, 0);
    }

    /*chiusura delle risorse IPC*/
    msgctl(messaggio_id, IPC_RMID, NULL);
    sem_close(semaforo_master);
    sem_unlink(semaforo_nome);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_giorni);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_nave);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_porto);
    memoria_condivisa_deallocazione(indirizzo_attachment_shared_memory_scadenze_statistiche);

    exit(EXIT_SUCCESS);
    return 0;
}  