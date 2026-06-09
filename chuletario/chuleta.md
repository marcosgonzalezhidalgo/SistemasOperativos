# Cheat Sheet — Comandos en C para el Examen Práctico

Formulario definitivo ordenado por temas. Incluye las librerías necesarias y ejemplos reales y funcionales.

---

## 1. Creación y Control de Procesos

**Librerías:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
```

- **Crear un proceso (Clonación):**
  ```c
  pid_t pid = fork(); // Devuelve 0 en el hijo, y el PID del hijo en el padre
  ```

- **Terminar un proceso (Hijo):**
  ```c
  exit(0); // 0 indica éxito. Cualquier otro número (1, -1) indica error.
  ```

- **Padre espera al hijo SIN importar cómo muera:**
  ```c
  wait(NULL); // Suspende al padre hasta que cualquier hijo termine
  ```

- **Padre espera al hijo y LEE su nota de salida:**
  ```c
  int status;
  wait(&status); // El padre espera y guarda el estado en la variable

  if (WIFEXITED(status)) { // ¿Terminó con un exit() de forma normal?
      int codigo = WEXITSTATUS(status); // Extrae el número del exit(X)
  }
  ```

- **Obtener el DNI del proceso:**
  ```c
  getpid();  // Obtiene el PID del proceso actual
  getppid(); // Obtiene el PID de su Padre
  ```

---

## 2. Ejecución de Comandos Externos (Exec)

**Librerías:**
```c
#include <unistd.h>
```

> **Recuerda:** El proceso que ejecuta esto se destruye y es reemplazado por el nuevo comando.

- **Ejecutar comando pasando argumentos en una Lista (`execl`):**
  ```c
  // Ruta comando, Nombre comando, arg1, arg2..., SIEMPRE TERMINA EN NULL
  execl("/bin/ls", "ls", "-l", NULL);
  execl("/bin/cat", "cat", "archivo.txt", NULL);
  ```

- **Ejecutar comando pasando un Vector/Array (`execv`):**
  ```c
  char *argumentos[] = {"ls", "-l", NULL};
  execv("/bin/ls", argumentos);
  ```

---

## 3. Tuberías (Pipes) — Comunicación Unidireccional

**Librerías:**
```c
#include <unistd.h>
```

- **Crear la tubería (SIEMPRE antes del `fork`):**
  ```c
  int tuberia[2];
  pipe(tuberia); // tuberia[0] es LECTURA, tuberia[1] es ESCRITURA
  ```

- **Cerrar extremos no usados (Imprescindible):**
  ```c
  close(tuberia[1]); // Cierra escritura (si solo vas a leer)
  close(tuberia[0]); // Cierra lectura (si solo vas a escribir)
  ```

- **Escribir variables en la tubería:**
  ```c
  int numero_enviar = 42;
  // fd de escritura, dirección de la variable, tamaño en bytes
  write(tuberia[1], &numero_enviar, sizeof(numero_enviar));
  ```

- **Leer variables de la tubería:**
  ```c
  int numero_recibido;
  // fd de lectura, dirección de la variable, tamaño en bytes
  read(tuberia[0], &numero_recibido, sizeof(numero_recibido));
  ```

---

## 4. Memoria Compartida (mmap)

**Librerías:**
```c
#include <sys/mman.h>
```

- **Crear la variable compartida (SIEMPRE antes del `fork`):**
  ```c
  // Ejemplo para compartir un entero (int)
  int *saldo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  ```

- **Usar la variable compartida:**
  ```c
  *saldo = 100;           // Inicializar
  *saldo += 50;           // Sumar
  printf("%d", *saldo);   // Imprimir
  ```

- **Liberar la memoria (Al final del programa en el padre):**
  ```c
  // Puntero de la variable, tamaño en bytes
  munmap(saldo, sizeof(int));
  ```

---

## 5. Hilos (Threads)

**Librerías:**
```c
#include <pthread.h>
```

> **Nota:** Compilar en la terminal requiere `-lpthread`

- **Estructura OBLIGATORIA de la función del hilo:**
  ```c
  void* mi_funcion(void* arg) {
      // ... código del hilo ...
      pthread_exit(0); // Fin del hilo
  }
  ```

- **Crear un hilo:**
  ```c
  pthread_t mi_hilo;
  // Dirección del ID, NULL, nombre de la función, argumento a pasar (NULL)
  pthread_create(&mi_hilo, NULL, mi_funcion, NULL);
  ```

- **Esperar a que termine un hilo (`join`):**
  ```c
  // Nombre del ID, NULL
  pthread_join(mi_hilo, NULL);
  ```

---

## 6. Sincronización de Hilos (Mutex)

**Librerías:**
```c
#include <pthread.h>
```

> Se usan para evitar condiciones de carrera modificando variables globales.

- **Declarar el candado (Variable Global):**
  ```c
  pthread_mutex_t candado;
  ```

- **Inicializar (En el `main`):**
  ```c
  pthread_mutex_init(&candado, NULL);
  ```

- **Bloquear y Desbloquear (En la función del hilo):**
  ```c
  pthread_mutex_lock(&candado);   // Cierra la puerta
  variable_global++;              // SECCIÓN CRÍTICA
  pthread_mutex_unlock(&candado); // Abre la puerta
  ```

- **Destruir el Mutex (En el `main`, al final):**
  ```c
  pthread_mutex_destroy(&candado);
  ```

---

## 7. Sincronización de Procesos (Semáforos con nombre)

**Librerías:**
```c
#include <semaphore.h>
#include <fcntl.h>
```

> Se usan para sincronizar procesos separados, generalmente combinados con `mmap`.

- **Declarar y Crear/Abrir el semáforo (Antes del `fork`):**
  ```c
  sem_t *semaforo;
  // Nombre (con /), O_CREAT, permisos, valor inicial (1 = abierto)
  semaforo = sem_open("/semaforo_examen", O_CREAT, S_IRUSR | S_IWUSR, 1);
  ```

- **Bloquear (Semáforo a rojo) y Desbloquear (Semáforo a verde):**
  ```c
  sem_wait(semaforo);   // Espero mi turno y pongo en rojo
  *saldo += 100;        // SECCIÓN CRÍTICA (memoria compartida)
  sem_post(semaforo);   // He terminado, pongo en verde
  ```

- **Cerrar y Desvincular (En el padre, al final):**
  ```c
  sem_close(semaforo);
  sem_unlink("/semaforo_examen"); // IMPORTANTE: Borra el semáforo del sistema
  ```

---

## 8. Señales (Interrupciones / Sincronización asíncrona)

**Librerías:**
```c
#include <signal.h>
#include <unistd.h>
```

- **Crear la función manejadora:**
  ```c
  void manejador(int sig) {
      printf("Señal %d recibida!\n", sig);
  }
  ```

- **Enganchar la señal a la función (Ej: `SIGUSR1`):**
  ```c
  signal(SIGUSR1, manejador);
  ```

- **Versión avanzada con `sigaction` (por si lo pide estricto):**
  ```c
  volatile sig_atomic_t contador_señal = 0;

  void manejador (int sig){
    contador_señal++;
  }
  int main(void) {
    struct sigaction sa;
    sa.sa_handler = manejador;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGUSR1, &sa, NULL) == -1) {
      perror("Error al registrar sigaction para SIGUSR1");
      exit(EXIT_FAILURE);
    } 
  }
  ```

- **Quedarse dormido esperando una señal:**
  ```c
  pause(); // Bloquea el proceso hasta que llegue cualquier señal
  ```

- **Mandar una señal a otro proceso (El despertador):**
  ```c
  kill(pid_destino, SIGUSR1); // Envía SIGUSR1 al proceso indicado
  ```

- **Retraso temporal:**
  ```c
  sleep(2); // Duerme la ejecución 2 segundos exactos
  ```