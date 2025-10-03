#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_ARCHIVOS 10
#define TAMANO_LINEA 200

// Prototipo de la función.
void generar_cadena_aleatoria(char *cadena, int longitud);

// Estructura para pasar datos a los hilos.
struct DatosHilo {
    char nombre_archivo[20];
};

// Arreglo de nombres de archivos y palabras clave.
char *archivos_log[NUM_ARCHIVOS];
const char *palabras_clave_comunes[] = {
    "error", "sistema", "proceso", "informacion", "advertencia"
};

// Función que ejecuta cada hilo y escribe en su log.
void *escribir_en_log(void *arg) {
    struct DatosHilo *datos = (struct DatosHilo *)arg;
    char linea[TAMANO_LINEA];
    char palabra1[20], palabra2[20];
    char fecha[30];
    time_t tiempo;
    int indice_palabra;

    while (1) {
        generar_cadena_aleatoria(palabra1, 8);
        generar_cadena_aleatoria(palabra2, 8);

        indice_palabra = rand() % 5;

        time(&tiempo);
        strftime(fecha, sizeof(fecha), "%a %b %d %H:%M:%S %Y", localtime(&tiempo));

        snprintf(linea, TAMANO_LINEA, "[%s] Este es un %s %s %s.\n",fecha,palabra1, palabras_clave_comunes[indice_palabra], palabra2);

        FILE *archivo = fopen(datos->nombre_archivo, "a");
        if (archivo) {
            fputs(linea, archivo);
            fclose(archivo);
        }

        sleep(1);
    }
    return NULL;
}

// Función para generar una cadena aleatoria (en buffer local).
void generar_cadena_aleatoria(char *cadena, int longitud) {
    for (int i = 0; i < longitud; i++) {
        cadena[i] = 'a' + (rand() % 26);
    }
    cadena[longitud] = '\0';
}
// Inicializa y crea múltiples hilos para generar logs aleatorios en archivos distintos.
// Configura los nombres de los archivos y mantiene el programa en ejecucións.
int main() {
    pthread_t hilos[NUM_ARCHIVOS];
    struct DatosHilo datos_hilo[NUM_ARCHIVOS];

    srand(time(NULL));

    for (int i = 0; i < NUM_ARCHIVOS; i++) {
        archivos_log[i] = malloc(20);
        snprintf(archivos_log[i], 20, "log_%d.log", i);

        FILE *archivo = fopen(archivos_log[i], "w");
        if (archivo) fclose(archivo);

        strcpy(datos_hilo[i].nombre_archivo, archivos_log[i]);

        if (pthread_create(&hilos[i], NULL, escribir_en_log, &datos_hilo[i]) != 0) {
            perror("Error creando hilo");
            exit(1);
        }
    }

    printf("Generando logs... Presiona Ctrl+C Para Detener.\n");
    while (1) {
        sleep(1);
    }

    for (int i = 0; i < NUM_ARCHIVOS; i++) {
        free(archivos_log[i]);
    }

    return 0;
}
// gcc -o GenerarArchivos GenerarArchivos.c -lpthread
// ./GenerarArchivos 