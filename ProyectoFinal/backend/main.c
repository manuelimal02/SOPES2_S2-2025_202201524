#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/syscall.h>
#include <microhttpd.h>

#define PUERTO 8081
#define SYS_SEND_KEY_EVENT 549
#define SYS_GET_SCREEN_RESOLUTION 550
#define SYS_CAPTURE_SCREEN 555
#define SYS_CPU_PERCENTAGE 556
#define SYS_RAM_PERCENTAGE 557
#define SYS_CONTROL_MOUSE_CLICK 558

// ESTRUCTURA PARA CPU
struct datos_cpu {
    int porcentaje_usado;
    int porcentaje_no_usado;
};

// ESTRUCTURA PARA RAM
struct datos_ram {
    int porcentaje_usado;
    int porcentaje_no_usado;
};

// ESTRUCTURA PARA CAPTURA DE PANTALLA
struct captura_pantalla {
    int ancho;
    int alto;
    int bits_pixel;
    char *datos_buffer;
    size_t tamano_buffer;
};

// ESTRUCTURA PARA DATOS DE POST
struct info_conexion {
    char *datos;
    size_t tamano;
};

// FUNCIÓN PARA OBTENER USO DE CPU
int obtener_datos_cpu(struct datos_cpu *info) {
    return syscall(SYS_CPU_PERCENTAGE, info) == 0 ? 0 : -1;
}

// FUNCIÓN PARA OBTENER USO DE RAM
int obtener_datos_ram(struct datos_ram *info) {
    return syscall(SYS_RAM_PERCENTAGE, info) == 0 ? 0 : -1;
}

// FUNCIÓN PARA CAPTURAR PANTALLA
int realizar_captura_pantalla(struct captura_pantalla *captura) {
    return syscall(SYS_CAPTURE_SCREEN, captura) == 0 ? 0 : -1;
}

// FUNCIÓN PARA CONTROLAR CLIC DEL RATÓN
int controlar_clic_raton(int x, int y, int boton) {
    int clic = (boton == 0) ? 1 : 2;
    return syscall(SYS_CONTROL_MOUSE_CLICK, x, y, clic);
}

// FUNCIÓN PARA CONVERTIR A FORMATO PPM
char *convertir_a_ppm(int ancho, int alto, const unsigned char *buffer, size_t *tamano_ppm) {
    size_t tamano_encabezado = snprintf(NULL, 0, "P6\n%d %d\n255\n", ancho, alto);
    size_t tamano_total = tamano_encabezado + ancho * alto * 3;
    char *datos_ppm = malloc(tamano_total);
    if (!datos_ppm) return NULL;
    snprintf(datos_ppm, tamano_encabezado + 1, "P6\n%d %d\n255\n", ancho, alto);
    unsigned char *ptr = (unsigned char *)(datos_ppm + tamano_encabezado);
    for (int i = 0; i < ancho * alto; i++) {
        ptr[i * 3 + 0] = buffer[i * 4 + 2]; // R
        ptr[i * 3 + 1] = buffer[i * 4 + 1]; // G
        ptr[i * 3 + 2] = buffer[i * 4 + 0]; // B
    }
    *tamano_ppm = tamano_total;
    return datos_ppm;
}

// FUNCIÓN PARA AGREGAR HEADERS CORS
void agregar_encabezados_cors(struct MHD_Response *respuesta) {
    MHD_add_response_header(respuesta, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(respuesta, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(respuesta, "Access-Control-Allow-Headers", "Content-Type");
}

// CALLBACK DE LIMPIEZA
static void solicitud_completada(void *cls, struct MHD_Connection *conexion,
                                void **con_cls, enum MHD_RequestTerminationCode toe) {
    (void)cls; (void)conexion; (void)toe;
    struct info_conexion *info_con = *con_cls;
    
    if (info_con) {
        if (info_con->datos) free(info_con->datos);
        free(info_con);
        *con_cls = NULL;
    }
}

// MANEJADOR HTTP
static enum MHD_Result manejar_solicitud(void *cls, struct MHD_Connection *conexion,
                 const char *url, const char *metodo, const char *version,
                 const char *datos_subidos, size_t *tamano_datos, void **ptr) {
    struct MHD_Response *respuesta;
    enum MHD_Result ret;
    char buffer[512];
    (void)cls; (void)version;

    // Manejar OPTIONS (CORS preflight)
    if (strcmp(metodo, "OPTIONS") == 0) {
        respuesta = MHD_create_response_from_buffer(0, "", MHD_RESPMEM_PERSISTENT);
        agregar_encabezados_cors(respuesta);
        ret = MHD_queue_response(conexion, MHD_HTTP_OK, respuesta);
        MHD_destroy_response(respuesta);
        return ret;
    }

    // Inicializar info de conexión para POST
    struct info_conexion *info_con = *ptr;
    if (!info_con) {
        info_con = malloc(sizeof(struct info_conexion));
        if (!info_con) return MHD_NO;
        info_con->datos = NULL;
        info_con->tamano = 0;
        *ptr = info_con;
        return MHD_YES;
    }
    // Procesar datos POST
    if (strcmp(metodo, "POST") == 0 && *tamano_datos > 0) {
        char *nuevo_dato = realloc(info_con->datos, info_con->tamano + *tamano_datos + 1);
        if (!nuevo_dato) return MHD_NO;
        info_con->datos = nuevo_dato;
        memcpy(info_con->datos + info_con->tamano, datos_subidos, *tamano_datos);
        info_con->tamano += *tamano_datos;
        info_con->datos[info_con->tamano] = '\0';
        *tamano_datos = 0;
        return MHD_YES;
    }
    // ENDPOINT: /cpu
    if (strcmp(url, "/cpu") == 0) {
        struct datos_cpu info;
        if (obtener_datos_cpu(&info) == 0) {
            snprintf(buffer, sizeof(buffer),
                    "{\"porcentaje_usado\": %d, \"porcentaje_no_usado\": %d}",
                    info.porcentaje_usado, info.porcentaje_no_usado);
        } else snprintf(buffer, sizeof(buffer), "{\"error\": \"No se pudo obtener CPU\"}");
    }
    // ENDPOINT: /ram
    else if (strcmp(url, "/ram") == 0) {
        struct datos_ram info;
        if (obtener_datos_ram(&info) == 0) {
            snprintf(buffer, sizeof(buffer),
                    "{\"porcentaje_usado\": %d, \"porcentaje_no_usado\": %d}",
                    info.porcentaje_usado, info.porcentaje_no_usado);
        } else snprintf(buffer, sizeof(buffer), "{\"error\": \"No se pudo obtener RAM\"}");
    }
    // ENDPOINT: /resolucion
    else if (strcmp(url, "/resolucion") == 0) {
        int ancho, alto;
        if (syscall(SYS_GET_SCREEN_RESOLUTION, &ancho, &alto) == 0) {
            snprintf(buffer, sizeof(buffer),
                    "{\"ancho\": %d, \"alto\": %d}",
                    ancho, alto);
        } else {
            snprintf(buffer, sizeof(buffer), "{\"error\": \"Fallo al obtener resolución\"}");
        }
    }
    // ENDPOINT: /pantalla
    else if (strcmp(url, "/pantalla") == 0) {
        int ancho, alto;
        if (syscall(SYS_GET_SCREEN_RESOLUTION, &ancho, &alto) != 0) {
            snprintf(buffer, sizeof(buffer), "{\"error\": \"Fallo al obtener resolución\"}");
        } else {
            struct captura_pantalla captura = {0};
            captura.ancho = ancho;
            captura.alto = alto;
            captura.bits_pixel = 32;
            captura.tamano_buffer = (size_t)ancho * alto * 4;
            captura.datos_buffer = malloc(captura.tamano_buffer);

            if (!captura.datos_buffer) {
                snprintf(buffer, sizeof(buffer), "{\"error\": \"Sin memoria\"}");
            } else if (realizar_captura_pantalla(&captura) == 0) {
                size_t tamano_ppm;
                char *datos_ppm = convertir_a_ppm(captura.ancho, captura.alto, 
                                                  (unsigned char *)captura.datos_buffer, &tamano_ppm);
                if (datos_ppm) {
                    respuesta = MHD_create_response_from_buffer(tamano_ppm, datos_ppm, MHD_RESPMEM_MUST_FREE);
                    MHD_add_response_header(respuesta, "Content-Type", "image/x-portable-pixmap");
                    agregar_encabezados_cors(respuesta);
                    ret = MHD_queue_response(conexion, MHD_HTTP_OK, respuesta);
                    MHD_destroy_response(respuesta);
                    free(captura.datos_buffer);
                    return ret;
                } else snprintf(buffer, sizeof(buffer), "{\"error\": \"Error al generar PPM\"}");
            } else snprintf(buffer, sizeof(buffer), "{\"error\": \"Fallo al capturar pantalla\"}");
            free(captura.datos_buffer);
        }
    }
    // ENDPOINT: /mouse
    else if (strcmp(url, "/mouse") == 0 && strcmp(metodo, "POST") == 0) {
        if (info_con->datos && info_con->tamano > 0) {
            int x = -1, y = -1, boton = -1;
            char *x_ptr = strstr(info_con->datos, "\"x\"");
            char *y_ptr = strstr(info_con->datos, "\"y\"");
            char *boton_ptr = strstr(info_con->datos, "\"boton\"");
            
            if (x_ptr) sscanf(x_ptr, "\"x\":%d", &x);
            if (y_ptr) sscanf(y_ptr, "\"y\":%d", &y);
            if (boton_ptr) sscanf(boton_ptr, "\"boton\":%d", &boton);

            if (x < 0 || y < 0 || boton < 0 || boton > 1) {
                snprintf(buffer, sizeof(buffer), 
                        "{\"error\": \"Parámetros inválidos\", \"x\": %d, \"y\": %d, \"boton\": %d}", 
                        x, y, boton);
            } else {
                int resultado = controlar_clic_raton(x, y, boton);
                if (resultado == 0) {
                    snprintf(buffer, sizeof(buffer), 
                            "{\"estado\": \"exito\", \"x\": %d, \"y\": %d, \"boton\": %d}", 
                            x, y, boton);
                } else {
                    snprintf(buffer, sizeof(buffer), 
                            "{\"error\": \"Fallo al controlar ratón\", \"codigo\": %d}", 
                            resultado);
                }
            }
        } else {
            snprintf(buffer, sizeof(buffer), "{\"error\": \"Sin datos en la petición\"}");
        }
    }
    // ENDPOINT: /teclado
    else if (strcmp(url, "/teclado") == 0 && strcmp(metodo, "POST") == 0) {
        if (info_con->datos && info_con->tamano > 0) {
            int codigo_tecla = -1;
            char *tecla_ptr = strstr(info_con->datos, "\"codigo_tecla\"");
            
            if (tecla_ptr) sscanf(tecla_ptr, "\"codigo_tecla\":%d", &codigo_tecla);

            if (codigo_tecla < 0 || codigo_tecla >= KEY_CNT) {
                snprintf(buffer, sizeof(buffer), 
                        "{\"error\": \"Código de tecla inválido\", \"codigo_tecla\": %d}", 
                        codigo_tecla);
            } else {
                int resultado = syscall(SYS_SEND_KEY_EVENT, codigo_tecla);
                if (resultado == 0) {
                    snprintf(buffer, sizeof(buffer), 
                            "{\"estado\": \"exito\", \"codigo_tecla\": %d}", 
                            codigo_tecla);
                } else {
                    snprintf(buffer, sizeof(buffer), 
                            "{\"error\": \"Fallo al enviar evento de tecla\", \"codigo\": %d}", 
                            resultado);
                }
            }
        } else {
            snprintf(buffer, sizeof(buffer), "{\"error\": \"Sin datos en la petición\"}");
        }
    }
    // ENDPOINT raíz
    else if (strcmp(url, "/") == 0) {
        snprintf(buffer, sizeof(buffer), "{\"mensaje\": \"API USACLinux\"}");
    } else {
        snprintf(buffer, sizeof(buffer), "{\"error\": \"Endpoint no encontrado\"}");
    }
    
    respuesta = MHD_create_response_from_buffer(strlen(buffer), (void *)buffer, MHD_RESPMEM_MUST_COPY);
    MHD_add_response_header(respuesta, "Content-Type", "application/json");
    agregar_encabezados_cors(respuesta);
    ret = MHD_queue_response(conexion, MHD_HTTP_OK, respuesta);
    MHD_destroy_response(respuesta);
    return ret;
}

int main() {
    printf("===========================================\n");
    printf("  USACLinux API - Servidor (PPM)\n");
    printf("===========================================\n");
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY,
        PUERTO,
        NULL, NULL,
        &manejar_solicitud, NULL,
        MHD_OPTION_NOTIFY_COMPLETED, solicitud_completada, NULL,
        MHD_OPTION_END
    );
    
    if (!daemon) {
        fprintf(stderr, "[ERROR] No se pudo iniciar el servidor\n");
        return 1;
    }
    printf("[OK] Servidor corriendo en http://localhost:%d\n", PUERTO);
    printf("[INFO] Presiona ENTER para detener...\n");
    getchar();
    MHD_stop_daemon(daemon);
    printf("\n[INFO] Servidor detenido\n");
    return 0;
}

/*
Compilación:
Make clean
Make
Make run
*/