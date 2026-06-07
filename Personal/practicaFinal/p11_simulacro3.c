#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

void funcion_señal(int sig){

    puts("Padre: ¡Alarma recibida del hijo! Abortando programa...");

    exit(1);
    
}

int main(){

    signal(SIGUSR1, funcion_señal);

    int tuberia_padre[2], tuberia_hijo[2];
    char entrada[100];
    pipe(tuberia_padre);
    pipe(tuberia_hijo);

    pid_t hijo = fork();

    if(hijo < 0){

        perror("Error al crear al hijo");
        exit(EXIT_FAILURE);

    } else if (hijo == 0){

        close(tuberia_padre[1]);
        close(tuberia_hijo[0]);
        
        char entrada[100];
        read(tuberia_padre[0], &entrada, sizeof(entrada));
        int contador = 0;
        close(tuberia_padre[0]);

        if(entrada[0] == 'z' || entrada[0] == 'Z'){

            pid_t id_padre = getppid();

            kill(id_padre, SIGUSR1);

            exit(1);

        } else {

            for (int i = 0; i < strlen(entrada); i++){

                char letra_min = tolower(entrada[i]);

                if(letra_min == 'a' || letra_min == 'e' || letra_min == 'i' || letra_min == 'o' || letra_min == 'u'){

                    contador++;

                }

                

            }

            if (contador == 0){

                puts("No hay vocales en la palabra...");

            }

        }

        write(tuberia_hijo[1], &contador, sizeof(contador));
        close(tuberia_hijo[1]);

        exit(0);

    }else {

        close(tuberia_padre[0]);
        close(tuberia_hijo[1]);

        puts("Escriba una palabra por favor...");
        scanf("%s", entrada);

        write(tuberia_padre[1], &entrada, sizeof(entrada));

        close(tuberia_padre[1]);

        int salida = 0;
        read(tuberia_hijo[0], &salida, sizeof(salida));

        printf("Padre: El hijo me dice que la palabra tiene %d vocales.\n", salida);

        close(tuberia_hijo[0]);

        wait(NULL);
    }

    return 0;
}