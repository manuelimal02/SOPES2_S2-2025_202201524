#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_IPC_CHANNEL_RECEIVE 552

int main() {
    char buffer[256];
    size_t longitud;
    long resultado = syscall(SYS_IPC_CHANNEL_RECEIVE, buffer, &longitud);

    if (resultado == 0) {
        printf("Mensaje Recibido: %.*s\n", (int)longitud, buffer);
    } else {
        fprintf(stderr, "Error Al Recibir Mensaje: %s\n", strerror(-resultado));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
// gcc -o test_recibir test_recibir.c