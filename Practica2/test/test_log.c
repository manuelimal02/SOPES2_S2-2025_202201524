#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_START_LOG_WATCH 553
#define SYS_STOP_LOG_WATCH 554

#define RUTA_BASE "/home/manuelima/Documentos/LaboratorioSopes2/Test_P2/Archivos/"
#define RUTA_LOG_CENTRAL "/home/manuelima/Documentos/LaboratorioSopes2/Test_P2/central.log"
#define LONGITUD_MAXIMA_RUTA 256

// Valida el uso y maneja errores
void validar_uso(int cantidad_argumentos, char *argumentos[]) {
    if (cantidad_argumentos < 3) {
        fprintf(stderr, "Uso: %s <iniciar|detener> <argumentos>\n", argumentos[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argumentos[1], "iniciar") != 0 && strcmp(argumentos[1], "detener") != 0) {
        fprintf(stderr, "Uso: %s <iniciar|detener> <argumentos>\n", argumentos[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argumentos[1], "iniciar") == 0 && cantidad_argumentos < 4) {
        fprintf(stderr, "Uso: %s iniciar <archivo1.log> [<archivo2.log> ...] <palabra_clave>\n", argumentos[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argumentos[1], "detener") == 0 && cantidad_argumentos != 3) {
        fprintf(stderr, "Uso: %s detener <identificador>\n", argumentos[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int cantidad_argumentos, char *argumentos[]) {
    validar_uso(cantidad_argumentos, argumentos);

    if (strcmp(argumentos[1], "iniciar") == 0) {
        int numero_rutas = cantidad_argumentos - 3;
        char rutas_completas[numero_rutas][LONGITUD_MAXIMA_RUTA];
        const char *rutas[numero_rutas];
        
        printf("Archivos Log A Monitorear\n");

        for (int i = 0; i < numero_rutas; i++) {
            snprintf(rutas_completas[i], LONGITUD_MAXIMA_RUTA, "%s%s", RUTA_BASE, argumentos[i + 2]);
            rutas_completas[i][LONGITUD_MAXIMA_RUTA - 1] = '\0';
            rutas[i] = rutas_completas[i];
            printf("  %d: %s\n", i + 1, rutas[i]);
        }
        
        const char *log_central = RUTA_LOG_CENTRAL;
        const char *palabra_clave = argumentos[cantidad_argumentos - 1];

        printf("Log Central: %s\n", log_central);
        printf("Palabra Clave: '%s'\n", palabra_clave);
        printf("-----------------------------------\n");

        long resultado = syscall(SYS_START_LOG_WATCH, rutas, numero_rutas, log_central, palabra_clave);
        if (resultado > 0) {
            printf("Monitoreo Iniciado Con ID: %ld.\n", resultado);
        } else {
            fprintf(stderr, "Error Al Iniciar Monitoreo: %s (Codigo: %ld).\n", strerror(-resultado), resultado);
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argumentos[1], "detener") == 0) {
        int identificador = atoi(argumentos[2]);
        long resultado = syscall(SYS_STOP_LOG_WATCH, identificador);
        if (resultado == 0) {
            printf("Monitoreo Detenido Con ID: %d.\n", identificador); // Cambiado %ld por %d
            printf("Verifica Central.log: %s\n", RUTA_LOG_CENTRAL);
        } else {
            fprintf(stderr, "Error Al Detener Monitoreo: %s (Codigo: %ld).\n", strerror(-resultado), resultado);
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
// gcc -o test_log test_log.c
// ./test_log iniciar log_1.log log_2.log error
// ./test_log iniciar log_1.log log_2.log log_3.log sistema