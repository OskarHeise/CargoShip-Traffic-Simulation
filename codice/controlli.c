#include "header.h"


main(){
    int indirizzo_attachment_shared_memory_porto;
    int indirizzo_attachment_shared_memory_scadenze;
    struct struct_porto *shared_memory_porto;
    struct struct_controllo_scadenze *shared_memory_scadenze;

    int i;


    indirizzo_attachment_shared_memory_scadenze = memoria_condivisa_get(SHM_KEY_CONTEGGIO, sizeof(struct struct_controllo_scadenze), SHM_W);
    shared_memory_scadenze = (struct struct_controllo_scadenze*)shmat(indirizzo_attachment_shared_memory_scadenze, NULL, 0);

    /*while(1){
        sleep(0.5);
        if(shared_memory_scadenze->numero_navi_senza_merce == SO_NAVI || shared_memory_scadenze->numero_porti_senza_merce == SO_NAVI){
            printf("termina!!\n");
            exit(0);
            /*termina il programma intero in qualche modo*/
        /*}
    }*/

    exit(0);
    return 0;
}