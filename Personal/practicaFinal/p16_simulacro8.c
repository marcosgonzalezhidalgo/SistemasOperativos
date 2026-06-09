#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define LONG_DATOS 10

int main(void) {
    
    int datos[LONG_DATOS] = {12, -5, 4, -1, 8, 9, -3, 7 ,2, -8};
    int *resultados = mmap(NULL, 2*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    resultados[0] = 0;
    resultados[1] = 0;

    int status;
    
    pid_t hijo1 = fork();

    if (hijo1 < 0){

        perror("Error al crear el primer hijo");
        exit(EXIT_FAILURE);

    } else if (hijo1 == 0){

        for (int i = 0; i < (LONG_DATOS/2); i++){

            if (datos[i] < 0){

                resultados[0]++;

            }

        }

        exit(1);

    }

    pid_t hijo2 = fork();

    if (hijo2 < 0){

        perror("Error al crear el primer hijo");
        exit(EXIT_FAILURE);

    } else if (hijo2 == 0){

        for (int j = 5; j < LONG_DATOS; j++){

            if (datos[j] < 0){

                resultados[1]++;

            }

        }

        exit(2);

    }

    for (int k = 0; k < 2; k++){

        wait(&status);

        if(WIFEXITED(status)){

            int codigo = WEXITSTATUS(status);
            if (codigo == 1) {

            printf("El hijo 1 ha terminado con codigo de salida: %d\n", codigo);

            } else if (codigo == 2) {
            
            printf("El hijo 2 ha terminado con codigo de salida: %d\n", codigo);

            } else {
                puts("Ha habido un error");
            }
        }

    }

    int suma_total = resultados[0] + resultados[1];

    printf("Total de numeros negativos en el array: %d\n", suma_total);

    munmap(resultados, 2*sizeof(int));

    return 0;
}