#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_MOVE_MOUSE 548

int main(int argc, char *argv[]) {
    int dx = strtol(argv[1], NULL, 10);
    int dy = strtol(argv[2], NULL, 10);
    long result = syscall(SYS_MOVE_MOUSE, dx, dy);

    if (result == 0) {
        printf("Movimiendo el mouse: dx=%d, dy=%d exitosamente\n", dx, dy);
    } else {
        fprintf(stderr, "Error\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// gcc -o test_mouse test_mouse.c