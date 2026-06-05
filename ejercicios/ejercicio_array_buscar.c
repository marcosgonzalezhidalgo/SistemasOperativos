/* Define un array de 10 números enteros y un "número objetivo" a buscar.

El proceso principal (Padre) debe crear dos procesos hijos utilizando la llamada al sistema fork().

El Hijo 1 deberá buscar el número objetivo en la primera mitad del array.

Si lo encuentra, terminará su ejecución devolviendo un estado de éxito (exit(1)).

Si no lo encuentra, terminará devolviendo un estado de fracaso (exit(0)).

El Hijo 2 deberá hacer lo mismo, pero buscando en la segunda mitad del array.

El proceso Padre debe usar la llamada al sistema wait() para esperar obligatoriamente a que ambos hijos terminen.

El Padre debe inspeccionar el estado de salida de cada hijo utilizando las macros adecuadas 
(WIFEXITED y WEXITSTATUS). Si al menos uno de los hijos devolvió un 1, el padre imprimirá por pantalla: 
"¡Número encontrado!". De lo contrario, imprimirá "Número no encontrado".
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 10

int main() {
    // Array de prueba y número a buscar
    int numeros[SIZE] = {4, 8, 15, 16, 23, 42, 50, 75, 90, 100};
    int objetivo = 23;
    
    pid_t pid1, pid2;
    int estado;
    int encontrado = 0; // Bandera para el padre

    printf("--- Iniciando Búsqueda Concurrente ---\n");
    printf("Proceso Padre (PID: %d) buscando el número %d.\n\n", getpid(), objetivo);

    // -----------------------------------------
    // CREACIÓN DEL PRIMER HIJO
    // -----------------------------------------
    pid1 = fork();
    
    if (pid1 < 0) {
        perror("Error al crear el primer hijo");
        exit(EXIT_FAILURE);
    } 
    else if (pid1 == 0) {
        // Código del Hijo 1
        printf("Hijo 1 (PID: %d): Buscando en la primera mitad [índices 0-4]...\n", getpid());
        for (int i = 0; i < SIZE / 2; i++) {
            if (numeros[i] == objetivo) {
                exit(1); // Encontrado
            }
        }
        exit(0); // No encontrado
    }

    // -----------------------------------------
    // CREACIÓN DEL SEGUNDO HIJO (El padre lo ejecuta)
    // -----------------------------------------
    pid2 = fork();
    
    if (pid2 < 0) {
        perror("Error al crear el segundo hijo");
        exit(EXIT_FAILURE);
    } 
    else if (pid2 == 0) {
        // Código del Hijo 2
        printf("Hijo 2 (PID: %d): Buscando en la segunda mitad [índices 5-9]...\n", getpid());
        for (int i = SIZE / 2; i < SIZE; i++) {
            if (numeros[i] == objetivo) {
                exit(1); // Encontrado
            }
        }
        exit(0); // No encontrado
    }

    // -----------------------------------------
    // CÓDIGO DEL PADRE: Esperar a los hijos
    // -----------------------------------------
    
    // Como tenemos 2 hijos, iteramos 2 veces con wait()
    for (int i = 0; i < 2; i++) {
        pid_t pid_hijo_terminado = wait(&estado);
        
        // Verificamos si el hijo terminó de forma normal (sin ser matado por una señal)
        if (WIFEXITED(estado)) {
            // Extraemos el código que el hijo puso en el exit()
            int codigo_salida = WEXITSTATUS(estado);
            printf("Padre: El hijo %d terminó con estado: %d\n", pid_hijo_terminado, codigo_salida);
            
            if (codigo_salida == 1) {
                encontrado = 1; // Un hijo lo encontró
            }
        }
    }

    // Resultado final evaluado por el padre
    printf("\n--- Resultado Final ---\n");
    if (encontrado) {
        printf("✅ El número %d SÍ está en el array.\n", objetivo);
    } else {
        printf("❌ El número %d NO está en el array.\n", objetivo);
    }

    return 0;
}