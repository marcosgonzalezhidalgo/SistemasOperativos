#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

  int status;

  pid_t hijo = fork();

  if (hijo < 0) {

    perror("Ha habido un error al generar el hijo");
    exit(EXIT_FAILURE);

  } else if (hijo == 0) {

    int numero;

    puts("Introduzca un numero: ");
    scanf("%d", &numero);

    if ((numero % 2) == 0) {
      exit(0);
    } else {
      exit(1);
    }

  } else {
    wait(&status);

    if (WIFEXITED(status)) {

      int codigo = WEXITSTATUS(status);
      if (codigo == 0) {
        printf("El hijo dice: el numero es PAR, con codigo %d\n", codigo);
      } else if (codigo == 1) {
        printf("El hijo dice: el numero es IMPAR, con codigo %d\n", codigo);
      }

    } else {
      printf("El hijo no terminó de forma normal.\n");
    }
  }

  return 0;
}
