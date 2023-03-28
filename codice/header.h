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

#define SHM_KEY_MERCE 1234
#define SHM_KEY_PORTO 1236
#define SHM_KEY_NAVE 7896
#define SHM_KEY_CONTEGGIO 7845
#define SHM_KEY_SEM 6789
#define SHM_KEY_GIORNO 2002
#define SEM_KEY 9876 
#define MSG_KEY 2367
#define SEM_VAL 0

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

const char *semaforo_nome = "/semaforo";
const char *semaforo_nave_nome = "/semaforoNave";

struct struct_merce{
    int id_merce; /*genero la tipologia di merce con un ID numerico*/
    int dimensione_merce; /*genero la grandezza delle merci*/
    int tempo_vita_merce; /*genero la durata di vita delle merci*/
};

struct struct_nave{
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
    int merce_richiesta_id;
    int merce_richiesta_quantita;
    int merce_offerta_id;
    int merce_offerta_quantita;
    int merce_offerta_tempo_vita;

    int numero_lotti_merce;

    int conteggio_merce_spedita_porto; 
    int conteggio_merce_ricevuta_porto;
};

struct struct_tempo_spostamento{
    int secondi;
    float nano_secondi;
};

struct struct_messaggio_buffer{
    long messaggio_tipo;
    char messaggio_testo[100];
}messaggio;

struct struct_controllo_scadenze_statistiche{
    int numero_navi_senza_merce;
    int numero_porti_senza_merce;
    int merce_consegnata[100];

    int navi_nel_porto;
    int navi_con_carico;
    int navi_senza_carico;

    int merce_generata_inizialmente[100];
    int merce_scaduta_porto[100];
    int merce_scaduta_nave[100];
};

struct struct_giorni{
    int giorni;
};


/*
######################################
######################################
######################################
            LE FUNZIONI
######################################
######################################
######################################
*/


/*calcola la potenza*/
double potenza(double n, int exp){
    int risultato, i;
    risultato = 1;
    for(i = 1; i <= exp; i++){
        risultato = risultato * n;
    }
    return risultato;
}

/*calcola la radice quadrata*/
double radice_quadrata(double n){
    int sqrt, temp;
    sqrt = n/2;
    temp = 0;

    while(sqrt != temp){
        temp = sqrt;
        sqrt = (n/temp + temp)/2;
    }

    return sqrt;
}

/*genera casualmente l'ID della merce*/
int generatore_id_merce(){
    /*cattura delle variabili*/
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

/*genera casualmente dimensione esatta delle merci*/
int generatore_dimensione_merce(){
    /*cattura delle variabili*/
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

/*genera casualmente il tempo di vita delle merci*/
int generatore_tempo_vita_merce(){ 
    /*cattura delle variabili*/
    FILE* config_file;
    int max_vita;
    int min_vita;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "MAX_VITA") == 0) {
            max_vita = value;
        }
        if (strcmp(name, "MIN_VITA") == 0) {
            min_vita = value;
        }
    }

    fclose(config_file);

    return rand()%(max_vita - min_vita +1) + min_vita;
}

/*genero casualmente il numero di lotti di merce*/
int generatore_lotti_merce(){
    return rand()%10+2;
}

/*genero casualmente il tempo di vita della merce offerta*/
int generatore_tempo_vita_merce_offerta(int id_merce, pid_t pid_porto){
    int i;
    int j;
    int risultato;
    struct struct_merce* vettore_di_merci;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_navi;
    int so_porti;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_NAVI") == 0) {
            so_navi = value;
        }
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
    }

    fclose(config_file);


    vettore_di_merci = (struct struct_merce*)malloc(sizeof(struct struct_merce)*(so_porti+so_navi));
    risultato = 0;
    srand(pid_porto);

    /*generazione delle merci e inserimento nell'array*/
    for(i = 0; i < (so_porti+so_navi); i++){
        vettore_di_merci[i].id_merce = generatore_id_merce();
        vettore_di_merci[i].dimensione_merce = generatore_dimensione_merce();
        vettore_di_merci[i].tempo_vita_merce = generatore_tempo_vita_merce();
        for(j = 0; j < (so_porti+so_navi); j++){
            if(vettore_di_merci[i].id_merce == vettore_di_merci[j].id_merce){
                vettore_di_merci[i].tempo_vita_merce = vettore_di_merci[j].tempo_vita_merce;
            }
        }
    }

    for(i = 0; i < (so_porti+so_navi); i++){
        if(vettore_di_merci[i].id_merce == id_merce){
            risultato = vettore_di_merci[i].tempo_vita_merce+1;
        }
    }

    if(risultato == 0){
        return generatore_tempo_vita_merce();
    }else{
        return risultato;
    }
}

/*restituisco il tempo di occupazione della banchina*/
int tempo_occupazione_banchina(int quantita_merce_scambiata, int velocita_carico_scarico){
    return quantita_merce_scambiata / velocita_carico_scarico;
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

int memoria_condivisa_get_id(key_t key, size_t grandezza_memoria){
    int id = shmget(key, grandezza_memoria, 0666);
    if(id == -1){
        printf("Errore, non esiste una memoria condivisa associata alla key! Error number = %d\n", errno);
    }
    return id;
}

void memoria_condivisa_deallocazione(int id){
    shmdt(&id);
    shmctl(id, IPC_RMID, NULL);
}

/*genera casualmente la posizione iniziale della nave, e restituisce un puntatore di array*/
double *generatore_posizione_iniziale_nave(){
    double *coordinate_generate;

    /*cattura delle variabili*/
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

/*calcola la distanza della nave da un porto*/
double distanza_nave_porto(double posizione_nave_X, double posizione_nave_Y, double posizione_porto_X, double posizione_porto_Y){
    return hypot(posizione_porto_X - posizione_nave_X, posizione_porto_Y - posizione_nave_Y);
}

/*restituisce lo spostamento della nave giorno per giorno*/
double spostamento_nave(double *posizione_nave, double posizione_porto_X, double posizione_porto_Y){
    double elemento_x1;
    double elemento_y1;
    double elemento_x2;
    double elemento_y2;
    double numeratore_senza_radice;
    double numeratore_con_radice;
    double risultato;

    /*cattura delle variabili*/
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

    numeratore_senza_radice = potenza((elemento_x2 - elemento_x1), 2) + potenza((elemento_y2 - elemento_y1), 2);
    numeratore_con_radice = radice_quadrata(numeratore_senza_radice);  
    risultato = (double)numeratore_con_radice/(double)so_speed;
    return risultato;
}

/*genera casualmente la posizione iniziale del porto*/
double *generatore_posizione_iniziale_porto(pid_t pid, pid_t parent_pid){  
    double *coordinate_generate;
    int dato;

    /*cattura delle variabili*/
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

/*generatore del numero di banchine del porto*/
double generatore_banchine_porto(){
    int numero_randomico;

    /*cattura delle variabili*/
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

/*generatore della capacita massima delle navi*/
int generatore_capacita_nave(){
    int numero_randomico;

    /*cattura delle variabili*/
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

/*generatore dell'id della merce offerta*/
int generatore_merce_offerta_id(){
    /*cattura delle variabili*/
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

/*generatore della quantita della merce offerta*/
int generatore_merce_offerta_quantita(int merce_richiesta_quantita){
    int numero_randomico;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_fill;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_FILL") == 0) {
            so_fill = value;
        }
    }

    fclose(config_file);

    numero_randomico = so_fill - merce_richiesta_quantita;
    return numero_randomico;
}

/*generatore dell'id della merce richiesta*/
int generatore_merce_richiesta_id(){
    int numero_randomico;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_merci;

    so_merci = 1;

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

/*generatore della quantita della merce richiesta*/
int generatore_merce_richiesta_quantita(){
    int numero_randomico;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_fill;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_FILL") == 0) {
            so_fill = value;
        }
    }

    fclose(config_file);

    numero_randomico = rand()%so_fill;
    return numero_randomico;
}

/*gestione dei messaggi*/
int coda_messaggi_creazione(key_t key){
    int risultato = msgget(key, IPC_CREAT | IPC_EXCL | 0666);

    if(risultato == -1){
        /*printf("Errore nella creazione della coda di messaggi\n");*/
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

void coda_messaggi_deallocazione(int coda_messaggi_id){
    if(msgctl(coda_messaggi_id, IPC_RMID, NULL) == -1){
        printf("Errore nella deallocazione della coda di messaggi\n");
        exit(EXIT_FAILURE);
    }
}

/*restituisce il tempo totale dello spostamento della nave da nave a porto*/
void tempo_spostamento_nave(double distanza_minima_temporanea){
    float nave_spostamento_nanosleep;
    struct timespec request;

    /*cattura delle variabili*/
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

int ricerca_binaria(struct struct_porto *shared_memory_porto, int inizio, int fine, double posizione_nave_X, double posizione_nave_Y){
    int indice_medio, distanza_corrente, indice_sinistra, indice_destra;

    /*cattura delle variabili*/
    FILE* config_file;
    int so_porti;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
    }

    fclose(config_file);

    if(fine < inizio){
        return so_porti + 10;
    }

    indice_medio = (inizio + fine) / 2;
    if(shared_memory_porto[indice_medio].numero_banchine_libere == 0){
        if(indice_medio == inizio){
            return ricerca_binaria(shared_memory_porto, indice_medio + 1, fine, posizione_nave_X, posizione_nave_Y);
        } else{
            return ricerca_binaria(shared_memory_porto, inizio, indice_medio - 1, posizione_nave_X, posizione_nave_Y);
        }
    } else{
        distanza_corrente = distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_medio].posizione_porto_X, shared_memory_porto[indice_medio].posizione_porto_Y);
        indice_sinistra = ricerca_binaria(shared_memory_porto, inizio, indice_medio - 1, posizione_nave_X, posizione_nave_Y);
        indice_destra = ricerca_binaria(shared_memory_porto, indice_medio + 1, fine, posizione_nave_X, posizione_nave_Y);

        if(distanza_corrente <= distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_sinistra].posizione_porto_X, shared_memory_porto[indice_sinistra].posizione_porto_Y) &&
           distanza_corrente <= distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_destra].posizione_porto_X, shared_memory_porto[indice_destra].posizione_porto_Y)){
            return indice_medio;
        } else if(distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_sinistra].posizione_porto_X, shared_memory_porto[indice_sinistra].posizione_porto_Y) <=
                  distanza_nave_porto(posizione_nave_X, posizione_nave_Y, shared_memory_porto[indice_destra].posizione_porto_X, shared_memory_porto[indice_destra].posizione_porto_Y)){
            return indice_sinistra;
        } else{
            return indice_destra;
        }
    }
}

int calcolo_porto_piu_vicino(double posizione_nave_X, double posizione_nave_Y, struct struct_porto *shared_memory_porto){
    /*cattura delle variabili*/
    FILE* config_file;
    int so_porti;

    config_file = fopen("config.txt", "r");
     if (config_file == NULL) {
        printf("Errore nell'apertura del file\n");
        
    }
    while (!feof(config_file)) {
        char name[20];
        int value;
        fscanf(config_file, "%19[^=]=%d\n", name, &value);
        if (strcmp(name, "SO_PORTI") == 0) {
            so_porti = value;
        }
    }

    fclose(config_file);

    return ricerca_binaria(shared_memory_porto, 0, so_porti - 1, posizione_nave_X, posizione_nave_Y);
}


/*generatore del tempo di sosta della nave nel porto*/
void tempo_sosta_porto(int dimensione_merce){
    float tempo_sosta_porto_nanosleep;
    struct timespec remaining, request;

    /*cattura delle variabili*/
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

    tempo_sosta_porto_nanosleep = dimensione_merce/so_speed;
    request.tv_sec = (int)tempo_sosta_porto_nanosleep;
    request.tv_nsec = (tempo_sosta_porto_nanosleep-request.tv_sec)*1000;

    if(nanosleep(&request, &remaining) < 0){
        perror("Errore nella nanosleep dello spostamento della nave");
    }
}

/*gestione segnale*/
void handle_child(int sig){
    int status;
    pid_t child;

    child = waitpid(-1, &status, WNOHANG);
    if(child > 0){
        WEXITSTATUS(status);
    }
}

void handle_ready(int sig) {
    /* invia il segnale SIGUSR1 al processo padre */
    kill(getppid(), SIGUSR1);
}


int ricerca_binaria_porto(int id_merce, struct struct_porto *shared_memory_porto, int num_porti, int id_porto_precedente){
    int index_porto_scelto = -1;
    int num_porti_validi = 0;
    int i;
    int index_porto_casuale;
    int indici_porti_validi[1000];

    /* Trova tutti i porti validi */
    for (i = 0; i < num_porti; i++){
        /* Verifica che il porto richieda la tipologia di merce cercata */
        if (shared_memory_porto[i].merce_richiesta_id == id_merce && shared_memory_porto[i].numero_banchine_libere > 0){
            /* Verifica che il porto non sia lo stesso della volta precedente */
            if (i != id_porto_precedente){
                /* Verifica se ci sono ancora lotti di merce disponibili in questo porto */
                if (shared_memory_porto[i].numero_lotti_merce > 0){
                    indici_porti_validi[num_porti_validi] = i;
                    num_porti_validi++;
                }
            }
        }
    }

    /* Scegli un porto valido a caso tra quelli disponibili */
    if (num_porti_validi > 0){
        index_porto_casuale = rand() % num_porti_validi;
        index_porto_scelto = indici_porti_validi[index_porto_casuale];
    }

    return index_porto_scelto;
}


























/*stampa del report giornaliero*/
/*void print_report_giornaliero(struct struct_conteggio_nave *conteggio_nave, struct struct_merce *merce_nella_nave, int numero_giorno, struct struct_porto *informazioni_porto, int *somma_merci_disponibili, int *conteggio_merce_consegnata){
    int i;
    int j;
    int conteggio_navi_con_carico_totale;
    int conteggio_navi_senza_carico_totale;
    int conteggio_navi_nel_porto_totale;

    conteggio_navi_con_carico_totale = 0;
    conteggio_navi_senza_carico_totale = 0;
    conteggio_navi_nel_porto_totale = 0;

    for(i = 0; i < SO_PORTI; i++){
        somma_merci_disponibili[i] = 0;
        conteggio_merce_consegnata[i] = 0;
    }


    for(i = 0; i < (SO_NAVI+2)-1; i++){
        conteggio_navi_con_carico_totale = conteggio_navi_con_carico_totale + conteggio_nave[i].conteggio_navi_con_carico;
        conteggio_navi_senza_carico_totale = conteggio_navi_senza_carico_totale + conteggio_nave[i].conteggio_navi_senza_carico;
        conteggio_navi_nel_porto_totale = conteggio_navi_nel_porto_totale + conteggio_nave[i].conteggio_navi_nel_porto;
    }

    sleep(1);

    printf("\n\n------------------------------------\n\n");
    printf("REPORT GIORNO %d\n", numero_giorno); 
    printf("Merci:\n");  
    
    for(i = 0; i < SO_MERCI; i++){
        for(j = 0; j < SO_PORTI; j++){
            if(i == informazioni_porto[j].merce_offerta_id){
                somma_merci_disponibili[i] = somma_merci_disponibili[i] + informazioni_porto[j].merce_offerta_quantita * informazioni_porto[j].numero_lotti_merce;
                conteggio_merce_consegnata[i] = conteggio_merce_consegnata[i] + informazioni_porto[j].conteggio_merce_ricevuta_porto;
            }
        } 
        
        printf("\tTipologia: %d -> Disponibile: %d tonnellate & Consegnata: %d tonnellate\n", i, somma_merci_disponibili[i], conteggio_merce_consegnata[i]); 
    }
      
    printf("Navi:\n");
    printf("\tCon un carico a bordo: %d - Senza un carico a bordo: %d - Nel porto: %d\n", conteggio_navi_con_carico_totale, conteggio_navi_senza_carico_totale, conteggio_navi_nel_porto_totale);                                                                              
    printf("Porti:\n");
    for(i = 0; i < SO_PORTI; i++){
        printf("\tNumero porto: %d - Merce disponibile: %d - Merce totale spedita e ricevuta in tonnellate: %d, %d - Numero di banchine libere: %d - Lotti rimanenti: %d\n", i+1, informazioni_porto[i].merce_offerta_quantita * informazioni_porto[i].numero_lotti_merce, informazioni_porto[i].conteggio_merce_spedita_porto, informazioni_porto[i].conteggio_merce_ricevuta_porto, informazioni_porto[i].numero_banchine_libere, informazioni_porto[i].numero_lotti_merce);
    }

    fflush(stdout); /*svuoto il buffer dello standard output*/
/*}

/*stampa del report finale*/
/*void print_report_finale(struct struct_conteggio_nave *conteggio_nave, struct struct_merce *merce_nella_nave, int numero_giorno, struct struct_porto *informazioni_porto, int *somma_merci_disponibili, int *conteggio_merce_consegnata, int* totale_merce_generata_inizialmente, int *merce_scaduta_in_nave, int *merce_scaduta_in_porto, int tappe_nei_porti){
    int i;
    int j;
    int k;
    int conteggio_navi_con_carico_totale;
    int conteggio_navi_senza_carico_totale;
    int conteggio_navi_nel_porto_totale;
    int porto_offerto_maggiore_conto;
    int porto_offerto_maggiore;
    int porto_richiesto_maggiore_conto;
    int porto_richiesto_maggiore;

    conteggio_navi_con_carico_totale = 0;
    conteggio_navi_senza_carico_totale = 0;
    conteggio_navi_nel_porto_totale = 0;
    porto_offerto_maggiore = 0;
    porto_offerto_maggiore_conto = 0;
    porto_richiesto_maggiore_conto = 0;
    porto_richiesto_maggiore = 0;

    for(i = 0; i < (SO_NAVI+2)-1; i++){
        conteggio_navi_con_carico_totale = conteggio_navi_con_carico_totale + conteggio_nave[i].conteggio_navi_con_carico;
        conteggio_navi_senza_carico_totale = conteggio_navi_senza_carico_totale + conteggio_nave[i].conteggio_navi_senza_carico;
        conteggio_navi_nel_porto_totale = conteggio_navi_nel_porto_totale + conteggio_nave[i].conteggio_navi_nel_porto;
    }

    for(i = 0; i < SO_MERCI; i++){
        somma_merci_disponibili[i] = 0;
        conteggio_merce_consegnata[i] = 0;
    }

    printf("\n\n------------------------------------\n\n");
    printf("REPORT FINALE\n");

    printf("Navi: \n");
    printf("\tNumero di navi in mare con un carico a bordo: %d\n", conteggio_navi_con_carico_totale);
    printf("\tNumero di navi in mare senza un carico: %d\n", conteggio_navi_senza_carico_totale);
    printf("\tNumero di navi che occupano una banchina: %d\n", conteggio_navi_nel_porto_totale);

    printf("Merci:\n");  
    for(i = 0; i < SO_MERCI; i++){
        for(j = 0; j < SO_PORTI; j++){
            if(i == informazioni_porto[j].merce_offerta_id){
                somma_merci_disponibili[i] = somma_merci_disponibili[i] + informazioni_porto[j].merce_offerta_quantita * informazioni_porto[j].numero_lotti_merce;
                conteggio_merce_consegnata[i] = conteggio_merce_consegnata[i] + informazioni_porto[j].conteggio_merce_ricevuta_porto;
            }
        } 
        printf("\tTipologia: %d -> \n\t\tDisponibile: %d tonnellate & Consegnata: %d tonnellate\n", i, somma_merci_disponibili[i], conteggio_merce_consegnata[i]);
        /*printing per ogni tipo di merce*/
        /*printf("\t\tTonnellate di merce iniziale: %d\n", totale_merce_generata_inizialmente[i]); 
        printf("\t\tTonnellate rimaste nel porto: %d & Scadute nel porto: %d &  Scadute nella nave: %d & Consegnata da qualche nave: %d\n", somma_merci_disponibili[i], merce_scaduta_in_porto[i], merce_scaduta_in_nave[i], conteggio_merce_consegnata[i]);
    }
    

    printf("Porti:\n");
    for(i = 0; i < SO_PORTI; i++){
        printf("\tNumero porto: %d - Merce totale spedita e ricevuta in tonnellate: %d, %d - Numero di banchine libere: %d - Lotti rimanenti: %d\n", i+1, informazioni_porto[i].conteggio_merce_spedita_porto, informazioni_porto[i].conteggio_merce_ricevuta_porto, informazioni_porto[i].numero_banchine_libere, informazioni_porto[i].numero_lotti_merce);
    }
    for(i = 0; i < SO_PORTI; i++){
        if(porto_offerto_maggiore_conto < informazioni_porto[i].conteggio_merce_spedita_porto){
            porto_offerto_maggiore_conto = informazioni_porto[i].conteggio_merce_spedita_porto;
            porto_offerto_maggiore = i+1;
        }
        if(porto_richiesto_maggiore_conto < informazioni_porto[i].conteggio_merce_ricevuta_porto){
            porto_richiesto_maggiore_conto = informazioni_porto[i].conteggio_merce_ricevuta_porto;
            porto_richiesto_maggiore = i+1;
        }
    }

    /*fflush(stdout); /*svuoto il buffer dello standard output*/
   
    /*print del porto che ha offerto quantita maggiore di merce*/
    /*if(porto_offerto_maggiore_conto == 0){
        printf("\nIl porto che ha offerto la quantita' maggiore di merce e': nessun porto ha offerto merce\n");
    }else{
        printf("\nIl porto che ha offerto la quantita' maggiore di merce e': %d\n", porto_offerto_maggiore);
    }
    if(porto_richiesto_maggiore_conto == 0){
        printf("Il porto che ha richiesto la quantita' maggiore di merce e': nessun porto ha richiesto merce\n"); 
    }else{
        printf("Il porto che ha richiesto la quantita' maggiore di merce e': %d\n", porto_richiesto_maggiore);
    }

    fflush(stdout); /*svuoto il buffer dello standard output*//*
}*/

#endif