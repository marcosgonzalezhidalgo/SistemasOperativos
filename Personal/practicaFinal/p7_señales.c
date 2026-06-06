#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

void manejador_hijo(int sig){

    puts("Soy el hijo, me han despertado con una señal");

}

int main(void) {

    pid_t hijo = fork();

    if (hijo < 0){

        perror("El hijo ha generado un error");
        exit(EXIT_FAILURE);
    } else if (hijo == 0) {

        signal(SIGUSR1, manejador_hijo);

        puts("Hijo: me voy a dormir...");

        pause();

        puts("Hijo: Terminando ejecución.");
        
        exit(0);

    } else {

        sleep(2);

        puts("Padre: pasaron 2 segundos, enviando señal al hijo");

        kill(hijo, SIGUSR1);

        wait(NULL);

    }

    return 0;
}