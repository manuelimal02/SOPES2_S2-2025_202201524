#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_IPC_CHANNEL_RECEIVE 552

int main() {
    char buffer[256];
    size_t len;
    long result = syscall(SYS_IPC_CHANNEL_RECEIVE, buffer, &len);

    if (result == 0) {
        printf("Mensaje recibido: %.*s\n", (int)len, buffer);
    } else {
        fprintf(stderr, "Error al recibir mensaje: %s\n", strerror(-result));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
// gcc -o test_recibir test_recibir.c