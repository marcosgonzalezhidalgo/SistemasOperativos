#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define NUM_ARRAY 6

int matriz[NUM_ARRAY] = {0,0,0,0,0,0};
int elementos_llenos = 0; 
pthread_mutex_t candado;
pthread_t tid_coordinador;
volatile sig_atomic_t aviso = 0;

void fun_signal (int sig){

    aviso = 1;

}

void* coordinador (void* arg){

    signal(SIGUSR1, fun_signal);

    int elementos_locales = 0;

    while (elementos_locales < NUM_ARRAY){
        aviso = 0;
        pause();

        if (aviso == 1) {
            pthread_mutex_lock(&candado);
            elementos_locales = elementos_llenos;
            pthread_mutex_unlock(&candado);

            printf("[Coordinador] Se ha llenado un hueco nuevo. Llevamos %d/6\n", elementos_locales);
        }
    }

    puts("[Coordinador] Se han llenado todos los huecos. Contenido:");
    for (int l = 0; l < NUM_ARRAY; l++){

        printf("%d ", matriz[l]);

    }
    printf("\n");

    return NULL;
}

void* relleno_pares (void* arg) {

    for (int j = 0; j < NUM_ARRAY; j++){

        if ((j % 2) == 0){
            
            pthread_mutex_lock(&candado);
            matriz[j] = 2;
            elementos_llenos++;
            pthread_mutex_unlock(&candado);
            printf("[Relleno pares] añadido un 2 a la posicion %d\n", j);
            pthread_kill(tid_coordinador,SIGUSR1);
            usleep(200000);

        }

    }

    return NULL;

}

void* relleno_impares (void* arg) {

    for (int k = 0; k < NUM_ARRAY; k++){

        if ((k % 2) != 0){
            
            pthread_mutex_lock(&candado);
            matriz[k] = 3;
            elementos_llenos++;
            pthread_mutex_unlock(&candado);
            printf("[Relleno impares] añadido un 3 a la posicion %d\n", k);
            pthread_kill(tid_coordinador,SIGUSR1);
            usleep(200000);

        }

    }

    return NULL;

}


int main(void){

    pthread_mutex_init(&candado, NULL);

    pthread_create(&tid_coordinador, NULL, coordinador, NULL);

    usleep(50000);

    pthread_t tid_pares, tid_impares;
    pthread_create(&tid_pares, NULL, relleno_pares, NULL);
    pthread_create(&tid_impares, NULL, relleno_impares, NULL);
    
    pthread_join(tid_coordinador, NULL);
    pthread_join(tid_pares, NULL);
    pthread_join(tid_impares, NULL);

    pthread_mutex_destroy(&candado);

    return 0;
}