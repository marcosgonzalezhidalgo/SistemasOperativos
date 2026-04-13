/* Escribe un programa en C que utilice memoria compartida (mmap) para almacenar 
un array de 10 números enteros. El proceso Padre debe inicializar este array en la 
memoria compartida con los números del 1 al 10 (inclusive) y crear una tubería (pipe). 
A continuación, el Padre creará dos procesos hijos. El Hijo 1 deberá leer los 5 primeros 
números del array compartido, calcular su suma y enviar el resultado parcial al Padre a 
través de la tubería. El Hijo 2 deberá leer los 5 últimos números del array compartido, 
calcular su suma y enviar su resultado parcial al Padre por la misma tubería. Tras enviar 
los datos, ambos hijos deben terminar su ejecución. El proceso Padre debe esperar a que 
ambos hijos terminen, leer los dos resultados parciales de la tubería, sumarlos para 
obtener el total, e imprimir por pantalla: "Soy el Padre. La suma total calculada por mis 
hijos es: X". Asegúrate de cerrar correctamente los extremos de la tubería en cada proceso.*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

static int *numeros_compartidos; 


int public(void){
    numeros_compartidos = mmap(NULL, sizeof *numeros_compartidos, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *numeros_compartidos = {1,2,3,4,5,6,7,8,9,10};

    int tuberia[2];
    pipe(tuberia);

    pid_t hijo1 = fork();

    if (hijo1 == 0){
        
        int suma1 = 0;
        for (int i = 0; i < 5; i++){
            suma1 += numeros_compartidos[i];
        }
        write(tuberia[1], &suma1, sizeof(suma1));
        close(tuberia[1]);
        close(tuberia[0]);
        exit(EXIT_SUCCESS);
        
    }

    pid_t hijo2 = fork();

    if (hijo2 == 0){
        
        int suma2 = 0;
        for (int i = 5; i < 10; i++){
            suma2 += numeros_compartidos[i];
        }
        write(tuberia[1], &suma2, sizeof(suma2));
        close(tuberia[1]);
        close(tuberia[0]);
        exit(EXIT_SUCCESS);
    }
    
    wait(NULL);
    wait(NULL);
    close(tuberia[0]);
    close(tuberia[1]);
    
    int suma1, suma2;
    read(tuberia[0], &suma1, sizeof(suma1));
    read(tuberia[0], &suma2, sizeof(suma2));
    
    int suma_total = suma1 + suma2;
    printf("Soy el Padre. La suma total calculada por mis hijos es: %d\n", suma_total);
    

    return 0;
}