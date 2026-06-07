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

#define NUM_ENTRADA 2

pthread_mutex_t candado;

int suma_total = 0;

void* funcion_hilo(void* arg){

    pthread_mutex_lock(&candado);

    int numero = *(int*)arg;
    numero *= 10;
    suma_total += numero;
    printf("Hilo ejecutado: he sumado %d\n", numero);

    pthread_mutex_unlock(&candado);

    return NULL;
}

int main(){

    int tuberia[2];
    pipe(tuberia);

    int status, entrada[2];

    pid_t hijo = fork();

    if (hijo < 0){

        perror("Error al crear el hijo");
        exit(EXIT_FAILURE);

    } else if (hijo == 0){

        close(tuberia[1]);

        pthread_mutex_init(&candado, NULL);

        int numeros[2];

        read(tuberia[0], &numeros, sizeof(numeros));

        close(tuberia[0]);

        pthread_t hilos[NUM_ENTRADA];

        for (int i = 0; i < NUM_ENTRADA; i++){

        pthread_create(&hilos[i], NULL, funcion_hilo, &numeros[i]);

        }

        for (int j = 0; j < NUM_ENTRADA; j++){

            pthread_join(hilos[j], NULL);

        }

        printf("Hijo: Esta es la suma total:%d\n", suma_total);

        pthread_mutex_destroy(&candado);

        exit(0);
    } else {

        close(tuberia[0]);

        printf("A continuacion va a escribir %d numeros...\n", NUM_ENTRADA);
        for(int k = 0; k < NUM_ENTRADA; k++){
            
            printf("%dº. Escriba un numero entero...\n", (k+1));
            scanf("%d", &entrada[k]);
        
        }

        write(tuberia[1], &entrada, sizeof(entrada));

        close(tuberia[1]);

        wait(&status);

        if(WIFEXITED(status)){
            int codigo = WEXITSTATUS(status);

            printf("Padre: El procesamiento ha finalizado con exito con codigo %d\n", codigo);

        }

    }

    return 0;
}