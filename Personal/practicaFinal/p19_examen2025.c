#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// a) Estructura de datos: Constantes y matriz global compartida
#define ROWS 4
#define COLS 3

int matriz[ROWS][COLS];

// Variables globales para saber qué celda toca rellenar
int fila_actual = 0;
int col_actual = 0;

// Identificadores de los hilos
pthread_t tid_coord, tid_pos, tid_neg;

// e) Sincronización: Manejador de señal vacío. 
// Como no podemos usar flags ni mutex, el trabajo de sincronización
// recae exclusivamente en la función pause() y el envío de señales.
void manejador_vacio(int sig) {
    // No hace nada. Su única función es hacer que pause() se interrumpa 
    // y continúe la ejecución de la siguiente línea de código.
}

// Hilo encargado de las celdas con producto PAR
void* hilo_positivo(void* arg) {
    while(1) {
        pause(); // Espera la señal (SIGUSR1) del coordinador
        
        printf("\n[Positivo] Rellenando posición [%d][%d] (Producto: %d)\n", 
               fila_actual, col_actual, fila_actual * col_actual);
        printf("Introduce un número entero: ");
        scanf("%d", &matriz[fila_actual][col_actual]);
        
        // Avisa al coordinador de que ya ha terminado su trabajo
        pthread_kill(tid_coord, SIGALRM); 
    }
    return NULL;
}

// Hilo encargado de las celdas con producto IMPAR
void* hilo_negativo(void* arg) {
    while(1) {
        pause(); // Espera la señal (SIGUSR2) del coordinador
        
        printf("\n[Negativo] Rellenando posición [%d][%d] (Producto: %d)\n", 
               fila_actual, col_actual, fila_actual * col_actual);
        printf("Introduce un número entero: ");
        scanf("%d", &matriz[fila_actual][col_actual]);
        
        // Avisa al coordinador de que ya ha terminado su trabajo
        pthread_kill(tid_coord, SIGALRM);
    }
    return NULL;
}

// Hilo Coordinador
void* hilo_coordinador(void* arg) {
    // Damos un pequeño margen para asegurar que Positivo y Negativo 
    // han arrancado y están bloqueados en su respectivo pause()
    usleep(50000); 

    // Recorremos la matriz
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            fila_actual = i;
            col_actual = j;
            
            int producto = i * j;
            
            // Retardo crítico: evita la condición de carrera donde el coordinador
            // envíe una señal antes de que el trabajador haya vuelto a ejecutar pause()
            usleep(10000); 

            // d) Distribución de trabajo
            if (producto % 2 == 0) { // Si es PAR (o cero)
                pthread_kill(tid_pos, SIGUSR1);
            } else {                 // Si es IMPAR
                pthread_kill(tid_neg, SIGUSR2);
            }
            
            // Esperar a que el trabajador elegido devuelva la señal de finalización (SIGALRM)
            pause(); 
        }
    }

    // f) Mostrar resultado
    printf("\n============================\n");
    printf("        MATRIZ FINAL\n");
    printf("============================\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%4d ", matriz[i][j]);
        }
        printf("\n");
    }
    printf("============================\n\n");

    // g) Finalización ordenada
    printf("[Coordinador] Eliminando a hilo 'negativo'...\n");
    pthread_cancel(tid_neg);
    usleep(10000); // Pequeño margen para limpieza interna
    
    printf("[Coordinador] Eliminando a hilo 'positivo'...\n");
    pthread_cancel(tid_pos);
    usleep(10000);

    printf("[Coordinador] Eliminándose a sí mismo...\n");
    pthread_exit(NULL);

    return NULL; 
}

int main(void) {
    // Preparamos struct para el sigaction (basado en el código de tu profesor)
    struct sigaction sa;
    sa.sa_handler = manejador_vacio;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Para interrumpir llamadas bloqueantes (como pause)

    // e) Sincronización SOLO por señales permitidas
    // Lo configuramos antes de lanzar los hilos para que todos hereden la misma configuración
    if (sigaction(SIGUSR1, &sa, NULL) == -1 ||
        sigaction(SIGUSR2, &sa, NULL) == -1 ||
        sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Error registrando señales");
        exit(EXIT_FAILURE);
    }

    printf("[Main] Iniciando programa y creando hilos...\n");

    // b) Creación de los hilos
    pthread_create(&tid_pos, NULL, hilo_positivo, NULL);
    pthread_create(&tid_neg, NULL, hilo_negativo, NULL);
    pthread_create(&tid_coord, NULL, hilo_coordinador, NULL);

    // h) Terminación del programa
    // El proceso principal espera a que todos terminen con join.
    // Incluso si un hilo ha sido cancelado con pthread_cancel, el join 
    // lo captura limpiamente y finaliza esa instancia.
    pthread_join(tid_coord, NULL);
    pthread_join(tid_pos, NULL);
    pthread_join(tid_neg, NULL);

    printf("[Main] Todos los hilos han terminado. Fin del proceso principal.\n");
    return EXIT_SUCCESS;
}