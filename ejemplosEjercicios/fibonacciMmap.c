// Comuicacion entre procesos con memoria compartida anonima (mmap). 
// El hijo calcula los 10 primeros numeros de Fibunacci y los escribe en la memoria compartida. 
// El padre los lee tras esperar al hijo y los saca por pantalla.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define N 10

int main() {
    int *shared_mem = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        shared_mem[0] = 0;
        shared_mem[1] = 1;

        for (int i = 2; i < N; i++) {
            shared_mem[i] = shared_mem[i-1] + shared_mem[i-2];
        }

        printf("Hijo: números de Fibonacci calculados.\n");

        munmap(shared_mem, N * sizeof(int));
        exit(0);
    } else {
        wait(NULL);

        printf("Padre: leyendo los números de Fibonacci:\n");
        for (int i = 0; i < N; i++) {
            printf("%d ", shared_mem[i]);
        }
        printf("\n");

        munmap(shared_mem, N * sizeof(int));
    }

    return 0;
}