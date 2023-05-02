#ifndef _HEADER_H_
#define _HEADER_H_

#define _GNU_SOURCE

/*lista delle librerie utilizzate*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/sem.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>

/*lista delle costanti utilizzate*/

#define SHM_KEY_MERCE 1234
#define SHM_KEY_PORTO 1236
#define SHM_KEY_NAVE 7896
#define SHM_KEY_CONTEGGIO 7845
#define SHM_KEY_SEM 6789
#define SHM_KEY_GIORNO 2002
#define MSG_KEY 455487



/*variabili globali utilizzate*/

int shared_memory_id_conteggio_nave;
int numero_giorno;
struct struct_porto porto; 
struct struct_nave nave;
clock_t inizio_tempo;
clock_t differenza_tempo;
int millisecondi;
int conteggio_navi_con_carico;
int conteggio_navi_senza_carico;
int conteggio_navi_nel_porto;


/*chiavi dei semafori*/
const char *semaforo_nome = "/sdsedssdmaforo";



/*strutture impiegate per lo sviluppo del progetto*/

struct struct_merce{
    int id_merce[100]; /*genero la tipologia di merce con un ID numerico*/
    int dimensione_merce[100]; /*genero la grandezza delle merci*/
    int tempo_vita_merce[100]; /*genero la durata di vita delle merci*/
};

struct struct_nave{
    pid_t pid_nave; /*pid del processo nave*/
    int index_nave; /*indice della nave*/
    int velocita_nave; /*velocità nave*/
    double posizione_nave_X;
    double posizione_nave_Y;
    int capacita_nave; /*capacità della nave*/
    struct struct_merce merce_nave; /*merce presente nella nave*/
};

struct struct_porto{
    pid_t pid_porto;
    double posizione_porto_X;
    double posizione_porto_Y;
    int numero_banchine_libere;
    int merce_richiesta_id[100];
    int merce_richiesta_quantita[100];
    int merce_offerta_id[100];
    int merce_offerta_quantita[100];
    int merce_offerta_tempo_vita[100];

    int numero_lotti_merce[100];

    int conteggio_merce_spedita_porto; 
    int conteggio_merce_ricevuta_porto;

    sem_t *semaforo_banchine;
    char semaforo_banchine_nome[1000];
};

struct struct_tempo_spostamento{
    int secondi;
    float nano_secondi;
};

struct struct_controllo_scadenze_statistiche{
    int numero_navi_senza_merce;
    int numero_porti_senza_merce;
    int merce_consegnata[10];

    int navi_nel_porto[10000];
    int navi_con_carico[10000];
    int navi_senza_carico[10000];

    int merce_generata_inizialmente[100];
    int merce_scaduta_porto[100];
    int merce_scaduta_nave[100];

    int navi_rallentate_tempesta;
    int porti_interessati_mareggiata[10000];
    int navi_affondate_maelstrom;

    int conto_so_fill;
    int conto_indice_porto;
};

struct struct_giorni{
    int giorni;
};

struct struct_messaggio_buffer{
    long messaggio_tipo;
    char messaggio_testo[100];
}messaggio;

































/*FIRME DELLE FUNZIONI*/

/*genera casualmente l'ID della merce*/
int generatore_id_merce();

/*genera casualmente dimensione esatta delle merci*/
int generatore_dimensione_merce();

/*genera casualmente il tempo di vita delle merci*/
int* generatore_tempo_vita_merce(int *merce_offerta_id);

/*genero casualmente il numero di lotti di merce*/
int generatore_lotti_merce();

/*creo una nuova memoria condivisa*/
int memoria_condivisa_creazione(key_t key, size_t grandezza_memoria);

/*utilizzo una nuova memoria condivisa*/
int memoria_condivisa_get(key_t key, size_t grandezza_memoria, int mode);

/*dealloco una memoria condivisa già esistente*/
void memoria_condivisa_deallocazione(int id);

/*genera casualmente la posizione iniziale della nave, e restituisce un puntatore di array*/
double *generatore_posizione_iniziale_nave();

/*calcola la distanza della nave da un porto*/
double distanza_nave_porto(double posizione_nave_X, double posizione_nave_Y, double posizione_porto_X, double posizione_porto_Y);

/*restituisce lo spostamento della nave giorno per giorno*/
double spostamento_nave(double *posizione_nave, double posizione_porto_X, double posizione_porto_Y);

/*genera casualmente la posizione iniziale del porto*/
double *generatore_posizione_iniziale_porto(pid_t pid, pid_t parent_pid);

/*generatore del numero di banchine del porto*/
double generatore_banchine_porto();

/*generatore della capacita massima delle navi*/
int generatore_capacita_nave();

/*creazione coda di messaggi*/
int coda_messaggi_creazione(key_t key);

/*gestione coda di messaggi*/
int coda_messaggi_get_id(key_t key);

/*attendo un messaggio dalla coda di messaggi*/
int attesa_messaggio(int messaggio_id);

/*deallocazione coda di messaggi*/
void coda_messaggi_deallocazione(int coda_messaggi_id);

/*restituisce il tempo totale dello spostamento della nave da nave a porto*/
void tempo_spostamento_nave(double distanza_minima_temporanea);

/*ricerca del porto in cui sbarcare*/
int ricerca_binaria(struct struct_porto *shared_memory_porto, int inizio, int fine, double posizione_nave_X, double posizione_nave_Y, int so_porti);
int ricerca_binaria_porto(struct struct_nave nave, struct struct_porto *shared_memory_porto, int num_porti, int id_porto_precedente);

/*generatore del tempo di sosta della nave nel porto*/
void tempo_sosta_porto(struct struct_nave nave);

/*gestione del segnale per i procesi figli*/
void handle_child(int sig);

/*i processi figli sono pronti*/
void handle_ready(int sig);

/*generazione della chiave del semaforo delle banchine*/
void generatore_semaforo_banchine_nome(int pid, char *nome_risultato);























/*FUNZIONI*/

int generatore_id_merce(){
    
    FILE* config_file;
    int so_merci;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    fclose(config_file);

    if(so_merci == 1){
        return 0;
    }else{
        return rand()%so_merci;

    }
}

int generatore_dimensione_merce(){
    
    FILE* config_file;
    int so_size;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_SIZE") == 0) {
            so_size = value;
        }
    }

    fclose(config_file);

    return rand()%(so_size -1 + 1) + 1;;
}

int* generatore_tempo_vita_merce(int *merce_offerta_id){
    int i, valore_elemento, differenza, valore_calcolato, so_merci;
    int* merce_tempo_vita;
    int max_vita, min_vita;
    FILE* config_file;

    config_file = fopen("config.txt", "r");
    if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        return NULL;
    }

    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_MAX_VITA") == 0) {
            max_vita = value;
        }
        if (strcmp(name, "SO_MIN_VITA") == 0) {
            min_vita = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    fclose(config_file);

    for (i = 0; i < so_merci; i++) {
        if(merce_offerta_id[i] > 0){
            valore_elemento = merce_offerta_id[i];
            if (valore_elemento == 1) {
                merce_tempo_vita[i] = min_vita;
            } else {
                differenza = max_vita - min_vita;
                valore_calcolato = min_vita + (valore_elemento % differenza);
                if (valore_calcolato > max_vita) {
                    valore_calcolato = max_vita;
                }
                merce_tempo_vita[i] = valore_calcolato;
            }
        }
    }

    return merce_tempo_vita;
}


int generatore_lotti_merce(){
    return rand()%10+2;
}

/*Gestione della Memoria Condivisa*/
int memoria_condivisa_creazione(key_t key, size_t grandezza_memoria){
    int id;
    id = shmget(key, grandezza_memoria, 0666 | IPC_CREAT);
    if(id == -1){
        printf("Errore nella creazione della memoria condivisa! Error number = %d: %s\n", errno, strerror(errno));
        shmctl(id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    return id;
}

int memoria_condivisa_get(key_t key, size_t grandezza_memoria, int mode){
    int id;
    id = shmget(key, grandezza_memoria, mode);
    if(id == -1){
        printf("Errore nella get della memoria condivisa! Error number = %d: %s\n", errno, strerror(errno));
        shmctl(id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    return id;
}

void memoria_condivisa_deallocazione(int id){
    shmdt(&id);
    shmctl(id, IPC_RMID, NULL);
}

double *generatore_posizione_iniziale_nave(){
    double *coordinate_generate;

    
    FILE* config_file;
    int so_lato;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_LATO") == 0) {
            so_lato = value;
        }
    }

    fclose(config_file);

    coordinate_generate = malloc(16);
    coordinate_generate[0] = rand()%(so_lato - 0 + 1) + 0;
    coordinate_generate[1] = rand()%(so_lato - 0 + 1) + 0;
    return coordinate_generate;
}

double distanza_nave_porto(double posizione_nave_X, double posizione_nave_Y, double posizione_porto_X, double posizione_porto_Y){
    return hypot(posizione_porto_X - posizione_nave_X, posizione_porto_Y - posizione_nave_Y);
}

double spostamento_nave(double *posizione_nave, double posizione_porto_X, double posizione_porto_Y){
    double elemento_x1;
    double elemento_y1;
    double elemento_x2;
    double elemento_y2;
    double numeratore_senza_radice;
    double numeratore_con_radice;
    double risultato;

    
    FILE* config_file;
    int so_speed;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_SPEED") == 0) {
            so_speed = value;
        }
    }

    fclose(config_file);

    elemento_x1 = *posizione_nave;
    posizione_nave++;
    elemento_y1 = *posizione_nave;
    elemento_x2 = posizione_porto_X;
    elemento_y2 = posizione_porto_Y;

    numeratore_senza_radice = pow((elemento_x2 - elemento_x1), 2) + pow((elemento_y2 - elemento_y1), 2);
    numeratore_con_radice = sqrt(numeratore_senza_radice);  
    risultato = (double)numeratore_con_radice/(double)so_speed;
    return risultato;
}

double *generatore_posizione_iniziale_porto(pid_t pid, pid_t parent_pid){  
    double *coordinate_generate;
    int dato;

    
    FILE* config_file;
    int so_lato;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_LATO") == 0) {
            so_lato = value;
        }
    }

    fclose(config_file);

    coordinate_generate = malloc(sizeof(double)*2);
    dato = pid % 4;

    if((pid - parent_pid) <= 4){
        switch(dato){
            case 0:
                coordinate_generate[0] = 0;
                coordinate_generate[1] = 0;
                break;
            case 1:
                coordinate_generate[0] = so_lato;
                coordinate_generate[1] = 0;
                break;
            case 2:
                coordinate_generate[0] = so_lato;
                coordinate_generate[1] = so_lato;
                break;
            case 3:
                coordinate_generate[0] = 0;
                coordinate_generate[1] = so_lato;
                break;
            default:
                break;
        }
    }else{
        coordinate_generate[0] = rand()%(so_lato - 0 + 1) + 0;
        coordinate_generate[1] = rand()%(so_lato - 0 + 1) + 0;
    }
    
    
    return coordinate_generate;
}

double generatore_banchine_porto(){
    int numero_randomico;

    
    FILE* config_file;
    int so_banchine;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_BANCHINE") == 0) {
            so_banchine = value;
        }
    }

    fclose(config_file);

    numero_randomico = rand()%so_banchine+2;
    return numero_randomico;
}

int generatore_capacita_nave(){
    int numero_randomico;

    
    FILE* config_file;
    int so_capacity;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_CAPACITY") == 0) {
            so_capacity = value;
        }
    }

    fclose(config_file);

    numero_randomico = rand()%so_capacity;
    return numero_randomico;
}

int coda_messaggi_creazione(key_t key){
    int risultato = msgget(key, IPC_CREAT | IPC_EXCL | 0666);

    if(risultato == -1){
        printf("Errore nella creazione della coda di messaggi\n");
        exit(EXIT_FAILURE);
    }

    return risultato;
}

int coda_messaggi_get_id(key_t key){
    int risultato;
    risultato = msgget(key, 0666);

    if(risultato == -1){
        printf("Errore, non esiste una coda di messaggi associata alla key\n");
    }

    return risultato;
}

int attesa_messaggio(int messaggio_id) {
    struct struct_messaggio_buffer messaggio;

    /*attesa del messaggio*/
    if (msgrcv(messaggio_id, &messaggio, sizeof(messaggio), 1, 0) == -1) {
        perror("Errore durante la ricezione del messaggio");
        exit(EXIT_FAILURE);
    }

    printf("Messaggio ricevuto: %s\n", messaggio.messaggio_testo);

    /* restituisce 0 se il messaggio ricevuto è '.', altrimenti 1*/
    if (strcmp(messaggio.messaggio_testo, ".") == 0) {
        return 0;
    } else {
        return 1;
    }
}

void coda_messaggi_deallocazione(int coda_messaggi_id){
    if(msgctl(coda_messaggi_id, IPC_RMID, NULL) == -1){
        printf("Errore nella deallocazione della coda di messaggi\n");
        exit(EXIT_FAILURE);
    }
}

void tempo_spostamento_nave(double distanza_minima_temporanea){
    float nave_spostamento_nanosleep;
    struct timespec request;

    
    FILE* config_file;
    int so_speed;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_SPEED") == 0) {
            so_speed = value;
        }
    }

    fclose(config_file);

    nave_spostamento_nanosleep = distanza_minima_temporanea/so_speed;
    request.tv_sec = (int)nave_spostamento_nanosleep;
    request.tv_nsec = (nave_spostamento_nanosleep-request.tv_sec)*1000;

    if(nanosleep(&request, NULL) < 0){
        perror("Errore nella nanosleep dello spostamento della nave");
    }

    nave_spostamento_nanosleep = 0;
    request.tv_sec = 0;
    request.tv_nsec = 0;
}

int ricerca_binaria(struct struct_porto *shared_memory_porto, int inizio, int fine, double posizione_nave_X, double posizione_nave_Y, int so_porti){
    int indice_medio, distanza_corrente, indice_sinistra, indice_destra;

    /*se non ci sono più porti disponibili, restituisce `so_porti + 10`.*/
    if(fine < inizio){
        return so_porti + 10;
    }

    /*calcola l'indice medio tra inizio e fine*/
    indice_medio = (inizio + fine) / 2;

    /*se il porto in posizione media ha zero banchine libere, cerca il porto più vicino a destra o a sinistra del porto in posizione media*/
    if(shared_memory_porto[indice_medio].numero_banchine_libere == 0){
        if(indice_medio == inizio){
            /*se il porto in posizione media è l'ultimo porto a sinistra, cerca il porto più vicino a destra del porto in posizione media*/
            return ricerca_binaria(shared_memory_porto, indice_medio + 1, fine, posizione_nave_X, posizione_nave_Y, so_porti);
        } else{
            /*se il porto in posizione media non è l'ultimo porto a sinistra, cerca il porto più vicino a sinistra del porto in posizione media*/
            return ricerca_binaria(shared_memory_porto, inizio, indice_medio - 1, posizione_nave_X, posizione_nave_Y, so_porti);
        }
    } else{
        /*se il porto in posizione media ha banchine libere, calcola la distanza tra la nave e il porto in posizione media*/
        distanza_corrente = distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_medio].posizione_porto_X, shared_memory_porto[indice_medio].posizione_porto_Y);

        /*esegue la ricerca binaria a sinistra di indice_medio e assegna il risultato alla variabile indice_sinistra*/
        indice_sinistra = ricerca_binaria(shared_memory_porto, inizio, indice_medio - 1, posizione_nave_X, posizione_nave_Y, so_porti);

        /*esegue la ricerca binaria a destra di indice_medio e assegna il risultato alla variabile indice_destra*/
        indice_destra = ricerca_binaria(shared_memory_porto, indice_medio + 1, fine, posizione_nave_X, posizione_nave_Y, so_porti);

        /*ee la distanza corrente è minore o uguale alla distanza tra la nave e i porti a sinistra e a destra, restituisce indice_medio*/
        if(distanza_corrente <= distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_sinistra].posizione_porto_X, shared_memory_porto[indice_sinistra].posizione_porto_Y) &&
           distanza_corrente <= distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_destra].posizione_porto_X, shared_memory_porto[indice_destra].posizione_porto_Y)){
            return indice_medio;
        } else if(distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_sinistra].posizione_porto_X, shared_memory_porto[indice_sinistra].posizione_porto_Y) <=
                  distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_destra].posizione_porto_X, shared_memory_porto[indice_destra].posizione_porto_Y)){
            /*se la distanza dal porto a sinistra è minore o uguale alla distanza dal porto a destra, restituisce indice_sinistra*/
            return indice_sinistra;
        } else{
            /*altrimenti restituisco indice_destra*/
            return indice_destra;
        }
    }
}


void tempo_sosta_porto(struct struct_nave nave){
    int dimensione_totale_merce, i;
    float tempo_sosta_porto_nanosleep;
    struct timespec remaining, request;

    
    FILE* config_file;
    int so_speed, so_merci;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_SPEED") == 0) {
            so_speed = value;
        }
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    fclose(config_file);

    dimensione_totale_merce = 0;

    for(i = 0; i < so_merci; i++){
        dimensione_totale_merce += nave.merce_nave.dimensione_merce[i];
    }

    tempo_sosta_porto_nanosleep = dimensione_totale_merce/so_speed;
    request.tv_sec = (int)tempo_sosta_porto_nanosleep;
    request.tv_nsec = (tempo_sosta_porto_nanosleep-request.tv_sec)*1000;

    if(nanosleep(&request, &remaining) < 0){
        perror("Errore nella nanosleep dello spostamento della nave");
    }
}

void handle_child(int sig){
    int status;
    pid_t child;

    child = waitpid(-1, &status, WNOHANG);
    if(child > 0){
        WEXITSTATUS(status);
    }
}

/*invia al padre il segnale SIGUSR1 per indicare che il figlio è pronto*/
void handle_ready(int sig) {
    kill(getppid(), SIGUSR1);
}

int ricerca_binaria_porto(struct struct_nave nave, struct struct_porto *shared_memory_porto, int num_porti, int id_porto_precedente){
    int index_porto_scelto = -1;
    int num_porti_validi = 0;
    int i, j;
    int index_porto_casuale;
    int *indici_porti_validi = NULL;

    FILE* config_file;
    int so_merci;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_MERCI") == 0) {
            so_merci = value;
        }
    }

    fclose(config_file);

    /*trova tutti i porti validi*/
    for (i = 0; i < num_porti; i++){
        /*verifica che il porto richieda la tipologia di merce cercata*/
        for (j = 0; j < so_merci; j++) {
            if (shared_memory_porto[i].merce_offerta_quantita[j] > 0 && nave.merce_nave.dimensione_merce[j] > 0 && shared_memory_porto[i].numero_banchine_libere > 0){
                /*verifica che il porto non sia lo stesso della volta precedente*/
                if (i != id_porto_precedente){
                    /*verifica se ci sono ancora lotti di merce disponibili in questo porto*/
                    if (shared_memory_porto[i].numero_lotti_merce[j] > 0){ /*QUA STA IL PROBLEMA, I LOTTI SONO SEMPRE A 0*/
                        num_porti_validi++;
                        indici_porti_validi = realloc(indici_porti_validi, num_porti_validi * sizeof(int));
                        if (indici_porti_validi == NULL) {
                            perror("Errore durante la reallocazione della memoria");
                            exit(EXIT_FAILURE);
                        }
                        indici_porti_validi[num_porti_validi - 1] = i;
                        break; /*esce da j*/
                    }
                }
            }
        }
    }

    /*scegli un porto valido a caso tra quelli disponibili*/
    if (num_porti_validi > 0){
        index_porto_casuale = rand() % num_porti_validi;
        index_porto_scelto = indici_porti_validi[index_porto_casuale];
    }

    /*libera la memoria allocata dinamicamente*/
    free(indici_porti_validi);

    return index_porto_scelto;
}

void generatore_semaforo_banchine_nome(int pid, char *nome_risultato){
    snprintf(nome_risultato, 100000, "%s_%d", "sem", pid); 
}









#endif