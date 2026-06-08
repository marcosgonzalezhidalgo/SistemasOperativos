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

void funcion_signal(int sig){

    puts("Corredor 2: ¡Testigo recibido! Empezando mi sprint...");

}   

int main(void){

    int *pid_compartido = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *pid_compartido = 0;

    pid_t corredor2 = fork();

    if (corredor2 < 0){

        perror("Error al crear el hijo correrdor 2");
        exit(EXIT_FAILURE);

    }else if (corredor2 == 0){

        *pid_compartido = getpid();

        signal(SIGUSR1, funcion_signal);

        puts("Corredor 2: En posicion, esperando el testigo...");

        pause();

        execl("/bin/ls", "ls", NULL);

        exit(1);

    }else {

        sleep(1);

        pid_t corredor1 = fork();

        if (corredor1 < 0){

            perror("Error al crear el hijo corredor 1");
            exit(EXIT_FAILURE);

        } else if (corredor1 == 0) {

            puts("Corredor 1: ¡Arranco la carrera!");
            
            sleep(2);

            printf("Corredor 1: Llegando a la zona del relevo... Pasando testigo al PID %d\n", *pid_compartido);

            kill(*pid_compartido, SIGUSR1);

            exit(0);

        } else {

            wait(NULL);
            wait(NULL);

            puts("Arbitro: La carrera ha finalizado con exito");

            munmap(pid_compartido, sizeof(int));

        }
    }

    return 0;
}