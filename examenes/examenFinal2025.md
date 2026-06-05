# Examen Final 2025 — Enunciado

Crear un programa que a través de procesos y/o hilos realice las siguientes tareas:

## a) Estructura de datos

Crea una matriz de enteros con tamaño `ROWS x COLS`, con valores iniciales para las constantes `ROWS = 4` y `COLS = 3`.

## b) Creación de hilos

Crea 3 hilos/procesos que se llamarán **"coordinador"**, **"positivo"**, **"negativo"**. Estos hilos deben permanecer vivos hasta el final del programa.

## c) Coordinación

El hilo/proceso **coordinador** gestiona a los otros dos hilos, que se encargan de llenar de valores la matriz de enteros pidiendo su valor al usuario.

## d) Distribución de trabajo

- Las posiciones de la matriz cuyo producto de los dos índices que la componen sea **par** (incluyendo el 0), será rellenada preguntando al usuario por el hilo/proceso **"positivo"**.
- Aquellas posiciones cuyo producto de los dos índices sea **impar**, se encargará de rellenarlas el hilo/proceso **"negativo"**.

> **Nota:** Cuando el producto de índices sea "0", se encarga el hilo o proceso **"positivo"**.

## e) Sincronización

Para sincronizar los 3 hilos/procesos **no** pueden emplearse flags o semáforos/mutex, sino únicamente señales. Para ello manejaremos un máximo de 3: `SIGALRM`, `SIGUSR1`, `SIGUSR2`.

## f) Mostrar resultado

Cuando la matriz está llena por completo, el **"coordinador"** muestra el contenido de la matriz en formato tabular. Los hilos/procesos restantes siguen vivos en este punto.

## g) Finalización ordenada

Una vez mostrada la matriz, el **"coordinador"** elimina primero a **"negativo"**, luego a **"positivo"** y por último a sí mismo.

## h) Terminación del programa

Solo entonces finalizará el proceso principal del programa, cuando los demás terminen sus tareas.
