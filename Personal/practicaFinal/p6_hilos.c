#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_HILOS 3

int inventario = 100;

void* reponer_stock(void* arg){
    int repuesto = 50;

    inventario += repuesto;

    printf("Hilo ejecutandose: repuestos %d articulos. Inventario actual: %d\n", repuesto, inventario);

    pthread_exit(0);

}

int main(void){

    pthread_t hilo[NUM_HILOS];

    for (int i = 0; i < NUM_HILOS; i++) {
    
    int resultado = pthread_create(&hilo[i], NULL, reponer_stock, NULL);
    
    if (resultado != 0) {
        perror("Error al crear el hilo");
        return 1;
        }
    }

    for (int j = 0; j < NUM_HILOS; j++){

        pthread_join(hilo[j], NULL);

    }
    
    return 0;
}