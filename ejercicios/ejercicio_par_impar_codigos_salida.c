/*Define una variable entera en el programa principal con un valor arbitrario (por ejemplo, int numero = 7;).
Utiliza la llamada al sistema fork() para crear un nuevo proceso.
Proceso Hijo: * Debe comprobar si la variable numero es par o impar.
Si es par, terminará su ejecución devolviendo un código de salida 0.
Si es impar, terminará devolviendo un código de salida 1.

Proceso Padre:
Debe esperar a que el hijo termine utilizando la llamada wait().
Una vez el hijo termine, el padre debe comprobar que finalizó de forma normal y voluntaria utilizando la macro WIFEXITED.
Si terminó de forma normal, el padre extraerá el código de salida usando WEXITSTATUS y mostrará por pantalla si el número era par o impar basándose en ese código.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;
    int numero = 7; // Puedes cambiar este número para probar

    printf("Padre (PID: %d): Voy a crear un proceso hijo.\n", getpid());

    // Creamos el proceso hijo
    pid = fork();

    if (pid < 0) {
        // Si fork() devuelve un número negativo, hubo un error
        perror("Error en el fork");
        exit(EXIT_FAILURE);
        
    } else if (pid == 0) {
        /* ==========================================
           CÓDIGO DEL PROCESO HIJO
           ========================================== */
        printf("Hijo  (PID: %d): Evaluando si el número %d es par o impar...\n", getpid(), numero);
        
        if (numero % 2 == 0) {
            // Es par. Devolvemos 0 al sistema/padre.
            exit(0); 
        } else {
            // Es impar. Devolvemos 1 al sistema/padre.
            exit(1); 
        }
        
    } else {
        /* ==========================================
           CÓDIGO DEL PROCESO PADRE
           ========================================== */
        printf("Padre (PID: %d): Esperando a que mi hijo (PID: %d) termine...\n", getpid(), pid);
        
        // Esperamos a que el hijo termine y guardamos su estado en la variable 'status'
        wait(&status); 

        // 1. Comprobamos si el hijo terminó usando exit() o return de forma normal
        if (WIFEXITED(status)) {
            
            // 2. Extraemos el código exacto que el hijo puso en el exit()
            int resultado_hijo = WEXITSTATUS(status);
            
            // 3. Actuamos en base al resultado
            if (resultado_hijo == 0) {
                printf("Padre: El hijo ha terminado. Me dice que el número es PAR.\n");
            } else if (resultado_hijo == 1) {
                printf("Padre: El hijo ha terminado. Me dice que el número es IMPAR.\n");
            } else {
                printf("Padre: El hijo devolvió un código desconocido (%d).\n", resultado_hijo);
            }
            
        } else {
            // Esto ocurriría si el hijo es matado por una señal externa (ej: kill -9)
            printf("Padre: Cuidado, el proceso hijo no terminó de forma normal.\n");
        }
    }

    return 0;
}
