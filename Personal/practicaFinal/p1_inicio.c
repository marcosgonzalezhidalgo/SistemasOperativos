#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

  pid_t hijo = fork();

  if (hijo == 0) {

    puts("Soy el hijo, he nacido");
    sleep(2);
    exit(0);

  } else {
    puts("Soy el padre, esperando a mi hijo...");
    wait(NULL);
    puts("Soy el padre, mi hijo ha muerto");
  }

  return 0;
}