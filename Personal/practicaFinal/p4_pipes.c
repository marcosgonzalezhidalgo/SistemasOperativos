#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){

    int tuberia[2];
    pipe(tuberia);

    pid_t hijo = fork();

    if (hijo < 0){

        perror("El hijo ha generado un error al crearse");
        exit(EXIT_FAILURE);
    
    } else if (hijo == 0){

        close(tuberia[1]);

        int var_recibida;
        read(tuberia[0], &var_recibida, sizeof(var_recibida));

        var_recibida *= 5;

        printf("Soy el hijo y el resultado es %d", var_recibida);

        close(tuberia[0]);

        exit(0);

    } else {

        close(tuberia[0]);

        int entrada;

        puts("Escribe un numero entero: ...");
        scanf("%d", &entrada);

        write(tuberia[1], &entrada, sizeof(entrada));

        close(tuberia[1]);

        wait(NULL);
    }

    return 0;
}
