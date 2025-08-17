#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_SEND_KEY_EVENT 549

int main(int argc, char *argv[]) {
    int keycode = strtol(argv[1], NULL, 10);
    long result = syscall(SYS_SEND_KEY_EVENT, keycode);

    if (result == 0) {
        printf("Enviando tecla: keycode=%d exitosamente.\n", keycode);
    } else {
        fprintf(stderr, "Error\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// gcc -o test_teclado test_teclado.c