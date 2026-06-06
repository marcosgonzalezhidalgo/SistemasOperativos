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

void funcion_hijo(int sig){

    puts("Cliente: ¡El cajero esta listo!");

}

int main(void){

    int *saldo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *saldo = 500;

    int status;

    sem_t *semaforo;
    semaforo = sem_open("/sem_cajero", O_CREAT, S_IRUSR | S_IWUSR, 1);

    pid_t hijo = fork();

    if(hijo < 0){

        perror("Se producido un error al generar el hijo");
        exit(EXIT_FAILURE);

    } else if (hijo == 0){

        signal(SIGUSR1, funcion_hijo);

        puts("Cliente: esperando a que el cajero se encienda...");


        pause();

        sem_wait(semaforo);

        *saldo -= 200;
        printf("Cliente: He sacado 200. Saldo restante: %d\n", *saldo);

        sem_post(semaforo);

        exit(0);

    } else {

        sleep(2);

        kill(hijo, SIGUSR1);

        wait(&status);

        if(WIFEXITED(status)){

            int codigo = WEXITSTATUS(status);
            printf("Banco: Operacion del cliente finalizada con exito con codigo - %d. Saldo final en la caja: %d\n", codigo, *saldo);

        }

    }

    sem_close(semaforo);
    sem_unlink("/sem_cajero");
    munmap(saldo, sizeof(int));

    return 0; 
}