#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>

// 1. Declaramos la variable compartida de forma global
static int *variable_compartida;

int main(void) {
    // Inicializamos la memoria compartida
    variable_compartida = mmap(NULL, sizeof *variable_compartida, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *variable_compartida = 0;

    // 2. Creamos las dos tuberías de sincronización
    int tuberia_ida[2];    // De "Lee" hacia "Escribe"
    int tuberia_vuelta[2]; // De "Escribe" hacia "Lee"

    if (pipe(tuberia_ida) == -1 || pipe(tuberia_vuelta) == -1) {
        perror("Error al crear las tuberías");
        exit(EXIT_FAILURE);
    }

    // 3. Creamos el primer proceso: LEE (Hijo 1)
    pid_t lee = fork();

    if (lee == 0) {
        // "Lee" escribe en la ida y lee de la vuelta. Cerramos lo que no usa.
        close(tuberia_ida[0]);    
        close(tuberia_vuelta[1]); 

        int entrada;
        printf("Proceso LEE: Introduce un numero por teclado: ");
        scanf("%d", &entrada);

        // Guardamos el número en la memoria compartida
        *variable_compartida = entrada;

        // AVISAMOS A ESCRIBE: Le mandamos un "1" por la tubería para despertarlo
        int aviso = 1;
        write(tuberia_ida[1], &aviso, sizeof(aviso));
        close(tuberia_ida[1]); // Ya hemos avisado, cerramos

        // ESPERAMOS PARA MORIR: Nos quedamos bloqueados hasta que "Escribe" nos avise
        int confirmacion_fin;
        read(tuberia_vuelta[0], &confirmacion_fin, sizeof(confirmacion_fin));
        close(tuberia_vuelta[0]);

        printf("Proceso LEE: He recibido la señal. Me muero, ¡adiós!\n");
        exit(EXIT_SUCCESS);
    }

    // 4. Creamos el segundo proceso: ESCRIBE (Hijo 2)
    pid_t escribe = fork();

    if (escribe == 0) {
        // "Escribe" lee de la ida y escribe en la vuelta. Cerramos lo que no usa.
        close(tuberia_ida[1]);    
        close(tuberia_vuelta[0]); 

        int esperar_aviso;
        
        // ESPERAMOS A LEE: El proceso se bloquea aquí hasta que "Lee" escriba en la tubería
        read(tuberia_ida[0], &esperar_aviso, sizeof(esperar_aviso));
        close(tuberia_ida[0]);

        // Si llegamos a esta línea, es porque "Lee" ya guardó el dato. Es seguro imprimir.
        printf("Proceso ESCRIBE: El numero guardado en la memoria es el: %d\n", *variable_compartida);

        // Pedimos confirmación para finalizar (usamos un string para que lea cualquier tecla/palabra)
        char tecla[20];
        printf("Proceso ESCRIBE: Pulse una tecla y de a ENTER para finalizar: ");
        scanf("%s", tecla);

        // AVISAMOS A LEE PARA MORIR: Le mandamos un "1" para que se desbloquee y muera
        int matar = 1;
        write(tuberia_vuelta[1], &matar, sizeof(matar));
        close(tuberia_vuelta[1]);

        printf("Proceso ESCRIBE: Me muero, ¡adiós!\n");
        exit(EXIT_SUCCESS);
    }

    // 5. Código exclusivo del PADRE
    
    // IMPORTANTE: El Padre no interactúa en esta comunicación. Debe cerrar TODAS las tuberías.
    close(tuberia_ida[0]);
    close(tuberia_ida[1]);
    close(tuberia_vuelta[0]);
    close(tuberia_vuelta[1]);

    // Esperamos a que los dos hijos terminen
    wait(NULL);
    wait(NULL);

    printf("Soy el PADRE: Mis dos hijos (Lee y Escribe) han muerto. Programa terminado.\n");

    return 0;
}