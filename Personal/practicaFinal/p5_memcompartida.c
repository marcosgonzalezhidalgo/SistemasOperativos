#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){

    int *saldo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *saldo = 100;

    printf("El saldo actual es %d\n", *saldo);

    pid_t hijo = fork();

    if (hijo < 0){

        perror("El proceso hijo ha generado un error");
        exit(EXIT_FAILURE);
    
    }else if (hijo == 0) {

        int entrada;

        do {
        printf("Por favor, introduce la cantidad a retirar: \n");
        scanf("%d", &entrada);

        if (entrada > *saldo) {
            printf("Error: %d supera el saldo en la cuenta\n", entrada);
        }

    } while (entrada > *saldo);

    printf("¡Perfecto! Has retirado %d\n", entrada);

    *saldo -= entrada;

    exit(0);

} else{

    wait(NULL);

    int entrada;

    printf("Por favor, introduce la cantidad a ingresar: ");
    scanf("%d", &entrada);

    *saldo += entrada;

    printf("Se ha ingresado %d al saldo\n", entrada);

    printf("Tu saldo actual es %d\n", *saldo);

    munmap(saldo, sizeof(int));

}

    return 0;
}