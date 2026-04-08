#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/mman.h>

static int *variable_compartida;

int main () {

	pid_t hijo;
	variable_compartida = mmap(NULL, sizeof(*variable_compartida), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	puts("Dame un valor para la variable");
	scanf("%d", &*variable_compartida);

	hijo = fork();

	if (hijo == 0) {

		puts("Dame un valor para la variable");
		scanf("%d", &*variable_compartida);
		munmap(variable_compartida, sizeof(*variable_compartida));
		exit(0);

	} else if (hijo > 0) {

		puts("Soy el proceso padre, quedo a la espera de que el hijo haga su trabajo");
		wait(NULL);
		printf("Soy el padre, y el hijo ha cambiado el valor de la variable a: %d\n", *variable_compartida);
		munmap(variable_compartida, sizeof(*variable_compartida));
	}
	return 0;
}
