/*Objetivo: Escribir un programa en C que utilice la llamada al sistema fork() para crear un proceso hijo y establezca una comunicación bidireccional entre el padre y el hijo utilizando dos pipes.

Descripción:

El programa principal (proceso padre) debe crear dos tuberías (pipes): una para enviar datos al hijo y otra para recibir
datos del hijo.
A continuación, el padre creará un proceso hijo.
El proceso padre solicitará al usuario que introduzca una cadena de texto por teclado (máximo 100 caracteres).
El padre enviará esta cadena de texto al proceso hijo a través de la primera tubería.
El proceso hijo leerá la cadena de la primera tubería, calculará cuántas vocales (mayúsculas y minúsculas) contiene 
el texto y enviará este número entero de vuelta al padre a través de la segunda tubería.
Finalmente, el proceso padre leerá el resultado de la segunda tubería, lo imprimirá por pantalla y esperará 
a que el hijo termine su ejecución para evitar procesos "zombie".
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_BUFFER 100

// Función auxiliar para contar vocales
int contar_vocales(const char *cadena) {
    int contador = 0;
    for (int i = 0; cadena[i] != '\0'; i++) {
        char c = tolower(cadena[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            contador++;
        }
    }
    return contador;
}

int main() {
    // fd1: Pipe para enviar del Padre al Hijo
    // fd2: Pipe para enviar del Hijo al Padre
    int fd1[2]; 
    int fd2[2];
    pid_t pid;

    // 1. Crear las tuberías con control de errores
    if (pipe(fd1) == -1) {
        perror("Error al crear el pipe 1");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd2) == -1) {
        perror("Error al crear el pipe 2");
        exit(EXIT_FAILURE);
    }

    // 2. Crear el proceso hijo
    pid = fork();

    if (pid < 0) {
        perror("Error en el fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // ==================== PROCESO PADRE ====================
        
        char mensaje_enviar[MAX_BUFFER];
        int total_vocales;

        // Cerrar los extremos que el padre no va a usar
        close(fd1[0]); // El padre no lee del pipe 1
        close(fd2[1]); // El padre no escribe en el pipe 2

        // Solicitar texto al usuario
        printf("[Padre] Introduce una cadena de texto: ");
        fgets(mensaje_enviar, MAX_BUFFER, stdin);
        
        // Eliminar el salto de línea que añade fgets
        mensaje_enviar[strcspn(mensaje_enviar, "\n")] = 0;

        // Enviar la cadena al hijo a través del pipe 1
        write(fd1[1], mensaje_enviar, strlen(mensaje_enviar) + 1);
        printf("[Padre] Mensaje enviado al hijo. Esperando respuesta...\n");

        // Cerrar el extremo de escritura una vez enviado el mensaje
        close(fd1[1]);

        // Leer la respuesta (el conteo) del hijo a través del pipe 2
        read(fd2[0], &total_vocales, sizeof(total_vocales));
        printf("[Padre] El hijo me informa que hay %d vocales en el texto.\n", total_vocales);

        // Cerrar el extremo de lectura y esperar a que el hijo termine
        close(fd2[0]);
        wait(NULL); 
        
    } else { // ==================== PROCESO HIJO ====================
        
        char mensaje_recibido[MAX_BUFFER];
        int num_vocales;

        // Cerrar los extremos que el hijo no va a usar
        close(fd1[1]); // El hijo no escribe en el pipe 1
        close(fd2[0]); // El hijo no lee del pipe 2

        // Leer el mensaje del padre desde el pipe 1
        read(fd1[0], mensaje_recibido, MAX_BUFFER);
        
        // Cerrar el extremo de lectura del pipe 1 ya que no se leerá más
        close(fd1[0]);

        // Procesar la información
        num_vocales = contar_vocales(mensaje_recibido);

        // Enviar el resultado al padre a través del pipe 2
        write(fd2[1], &num_vocales, sizeof(num_vocales));

        // Cerrar el extremo de escritura del pipe 2
        close(fd2[1]);

        // El hijo termina su ejecución
        exit(EXIT_SUCCESS);
    }

    return 0;
}