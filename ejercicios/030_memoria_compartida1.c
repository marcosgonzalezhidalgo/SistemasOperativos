#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>  
#include <sys/wait.h>   
#include <unistd.h>
#include <fcntl.h>
     
#define LIMITE 1000000

int main(void)
{
	pid_t hijo;
	
    int *variable_compartida = mmap(NULL, sizeof(int),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,-1,0);
    *variable_compartida=0;
    
    hijo=fork();
    
    if (hijo==0)
   {
	  for (int i=0;i<LIMITE;i++)
	{
		*variable_compartida=*variable_compartida+1;
		
	}

	   exit(0);
   }
   else
   {
	   for (int i=0;i<LIMITE;i++)
	   {
		*variable_compartida=*variable_compartida-1;
		
		}
	   /**/
   }
    wait (NULL);
    printf("Soy el proceso padre, y la variable compartida vale %d\n", *variable_compartida);
    return 0;
}
