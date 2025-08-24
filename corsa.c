#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#define max_total 100 
#define minimo 1
#define massimo 10
#define giocatori_max 10

sem_t semafori;
int cavallo_vincitore=0;

typedef struct Cavallo{
    int valore_partenza;
    int numero_cavallo;
    int classifica;
    int cavallo_differenza;
}Cavallo;

void ordineArrivo_cavalli(Cavallo* cavallo,int primo_classificato,int numero_cavalli){
    int posizione=1;
    for(int i=0;i<numero_cavalli;i++){
        if(i!=primo_classificato){
            int dif=cavallo[primo_classificato].valore_partenza-cavallo[i].valore_partenza;
            cavallo[i].cavallo_differenza=dif;
        }
    }
    while(posizione<numero_cavalli){
        int valore_min=110;
        int pos=0;
            for(int j=0;j<numero_cavalli;j++){
                if(cavallo[j].cavallo_differenza<valore_min & j!=primo_classificato && cavallo[j].classifica==0){
                    valore_min=cavallo[j].cavallo_differenza;
                    pos=j;
                }
            }
            posizione++;
            cavallo[pos].classifica=posizione;
            printf("Cavallo[%d] è arrivato %d° con %d\n",cavallo[pos].numero_cavallo,cavallo[pos].classifica,cavallo[pos].valore_partenza);
    }


}

int valore_corsa(int min,int max){
    int val_corsa = rand()%(max-min + 1) + min;
    return val_corsa;
}


void* corsa(void *inizio){
    Cavallo* bianco=(Cavallo*)inizio;
    while(!cavallo_vincitore){
        bianco->valore_partenza+=valore_corsa(minimo,massimo);
        printf("cavallo[%d]-corsa:[%d]\n", bianco->numero_cavallo, bianco->valore_partenza);
        int ret=sem_wait(&semafori);
        if(ret<0) printf("errore nella sem_wait");
        if(bianco->valore_partenza>=max_total) {
            cavallo_vincitore=1; 
            printf("il cavallo[%d] ha vinto\n", bianco->numero_cavallo);
            ret=sem_post(&semafori);
            if (ret<0) printf("errore nella post");
            break;
        }
         if(cavallo_vincitore){
            sem_post(&semafori);
            break;
         }
        ret=sem_post(&semafori);
        if (ret<0) printf("errore nella post");
        usleep(rand()%(50000));
    }
    pthread_exit(NULL);
    }


int main(){
    srand(time(NULL));
    int num_cavalli;
    printf("Quanti siete a giocare:\t");
    scanf("%d",&num_cavalli);
    int ret;
    ret=sem_init(&semafori,0,1);  
    if(ret) printf("errore nella sem_init");
    Cavallo* cavallo=calloc(num_cavalli,sizeof(Cavallo));
    for(int i=0;i<num_cavalli;i++){
        cavallo[i].valore_partenza=0;
        cavallo[i].numero_cavallo=i;
        cavallo[i].classifica=0;
        cavallo[i].cavallo_differenza=110;
    }
    pthread_t thread[num_cavalli];
    int num=0;
    while(num<num_cavalli){
        ret=pthread_create(&thread[num],NULL,corsa,cavallo+num);
        if(ret) printf("errore nella create");
        num++;
    }
    num=0;
    while(num<num_cavalli){
        ret=pthread_join(thread[num],NULL);
        if(ret) printf("errore nella join");
        num++;
    }
    int cavallo_vincente;
    for(int i=0;i<num_cavalli;i++){
        if(cavallo[i].valore_partenza>=100) {
            printf("Cavallo[%d] è arrivato 1° con %d\n",cavallo[i].numero_cavallo,cavallo[i].valore_partenza);
            cavallo[i].classifica=1;
            cavallo_vincente=i;
        }
    }
    ordineArrivo_cavalli(cavallo,cavallo_vincente,num_cavalli);
    ret=sem_destroy(&semafori);
    free(cavallo);
    return 0;
}
