// Padre crea un hijo. el padre una vez creado el hijo, el padre lee un numero entero, 
// cuando el padre lee el numero, el hijo va a calcular su factorial, y el padre una vez el 
// hijo ha calculado el factorial, saca el valor por pantalla.

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
	*variable_compartida = 0;

	hijo = fork();

	if (hijo == 0) //hijo
	{ 
		long unsigned int factorial = 1, i;
		while (*variable_compartida==0)
		{
			usleep(2);
		}
		for (i = 1; i <= *variable_compartida; i++)
		{
			factorial = factorial*i;
		}
		*variable_compartida = factorial;
		exit(0);
		

	} 
	
	else if (hijo > 0) //padre
	{ 

		puts("Introduce un numero para calcular su factorial: ");
		scanf("%lu", &*variable_compartida);
		wait(NULL);
		printf("El factorial es: %lu\n", *variable_compartida);

	}
	return 0;
}
