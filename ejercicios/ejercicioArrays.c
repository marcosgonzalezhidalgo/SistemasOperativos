#include <stdio.h>

int main() {
    // Definimos un array con un tamaño máximo suficiente
    int array[100];
    int n, i, posicion, nuevoNumero;
    
    // 1. Solicitar el número de elementos
    printf("introduce cuantos elementos tiene el array: ");
    scanf("%d", &n);
    
    // 2. Introducir los elementos
    printf("introduce los elementos del array:\n");
    for (i = 0; i < n; i++) {
        printf("Elemento %d: ", i + 1);
        scanf("%d", &array[i]);
    }
    
    // 3. Mostrar el array inicial
    printf("\nel array inicial es: [ ");
    for (i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
    
    // 4. Pedir posición y el nuevo número
    printf("\nintroduce la posicion en la que quieres introducir un numero (0 a %d): ", n);
    scanf("%d", &posicion);
    
    printf("introduce el numero que quieres insertar: ");
    scanf("%d", &nuevoNumero);
    
    // Lógica de inserción:
    // Movemos los elementos una posición a la derecha desde el final hasta la posición deseada
    for (i = n; i > posicion; i--) {
        array[i] = array[i - 1];
    }
    
    // Insertamos el número en el hueco creado
    array[posicion] = nuevoNumero;
    n++; // El array ahora tiene un elemento más
    
    // 5. Mostrar el array final
    printf("el array final es: [ ");
    for (i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("]\n");
    
    return 0;
}