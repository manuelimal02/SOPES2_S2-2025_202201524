#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/string.h>

#define LONGITUD_MAXIMA_RUTA 256
#define LONGITUD_MAXIMA_PALABRA_CLAVE 64
#define ARCHIVOS_LOG_MAXIMOS 10
#define TAMANO_BUFFER 4096
#define TAMANO_LINEA 512

struct observador_log {
    int identificador;
    struct task_struct *hilo;
    char rutas_log[ARCHIVOS_LOG_MAXIMOS][LONGITUD_MAXIMA_RUTA];
    int numero_rutas;
    char log_central[LONGITUD_MAXIMA_RUTA];
    char palabra_clave[LONGITUD_MAXIMA_PALABRA_CLAVE];
    bool debe_detenerse;
    struct list_head lista;
    loff_t posiciones_archivo[ARCHIVOS_LOG_MAXIMOS];
};

static LIST_HEAD(lista_observadores);
static DEFINE_SPINLOCK(bloqueo_observador);
static int siguiente_identificador_observador = 1;

// Esta función se encarga de escribir una coincidencia encontrada en un archivo de log al archivo log centralizado.
// Toma la ruta del log central, la fuente del archivo, la palabra clave buscada y la línea que contiene la coincidencia.
static int escribir_en_log_central(const char *ruta_central, const char *archivo_fuente, const char *palabra_clave, const char *linea)
{
    struct file *archivo;
    char *buffer;
    int longitud;
    loff_t pos = 0;
    
    archivo = filp_open(ruta_central, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(archivo)) {
        printk(KERN_ERR "Error En El Archivo Central.log: %ld\n", PTR_ERR(archivo));
        return PTR_ERR(archivo);
    }
    
    buffer = kmalloc(1024, GFP_KERNEL);
    if (!buffer) {
        filp_close(archivo, NULL);
        return -ENOMEM;
    }
    
    longitud = snprintf(buffer, 1024, "--- Archivo Encontrado: %s | Palabra Clave: %s | Línea Completa: %s\n",archivo_fuente, palabra_clave, linea);
    
    pos = archivo->f_pos;
    kernel_write(archivo, buffer, longitud, &pos);
    archivo->f_pos = pos;
    
    kfree(buffer);
    filp_close(archivo, NULL);
    
    printk(KERN_INFO "Concidencia Escrita En El Archivo Central.log: %s\n", linea);
    return 0;
}

// Esta función lee un archivo de log desde la última posición conocida y busca la palabra clave especificada.
// Compara el tamaño actual del archivo con la última posición leída para detectar contenido nuevo, lee el archivo en bloques.
static int buscar_palabra_clave(const char *ruta_archivo, const char *palabra_clave, const char *log_central, loff_t *ultima_posicion)
{
    struct file *archivo;
    char *buffer;
    char *buffer_linea;
    int coincidencias = 0;
    loff_t posicion_archivo = *ultima_posicion;
    ssize_t bytes_leidos;
    int i, inicio_linea = 0;
    
    archivo = filp_open(ruta_archivo, O_RDONLY, 0);
    if (IS_ERR(archivo)) {
        if (PTR_ERR(archivo) != -ENOENT)
            printk(KERN_WARNING "Error Abriendo Archivo %s: %ld\n", ruta_archivo, PTR_ERR(archivo));
        return 0;
    }
    
    loff_t tamano_archivo = i_size_read(file_inode(archivo));
    if (posicion_archivo >= tamano_archivo) {
        filp_close(archivo, NULL);
        return 0; 
    }
    
    buffer = kmalloc(TAMANO_BUFFER, GFP_KERNEL);
    buffer_linea = kmalloc(TAMANO_LINEA, GFP_KERNEL);
    if (!buffer || !buffer_linea) {
        kfree(buffer);
        kfree(buffer_linea);
        filp_close(archivo, NULL);
        return 0;
    }
    
    while (posicion_archivo < tamano_archivo) {
        bytes_leidos = kernel_read(archivo, buffer, TAMANO_BUFFER, &posicion_archivo);
        if (bytes_leidos <= 0)
            break;
            
        for (i = 0; i < bytes_leidos; i++) {
            if (buffer[i] == '\n' || (inicio_linea >= TAMANO_LINEA - 1)) {
                buffer_linea[inicio_linea] = '\0';
                
                if (inicio_linea > 0 && strstr(buffer_linea, palabra_clave)) {
                    escribir_en_log_central(log_central, ruta_archivo, palabra_clave, buffer_linea);
                    coincidencias++;
                }
                
                inicio_linea = 0;
            } else if (buffer[i] != '\r') { 
                buffer_linea[inicio_linea++] = buffer[i];
            }
        }
    }
    
    *ultima_posicion = posicion_archivo;
    
    kfree(buffer);
    kfree(buffer_linea);
    filp_close(archivo, NULL);
    
    return coincidencias;
}

// Esta función representa el hilo que monitorea continuamente los archivos de log especificados.
// Inicializa las posiciones de los archivos, luego entra en un bucle que verifica cada archivo en busca de la palabra. 
static int hilo_monitor_log(void *data)
{
    struct observador_log *observador = (struct observador_log *)data;
    int i;
    
    printk(KERN_INFO "Iniciando Monitor, Numero ID: %d\n", observador->identificador);
    
    for (i = 0; i < observador->numero_rutas; i++) {
        struct file *archivo = filp_open(observador->rutas_log[i], O_RDONLY, 0);
        if (!IS_ERR(archivo)) {
            observador->posiciones_archivo[i] = i_size_read(file_inode(archivo));
            filp_close(archivo, NULL);
        } else {
            observador->posiciones_archivo[i] = 0;
        }
    }
    
    while (!observador->debe_detenerse && !kthread_should_stop()) {
        for (i = 0; i < observador->numero_rutas; i++) {
            if (observador->debe_detenerse)
                break;
            buscar_palabra_clave(observador->rutas_log[i], observador->palabra_clave, observador->log_central,&observador->posiciones_archivo[i]);
        }
        if (msleep_interruptible(2000) != 0)
            break;
    }
    
    printk(KERN_INFO "Deteniendo Monitor, Numero ID: %d\n", observador->identificador);
    return 0;
}

// Syscall para iniciar monitoreo
SYSCALL_DEFINE4(start_log_watch, const char __user * const __user *, rutas_log,int, numero_rutas, const char __user *, log_central, const char __user *, palabra_clave)
{
    struct observador_log *observador;
    unsigned long flags;
    int i, resultado;
    
    if (!rutas_log || !log_central || !palabra_clave || numero_rutas <= 0 || numero_rutas > ARCHIVOS_LOG_MAXIMOS)
        return -EINVAL;
    
    observador = kzalloc(sizeof(struct observador_log), GFP_KERNEL);
    if (!observador)
        return -ENOMEM;
    
    for (i = 0; i < numero_rutas; i++) {
        const char __user *ruta;
        if (get_user(ruta, &rutas_log[i])) {
            kfree(observador);
            return -EFAULT;
        }
        
        if (strncpy_from_user(observador->rutas_log[i], ruta, LONGITUD_MAXIMA_RUTA - 1) < 0) {
            kfree(observador);
            return -EFAULT;
        }
        observador->rutas_log[i][LONGITUD_MAXIMA_RUTA - 1] = '\0';
    }
    
    if (strncpy_from_user(observador->log_central, log_central, LONGITUD_MAXIMA_RUTA - 1) < 0) {
        kfree(observador);
        return -EFAULT;
    }
    observador->log_central[LONGITUD_MAXIMA_RUTA - 1] = '\0';
    
    if (strncpy_from_user(observador->palabra_clave, palabra_clave, LONGITUD_MAXIMA_PALABRA_CLAVE - 1) < 0) {
        kfree(observador);
        return -EFAULT;
    }
    observador->palabra_clave[LONGITUD_MAXIMA_PALABRA_CLAVE - 1] = '\0';
    
    observador->numero_rutas = numero_rutas;
    observador->debe_detenerse = false;
    
    spin_lock_irqsave(&bloqueo_observador, flags);
    observador->identificador = siguiente_identificador_observador++;
    list_add_tail(&observador->lista, &lista_observadores);
    spin_unlock_irqrestore(&bloqueo_observador, flags);
    
    observador->hilo = kthread_run(hilo_monitor_log, observador, "hilo_monitor_%d", observador->identificador);
    if (IS_ERR(observador->hilo)) {
        spin_lock_irqsave(&bloqueo_observador, flags);
        list_del(&observador->lista);
        spin_unlock_irqrestore(&bloqueo_observador, flags);
        
        resultado = PTR_ERR(observador->hilo);
        kfree(observador);
        return resultado;
    }
    
    printk(KERN_INFO "SYS Monitor Iniciado. ID: %d. Central.log: %s\n.", observador->identificador, observador->log_central);
    return observador->identificador;
}

// Syscall para detener monitoreo
SYSCALL_DEFINE1(stop_log_watch, int, identificador)
{
    struct observador_log *observador = NULL, *temporal;
    unsigned long flags;
    bool encontrado = false;
    
    spin_lock_irqsave(&bloqueo_observador, flags);
    list_for_each_entry_safe(observador, temporal, &lista_observadores, lista) {
        if (observador->identificador == identificador) {
            list_del(&observador->lista);
            encontrado = true;
            break;
        }
    }
    spin_unlock_irqrestore(&bloqueo_observador, flags);
    
    if (!encontrado)
        return -ENOENT;

    observador->debe_detenerse = true;
    if (observador->hilo && !IS_ERR(observador->hilo)) {
        kthread_stop(observador->hilo);
    }
    
    kfree(observador);
    
    printk(KERN_INFO "SYS Monitor Detenido. ID: %d)\n", identificador);
    return 0;
}