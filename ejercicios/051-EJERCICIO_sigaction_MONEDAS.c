#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
// Variable global para contar las monedas (señales SIGUSR1 recibidas)
// De nuevo, usamos volatile sig_atomic_t por seguridad en la concurrencia.
volatile sig_atomic_t monedas_insertadas = 0;

// 1. Manejador de la señal SIGUSR1
void manejador_monedas(int sig) {
    // Solo actualizamos el estado. Cero printf() aquí dentro.
    monedas_insertadas++;
}

int main() {
    struct sigaction sa;

    // 2. Configuramos la estructura sigaction
    sa.sa_handler = manejador_monedas;
    sigemptyset(&sa.sa_mask);

    // Usamos SA_RESTART para evitar que la señal interrumpa abruptamente
    // funciones bloqueantes del sistema
    sa.sa_flags = SA_RESTART;

    // 3. Registramos el manejador para la señal SIGUSR1
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error al registrar sigaction para SIGUSR1");
        exit(EXIT_FAILURE);
    }

    int mi_pid = getpid();
    printf(" ¡Máquina de Café v1.0 Iniciada!\n");
    printf("Mi PID es: %d\n", mi_pid);
    printf("Abre otra terminal y ejecuta: kill -SIGUSR1 %d\n\n", mi_pid);

    int monedas_previas = 0;

    // 4. Bucle principal de la máquina
    while (monedas_insertadas < 3) {

        // Comprobamos si el estado ha cambiado de forma asíncrona
        if (monedas_insertadas > monedas_previas) {
            printf("\n--> ¡Clinc! Moneda aceptada. (Llevas %d de 3)\n", monedas_insertadas);
            monedas_previas = monedas_insertadas;
        } else {
            // Si no hay novedades, la máquina sigue a lo suyo
            printf("Máquina en espera... Inserta monedas. (PID: %d)\n", mi_pid);
            sleep(3);
        }
    }

    // 5. Condición de salida alcanzada
    printf("\n ¡Bzzzzzz! Preparando tu café...\n");
    printf("¡Aquí tienes! Gracias por tu compra. Apagando máquina...\n");

    return 0;
}
