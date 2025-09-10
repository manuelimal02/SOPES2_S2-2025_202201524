# Práctica 2: Hilos, Concurrencia y Paralelismo

## Datos Personales

| **Nombre** | Carlos Manuel Lima Y Lima |
| --- | --- |
| **Registro Académico** | 202201524 |
| **Curso** | Sistemas Operativos 2, Sección A |

## Introducción

Esta documentación detalla el proceso seguido para implementar cuatro nuevas llamadas al sistema (syscalls) en el kernel de Linux, versión 6.12.41, como parte de la Práctica 2. El objetivo principal es modificar el kernel para agregar funcionalidades avanzadas relacionadas con la captura de pantalla, la comunicación interprocesos y el monitoreo de archivos de log. Las syscalls implementadas son `sys_capture_screen`, `sys_ipc_channel_send`, `sys_ipc_channel_receive`, `sys_start_log_watch`, y `sys_stop_log_watch`, diseñadas para simular escenarios de escritorio remoto, comunicación segura entre procesos y gestión de logs en tiempo real.

 Las syscalls se implementaron en archivos separados dentro del directorio `kernel/` (`syscall_P2_1.c`, `syscall_P2_2.c`, `syscall_P2_3.c`) para mantener una estructura organizada. Se modificó la tabla de syscalls y el `Makefile` del kernel para integrar las nuevas funcionalidades. Los programas de prueba (`test_enviar.c`, `test_recibir.c`, `test_log.c`, `test_captura.c`) fueron diseñados para ser minimalistas, ejecutando las syscalls y reportando resultados básicos (Éxito o Error) con mensajes de depuración.

## Objetivos De Cada Nueva Syscall

### sys_capture_screen

- **Propósito:** Capturar el contenido de la pantalla en el instante en que fue llamada y devolver los datos necesarios para construir la imagen en el espacio de usuario.
- **Objetivo logrado:**

### sys_ipc_channel_send

- **Propósito:** Enviar un mensaje a una cola de comunicación interprocesos para que otro proceso lo reciba.
- **Objetivo logrado:** Implementar una cola de mensajes en el kernel, gestionada con sincronización para garantizar que los mensajes se entreguen en orden y que solo un proceso reciba cada mensaje, utilizando estructuras de datos seguras y concurrencia.

### sys_ipc_channel_receive

- **Propósito:** Recibir un mensaje de la cola de comunicación interprocesos, bloqueando al proceso si no hay mensajes disponibles.
- **Objetivo logrado:** Proporcionar un mecanismo de recepción con espera activa, asegurando que cada mensaje sea consumido por un solo proceso y manejando la sincronización para evitar condiciones de carrera.

### sys_start_log_watch

- **Propósito:** Iniciar el monitoreo de una lista de archivos `.log` y detectar si alguno contiene una palabra específica, notificando al espacio de usuario.
- **Objetivo logrado:** Se implementó un mecanismo de monitoreo periódico utilizando un hilo en el kernel, garantizando la detección de la palabra clave y la escritura en el log centralizado con identificación del archivo origen.

### **sys_stop_log_watch**

- **Propósito:** Detener el monitoreo de archivos iniciado por `sys_start_log_watch`.
- **Objetivo logrado:** Se proporcionó un mecanismo para finalizar el monitoreo de forma segura, liberando recursos y evitando condiciones de carrera.

## Detalle de los Pasos Seguidos para Modificar el Kernel, Compilarlo e Instalarlo

Los pasos descritos a continuación se siguieron para implementar las cuatro llamadas al sistema (`sys_capture_screen`, `sys_ipc_channel_send`, `sys_ipc_channel_receive`, `sys_start_log_watch`, `sys_stop_log_watch`) en el kernel de Linux, versión 6.12.41. Las modificaciones se integraron en varios procesos de compilación e instalación, realizados en una máquina virtual VMware con Linux Mint, en el directorio `~/Documentos/LaboratorioSopes2/linux-6.12.41`.

### **Agregado de Entradas en la Tabla de Syscalls**

Se modificó el archivo `arch/x86/entry/syscalls/syscall_64.tbl` para registrar las cuatro syscalls con números secuenciales a partir de 551 (evitando conflictos con las syscalls de la Práctica 1):

```bash
550 common get_screen_resolution sys_get_screen_resolution
551 common ipc_channel_send sys_ipc_channel_send
552 common ipc_channel_receive sys_ipc_channel_receive
553 common start_log_watch sys_start_log_watch
554 common stop_log_watch sys_stop_log_watch
555 common capture_screen sys_capture_screen
```

Los números 551, 552, 553, 554 y 555 fueron elegidos para evitar conflictos con syscalls existentes, siguiendo la secuencia de syscalls existentes en la tabla.

### **Creación de Archivos Para las Syscalls**

Se crearon tres archivos en el directorio `kernel/`:

- `syscall_P2_1.c` para `sys_ipc_channel_send` y `sys_ipc_channel_receive`.
- `syscall_P2_2.c` para `sys_start_log_watch` y `sys_stop_log_watch`.
- `syscall_P2_3.c` para `sys_capture_screen`.

Cada archivo contiene la implementación de la respectiva syscall, incluyendo la lógica para interactuar con el framebuffer, gestionar colas de mensajes o monitorear archivos, y mensajes de log con `pr_err` (Error:) y `pr_info` (Info:).

### **Actualización del Makefile**

Se modificó `kernel/Makefile` para incluir los nuevos archivos objeto en la compilación: Fragmento de código (`kernel/Makefile`):

```bash
obj-y = fork.o exec_domain.o panic.o \\
        cpu.o exit.o softirq.o resource.o \\
        sysctl.o capability.o ptrace.o user.o \\
        signal.o sys.o umh.o workqueue.o pid.o task_work.o \\
        extable.o params.o \\
        kthread.o sys_ni.o nsproxy.o \\
        notifier.o ksysfs.o cred.o reboot.o \\
        async.o range.o smpboot.o ucount.o regset.o ksyms_common.o \\
        syscall_usac1.o syscall_usac2.o syscall_usac3.o \\
        syscall_P2_1.o syscall_P2_2.o syscall_P2_3.o

```

### **Compilación del kernel**

Se compiló el kernel utilizando todos los núcleos disponibles para optimizar el tiempo de compilación:

```bash
make -j$(nproc)
```

### **Instalación de Módulos y Kernel**

Se instalaron los módulos del kernel y el kernel compilado:

```bash
sudo make modules_install
sudo make install
```

### **Reinicio y Verificación**

Se reinició la máquina virtual para cargar el nuevo kernel:

```bash
sudo reboot
```

Se verificó que el kernel correcto estuviera en uso:

```bash
uname -r
```

Salida esperada: `6.12.41-carlos-lima-202201524`.

## Implementación de `sys_ipc_channel_send`

La syscall `sys_ipc_channel_send` permite enviar un mensaje desde el espacio de usuario a una cola de comunicación interprocesos (IPC) gestionada por el kernel, facilitando la comunicación segura entre procesos. Esta implementación se encuentra en el archivo `kernel/syscall_P2_1.c` y opera en un entorno virtualizado con VMware, utilizando sincronización para garantizar la integridad de la cola.

### Fragmento de Código Relevante

```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>

struct mensaje_ipc {
    struct list_head nodo_lista;
    char *contenido;
    size_t longitud;
};

static struct list_head cola_mensajes = LIST_HEAD_INIT(cola_mensajes);
static DEFINE_MUTEX(mutex_cola);
static DECLARE_WAIT_QUEUE_HEAD(cola_espera);

SYSCALL_DEFINE2(sys_ipc_channel_send, const void __user *, mensaje_usuario, size_t, longitud_mensaje) {
    struct mensaje_ipc *nuevo_mensaje = kmalloc(sizeof(*nuevo_mensaje), GFP_KERNEL);
    if (!nuevo_mensaje) return -ENOMEM;

    nuevo_mensaje->contenido = kmalloc(longitud_mensaje, GFP_KERNEL);
    if (!nuevo_mensaje->contenido) {
        kfree(nuevo_mensaje);
        return -ENOMEM;
    }

    if (copy_from_user(nuevo_mensaje->contenido, mensaje_usuario, longitud_mensaje)) {
        kfree(nuevo_mensaje->contenido);
        kfree(nuevo_mensaje);
        return -EFAULT;
    }
    nuevo_mensaje->longitud = longitud_mensaje;

    mutex_lock(&mutex_cola);
    list_add_tail(&nuevo_mensaje->nodo_lista, &cola_mensajes);
    mutex_unlock(&mutex_cola);

    wake_up_interruptible(&cola_espera);
    return 0;
}

```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Define una estructura `mensaje_ipc` para almacenar mensajes, con un puntero a datos (`contenido`), longitud (`longitud`) y un nodo de lista enlazada (`nodo_lista`).
- **Funcionamiento:**
    - `cola_mensajes` se inicializa como una lista enlazada vacía usando `LIST_HEAD_INIT`.
    - `mutex_cola` protege la cola contra accesos concurrentes.
    - `cola_espera` es una cola de espera para notificar a procesos bloqueados cuando hay mensajes disponibles.

### Lógica de la Syscall

- **Propósito:** Enviar un mensaje desde el espacio de usuario a la cola del kernel.
- **Funcionamiento:**
    - **Validación de Memoria:** Asigna un `struct mensaje_ipc` con `kmalloc`. Si falla, retorna `ENOMEM`.
    - **Asignación de Datos:** Reserva memoria para `contenido` del tamaño `longitud_mensaje`. Si falla, libera `nuevo_mensaje` y retorna `ENOMEM`.
    - **Copia desde Usuario:** Usa `copy_from_user` para transferir `longitud_mensaje` bytes desde `mensaje_usuario` al buffer `nuevo_mensaje->contenido`. Si falla, libera recursos y retorna `EFAULT`.
    - **Almacenamiento:** Asigna `longitud_mensaje` a `nuevo_mensaje->longitud` y agrega el mensaje a la cola con `list_add_tail` bajo protección de `mutex_cola`.
    - **Notificación:** Despierta procesos en espera con `wake_up_interruptible`.
    - **Retorno:** Retorna `0` en caso de éxito.

## Implementación de `sys_ipc_channel_receive`

La syscall `sys_ipc_channel_receive` permite a un proceso extraer y recibir un mensaje de la cola de comunicación interprocesos (IPC) gestionada por el kernel, bloqueándose si no hay mensajes disponibles. Esta implementación se encuentra en el archivo `kernel/syscall_P2_1.c` y opera en un entorno virtualizado con VMware, utilizando sincronización para asegurar la integridad de la operación.

### Fragmento de Código Relevante

```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>

struct mensaje_ipc {
    struct list_head nodo_lista;
    char *contenido;
    size_t longitud;
};

static struct list_head cola_mensajes = LIST_HEAD_INIT(cola_mensajes);
static DEFINE_MUTEX(mutex_cola);
static DECLARE_WAIT_QUEUE_HEAD(cola_espera);

SYSCALL_DEFINE2(sys_ipc_channel_receive, void __user *, mensaje_usuario, size_t __user *, longitud_usuario) {
    struct mensaje_ipc *mensaje;
    int resultado = 0;

    wait_event_interruptible(cola_espera, !list_empty(&cola_mensajes));

    mutex_lock(&mutex_cola);
    if (list_empty(&cola_mensajes)) {
        resultado = -EAGAIN;
        goto salir;
    }

    mensaje = list_first_entry(&cola_mensajes, struct mensaje_ipc, nodo_lista);
    list_del(&mensaje->nodo_lista);

    if (copy_to_user(longitud_usuario, &mensaje->longitud, sizeof(size_t))) {
        resultado = -EFAULT;
        goto liberar;
    }
    if (copy_to_user(mensaje_usuario, mensaje->contenido, mensaje->longitud)) {
        resultado = -EFAULT;
        goto liberar;
    }

liberar:
    kfree(mensaje->contenido);
    kfree(mensaje);
salir:
    mutex_unlock(&mutex_cola);
    return resultado;
}

```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Define una estructura `mensaje_ipc` para almacenar mensajes, con un puntero a datos (`contenido`), longitud (`longitud`) y un nodo de lista enlazada (`nodo_lista`).
- **Funcionamiento:**
    - `cola_mensajes` se inicializa como una lista enlazada vacía usando `LIST_HEAD_INIT`.
    - `mutex_cola` protege la cola contra accesos concurrentes.
    - `cola_espera` es una cola de espera para bloquear el proceso hasta que haya mensajes disponibles.

### Lógica de la Syscall

- **Propósito:** Recibir un mensaje de la cola y transferirlo al espacio de usuario.
- **Funcionamiento:**
    - **Espera Activa:** Usa `wait_event_interruptible` para bloquear el proceso hasta que `cola_mensajes` no esté vacía, evitando consumo innecesario de CPU.
    - **Acceso a la Cola:** Adquiere `mutex_cola` y verifica si la cola está vacía. Si lo está, retorna `EAGAIN` y sale.
    - **Extracción:** Obtiene el primer mensaje con `list_first_entry` y lo elimina de la cola con `list_del`.
    - **Copia al Usuario:** Transfiere `mensaje->longitud` a `longitud_usuario` y `mensaje->contenido` a `mensaje_usuario` usando `copy_to_user`. Si falla alguna copia, retorna `EFAULT`.
    - **Liberación:** Libera la memoria de `mensaje->contenido` y `mensaje` con `kfree`.
    - **Salida:** Libera `mutex_cola` y retorna `resultado` (0 en caso de éxito, error en caso contrario).

## Implementación de `sys_start_log_watch`

La syscall `sys_start_log_watch` permite iniciar un monitoreo continuo de una lista de archivos `.log` para detectar si alguno contiene una palabra clave específica, registrando las coincidencias en un archivo log centralizado. Esta implementación se encuentra en el archivo `kernel/syscall_P2_2.c` y utiliza un hilo kernel (`kthread`) para realizar el monitoreo periódico, operando en un entorno virtualizado con VMware.

### Fragmento de Código Relevante

```c
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

SYSCALL_DEFINE4(start_log_watch, const char __user * const __user *, rutas_log, int, numero_rutas, const char __user *, log_central, const char __user *, palabra_clave)
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
        observador->rutas_log[i][LONGITUD_MAXIMA_RUTA - 1] = '\\0';
    }

    if (strncpy_from_user(observador->log_central, log_central, LONGITUD_MAXIMA_RUTA - 1) < 0) {
        kfree(observador);
        return -EFAULT;
    }
    observador->log_central[LONGITUD_MAXIMA_RUTA - 1] = '\\0';

    if (strncpy_from_user(observador->palabra_clave, palabra_clave, LONGITUD_MAXIMA_PALABRA_CLAVE - 1) < 0) {
        kfree(observador);
        return -EFAULT;
    }
    observador->palabra_clave[LONGITUD_MAXIMA_PALABRA_CLAVE - 1] = '\\0';

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

    printk(KERN_INFO "SYS Monitor Iniciado. ID: %d. Central.log: %s\\n", observador->identificador, observador->log_central);
    return observador->identificador;
}

```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** La estructura `observador_log` almacena información sobre el monitoreo, incluyendo las rutas de los archivos `.log`, la ruta del log central, la palabra clave, un identificador único, un hilo asociado, y un flag para detener el monitoreo.
- **Funcionamiento:**
    - `lista_observadores` es una lista enlazada global que gestiona todos los observadores activos.
    - `bloqueo_observador` es un spinlock que asegura la concurrencia segura al modificar la lista.
    - `siguiente_identificador_observador` genera identificadores únicos para cada instancia de monitoreo.

**Lógica de la Syscall:**

- **Propósito:** Iniciar un monitoreo de archivos `.log` y devolver un identificador único.
- **Funcionamiento:**
    - **Validación de Entradas:** Verifica que los parámetros (`rutas_log`, `log_central`, `palabra_clave`) no sean nulos y que `numero_rutas` esté dentro de los límites (1 a `ARCHIVOS_LOG_MAXIMOS`).
    - **Asignación de Memoria:** Reserva memoria para un nuevo `observador_log` con `kzalloc`.
    - **Copia de Datos:** Copia las rutas de los archivos, la ruta del log central y la palabra clave desde el espacio de usuario usando `strncpy_from_user`, asegurando terminación nula (`\\0`) para evitar desbordamientos.
    - **Configuración:** Inicializa `numero_rutas`, `debe_detenerse`, y asigna un identificador único bajo protección del spinlock.
    - **Creación del Hilo:** Inicia un hilo con `kthread_run`, pasando `hilo_monitor_log` como función y el observador como dato. Si falla, libera recursos y retorna el error.
    - **Retorno:** Devuelve el identificador del monitoreo en caso de éxito.

**Función `hilo_monitor_log` (Resumen):**

- Monitorea continuamente los archivos, actualizando las posiciones leídas y buscando la palabra clave con `buscar_palabra_clave`, escribiendo coincidencias en el log central con `escribir_en_log_central`. Usa `msleep_interruptible` para pausar 2 segundos entre verificaciones.

## Implementación de `sys_stop_log_watch`

La syscall `sys_stop_log_watch` detiene el monitoreo de archivos iniciado por `sys_start_log_watch`, liberando los recursos asociados. Esta implementación también se encuentra en el archivo `kernel/syscall_P2_2.c` y utiliza sincronización para asegurar una detención segura.

### Fragmento de Código Relevante

```c
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

    printk(KERN_INFO "SYS Monitor Detenido. ID: %d\\n", identificador);
    return 0;
}

```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Utiliza la misma estructura `observador_log` y la lista `lista_observadores` para localizar y gestionar el monitoreo a detener.
- **Funcionamiento:** El spinlock `bloqueo_observador` asegura que la búsqueda y eliminación del observador sean seguras.

**Lógica de la Syscall:**

- **Propósito:** Detener un monitoreo específico y liberar recursos.
- **Funcionamiento:**
    - **Búsqueda:** Usa `list_for_each_entry_safe` bajo protección del spinlock para encontrar el observador con el `identificador` dado y lo elimina de la lista.
    - **Validación:** Si no se encuentra el identificador, retorna `ENOENT`.
    - **Detención:** Marca `debe_detenerse` como verdadero y detiene el hilo con `kthread_stop` si existe y es válido.
    - **Liberación:** Libera la memoria del observador con `kfree`.
    - **Retorno:** Devuelve `0` en caso de éxito, indicando que el monitoreo se detuvo correctamente.

## Reporte de Errores

Trabajar con el kernel de Linux es una tarea compleja que requiere un entendimiento profundo de sus componentes y estructuras, especialmente al implementar nuevas syscalls con hilos, sincronización y acceso a archivos. Durante la realización de la práctica, se encontraron varios problemas relacionados con las syscalls `sys_ipc_channel_send`, `sys_start_log_watch`, y `sys_capture_screen` en `kernel/syscall_P2_1.c`, `kernel/syscall_P2_2.c`, y `kernel/syscall_P2_3.c`. A continuación, se detallan tres errores significativos observados, las fuentes consultadas para resolverlos y las soluciones aplicadas.

## Error 1: Error de Compilación por Uso Incorrecto de `LIST_HEAD_INITIALIZER`

### Información del Error

Se intentó inicializar la lista `msg_queue` en `kernel/syscall_P2_1.c` usando `LIST_HEAD_INITIALIZER`, pero al compilar el kernel con `make -j$(nproc)`, se generó el siguiente error:

```powershell
kernel/syscall_P2_1.c:15:37: error: implicit declaration of function ‘LIST_HEAD_INITIALIZER’; did you mean ‘LIST_HEAD_INIT’? [-Werror=implicit-function-declaration]
   15 | static struct list_head msg_queue = LIST_HEAD_INITIALIZER(msg_queue);
      |                                     ^~~~~~~~~~~~~~~~~~~~~
      |                                     LIST_HEAD_INIT
kernel/syscall_P2_1.c:15:37: error: invalid initializer
cc1: some warnings being treated as errors
make[3]: *** [scripts/Makefile.build:229: kernel/syscall_P2_1.o] Error 1
make[3]: *** Se espera a que terminen otras tareas....

```

Este error indicó que `LIST_HEAD_INITIALIZER` no es una macro válida en el kernel Linux, y el compilador sugirió `LIST_HEAD_INIT` como alternativa.

### Fuentes que Ayudaron a Resolverlo

- **Documentación del kernel:** `include/linux/list.h`, que define `LIST_HEAD_INIT` como la macro correcta para inicializar una lista enlazada estática.
- **Código fuente del kernel:** Revisión de ejemplos en `drivers/` donde se usa `LIST_HEAD_INIT` para inicializar listas.
- **Foro de Kernel Newbies:** Discusiones sobre errores comunes al usar macros de listas enlazadas.

### Solución Encontrada

Se corrigió el código reemplazando `LIST_HEAD_INITIALIZER(msg_queue)` por `LIST_HEAD_INIT(msg_queue)` en la línea 15 de `kernel/syscall_P2_1.c`. Esto permitió que la lista `msg_queue` se inicializara correctamente como una lista enlazada vacía, resolviendo el error de compilación.

## Error 2: Error de Sintaxis en la Definición de `start_log_watch`

### Información del Error

Durante la implementación de `sys_start_log_watch` en `kernel/syscall_P2_2.c`, se cometió un error al definir los parámetros de la syscall, utilizando una sintaxis incorrecta con corchetes (`[]`) en lugar de punteros. Al compilar el kernel, se generó el siguiente error:

```powershell
kernel/syscall_P2_2.c: In function ‘__se_sys_start_log_watch’:
kernel/syscall_P2_2.c:93:64: error: expected expression before ‘]’ token
   93 | SYSCALL_DEFINE4(start_log_watch, const char __user *, archivos[], const char __user *, log_central, const char __user *, palabra_clave, int, num_archivos) {
      |                                                      ^

```

Este error indicó que la sintaxis para pasar un array de cadenas desde el espacio de usuario no era válida en la macro `SYSCALL_DEFINE4`.

### Fuentes que Ayudaron a Resolverlo

- **Documentación del kernel:** `include/linux/syscalls.h`, que detalla el uso correcto de `SYSCALL_DEFINE` con punteros a cadenas (`const char __user * const __user *` para arrays de punteros).
- **Código fuente del kernel:** Revisión de otras syscalls que manejan arrays de cadenas, como `sys_execve`.
- **Manual de programación del kernel:** Guías sobre cómo pasar estructuras de datos complejas desde el espacio de usuario.

### Solución Encontrada

Se corrigió la definición de la syscall cambiando `const char __user *, archivos[]` por `const char __user * const __user *, rutas_log` en la línea 93 de `kernel/syscall_P2_2.c`, reflejando que se espera un puntero a un array de punteros a cadenas. Esto alineó la declaración con el uso correcto de `get_user` para acceder a las rutas de los archivos.

## Error 3: Error de Compilación por Uso Incorrecto de Funciones de Framebuffer

### Información del Error

En la implementación de `sys_capture_screen` en `kernel/syscall_P2_3.c`, se intentó usar las funciones `fb_info_alloc` y `fb_info_release` para gestionar el framebuffer, pero al compilar el kernel, se generaron los siguientes errores:

```
kernel/syscall_P2_3.c: In function ‘__do_sys_sys_capture_screen’:
kernel/syscall_P2_3.c:37:15: error: implicit declaration of function ‘fb_info_alloc’; did you mean ‘bio_alloc’? [-Werror=implicit-function-declaration]
   37 |     fb_info = fb_info_alloc();
      |               ^~~~~~~~~~~~~
      |               bio_alloc
kernel/syscall_P2_3.c:37:13: warning: assignment to ‘struct fb_info *’ from ‘int’ makes pointer from integer without a cast [-Wint-conversion]
   37 |     fb_info = fb_info_alloc();
      |             ^
kernel/syscall_P2_3.c:52:9: error: implicit declaration of function ‘fb_info_release’; did you mean ‘spin_release’? [-Werror=implicit-function-declaration]
   52 |         fb_info_release(fb_info);
      |         ^~~~~~~~~~~~~~~
      |         spin_release

```

Estos errores indicaron que `fb_info_alloc` y `fb_info_release` no estaban declaradas y que el compilador interpretó los retornos como enteros en lugar de punteros.

### Fuentes que Ayudaron a Resolverlo

- **Documentación del kernel:** `Documentation/fb/framebuffer.rst`, que explica que `fb_info_alloc` y `fb_info_release` no son funciones públicas del kernel; en su lugar, se debe usar `registered_fb` directamente.
- **Código fuente del kernel:** Revisión de `drivers/video/fbdev/core/fbmem.c`, donde se muestra el uso de `registered_fb` para acceder al framebuffer existente.
- **Foro de Stack Overflow:** Sugerencias sobre evitar funciones no documentadas y usar el framebuffer registrado en syscalls.

### Solución Encontrada

Se eliminaron las llamadas a `fb_info_alloc` y `fb_info_release` en las líneas 37 y 52 de `kernel/syscall_P2_3.c`. En su lugar, se accedió directamente al framebuffer existente con `struct fb_info *info = registered_fb[0]`, asegurando que se usara el dispositivo framebuffer principal sin necesidad de alocación manual. Esto resolvió los errores de declaración y conversión.

## Conclusión

Estos tres errores reflejan los desafíos de trabajar con macros de listas, definiciones de syscalls y dependencias específicas del subsistema framebuffer en el kernel. Las soluciones, basadas en la documentación oficial, análisis del código fuente y recursos en línea, permitieron superar los problemas de compilación y completar las implementaciones. Este proceso destacó la importancia de usar las macros y funciones correctas según las especificaciones del kernel y evitar suposiciones sobre APIs no documentadas.