#include <unistd.h>
#include <string.h>

int main() {
    char nombre[50];
    char edad[5];
    ssize_t bytes;

    char msg_nombre[] = "Introduce tu nombre: ";
    write(STDOUT_FILENO, msg_nombre, strlen(msg_nombre));

    bytes = read(STDIN_FILENO, nombre, sizeof(nombre));
    if (bytes > 0 && nombre[bytes - 1] == '\n') bytes--; 
    ssize_t len_nombre = bytes;

    char msg_edad[] = "Introduce tu edad: ";
    write(STDOUT_FILENO, msg_edad, strlen(msg_edad));

    bytes = read(STDIN_FILENO, edad, sizeof(edad));
    if (bytes > 0 && edad[bytes - 1] == '\n') bytes--;
    ssize_t len_edad = bytes;

    write(STDOUT_FILENO, "Hola, ", 6);
    write(STDOUT_FILENO, nombre, len_nombre);
    write(STDOUT_FILENO, "! Tienes ", 9);
    write(STDOUT_FILENO, edad, len_edad);
    write(STDOUT_FILENO, " años.\n", 8);

    return 0;
}