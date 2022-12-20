#ifndef _HEADER_H_
#define _HEADER_H_

#define _GNU_SOURCE

/*lista delle librerie utilizzate*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<time.h>
#include<string.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/sem.h>
#include<signal.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>

#define NO_NAVI 1+1 /*numero di navi*/
#define NO_PORTI 7 /*numero di porti, metterne sempre uno in piu*/
#define SO_MERCI 5 /*numero di tipologie di merci*/
#define NUMERO_TOTALE_MERCI 100 /*numero massimo di merci in tonnellate*/

#define SO_DAYS 5 /*durata totale in giorni dell'esperimento*/
#define MIN_VITA 5 /*minima vita della merce*/
#define MAX_VITA 15 /*massima vita merce*/
#define SO_SIZE 10000 /*peso massimo della merce di 40.000 Kg*/

#define SHM_KEY_MERCE 1234
#define SHM_KEY_PORTO 1236
#define SEM_KEY 9876 

#define SO_LATO 10000 /*grandezza per lato della mappa di 10.000 Km*/
#define SO_BANCHINE 5 /*numero di banchine*/

#define SO_CAPACITY 10000 /*massima capacità della nave di 10.000 T*/
#define SO_SPEED 1000 /*la velocità è di mille Kh/giorno*/

const char *semaforo_nome = "/semaforo";
const char *semaforo_nave_nome = "/semaforoNave";

int shared_memory_id_merce;
int numero_giorno;
struct struct_porto porto; 
struct struct_nave nave;
clock_t inizio_tempo;
clock_t differenza_tempo;
int millisecondi;
int conteggio_navi_con_carico;
int conteggio_navi_senza_carico;
int conteggio_navi_nel_porto;
int somma_merci_disponibili[SO_MERCI];
int conteggio_merce_consegnata[SO_MERCI];

struct struct_merce{
    int id_merce; /*genero la tipologia di merce con un ID numerico*/
    int dimensione_merce; /*genero la grandezza delle merci*/
    int tempo_vita_merce; /*genero la durata di vita delle merci*/
};

struct struct_nave{
    int velocita_nave; /*velocità nave*/
    double *posizione_nave; /*posizione della nave con coordinate*/
    int capacita_nave; /*capacità della nave*/
};

struct struct_conteggio_nave{
    int conteggio_navi_con_carico;
    int conteggio_navi_senza_carico;
    int conteggio_navi_nel_porto;
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
    int numero_randomico;
    numero_randomico = rand()%SO_MERCI+1;
    return numero_randomico;
}

/*genera casualmente dimensione esatta delle merci*/
int generatore_dimensione_merce(){
    int numero_randomico;
    numero_randomico = rand()%(SO_SIZE -1 + 1) + 1;
    return numero_randomico;
}

/*genera casualmente il tempo di vita delle merci*/
int generatore_tempo_vita_merce(){
    int numero_randomico;
    numero_randomico = rand()%(MAX_VITA - MIN_VITA +1) + MIN_VITA;
    return numero_randomico+1;
}

int generatore_lotti_merce(){
    int numero_randomico;
    numero_randomico = rand()%10+1;
    return numero_randomico;
}

int generatore_tempo_vita_merce_offerta(int id_merce, pid_t pid_porto){
    int i;
    int j;
    int risultato;
    struct struct_merce* vettore_di_merci;
    vettore_di_merci = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);
    risultato = 0;
    srand(pid_porto);

    /*generazione delle merci e inserimento nell'array*/
    for(i = 0; i < NUMERO_TOTALE_MERCI; i++){
        vettore_di_merci[i].id_merce = generatore_id_merce();
        vettore_di_merci[i].dimensione_merce = generatore_dimensione_merce();
        vettore_di_merci[i].tempo_vita_merce = generatore_tempo_vita_merce();
        for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
            if(vettore_di_merci[i].id_merce == vettore_di_merci[j].id_merce){
                vettore_di_merci[i].tempo_vita_merce = vettore_di_merci[j].tempo_vita_merce;
            }
        }
    }

    for(i = 0; i < NUMERO_TOTALE_MERCI; i++){
        if(vettore_di_merci[i].id_merce == id_merce){
            risultato = vettore_di_merci[i].tempo_vita_merce;
        }
    }

    return risultato;
}

/*genera casualmente un array di merci e lo restituisce*/
struct struct_merce* generatore_array_merci(){
    int i;
    int j;
    struct struct_merce* vettore_di_merci;
    vettore_di_merci = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NUMERO_TOTALE_MERCI);

    /*generazione delle merci e inserimento nell'array*/
    for(i = 0; i < NUMERO_TOTALE_MERCI; i++){
        vettore_di_merci[i].id_merce = generatore_id_merce();
        vettore_di_merci[i].dimensione_merce = generatore_dimensione_merce();
        vettore_di_merci[i].tempo_vita_merce = generatore_tempo_vita_merce();
        for(j = 0; j < NUMERO_TOTALE_MERCI; j++){
            if(vettore_di_merci[i].id_merce == vettore_di_merci[j].id_merce){
                vettore_di_merci[i].tempo_vita_merce = vettore_di_merci[j].tempo_vita_merce;
            }
        }
    }

    return vettore_di_merci;
}

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
    coordinate_generate = malloc(16);
    coordinate_generate[0] = rand()%(SO_LATO - 0 + 1) + 0;
    coordinate_generate[1] = rand()%(SO_LATO - 0 + 1) + 0;
    return coordinate_generate;
}

/*calcola la distanza della nave da un porto*/
double distanza_nave_porto(double *posizione_nave, double posizione_porto_X, double posizione_porto_Y){
    double elemento_x1;
    double elemento_y1;
    double elemento_x2;
    double elemento_y2;
    double numeratore_senza_radice;
    double numeratore_con_radice;
    double risultato;

    elemento_x1 = *posizione_nave;
    posizione_nave++;
    elemento_y1 = *posizione_nave;
    elemento_x2 = posizione_porto_X;
    elemento_y2 = posizione_porto_Y;

    numeratore_senza_radice = potenza((elemento_x2 - elemento_x1), 2) + potenza((elemento_y2 - elemento_y1), 2);
    numeratore_con_radice = radice_quadrata(numeratore_senza_radice); 

    return numeratore_con_radice;
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

    elemento_x1 = *posizione_nave;
    posizione_nave++;
    elemento_y1 = *posizione_nave;
    elemento_x2 = posizione_porto_X;
    elemento_y2 = posizione_porto_Y;

    numeratore_senza_radice = potenza((elemento_x2 - elemento_x1), 2) + potenza((elemento_y2 - elemento_y1), 2);
    numeratore_con_radice = radice_quadrata(numeratore_senza_radice);  
    risultato = (double)numeratore_con_radice/(double)SO_SPEED;
    return risultato;
}

double *generatore_posizione_iniziale_porto(pid_t pid, pid_t parent_pid){  /*modificare quando i porti sono piu di quattro*/
    double *coordinate_generate;
    int dato;
    coordinate_generate = malloc(sizeof(double)*2);

    dato = pid % 4;

    if((pid - parent_pid) <= 4){
        switch(dato){
            case 0:
                coordinate_generate[0] = 0;
                coordinate_generate[1] = 0;
                break;
            case 1:
                coordinate_generate[0] = SO_LATO;
                coordinate_generate[1] = 0;
                break;
            case 2:
                coordinate_generate[0] = SO_LATO;
                coordinate_generate[1] = SO_LATO;
                break;
            case 3:
                coordinate_generate[0] = 0;
                coordinate_generate[1] = SO_LATO;
                break;
            default:
                break;
        }
    }else{
        coordinate_generate[0] = rand()%(SO_LATO - 0 + 1) + 0;
        coordinate_generate[1] = rand()%(SO_LATO - 0 + 1) + 0;
    }
    
    
    return coordinate_generate;
}

double generatore_banchine_porto(){
    int numero_randomico;
    numero_randomico = rand()%SO_BANCHINE+2;
    return numero_randomico;
}

int generatore_capacita_nave(){
    int numero_randomico;
    numero_randomico = rand()%SO_CAPACITY;
    return numero_randomico;
}

/*genera casualmente un array di merci e lo restituisce*/
struct struct_nave* generatore_array_navi(){
    int i;
    int j;
    struct struct_nave* vettore_di_navi;
    vettore_di_navi = (struct struct_nave*)malloc(sizeof(struct struct_nave)*NO_NAVI);

    /*generazione delle merci e inserimento nell'array*/
    for(i = 0; i < NO_NAVI; i++){
        vettore_di_navi[i].posizione_nave = generatore_posizione_iniziale_nave();
        vettore_di_navi[i].capacita_nave = generatore_capacita_nave();
        vettore_di_navi[i].velocita_nave = SO_SPEED;
    }

    return vettore_di_navi;
}

int generatore_merce_offerta_id(){
    int numero_randomico;
    numero_randomico = rand()%SO_MERCI;
    return numero_randomico;
}

int generatore_merce_offerta_quantita(){
    int numero_randomico;
    numero_randomico = rand()%SO_SIZE;
    return numero_randomico;
}

int generatore_merce_richiesta_id(){
    int numero_randomico;
    numero_randomico = rand()%SO_MERCI;
    return numero_randomico;
}

int generatore_merce_richiesta_quantita(){
    int numero_randomico;
    numero_randomico = rand()%SO_SIZE;
    return numero_randomico;
}

int *generatore_merce_offerta_richiesta(){
    int *numero_randomico;

    do{
        numero_randomico[0] = rand()%SO_MERCI;
        numero_randomico[1] = rand()%NUMERO_TOTALE_MERCI+1;
        numero_randomico[2] = rand()%SO_MERCI;
        numero_randomico[3] = rand()%NUMERO_TOTALE_MERCI+1;
    }while(numero_randomico[0] == numero_randomico[2]);

    return numero_randomico;
}

/*gestione dei messaggi*/
int coda_messaggi_creazione(key_t key){
    int risultato = msgget(key, IPC_CREAT | IPC_EXCL | 0666);

    if(risultato == -1){
        printf("Errore nella creazione della coda di messaggi\n");
        exit(EXIT_FAILURE);
    }

    return risultato;
}

int coda_messaggi_get_id(key_t key){
    int risultato = msgget(key, 0666);

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

void tempo_spostamento_nave(float distanza_minima_temporanea){
    float nave_spostamento_nanosleep;
    struct timespec remaining, request;

    nave_spostamento_nanosleep = distanza_minima_temporanea/SO_SPEED;
    request.tv_sec = (int)nave_spostamento_nanosleep;
    request.tv_nsec = (nave_spostamento_nanosleep-request.tv_sec)*1000;

    if(nanosleep(&request, &remaining) < 0){
        perror("Errore nella nanosleep dello spostamento della nave");
    }
}

void print_report_giornaliero(struct struct_conteggio_nave conteggio_nave, struct struct_merce *merce_nella_nave, int numero_giorno, struct struct_porto *informazioni_porto, int *somma_merci_disponibili, int *conteggio_merce_consegnata){
    int i;
    int j;

    printf("\n\n------------------------------------\n\n");
    printf("REPORT GIORNO %d\n", numero_giorno); 
    printf("Merci:\n");  
    
    for(i = 0; i < SO_MERCI; i++){
        for(j = 0; j < NO_PORTI; j++){
            if(i == informazioni_porto[j].merce_offerta_id){
                somma_merci_disponibili[i] = somma_merci_disponibili[i] + informazioni_porto[j].merce_offerta_quantita;
                conteggio_merce_consegnata[i] = conteggio_merce_consegnata[i] + informazioni_porto[j].conteggio_merce_ricevuta_porto;
            }
        } 
        
        printf("\tTipologia: %d -> Disponibile: %d tonnellate & Consegnata: %d tonnellate\n", i, somma_merci_disponibili[i], conteggio_merce_consegnata[i]); /*fare poi un contatore per il consegnato*/
    }
      
    printf("Navi:\n");
    printf("\tCon un carico a bordo: %d - Senza un carico a bordo: %d - Nel porto: %d\n", conteggio_nave.conteggio_navi_con_carico, conteggio_nave.conteggio_navi_senza_carico, conteggio_nave.conteggio_navi_nel_porto);                                                                              
    printf("Porti:\n");
    for(i = 0; i < NO_PORTI; i++){
        printf("\tNumero porto: %d - Merce totale spedita e ricevuta in tonnellate: %d, %d - Numero di banchine libere: %d - Lotti rimanenti: %d\n", i+1, informazioni_porto[i].conteggio_merce_spedita_porto, informazioni_porto[i].conteggio_merce_ricevuta_porto, informazioni_porto[i].numero_banchine_libere, informazioni_porto[i].numero_lotti_merce);
    }
}

#endif