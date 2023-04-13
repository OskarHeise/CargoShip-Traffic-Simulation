#include "header.h"

volatile int num_child_ready;

/*incrementa la variabile  num_child_ready quando viene ricevuto il segnale SIGUSR1 dal figlio.*/
void handle_child_ready(int sig) {
    num_child_ready++;
}

int main() {
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze_statistiche;
    int indirizzo_attachment_shared_memory_nave;
    int indirizzo_attachment_shared_memory_giorni;
    pid_t pid_processi;
    sem_t *semaforo_master;
    pid_t *pid_figli; /*salvo i pid dei processi figli in un array*/
    int i,j;
    int messaggio_id;
    char **args;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_navi, so_porti, so_maelstrom, so_days;

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
    }

    fclose(config_file);

    /*inizializzazione della variabile necessaria per la chiusura di tutti i processi nave*/
    num_child_ready = 0;
    pid_figli = realloc(pid_figli, so_navi * sizeof(pid_t));
    
    /*creazione semaforo e generazione merci*/
    semaforo_master = sem_open(semaforo_nome, O_CREAT, 0666, 1);

    /*apertura di tutte le memorie condivise e la coda di messaggi che mi saranno utili successivamente*/
    indirizzo_attachment_shared_memory_nave = memoria_condivisa_creazione(SHM_KEY_NAVE,  sizeof(struct struct_nave) * so_navi);
    indirizzo_attachment_shared_memory_porto = memoria_condivisa_creazione(SHM_KEY_PORTO, sizeof(struct struct_porto) * so_porti);
    indirizzo_attachment_shared_memory_scadenze_statistiche = memoria_condivisa_creazione(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze_statistiche));
    indirizzo_attachment_shared_memory_giorni = memoria_condivisa_creazione(SHM_KEY_GIORNO,  sizeof(struct struct_giorni));
    messaggio_id = coda_messaggi_creazione(MSG_KEY);

    printf("\n\n\n");    

    /*creazione processi porto*/
    for(i = 0; i < so_porti; i += 256){  /*incremento di 256 ad ogni blocco*/
        for(j = i; j < i + 256 && j < so_porti; j++){
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
                    signal(SIGCHLD, handle_child);
                    pause();
                    break;
            }
        }
        sleep(0); /*inseriamo uno sleep(0) tra un blocco e l'altro*/
        printf("Creazione processi porto: %d/%d \n", j, so_porti);
    }

    printf("\n\n");

    for(i = 0; i < so_navi; i += 256){ /*incremento di 256 ad ogni blocco*/
        for(j = i; j < i + 256 && j < so_navi; j++){
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
                    signal(SIGUSR1, handle_child_ready);
                    break;
            }
        }
        sleep(0); /*inseriamo uno sleep(0) tra un blocco e l'altro*/
        printf("Creazione processi nave: %d/%d \n", j, so_navi);
    }

    /*attendo che tutti i processi figli (navi) inviino il segnale SIGUSR1 al padre per indicare che sono pronti.*/
    /*fermo l'esecuzione del processo padre finché non riceve un segnale, utilizzando la funzione "pause()" */
    while (num_child_ready < so_navi) {
        pause();
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

    if("CIAO")


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

    return 0;
}  