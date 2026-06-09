#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define NUM_HILOS 3
#define NUM_GEN 10

int inventario[NUM_GEN * 2];
int indice = 0;
volatile sig_atomic_t flag_nuevo = 0;
volatile sig_atomic_t contador_fin_prod = 0;

pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;

pthread_t tid_consumidor;

void fun_signal1 (int sig) {

    flag_nuevo = 1;

}

void fun_signal2 (int sig){

    contador_fin_prod++;

}

void* consumidor (void* arg){

    signal(SIGUSR1, fun_signal1);
    signal(SIGUSR2, fun_signal2);

    int suma_total = 0;
    int indice_procesado = 0;

    puts("Consumidor: iniciando... esperando datos...");

    while(contador_fin_prod < 2){

        flag_nuevo = 0;
        pause();
        if(flag_nuevo == 1){
            
            pthread_mutex_lock(&candado);

            while(indice_procesado < indice){

                int valor = inventario[indice_procesado];
                suma_total += valor;
                printf("Consumidor: Leido el valor: %d | Suma actual: %d\n", valor, suma_total);
                indice_procesado++;

            }

            pthread_mutex_unlock(&candado);

        }

    }

    puts("Consumidor: Los dos productores han terminado");
    printf("Consumidor: Finalizando... Suma total = %d. \n", suma_total);

    return NULL;

}

void* productor_pares (void* arg){

    for (int i = 0; i < NUM_GEN; i++){

        pthread_mutex_lock(&candado);
        int num_par = i*2;
        inventario[indice] = num_par;
        indice++;
        pthread_mutex_unlock(&candado);

        pthread_kill(tid_consumidor, SIGUSR1);
        usleep(100000);
        
    }

    pthread_kill(tid_consumidor, SIGUSR2);

    return NULL;
}

void* productor_impares (void* arg){

    for (int i = 0; i < NUM_GEN; i++){

        pthread_mutex_lock(&candado);
        int num_par = (i*2)+1;
        inventario[indice] = num_par;
        indice++;
        pthread_mutex_unlock(&candado);

        pthread_kill(tid_consumidor, SIGUSR1);
        usleep(100000);
        
    }

    pthread_kill(tid_consumidor, SIGUSR2);

    return NULL;

}

int main(void){

    pthread_t pares, impares;
    puts("MAIN: Iniciando sistema...");
    pthread_create(&tid_consumidor, NULL, consumidor, NULL);
    usleep(500000);
    pthread_create(&pares, NULL, productor_pares, NULL);
    pthread_create(&impares, NULL, productor_impares, NULL);

    pthread_join(tid_consumidor, NULL);
    pthread_join(pares, NULL);
    pthread_join(impares, NULL);

    pthread_mutex_destroy(&candado);
    
    puts("MAIN: Programa finalizado... Todos los recursos liberados");

    return 0;
}