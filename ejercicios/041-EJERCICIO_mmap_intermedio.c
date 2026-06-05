/*
 * Suma paralela de un array usando memoria compartida (mmap + fork)
 *  * Implementa un programa en C que calcule, de forma colaborativa entre un proceso padre
 * y un proceso hijo, la suma de todos los elementos de un array de enteros. El array tendrá N elementos
 * (con N par). Cada proceso calculará la suma de su mitad del array y escribirá
 * el resultado en una zona de memoria compartida. Al final, el proceso padre leerá
 * ambos resultados y mostrará la suma total.
 *
 * 1 Reservar la zona de memoria compartida antes de llamar a fork
 *
 * 2 La zona compartida debe contener un array de dos enteros: sumas[0] para el padre y sumas[1] para el hijo.
 *
 * 3 El proceso hijo calculará la suma de la mitad superior del array y la guardará en sumas[1].
 * Después terminará con exit(0).
 *
 * 4 El proceso padre calculará la suma de la mitad inferior, la guardará en sumas[0] y
 * esperará al hijo con wait() antes de leer sumas[1].
 * 5 Liberar la memoria con munmap() al terminar.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>   /* mmap, munmap */
#include <sys/wait.h>   /* wait         */
#include <unistd.h>     /* fork, getpid */
#define N 10  /* Número de elementos (debe ser par) */

int main(void)
{
    /* --- Array de datos a sumar --- */
    int datos[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int *sumas = mmap(NULL,2 * sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);

    if (sumas == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sumas[0] = 0;  /* reservado para el padre */
    sumas[1] = 0;  /* reservado para el hijo  */

    /* ------------------------------------------------------------------ *
     * 2. Crear el proceso hijo                                            *
     * ------------------------------------------------------------------ */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        munmap(sumas, 2 * sizeof(int));
        exit(EXIT_FAILURE);
    }

    /* ------------------------------------------------------------------ *
     * 3a. Código del HIJO — suma la mitad superior [N/2 … N-1]           *
     * ------------------------------------------------------------------ */
    if (pid == 0) {
        int suma = 0;
        for (int i = N / 2; i < N; i++)
            suma += datos[i];

        sumas[1] = suma;  /* escribe en la zona compartida */

        printf("[hijo,  PID=%d] Mi suma parcial (mitad superior): %d\n",
               getpid(), sumas[1]);

        /* munmap opcional en el hijo: el SO lo libera al hacer exit() */
        munmap(sumas, 2 * sizeof(int));
        exit(EXIT_SUCCESS);
    }

    /* ------------------------------------------------------------------ *
     * 3b. Código del PADRE — suma la mitad inferior [0 … N/2-1]          *
     * ------------------------------------------------------------------ */
    int suma = 0;
    for (int i = 0; i < N / 2; i++)
        suma += datos[i];

    sumas[0] = suma;

    printf("[padre, PID=%d] Mi suma parcial (mitad inferior): %d\n",
           getpid(), sumas[0]);

    /* ------------------------------------------------------------------ *
     * 4. Esperar a que el hijo termine antes de leer sumas[1]            *
     *    (sin wait el hijo podría no haber escrito aún)                  *
     * ------------------------------------------------------------------ */
    wait(NULL);

    /* ------------------------------------------------------------------ *
     * 5. Combinar resultados y verificar                                  *
     * ------------------------------------------------------------------ */
    int total = sumas[0] + sumas[1];
    printf("[padre, PID=%d] Suma total = %d + %d = %d\n",
           getpid(), sumas[0], sumas[1], total);

    /* Verificación secuencial */
    int verificacion = 0;
    for (int i = 0; i < N; i++)
        verificacion += datos[i];

    if (total == verificacion)
        printf("[padre, PID=%d] Verificación secuencial = %d  ✓\n",
               getpid(), verificacion);
    else
        printf("[padre, PID=%d] ERROR: esperado %d, obtenido %d  ✗\n",
               getpid(), verificacion, total);


     // 6. Liberar la memoria compartida

    munmap(sumas, 2 * sizeof(int));

    return EXIT_SUCCESS;
}
