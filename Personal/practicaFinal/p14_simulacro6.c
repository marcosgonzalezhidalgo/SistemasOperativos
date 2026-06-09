#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define NUM_GEN 10

// Correcto: el uso de volatile sig_atomic_t
volatile sig_atomic_t contador[2] = {0, 0};

void manejador(int sig){
    // Correcto: modificar solo variables aquí, nada de printf
    if (sig == SIGUSR1) {
        contador[0]++;
    } else if (sig == SIGUSR2) {
        contador[1]++;
    }
}

int main(void){
    // 1. Crear memoria compartida
    int *controlador = mmap(NULL, 2*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (controlador == MAP_FAILED) {
        perror("Error mmap");
        exit(EXIT_FAILURE);
    }
    controlador[0] = 0;
    controlador[1] = 0;

    // 2. Crear tubería
    int tuberia[2];
    if(pipe(tuberia) == -1) {
        perror("Error pipe");
        exit(EXIT_FAILURE);
    }

    // 3. Configurar señales ANTES de crear a los hijos
    struct sigaction sa;
    sa.sa_handler = manejador;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    if(sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error SIGUSR1"); exit(EXIT_FAILURE);
    }
    if(sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Error SIGUSR2"); exit(EXIT_FAILURE);
    }

    // 4. Creación ESTRICTA de hijos
    pid_t sensor = fork();

    if (sensor < 0) {
        perror("Error fork sensor");
        exit(EXIT_FAILURE);
    } else if (sensor == 0) {
        // ---- CÓDIGO DEL SENSOR ----
        close(tuberia[0]); // Cerramos lectura

        int num;
        for (int i = 1; i <= NUM_GEN; i++){
            num = i;
            write(tuberia[1], &num, sizeof(int)); // Escribimos SECUENCIALMENTE
            usleep(10000); // Pequeña pausa para simular flujo
        }

        close(tuberia[1]); // Importante cerrar para que el read del procesador devuelva 0
        
        kill(getppid(), SIGUSR1); // Avisamos al padre
        pause(); // Queda bloqueado
        
        exit(0); // Nunca llega, pero es buena práctica ponerlo
    }

    // Ojo aquí: El fork del procesador lo hace SOLO el padre, porque el sensor está atrapado arriba (o hizo exit/pause).
    pid_t procesador = fork();

    if (procesador < 0) {
        perror("Error fork procesador");
        exit(EXIT_FAILURE);
    } else if (procesador == 0) {
        // ---- CÓDIGO DEL PROCESADOR ----
        close(tuberia[1]); // Cerramos escritura

        int num_leido;
        int pid_padre = getppid();

        // Bucle while leyendo secuencialmente hasta que el sensor cierre el pipe
        while (read(tuberia[0], &num_leido, sizeof(int)) > 0) {
            if ((num_leido % 2) == 0){
                controlador[0]++; // Par: dato válido
            } else {
                controlador[1]++; // Impar: alerta
                kill(pid_padre, SIGUSR2); // Avisamos al padre
            }
        }

        close(tuberia[0]);
        exit(7); // Finalización solicitada
    }

    // ---- CÓDIGO DEL PADRE (CONTROLADOR) ----
    // El padre no usa el pipe, cerramos ambos lados obligatoriamente
    close(tuberia[0]);
    close(tuberia[1]);

    int status;
    // Esperamos concretamente al procesador
    waitpid(procesador, &status, 0);

    if(WIFEXITED(status)) {
        int codigo = WEXITSTATUS(status);
        if (codigo == 7) {
            printf("El Procesador termino correctamente con codigo -%d-\n", codigo);
        }
    }

    // Matamos al sensor que estaba bloqueado en el pause()
    kill(sensor, SIGKILL);
    // Un waitpid extra para limpiar al sensor zombie y que no quede colgado en el sistema
    waitpid(sensor, NULL, 0);

    // Impresiones
    puts("---- RESUMEN DEL PROGRAMA ----");
    printf("- Datos válidos almacenados (mmap) = %d -\n", controlador[0]);
    printf("- Señales almacenadas (mmap) = %d -\n", controlador[1]);
    printf("- Señales SIGUSR2 recibidas = %d -\n", contador[1]);
    printf("- Señales SIGUSR1 recibidas = %d -\n", contador[0]);

}
