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

int main(void){

    int *cuenta = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *cuenta = 100;

    sem_t *semaforo;
    semaforo = sem_open("/semaforo_examen", O_CREAT, S_IRUSR | S_IWUSR, 1);

    pid_t hijo = fork();

    if(hijo < 0) {

        perror("Error al crear el hijo");
        exit(EXIT_FAILURE);
    
    } else if (hijo == 0){

        sem_wait(semaforo);

        *cuenta += 50;

        printf("Hijo sumo 50, cuenta: %d\n", *cuenta);
        
        sem_post(semaforo);

        exit(EXIT_SUCCESS);

    } else {

        sem_wait(semaforo);

        *cuenta -= 30;

        printf("Padre resto 30, cuenta: %d\n", *cuenta);

        sem_post(semaforo);

        wait(NULL);

        sem_close(semaforo);
        sem_unlink("/semaforo_examen");

        munmap(cuenta, sizeof(int));

    }

    return 0;
}