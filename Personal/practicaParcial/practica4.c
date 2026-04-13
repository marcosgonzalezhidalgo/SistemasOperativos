/* Escribe un programa en C que cree un proceso hijo utilizando la 
llamada al sistema fork(). El proceso hijo deberá utilizar alguna función de 
la familia exec (como execl o execlp) para ejecutar el comando del sistema ls 
con el argumento -l sobre el directorio /tmp (el equivalente a escribir 
ls -l /tmp en la terminal). Asegúrate de incluir el control de errores pertinente 
en caso de que la función exec falle. Mientras tanto, el proceso padre debe suspender 
su ejecución esperando a que su hijo termine de ejecutar el comando. Una vez que el 
hijo haya finalizado por completo, el proceso padre deberá imprimir por pantalla el 
mensaje: "Soy el Padre. El listado del directorio ha finalizado." y terminar el 
programa con éxito.*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

int main (void){

    pid_t hijo = fork();

    if (hijo < 0){
        perror("Error al crear el proceso hijo");
        exit(EXIT_FAILURE);
    }

    if (hijo==0){
        execlp("ls", "ls", "-l", "/tmp", NULL);
        
        perror("Error al ejecutar el comando ls");
        exit(EXIT_FAILURE);
    }

    if (hijo > 0){
        wait(NULL);
        printf("Soy el Padre. El listado del directorio ha finalizado.\n");
        exit(EXIT_SUCCESS);
    }

    return 0;
}