/* Escribe un programa en C que utilice una tubería (pipe) y una zona de memoria compartida 
(mmap) para procesar un número. El programa principal (Padre) deberá inicializar 
(mmap) para procesar un número. El programa principal (Padre) deberá inicializar 
la memoria compartida y la tubería, y a continuación crear dos procesos hijos. 
El Hijo 1 solicitará al usuario un número entero por teclado y lo enviará al Hijo 2 
exclusivamente a través de la tubería. El Hijo 2 leerá ese número de la tubería, 
le sumará el valor 100, y guardará el resultado final en la variable de memoria 
compartida. Finalmente, el hijo Padre debe esperar a que ambos hijos terminen su 
ejecución por completo y, una vez finalizados, leer el valor de la memoria compartida 
e imprimirlo por pantalla con el mensaje "Soy el Padre y el resultado final es: X". 
Recuerda cerrar adecuadamente los descriptores de la tubería en los tres procesos 
para evitar bloqueos. */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

static int *variable_compartida; 

int main(){
    variable_compartida=mmap (NULL, sizeof *variable_compartida, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *variable_compartida=0;

    int tuberia[2];
    pipe(tuberia);
    
    pid_t hijo1;
    hijo1 = fork();

    if (hijo1 == 0){ // proceso hijo1
        int entrada;
        puts("Soy el hijo 1, dame un numero entero: ");
        scanf("%d", &entrada);
        write(tuberia[1], &entrada, sizeof(entrada));
        close(tuberia[1]);
        close(tuberia[0]);
        exit(EXIT_SUCCESS);
        

    } 
        
    pid_t hijo2;
    hijo2 = fork();

    if (hijo2 == 0){ // proceso hijo2
        int entrada;
        read(tuberia[0], &entrada, sizeof(entrada));
        close(tuberia[0]);
        close(tuberia[1]);
        *variable_compartida = entrada + 100;
        exit(EXIT_SUCCESS);
        
    }

    wait(NULL);
    wait(NULL);
    close(tuberia[0]);
    close(tuberia[1]);

    printf("Soy el Padre y el resultado final es: %d\n", *variable_compartida);

    return 0;
}

