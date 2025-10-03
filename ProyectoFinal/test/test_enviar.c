#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_IPC_CHANNEL_SEND 551

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <mensaje_usuario>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *mensaje = argv[1];
    size_t longitud = strlen(mensaje) + 1;
    long resultado = syscall(SYS_IPC_CHANNEL_SEND, mensaje, longitud);

    if (resultado == 0) {
        printf("Mensaje Enviado: %s\n", mensaje);
    } else {
        fprintf(stderr, "Error Al Enviar Mensaje: %s\n", strerror(-resultado));
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// gcc -o test_enviar test_enviar.c