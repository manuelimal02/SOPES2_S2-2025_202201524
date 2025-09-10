#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_IPC_CHANNEL_SEND 551

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <mensaje>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *message = argv[1];
    size_t len = strlen(message) + 1;
    long result = syscall(SYS_IPC_CHANNEL_SEND, message, len);

    if (result == 0) {
        printf("Mensaje enviado: %s\n", message);
    } else {
        fprintf(stderr, "Error al enviar mensaje: %s\n", strerror(-result));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// gcc -o test_enviar test_enviar.c