#ifndef _HEADER_H_
#define _HEADER_H_

#define _GNU_SOURCE

/*lista delle librerie utilizzate*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<signal.h>
#include<errno.h>
#include<pthread.h>

/*lista delle librerie utilizzate*/
#define NO_NAVI 10 /*numero di navi*/
#define NO_PORTI 4 /*numero di porti*/
#define NO_MERCI 6 /*numero di tipologie di merci*/

#define SO_DAYS 365 /*durata totale in giorni dell'esperimento*/
#define MIN_VITA 4 /*minima vita della merce*/
#define MAX_VITA 45 /*massima vita merce*/
#define SO_SIZE 40000 /*peso massimo della merce di 40.000 Kg*/
#define TEMPO_SIMULATO 100 /*tempo che trascorre nella simulazione, in questo caso sono giorni*/
#define TEMPO_REALE 100 /*tempo di durata dell'esecuzione della simulazione, in questo caso sono secondi*/

#define SHM_KEY 45466 /*DA CAMBIARE ASSOLUTAMENTE*/
#define MSQ_KEY 589621 /*IDEMMMM*/

#define SO_LATO 1000 /*grandezza per lato della mappa di 10.000 Km*/
#define SO_BANCHINE 10

#define SO_CAPACITY 40000000 /*massima capacità della nave di 40.000 T*/
#define SO_SPEED 1000 /*la velocità è di mille Kh/giorno*/

int shared_memory_id;
int msq_id;
int indice; 

struct struct_merce{
    int id_merce; /*genero la tipologia di merce con un ID numerico*/
    int dimensione_merce; /*genero la grandezza delle merci*/
    int tempo_vita_merce; /*genero la durata di vita delle merci*/
};

struct memoria_condivisa{
    struct struct_merce merce[NO_MERCI];
    int contatore_processi;
};
struct memoria_condivisa * sh_merce;



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
    numero_randomico = rand()%10;
    return numero_randomico;
}

/*genera casualmente dimensione esatta delle merci*/
int generatore_dimensione_merce(){
    int numero_randomico;
    numero_randomico = rand()%(SO_SIZE+-1 + 1) + 1;
    return numero_randomico;
}

/*genera casualmente il tempo di vita delle merci*/
int generatore_tempo_vita_merce(){
    int numero_randomico;
    numero_randomico = (rand()%(MAX_VITA - MIN_VITA +1)) + MIN_VITA;
    return numero_randomico;
}

/*genera casualmente un array di merci e lo restituisce*/
struct struct_merce* generatore_array_merci(){
    int i;
    int j;
    struct struct_merce* vettore_di_merci;
    vettore_di_merci = (struct struct_merce*)malloc(sizeof(struct struct_merce)*NO_MERCI);

    /*generazione delle merci e inserimento nell'array*/
    for(i = 0; i < NO_MERCI; i++){
        vettore_di_merci[i].id_merce = generatore_id_merce();
        vettore_di_merci[i].dimensione_merce = generatore_dimensione_merce();
        vettore_di_merci[i].tempo_vita_merce = generatore_tempo_vita_merce();
        for(j = 0; j < NO_MERCI; j++){
            if(vettore_di_merci[i].id_merce == vettore_di_merci[j].id_merce){
                vettore_di_merci[i].tempo_vita_merce = vettore_di_merci[j].tempo_vita_merce;
            }
        }
    }

    return vettore_di_merci;
}

/*Gestione della Memoria Condivisa*/
int memoria_condivisa_creazione(key_t key, size_t grandezza_memoria){
    int id;
    id = shmget(key, grandezza_memoria, 0666 | IPC_CREAT);
    if(id == -1){
        printf("Errore nella creazione della memoria condivisa! Error number = %d\n", errno);
        shmctl(id, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    return id;
}

int memoria_condivisa_id(key_t key, size_t grandezza_memoria){
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

/*restituisce lo spostamento della nave giorno per giorno*/
double spostamento_nave(int velocita_nave, double *posizione_nave, double *posizione_porto){
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
    elemento_x2 = *posizione_porto;
    posizione_porto++;
    elemento_y2 = *posizione_porto;

    numeratore_senza_radice = potenza((elemento_x2 - elemento_x1), 2) + potenza((elemento_y2 - elemento_y1), 2);
    numeratore_con_radice = radice_quadrata(numeratore_senza_radice);  
    risultato = (double)numeratore_con_radice/(double)velocita_nave;
    return risultato;
}

double *generatore_posizione_iniziale_porto(){
    double *coordinate_generate;
    coordinate_generate = malloc(16);
    coordinate_generate[0] = rand()%(SO_LATO - 0 + 1) + 0;
    coordinate_generate[1] = rand()%(SO_LATO - 0 + 1) + 0;
    return coordinate_generate;
}

double generatore_banchine_porto(){
    int numero_randomico;
    numero_randomico = rand()%SO_BANCHINE;
    return numero_randomico;
}

#endif