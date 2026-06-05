/*

Implementar en C (Linux, pthreads) un programa con dos hilos que cooperan mediante señales y un mutex.
Hilo Productor (productor):

Genera N números enteros aleatorios (entre 1 y 100), uno por iteración.
Antes de escribir cada número, adquiere el mutex para escribir en la variable compartida dato.
Una vez escrito el dato y liberado el mutex, notifica al consumidor enviándole SIGUSR1 con pthread_kill.
Tras enviar todos los datos, envía SIGUSR2 al consumidor para indicarle que ha terminado.

Hilo Consumidor (consumidor):

regisrtar manejadores para SIGUSR1 y SIGUSR2.
En cada iteración llama a pause() para quedarse bloqueado hasta recibir una señal.
Al recibir SIGUSR1, adquiere el mutex, lee dato, lo acumula en una suma y lo imprime.
Al recibir SIGUSR2, imprime la suma total y termina.

Requisitos concretos:

Usar pthread_mutex_lock / pthread_mutex_unlock para proteger dato.
Usar pause() en el consumidor como mecanismo de espera.
El número de iteraciones configurable mediante #define.

 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define N            5
#define RETARDO_US   50000   /* 50 ms: tiempo que espera el productor * para que el consumidor instale signal() */

static int dato = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static pthread_t tid_consumidor;

static volatile int hay_dato = 0;
static volatile int fin      = 0;



static void manejador_sigusr1(int sig) {
    (void)sig;
    hay_dato = 1;
}

static void manejador_sigusr2(int sig) {
    (void)sig;
    fin = 1;
}

/* ─── Hilo consumidor ───────────────────────────────────────────────── */
static void *consumidor(void *arg) {
    (void)arg;

    /* Instalar manejadores en el hilo consumidor */
    signal(SIGUSR1, manejador_sigusr1);
    signal(SIGUSR2, manejador_sigusr2);

    printf("[Consumidor] Listo. Esperando datos...\n");

    int suma = 0;

    while (!fin) {
        hay_dato = 0;
        pause();              /* duerme hasta recibir SIGUSR1 o SIGUSR2 */

        if (hay_dato) {
            pthread_mutex_lock(&mtx);
            int valor = dato;
            pthread_mutex_unlock(&mtx);

            suma += valor;
            printf("[Consumidor] Recibido: %3d  |  Suma parcial: %d\n",
                   valor, suma);
        }
    }

    printf("[Consumidor] Fin. SUMA TOTAL = %d\n", suma);
    return NULL;
}

/* ─── Hilo productor ────────────────────────────────────────────────── */
static void *productor(void *arg) {
    (void)arg;

    srand((unsigned)time(NULL));

    /*
     * Pequeña espera para que el consumidor ejecute signal() antes
     * de que llegue el primer SIGUSR1.
     * Con sigaction esto no haría falta (se instalaría en main antes
     * de crear los hilos), pero signal() solo puede instalarse desde
     * el propio hilo que va a recibirla, así que necesitamos este
     * margen de tiempo.
     */
    usleep(RETARDO_US);

    for (int i = 0; i < N; i++) {
        int valor = (rand() % 100) + 1;

        pthread_mutex_lock(&mtx);
        dato = valor;
        pthread_mutex_unlock(&mtx);

        printf("[Productor]  Enviado:  %3d\n", valor);

        pthread_kill(tid_consumidor, SIGUSR1);

        sleep(1);
    }

    pthread_kill(tid_consumidor, SIGUSR2);
    printf("[Productor]  Fin de producción.\n");
    return NULL;
}

/* ─── main ──────────────────────────────────────────────────────────── */
int main(void) {
    pthread_t tid_productor;

    pthread_create(&tid_consumidor, NULL, consumidor, NULL);
    pthread_create(&tid_productor,  NULL, productor,  NULL);

    pthread_join(tid_productor,  NULL);
    pthread_join(tid_consumidor, NULL);

    pthread_mutex_destroy(&mtx);
    printf("[main] Programa terminado correctamente.\n");
    return 0;
}
