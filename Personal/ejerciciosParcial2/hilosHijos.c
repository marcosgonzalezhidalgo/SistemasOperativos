#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIMITE 5

void calcula() {
    long long unsigned t;
    for (t=0; t<(0xFFFFFFF); t++);
}

// Ahora la función recibe un argumento genérico (void *arg)
void *creaThread(void *arg) {
    // Rescatamos el nivel actual que nos ha pasado el padre
    int nivel = *(int*)arg; 
    pthread_t mi_id = pthread_self();

    printf("He nacido. Soy el hilo con ID %lu\n", (unsigned long)mi_id);
    
    calcula();

    // Condición de parada (caso base de la recursividad)
    if (nivel < LIMITE) {
        pthread_t hijo; // Ya no necesitamos un array global
        int nivel_hijo = nivel + 1; // Calculamos el nivel a pasarle al hijo
        
        // Creamos al hijo y le pasamos su nivel (nivel_hijo) por referencia
        pthread_create(&hijo, NULL, creaThread, &nivel_hijo);
        
        // Esperamos a que termine (la "burbuja" de subida recursiva)
        pthread_join(hijo, NULL);
    } else {
        // Caso base: hemos llegado al final de la cadena
        printf("%d\n", LIMITE);
    }

    printf("Soy el hilo con ID %lu y acabo mi ejecucion\n", (unsigned long)mi_id);
    
    return NULL;
}

int main(void) {
    pthread_t primer_hilo;
    int nivel_inicial = 1; // Empezamos en el nivel 1
    
    // Creamos el primer hilo pasándole la dirección de memoria de nivel_inicial
    pthread_create(&primer_hilo, NULL, creaThread, &nivel_inicial);
    
    pthread_join(primer_hilo, NULL);
    
    return 0;
}