#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

int saldo = 100;

pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;
pthread_t tid_app;

volatile sig_atomic_t actualizacion = 0;

void fun_signal (int sig){

    actualizacion = 1;

}

void* app_bancaria (void* arg){

    signal(SIGUSR1, fun_signal);

    int local_var = 0;
    
    while (local_var < 200){
        actualizacion = 0;

        pause();

        if (actualizacion == 1){

            pthread_mutex_lock(&candado);
            local_var = saldo;
            pthread_mutex_unlock(&candado);
            printf("[App] Notificacion: Nuevo ingreso. Saldo actual: %d€\n", local_var);

        }

    }

    return NULL;

}

void* cajero (void* arg){

    for (int i = 0; i < 5; i++){

        pthread_mutex_lock(&candado);
        saldo += 20;
        pthread_mutex_unlock(&candado);
        puts("[Cajero] Ingresando 20€...");
        pthread_kill(tid_app, SIGUSR1);
        sleep(1);

    }

}

int main(void){

    pthread_t tid_cajero;

    pthread_create(&tid_app, NULL, app_bancaria, NULL);

    usleep(50000);

    pthread_create(&tid_cajero, NULL, cajero, NULL);

    pthread_join(tid_app, NULL);
    pthread_join(tid_cajero, NULL);

    pthread_mutex_destroy(&candado);

    printf("[Main] Saldo total de la cuenta: %d\n", saldo);

    return 0;
}