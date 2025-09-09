#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h>  
#include <errno.h>

#define SYS_GET_SCREEN_RESOLUTION 550  

int main(int argc, char *argv[]) {
    int width, height;
    long result;
    result = syscall(SYS_GET_SCREEN_RESOLUTION, &width, &height);

    if (result == 0) {
        printf("Resolucion obtenida: %dx%d exitosamente.\n", width, height);
    } else {
        fprintf(stderr, "Error\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
// gcc -o test_resolucion test_resolucion.c