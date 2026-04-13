/* Escribe un programa en C donde un proceso Padre y un proceso Hijo se 
comuniquen de forma bidireccional utilizando dos tuberías independientes. 
El Padre debe solicitar un número entero al usuario por teclado y enviarlo 
al Hijo a través de la primera tubería. El Hijo debe leer ese número, 
multiplicarlo por 5, y enviar el nuevo resultado de vuelta al Padre utilizando
la segunda tubería. Tras enviar el dato, el Hijo debe terminar su ejecución. 
Finalmente, el Padre debe leer el resultado recibido por la segunda tubería, 
imprimir por pantalla "Soy el Padre, el resultado devuelto por mi hijo es: X", 
y esperar correctamente a que su hijo termine. Es estrictamente obligatorio cerrar 
todos los extremos de las tuberías que no se estén utilizando en cada momento para 
evitar interbloqueos.*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

int main(void) {

    int tuberia_padre[2], tuberia_hijo[2];
   
    if (pipe(tuberia_padre) == -1 || pipe(tuberia_hijo) == -1){ // si falla la creacion de las tuberias
        perror("Error al crear las tuberias");
        exit(EXIT_FAILURE);
    }

    pid_t hijo = fork();

    if (hijo == 0){ // proceso hijo

        close(tuberia_padre[1]);
        close(tuberia_hijo[0]);

        int entrada;
        read(tuberia_padre[0], &entrada, sizeof(entrada));
        
        int num_final = entrada * 5;

        write(tuberia_hijo[1], &num_final, sizeof(num_final));

        close(tuberia_padre[0]);
        close(tuberia_hijo[1]);
        exit(EXIT_SUCCESS);
    }

    if (hijo > 0){ // proceso padre

        close(tuberia_padre[0]);
        close(tuberia_hijo[1]);

        int entrada, num_final;
        puts("Introduzca un número a continuación: ...");
        scanf("%d", &entrada);

        write(tuberia_padre[1], &entrada, sizeof(entrada));
        close(tuberia_padre[1]);

        read(tuberia_hijo[0], &num_final, sizeof(num_final));
        close(tuberia_hijo[0]);

        printf("Soy el Padre, el resultado devuelto por mi hijo es: %d\n", num_final);
        wait(NULL);
        exit(EXIT_SUCCESS);
        
    }

    

    return 0;
}