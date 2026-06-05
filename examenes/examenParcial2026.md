# Examen Parcial 2026 — Enunciado

Desarrolle un programa en C que implemente la siguiente funcionalidad:

## Paso 1 — Crear una memoria compartida

Antes de lanzar ningún proceso hijo, el padre reservará una región de memoria compartida usando `mmap`. Dicha región contendrá un único entero (`int`) que actuará como **contador global**, inicializado a 0.

## Paso 2 — Crear los procesos hijos

El proceso padre creará exactamente **4 procesos hijos** identificados internamente con los números del 1 al 4.

Cada proceso hijo deberá:

- Ejecutar un bucle de **100.000 iteraciones** en el que incrementa en una unidad el contador compartido en cada iteración.
- Al terminar el bucle, finalizar el proceso enviando mediante `exit` su número de identificación (1, 2, 3 o 4).

## Paso 3 — Recoger a los hijos con `wait`

El padre esperará a que todos sus hijos terminen usando `wait` en un bucle. Por cada hijo recogido deberá:

- Comprobar mediante `WIFEXITED()` que el hijo terminó de forma normal.
- Extraer su identificador mediante `WEXITSTATUS()`.
- Imprimir en qué posición de finalización quedó ese hijo y cuál es su PID.

## Paso 4 — Mostrar el resultado del contador

Una vez recogidos todos los hijos, el padre imprimirá:

- El valor **esperado** del contador.
- El valor **obtenido** del contador en la memoria compartida.
- El número de **incrementos perdidos** (diferencia entre ambos).

## Paso 5 — Liberar los recursos

Liberar la memoria compartida reservada con `mmap`.

---

## Comienzo del ejercicio

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define NUM_HIJOS    4
#define INCREMENTOS  100000

int main(void)
{
    int *contador = mmap(NULL, sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // ...

    return 0;
}
```