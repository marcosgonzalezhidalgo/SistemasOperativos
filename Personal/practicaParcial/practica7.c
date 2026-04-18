/* Escribe un programa en C que cree una zona de memoria compartida (mmap) 
con espacio para un array de 2 enteros: la posición [0] almacenará un resultado 
matemático y la posición [1] actuará como un "flag" de estado (inicializado a 0). 
A continuación, el programa creará una única tubería y generará dos procesos hijos 
(Hijo 1 e Hijo 2). El Hijo 1 deberá solicitar al usuario un número entero por teclado, 
enviarlo al Hijo 2 a través de la tubería, y quedarse esperando (usando un bucle de 
espera activa sobre la posición [1] de la memoria compartida) hasta que el flag 
cambie a 1. Una vez que el flag sea 1, el Hijo 1 usará una función de la familia 
exec para ejecutar el comando del sistema "ls -l" y se reemplazará. Por su parte, 
el Hijo 2 deberá leer el número de la tubería, multiplicarlo por 100, guardar el 
resultado final en la posición [0] de la memoria compartida, y finalmente cambiar 
el flag de la posición [1] a 1 para despertar a su hermano, tras lo cual terminará 
su ejecución. El proceso Padre no usará la tubería, simplemente esperará a que ambos 
hijos terminen por completo y, finalmente, imprimirá por pantalla "Soy el Padre. El 
resultado final calculado es: X", donde X es el valor almacenado en la memoria 
compartida. Es estrictamente obligatorio cerrar adecuadamente todos los extremos de 
la tubería en los tres procesos. */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

static int *variable_compartida;

int main(void) {

    variable_compartida = mmap(NULL, 2 * sizeof(*variable_compartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    variable_compartida[0] = 0;
    variable_compartida[1] = 0;

    int tuberia[2];
    
    if (pipe(tuberia) < 0){
        perror("Error al inicializar la tuberia");
        exit(EXIT_FAILURE);
    }

    pid_t hijo1 = fork();

    if (hijo1 == 0) {
        close(tuberia[0]);

        int entrada;

        puts("Introduzca un numero: ...");
        scanf("%d", &entrada);

        write(tuberia[1], &entrada, sizeof(entrada));
        close(tuberia[1]);

        while (variable_compartida[1] == 0) {
            usleep(1000);
        }

        execlp("ls", "ls", "-l", (char *)NULL);

        exit(EXIT_FAILURE);
    }

    pid_t hijo2 = fork();

    if (hijo2 == 0) {
        close(tuberia[1]);

        int entrada;
        read(tuberia[0], &entrada, sizeof(entrada));
        close(tuberia[0]);

        variable_compartida[0] = entrada * 100;

        variable_compartida[1] = 1;

        exit(EXIT_SUCCESS);

    }

    close(tuberia[0]);
    close(tuberia[1]);

    wait(NULL);
    wait(NULL);

    printf("Soy el Padre. El resultado final calculado es: %d\n", variable_compartida[0]);
    return 0;
}
