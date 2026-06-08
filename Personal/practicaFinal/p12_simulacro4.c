#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

#define NUM_HILOS 3
#define SUMA_OPERARIO 50

int produccion_local = 0;

void funcion_señal (int sig){

    puts("¡Señal del Supervisor recibida! Deteniendo maquinas...");
}

void* operario (void* arg){

    pthread_mutex_t* candado_hilo = arg;

    pthread_mutex_lock(candado_hilo);
    produccion_local += SUMA_OPERARIO;
    printf("Operario: He fabricado %d piezas.\n", SUMA_OPERARIO);
    pthread_mutex_unlock(candado_hilo);
    pthread_exit(0);
}

int main(void){

    int *inventario_general = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *inventario_general = 0;

    sem_t *semaforo;
    semaforo = sem_open("/sem_fabrica", O_CREAT, S_IRUSR | S_IWUSR, 1);

    pid_t hijo = fork();

    if(hijo < 0){

        perror("Error al crear al hijo");
        exit(EXIT_FAILURE);

    } else if (hijo == 0){

        signal(SIGUSR1, funcion_señal);

        pthread_mutex_t candado;
        pthread_mutex_init(&candado, NULL); 

        pthread_t hilos[NUM_HILOS];
        for (int i = 0; i < NUM_HILOS; i++){

            pthread_create(&hilos[i], NULL, operario, &candado);

        }
        for (int j = 0; j < NUM_HILOS; j++){
            
            pthread_join(hilos[j], NULL);

        }

        puts("Planta: Produccion local terminada. Esperando orden de volcado...");
        pause();

        sem_wait(semaforo);
        *inventario_general += produccion_local;
        sem_post(semaforo);

        pthread_mutex_destroy(&candado);

        exit(0);

    } else {

        sleep(3);

        kill(hijo, SIGUSR1);

        wait(NULL);

        sem_wait(semaforo);
        printf("Supervisor: El inventario general final es de %d piezas.\n", *inventario_general);
        sem_close(semaforo);
        sem_unlink("/sem_fabrica");
        munmap(inventario_general, sizeof(int));

    }

    return 0;
}