#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYS_GET_SCREEN_RESOLUTION 550
#define SYS_CAPTURE_SCREEN 555

int main(int argc, char *argv[]) {
    struct datos_captura_pantalla {
        int ancho;
        int alto;
        int bits_pixel;
        char *buffer_datos;
        size_t tamano_buffer;
    } captura;

    int ancho_res, alto_res;
    long resultado_res = syscall(SYS_GET_SCREEN_RESOLUTION, &ancho_res, &alto_res);
    
    if (resultado_res != 0) {
        fprintf(stderr, "Error Al Obtener Resolucion: %s\n", strerror(-resultado_res));
        exit(EXIT_FAILURE);
    }

    printf("Resolucion Detectada: %dx%d\n", ancho_res, alto_res);

    size_t tamano_buffer = (size_t)ancho_res * alto_res * 4;
    captura.buffer_datos = malloc(tamano_buffer);
    if (!captura.buffer_datos) {
        perror("Error Al Asignar Memoria");
        exit(EXIT_FAILURE);
    }
    // Configurar estructura
    captura.ancho = ancho_res;
    captura.alto = alto_res;
    captura.bits_pixel = 32;
    captura.tamano_buffer = tamano_buffer;
    // Llamar a la syscall de captura
    long resultado = syscall(SYS_CAPTURE_SCREEN, &captura);
    if (resultado == 0) {
        printf("Captura De Pantalla Exitosa: %dx%d @ %d bpp\n", captura.ancho, captura.alto, captura.bits_pixel);
        
        FILE *archivo = fopen("captura.ppm", "wb");
        if (archivo) {
            fprintf(archivo, "P6\n%d %d\n255\n", captura.ancho, captura.alto);
            size_t rgb_size = tamano_buffer * 3 / 4;
            unsigned char *rgb = malloc(rgb_size);
            if (rgb) {
                for (size_t i = 0; i < tamano_buffer; i += 4) {
                    size_t idx = (i / 4) * 3;
                    if (idx + 2 < rgb_size) {
                        rgb[idx] = captura.buffer_datos[i];     // R
                        rgb[idx + 1] = captura.buffer_datos[i + 1]; // G
                        rgb[idx + 2] = captura.buffer_datos[i + 2]; // B
                    }
                }
                fwrite(rgb, 1, rgb_size, archivo);
                free(rgb);
            }
            fclose(archivo);
            printf("Imagen Guardada: captura.ppm\n");
        }
        
        free(captura.buffer_datos);
    } else {
        fprintf(stderr, "Error Al Capturar Pantalla: %s\n", strerror(-resultado));
        if (captura.buffer_datos) free(captura.buffer_datos);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

// gcc -o test_captura test_captura.c
// ./test_captura