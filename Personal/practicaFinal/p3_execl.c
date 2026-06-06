#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){

    pid_t hijo = fork();

    if (hijo < 0){

        perror("El proceso hijo ha generado un error");
        exit(EXIT_FAILURE);
    } else if (hijo == 0){

        char entradaUsuario[] = "";

        puts("Introduce un archivo para ver su contenido");
        scanf("%s", entradaUsuario);

        execl("/bin/cat", "cat", entradaUsuario, NULL);

        puts("El programa se ha cerrado por un error");
        exit(EXIT_FAILURE);

    } else {

        wait(NULL);

    }

    return 0;
}
