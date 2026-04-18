/* Escribe un programa en C que cree una zona de memoria compartida (mmap) 
capaz de almacenar un único número entero, inicializado a 0. A continuación, 
el programa debe crear dos tuberías independientes (Pipe A y Pipe B) y generar 
dos procesos hijos (Hijo 1 e Hijo 2). El proceso Padre solicitará al usuario un 
número entero mayor que cero por teclado y lo enviará al Hijo 1 a través del Pipe A.

El Hijo 1 leerá este número del Pipe A. Si el número es par, el Hijo 1 lo guardará en 
la memoria compartida y terminará su ejecución. Si el número es impar, el Hijo 1 lo 
enviará al Hijo 2 a través del Pipe B y terminará su ejecución.

El Hijo 2 intentará leer del Pipe B. Si recibe un número (porque era impar), 
utilizará una función de la familia exec para ejecutar el comando del sistema 
date (que imprime la fecha actual) y su proceso será reemplazado.

El proceso Padre debe esperar a que sus dos hijos terminen completamente. Una 
vez finalizados, el Padre leerá la memoria compartida: si el valor es mayor que 
0, imprimirá "Soy el Padre: El número era PAR y su valor es X". Si el valor sigue 
siendo 0, imprimirá "Soy el Padre: El número era IMPAR y mi hijo imprimió la fecha". 
Es estrictamente obligatorio cerrar correctamente los extremos de las tuberías en 
cada uno de los tres procesos para evitar bloqueos infinitos.*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

static int *variable_compartida; 

int main(void) {
    variable_compartida=mmap (NULL, sizeof *variable_compartida, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *variable_compartida = 0;

    int tuberia_padre[2], tuberia_hijo[2];

    if (pipe(tuberia_padre) == -1 || pipe(tuberia_hijo) == -1) {
        perror("La tuberia ha provocado un error");
        exit(EXIT_FAILURE);
    }

    pid_t hijo1 = fork();

    if (hijo1 == 0){
        close(tuberia_padre[1]);
        close(tuberia_hijo[0]);

        int entrada;

        read(tuberia_padre[0], &entrada, sizeof(entrada));
        close(tuberia_padre[0]);

        if (entrada % 2 == 0) {

            *variable_compartida = entrada;
            
            exit(EXIT_SUCCESS);

        } else {

            write(tuberia_hijo[1], &entrada, sizeof(entrada));
            close(tuberia_hijo[1]);
            
            exit(EXIT_SUCCESS);
        }
        
    } else {


    pid_t hijo2 = fork();

        if (hijo2 == 0) {
            close(tuberia_padre[1]);
            close(tuberia_padre[0]);
            close(tuberia_hijo[1]);


            int entrada = 0;

            read(tuberia_hijo[0], &entrada, sizeof(entrada));
            close(tuberia_hijo[0]);

            if(entrada != 0) {

                execlp("date", "date", NULL);

            }
        } else {
            close(tuberia_padre[0]);
            close(tuberia_hijo[0]);
            close(tuberia_hijo[1]);

            int entrada;

            puts("Dame un numero entero mayor que cero: ...");
            scanf("%d", &entrada);

            write(tuberia_padre[1], &entrada, sizeof(entrada));
            close(tuberia_padre[1]);

            wait(NULL);
            wait(NULL);

            if (*variable_compartida > 0) {
                printf("Soy el Padre: El número era PAR y su valor es %d\n", *variable_compartida);
            
                exit(EXIT_SUCCESS);
            } else {
                puts("Soy el Padre: El número era IMPAR y mi hijo imprimió la fecha");

                exit(EXIT_SUCCESS);
            }
        }
    }


    return 0;
}



