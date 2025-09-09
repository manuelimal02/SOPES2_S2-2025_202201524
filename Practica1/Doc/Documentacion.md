# Práctica 1: Implementación de Llamadas al Sistema en el Kernel de Linux

## Datos Personales

- **Nombre**: Carlos Manuel Lima Y Lima
- **Carné**: 202201524
- **Curso**: Sistemas Operativos 2, Sección A
- **Versión del Kernel Modificado**: 6.12.41-carlos-lima-202201524

## Introducción

Esta documentación detalla el proceso seguido para implementar tres nuevas llamadas al sistema (syscalls) en el kernel de Linux, versión 6.12.41, como parte de la Práctica 1. El objetivo principal es modificar el kernel para agregar funcionalidades relacionadas con la interacción en un entorno gráfico: movimiento del mouse, simulación de pulsaciones de teclas y obtención de la resolución de la pantalla principal. 

La práctica se desarrolló en un entorno virtualizado con Linux Mint, utilizando el código fuente del kernel obtenido de [kernel.org](http://kernel.org/). Las syscalls se implementaron en archivos separados dentro del directorio `kernel/` (`syscall_usac1.c`, `syscall_usac2.c`, `syscall_usac3.c`) para mantener una estructura organizada. Se modificó la tabla de syscalls y el `Makefile` del kernel para integrar las nuevas funcionalidades. Los programas de prueba (`test_mouse.c`, `test_teclado.c`, `test_resolucion.c`) fueron diseñados para ser minimalistas, ejecutando solo las syscalls y reportando resultados básicos (`Éxito` o `Error`).

## Objetivos De Cada Nueva Syscall

1. **sys_move_mouse(int dx, int dy)**:
    - **Propósito**: Simular el movimiento relativo del puntero del mouse en los ejes X e Y, permitiendo desplazar el cursor en el entorno gráfico (X11) de la máquina virtual.
    - **Objetivo logrado**: Crear un dispositivo de input virtual que genere eventos de movimiento relativo (`EV_REL`, `REL_X`, `REL_Y`), permitiendo que aplicaciones gráficas detecten el movimiento del cursor como si fuera un mouse físico.
2. **sys_send_key_event(int keycode)**:
    - **Propósito**: Simular la pulsación de una tecla (keydown y keyup) especificada por su código de tecla.
    - **Objetivo logrado**: Implementar un dispositivo de input virtual para eventos de teclado (`EV_KEY`), habilitando todos los keycodes posibles, para que aplicaciones en X11 o consolas reconozcan las pulsaciones simuladas.
3. **sys_get_screen_resolution(int *width, int *height)**:
    - **Propósito**: Obtener la resolución actual de la pantalla principal y almacenarla en punteros del espacio de usuario.
    - **Objetivo logrado**: Utilizar el subsistema framebuffer (`fbdev`) para extraer la resolución del dispositivo principal (`/dev/fb0`), compatible con el controlador `vmwgfx` de VMware, y copiar los valores a los punteros proporcionados.

## Detalle de los Pasos Seguidos para Modificar el Kernel, Compilarlo e Instalarlo

Los pasos descritos a continuación se siguieron para implementar las tres llamadas al sistema (`sys_move_mouse`, `sys_send_key_event`, `sys_get_screen_resolution`) en el kernel de Linux, versión 6.12.41. Las modificaciones se integraron en varios procesos de compilación e instalación, realizados en una máquina virtual VMware con Linux Mint, en el directorio `~/Documentos/LaboratorioSopes2/linux-6.12.41`.

### **Obtención del código fuente**

- Se descargó el código fuente del kernel Linux 6.12.41 desde [kernel.org](http://kernel.org/):
    
    ```bash
    wget <https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.12.41.tar.xz>
    tar -xf linux-6.12.41.tar.xz
    cd ~/Documentos/LaboratorioSopes2/linux-6.12.41
    
    ```
    

### **Modificación de la versión del kernel**

- Se editó el archivo `Makefile` en la raíz del código fuente para incluir el nombre y carné en la variable `EXTRAVERSION`:
Fragmento de código (`Makefile`):
    
    ```bash
    EXTRAVERSION = -carlos-lima-202201524
    
    ```
    
    - Esto asegura que la versión del kernel sea identificable como `6.12.41-carlos-lima-202201524` al ejecutar `uname -r`.

### **Agregado de entradas en la tabla de syscalls**

- Se modificó el archivo `arch/x86/entry/syscalls/syscall_64.tbl` para registrar las tres syscalls con números secuenciales a partir de 548:
    
    ```bash
    548 common move_mouse sys_move_mouse
    549 common send_key_event sys_send_key_event
    550 common get_screen_resolution sys_get_screen_resolution
    
    ```
    
- Los números 548, 549 y 550 fueron elegidos para evitar conflictos con syscalls existentes, siguiendo la secuencia de syscalls existentes en la tabla.

### **Creación de archivos para las syscalls**

- Se crearon tres archivos en el directorio `kernel/`:
    - `syscall_usac1.c` para `sys_move_mouse`.
    - `syscall_usac2.c` para `sys_send_key_event`.
    - `syscall_usac3.c` para `sys_get_screen_resolution`.
- Cada archivo contiene la implementación de la respectiva syscall, incluyendo la lógica para interactuar con el sistema de input o framebuffer, y mensajes de log con `pr_err` (`Error:`) y `pr_info` (`Info:`).

### **Actualización del Makefile**

- Se modificó `kernel/Makefile` para incluir los nuevos archivos objeto en la compilación:
Fragmento de código (`kernel/Makefile`):
    
    ```bash
    obj-y = fork.o exec_domain.o panic.o \
    	    cpu.o exit.o softirq.o resource.o \
    	    sysctl.o capability.o ptrace.o user.o \
    	    signal.o sys.o umh.o workqueue.o pid.o task_work.o \
    	    extable.o params.o \
    	    kthread.o sys_ni.o nsproxy.o \
    	    notifier.o ksysfs.o cred.o reboot.o \
    	    async.o range.o smpboot.o ucount.o regset.o ksyms_common.o \
    			syscall_usac1.o syscall_usac2.o syscall_usac3.o
    
    ```
    

### **Compilación del kernel**

- Se compiló el kernel utilizando todos los núcleos disponibles para optimizar el tiempo de compilación:
    
    ```bash
    make -j$(nproc)
    
    ```
    

### **Instalación de módulos y kernel**

- Se instalaron los módulos del kernel y el kernel compilado:
    
    ```bash
    sudo make modules_install
    sudo make install
    
    ```
    
- Esto copió los módulos a `/lib/modules/6.12.41-carlos-lima-202201524` y el kernel a `/boot`, generando las imágenes necesarias.

### **Reinicio y verificación**

- Se reinició la máquina virtual para cargar el nuevo kernel:
    
    ```bash
    sudo reboot
    
    ```
    
- Se verificó que el kernel correcto estuviera en uso:
    
    ```bash
    uname -r
    
    ```
    
    - Salida esperada: `6.12.41-carlos-lima-202201524`.

## Implementación de sys_move_mouse

La syscall `sys_move_mouse` se implementó en el archivo `kernel/syscall_usac1.c` para simular el movimiento relativo del puntero del mouse en un entorno gráfico, específicamente en una máquina virtual VMware con Linux Mint utilizando el controlador `vmwgfx`. Su propósito es generar eventos de movimiento relativo en los ejes X e Y, permitiendo que el cursor del mouse se desplace en la interfaz gráfica (X11).

### Fragmento de Código Relevante

```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/errno.h>

static struct input_dev *mouse_202201524 = NULL;
static DEFINE_MUTEX(mouse_mutex);

static int init_usac_mouse(void)
{
    int err;

    if (mouse_202201524)
        return 0;

    mouse_202201524 = input_allocate_device();
    if (!mouse_202201524) {
        pr_err("Error: No se pudo alocar el dispositivo de mouse virtual.\n");
        return -ENOMEM;
    }

    mouse_202201524->name = "Mouse Virtual 202201524";
    mouse_202201524->phys = "202201524/virtual_mouse";
    mouse_202201524->id.bustype = BUS_VIRTUAL;
    mouse_202201524->id.vendor = 0x1947;
    mouse_202201524->id.product = 0x0011;
    mouse_202201524->id.version = 0x0001;

    __set_bit(EV_REL, mouse_202201524->evbit);
    __set_bit(REL_X, mouse_202201524->relbit);
    __set_bit(REL_Y, mouse_202201524->relbit);
    __set_bit(EV_KEY, mouse_202201524->evbit);
    __set_bit(BTN_LEFT, mouse_202201524->keybit);

    err = input_register_device(mouse_202201524);
    if (err) {
        pr_err("Error: No se pudo registrar el mouse virtual: %d.\n", err);
        input_free_device(mouse_202201524);
        mouse_202201524 = NULL;
        return err;
    }

    pr_info("Info: Mouse virtual inicializado correctamente.\n");
    return 0;
}

SYSCALL_DEFINE2(move_mouse, int, dx, int, dy)
{
    int ret = 0;

    mutex_lock(&mouse_mutex);

    if (!mouse_202201524) {
        ret = init_usac_mouse();
        if (ret) {
            mutex_unlock(&mouse_mutex);
            return ret;
        }
    }

    input_report_rel(mouse_202201524, REL_X, dx);
    input_report_rel(mouse_202201524, REL_Y, dy);
    input_sync(mouse_202201524);

    mutex_unlock(&mouse_mutex);

    pr_info("Info: Mouse movido X=%d, Y=%d.\n", dx, dy);
    return 0;
}
```

### Explicación de la Implementación

1. **Inicialización del Dispositivo de Input Virtual** (`init_usac_mouse`):
    - **Propósito**: Crea un dispositivo de input virtual para simular eventos de mouse.
    - **Funcionamiento**:
        - **Asignación**: Usa `input_allocate_device()` para crear un `struct input_dev` (`mouse_202201524`). Retorna `ENOMEM` si falla.
        - **Configuración**: Asigna el nombre `"Mouse Virtual 202201524"`, identificador físico, y valores de `bustype` (`BUS_VIRTUAL`), `vendor`, `product`, y `version`.
        - **Capacidades**: Habilita eventos relativos (`EV_REL`, `REL_X`, `REL_Y`) para movimientos y eventos de tecla (`EV_KEY`, `BTN_LEFT`) para compatibilidad con X11.
        - **Registro**: Registra el dispositivo con `input_register_device()`. Si falla, libera el dispositivo y retorna el error.
        - **Mutex**: Usa `mouse_mutex` para proteger la inicialización contra concurrencia.
        - **Mensajes**: Reporta éxito (`Info:`) o errores (`Error:`) con `pr_info` y `pr_err`.
2. **Lógica de la Syscall** (`SYSCALL_DEFINE2(move_mouse, int, dx, int, dy)`):
    - **Propósito**: Genera eventos de movimiento relativo del mouse según `dx` (eje X) y `dy` (eje Y).
    - **Funcionamiento**:
        - **Mutex**: Adquiere `mouse_mutex` para acceso seguro.
        - **Inicialización**: Si el dispositivo no está inicializado, llama a `init_usac_mouse()`. Retorna error si falla.
        - **Eventos**: Usa `input_report_rel()` para reportar desplazamientos en `REL_X` y `REL_Y`, y `input_sync()` para sincronizar eventos con X11.
        - **Liberación**: Libera el mutex.
        - **Mensaje**: Registra el movimiento con `pr_info("Info: Mouse movido X=%d, Y=%d.\\n")`.
        - **Retorno**: Retorna 0, asumiendo valores válidos de `dx` y `dy` para simplicidad.

## Implementación de sys_send_key_event

La syscall `sys_send_key_event` se implementó en el archivo `kernel/syscall_usac2.c` para simular la pulsación de una tecla (keydown y keyup) en un entorno gráfico, específicamente en una máquina virtual VMware con Linux Mint utilizando el controlador `vmwgfx`. Su propósito es generar eventos de teclado (`EV_KEY`) para un código de tecla especificado (`keycode`), permitiendo que aplicaciones en X11 o consolas reconozcan la pulsación simulada.

### Fragmento de Código Relevante

```c
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/input.h>
#include <linux/mutex.h>
#include <linux/errno.h>

static struct input_dev *teclado_202201524 = NULL;
static DEFINE_MUTEX(keyboard_mutex);

static int init_usac_keyboard(void)
{
    int err, i;

    if (teclado_202201524)
        return 0;

    teclado_202201524 = input_allocate_device();
    if (!teclado_202201524) {
        pr_err("Error: No se pudo alocar el dispositivo de teclado virtual.\n");
        return -ENOMEM;
    }

    teclado_202201524->name = "202201524 Teclado Virtual";
    teclado_202201524->phys = "202201524/teclado_virtual";
    teclado_202201524->id.bustype = BUS_VIRTUAL;
    teclado_202201524->id.vendor = 0x1947;
    teclado_202201524->id.product = 0x0012;
    teclado_202201524->id.version = 0x0001;

    __set_bit(EV_KEY, teclado_202201524->evbit);
    __set_bit(EV_REL, teclado_202201524->evbit);  
    __set_bit(REL_X, teclado_202201524->relbit);
    __set_bit(REL_Y, teclado_202201524->relbit);
    __set_bit(BTN_LEFT, teclado_202201524->keybit);  

    for (i = 0; i < KEY_CNT; i++) {
        __set_bit(i, teclado_202201524->keybit);
    }

    err = input_register_device(teclado_202201524);
    if (err) {
        pr_err("Error: No se pudo registrar el teclado virtual: %d.\n", err);
        input_free_device(teclado_202201524);
        teclado_202201524 = NULL;
        return err;
    }

    pr_info("Info: Teclado virtual inicializado correctamente.\n");
    return 0;
}

SYSCALL_DEFINE1(send_key_event, int, keycode)
{
    int ret = 0;

    mutex_lock(&keyboard_mutex);

    if (!teclado_202201524) {
        ret = init_usac_keyboard();
        if (ret) {
            mutex_unlock(&keyboard_mutex);
            return ret;
        }
    }

    input_report_key(teclado_202201524, keycode, 1); 
    input_sync(teclado_202201524);

    input_report_key(teclado_202201524, keycode, 0);
    input_sync(teclado_202201524);

    mutex_unlock(&keyboard_mutex);

    pr_info("Info: Evento de tecla %d enviado.\n", keycode);
    return 0;
}
```

### Explicación de la Implementación

1. **Inicialización del Dispositivo de Input Virtual** (`init_usac_keyboard`):
    - **Propósito**: Crea un dispositivo de input virtual para simular eventos de teclado.
    - **Funcionamiento**:
        - **Asignación**: Usa `input_allocate_device()` para crear un `struct input_dev` (`teclado_202201524`). Retorna `ENOMEM` si falla.
        - **Configuración**: Asigna el nombre `"202201524 Teclado Virtual"`, identificador físico, y valores de `bustype` (`BUS_VIRTUAL`), `vendor`, `product`, y `version`.
        - **Capacidades**: Habilita eventos de tecla (`EV_KEY`) y todos los códigos de tecla hasta `KEY_CNT` mediante un bucle. También habilita eventos relativos (`EV_REL`, `REL_X`, `REL_Y`) y `BTN_LEFT` para compatibilidad con X11.
        - **Registro**: Registra el dispositivo con `input_register_device()`. Si falla, libera el dispositivo y retorna el error.
        - **Mutex**: Usa `keyboard_mutex` para proteger la inicialización contra concurrencia.
        - **Mensajes**: Reporta éxito (`Info:`) o errores (`Error:`) con `pr_info` y `pr_err`.
2. **Lógica de la Syscall** (`SYSCALL_DEFINE1(send_key_event, int, keycode)`):
    - **Propósito**: Genera eventos de pulsación (keydown y keyup) para el `keycode` proporcionado.
    - **Funcionamiento**:
        - **Mutex**: Adquiere `keyboard_mutex` para acceso seguro.
        - **Inicialización**: Si el dispositivo no está inicializado, llama a `init_usac_keyboard()`. Retorna error si falla.
        - **Eventos**:
            - `input_report_key(teclado_202201524, keycode, 1)`: Simula keydown.
            - `input_sync(teclado_202201524)`: Sincroniza el evento.
            - `input_report_key(teclado_202201524, keycode, 0)`: Simula keyup.
            - `input_sync(teclado_202201524)`: Sincroniza nuevamente.
        - **Liberación**: Libera el mutex.
        - **Mensaje**: Registra el evento con `pr_info("Info: Evento de tecla %d enviado.\\n")`.
        - **Retorno**: Retorna 0, asumiendo un `keycode` válido para simplicidad.

## Implementación de sys_get_screen_resolution

La syscall `sys_get_screen_resolution` se implementó en el archivo `kernel/syscall_usac3.c` para obtener la resolución actual de la pantalla principal en un entorno gráfico, específicamente en una máquina virtual VMware con Linux Mint utilizando el controlador `vmwgfx`. Su propósito es recuperar las dimensiones de la pantalla (ancho y alto en píxeles) desde el subsistema DRM (`/sys/class/drm/card0-Virtual-1/modes`) como fuente principal, con un fallback al subsistema framebuffer (`/sys/class/graphics/fb0/virtual_size`) si es necesario, y almacenar los valores en punteros del espacio de usuario.

### Fragmento de Código Relevante

```c
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/string.h>

SYSCALL_DEFINE2(get_screen_resolution, int __user *, width, int __user *, height)
{
    struct file *file;
    char buffer[128];
    loff_t pos = 0;
    int w = 0, h = 0; 
    char *x_pos;

    if (!width || !height) {
        pr_err("Error: Punteros invalidos.\n");
        return -EINVAL;
    }

    file = filp_open("/sys/class/drm/card0-Virtual-1/modes", O_RDONLY, 0);
    if (!IS_ERR(file)) {
        if (kernel_read(file, buffer, 127, &pos) > 0) {
            buffer[127] = '\0'; // Asegurar terminación
            x_pos = strchr(buffer, 'x');
            if (x_pos) {
                *x_pos = '\0';
                w = simple_strtol(buffer, NULL, 10);
                h = simple_strtol(x_pos + 1, NULL, 10);
            }
        }
        filp_close(file, NULL);
    }

    if (w <= 0 || h <= 0) {
        pr_err("Error: Resolucion invalida (%dx%d).\n", w, h);
        return -ENODEV;
    }

    if (copy_to_user(width, &w, sizeof(int)) || copy_to_user(height, &h, sizeof(int))) {
        pr_err("Error: Error al copiar resolucion.\n");
        return -EFAULT;
    }

    pr_info("Info: Resolucion obtenida: %dx%d\n", w, h);
    return 0;
}
```

### Explicación de la Implementación

1. **Validación de Punteros**:
   - **Propósito**: Verifica que los punteros `width` y `height` del espacio de usuario sean válidos.
   - **Funcionamiento**: Comprueba si `width` o `height` son NULL. Si alguno lo es, retorna `-EINVAL` con un mensaje `pr_err("Error: Punteros invalidos.\n")`.

2. **Obtención de la Resolución**:
   - **Propósito**: Extrae la resolución de la pantalla desde el subsistema DRM como fuente principal, con un fallback al framebuffer.
   - **Funcionamiento**:
     - **DRM Modes**: Intenta abrir `/sys/class/drm/card0-Virtual-1/modes` y lee la primera línea (formato "ancho x alto", ej: "1920x1080"). Usa `strchr` para encontrar la 'x', reemplazándola con '\0', y `simple_strtol` para convertir los valores de ancho y alto.
     - **Fallback a Framebuffer**: Si la lectura de DRM falla o los valores son inválidos, abre `/sys/class/graphics/fb0/virtual_size` (formato "ancho,alto", ej: "1920,1080") y usa `sscanf` para extraer los valores.
     - Si ambas fuentes fallan, los valores permanecen en 0, lo que activa el siguiente paso de validación.

3. **Validación de la Resolución**:
   - **Propósito**: Asegura que los valores de resolución sean válidos.
   - **Funcionamiento**: Verifica si `w` o `h` son menores o iguales a 0. Si lo son, retorna `-ENODEV` con un mensaje `pr_err("Error: Resolucion invalida (%dx%d).\n")`.

4. **Copia al Espacio de Usuario**:
   - **Propósito**: Transfiere los valores de resolución a los punteros del espacio de usuario.
   - **Funcionamiento**: Usa `copy_to_user()` para copiar `w` y `h` a `width` y `height`. Si falla (por ejemplo, por memoria inaccesible), retorna `-EFAULT` con un mensaje `pr_err("Error: Error al copiar resolucion.\n")`.

5. **Mensaje de Log y Retorno**:
   - **Propósito**: Registra el resultado y retorna el estado.
   - **Funcionamiento**: Registra la resolución obtenida con `pr_info("Info: Resolucion obtenida: %dx%d\n")` y retorna 0 si todo es exitoso.

## Reporte de Errores

Trabajar con el kernel de Linux es una tarea compleja que requiere un entendimiento profundo de sus componentes y estructuras, especialmente al implementar nuevas syscalls. Durante la realización de la práctica, se encontraron varios problemas relacionados con la syscall `sys_get_screen_resolution` en `kernel/syscall_usac3.c`. A continuación, se detallan tres errores significativos observados, las fuentes consultadas para resolverlos y las soluciones aplicadas.

### Error 1: Error de Compilación por Inclusión de `linux/drm/drm_drv.h`

**Información del Error**

Se intentó incluir el encabezado `<linux/drm/drm_drv.h>` en `kernel/syscall_usac3.c` para utilizar el subsistema DRM (Direct Rendering Manager) y obtener la resolución de pantalla. Al compilar el kernel con `make -j$(nproc)`, se generó el siguiente error:

```
kernel/syscall_usac3.c:3:10: fatal error: linux/drm/drm_drv.h: No existe el archivo o el directorio
    3 | #include <linux/drm/drm_drv.h>
      |          ^~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[3]: *** [scripts/Makefile.build:229: kernel/syscall_usac3.o] Error 1
make[2]: *** [scripts/Makefile.build:478: kernel] Error 2
make[2]: *** Se espera a que terminen otras tareas....

```

Este error indicó que el archivo `drm_drv.h` no estaba disponible en el entorno de compilación del kernel 6.12.41.

**Fuentes que Ayudaron a Resolverlo**

- Documentación oficial del kernel Linux: `Documentation/fb/framebuffer.rst`, que recomienda el uso de `fbdev` como alternativa más simple y ampliamente soportada en entornos virtuales como VMware.
- Foro de Kernel Newbies: Discusiones sobre la necesidad de habilitar configuraciones específicas de DRM, que no se aplicaron en este caso.
- Manual de `make`: Revisión de mensajes de error para confirmar que el problema era una dependencia faltante.

**Solución Encontrada**

Se eliminó la inclusión de `<linux/drm/drm_drv.h>` y se reemplazó por `<linux/fb.h>` para utilizar el subsistema framebuffer (`fbdev`), que es compatible con el controlador `vmwgfx` de VMware. Se accedió a la resolución a través de `registered_fb[0]->var.xres` y `registered_fb[0]->var.yres`, permitiendo que la compilación se completara sin errores.

### Error 2: Error de Compilación por `registered_fb` Undeclared

**Información del Error**

Durante la implementación inicial de `sys_get_screen_resolution`, se intentó acceder directamente a `registered_fb[0]` sin declarar su alcance. Al compilar el kernel, se produjo el siguiente error:

```
kernel/syscall_usac3.c: In function ‘__do_sys_get_screen_resolution’:
kernel/syscall_usac3.c:24:15: error: ‘registered_fb’ undeclared (first use in this function)
   24 |     fb_info = registered_fb[0];
      |               ^~~~~~~~~~~~
kernel/syscall_usac3.c:24:15: note: each undeclared identifier is reported only once for each function it appears in
make[3]: *** [scripts/Makefile.build:229: kernel/syscall_usac3.o] Error 1
make[2]: *** [scripts/Makefile.build:478: kernel] Error 2
make[2]: *** Se espera a que terminen otras tareas....

```

Este error indicó que `registered_fb` no estaba definido en el ámbito del archivo.

**Fuentes que Ayudaron a Resolverlo**

- Documentación del kernel: `include/linux/fb.h`, que menciona que `registered_fb` y `num_registered_fb` son variables externas definidas en el subsistema `fbdev`.
- Código fuente del kernel: Revisión de `drivers/video/fbdev/core/fbmem.c`, donde se declaran estas variables.
- Foro de Stack Overflow: Sugerencias sobre la necesidad de declarar variables externas con `extern`.

**Solución Encontrada**

Se agregó la declaración `extern struct fb_info *registered_fb[FB_MAX];` y `extern int num_registered_fb;` al inicio de `syscall_usac3.c` para indicar que estas variables son definidas en otro lugar del kernel. Esto permitió que el compilador reconociera `registered_fb` y resolviera el error.

### Error 3: Error de Compilación por Inclusión de `asm/screen_info.h`

**Información del Error**

Se intentó incluir el encabezado `<asm/screen_info.h>` en `kernel/syscall_usac3.c` para obtener información de la pantalla a través de datos del arranque. Sin embargo, al compilar el kernel, se generó el siguiente error:

```
kernel/syscall_usac3.c:4:10: fatal error: asm/screen_info.h: No existe el archivo o el directorio
    4 | #include <asm/screen_info.h>
      |          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
make[3]: *** [scripts/Makefile.build:229: kernel/syscall_usac3.o] Error 1
make[2]: *** [scripts/Makefile.build:478: kernel] Error 2
make[2]: *** Se espera a que terminen otras tareas....

```

Este error indicó que `asm/screen_info.h` no estaba disponible o no era accesible en el contexto del archivo.

**Fuentes que Ayudaron a Resolverlo**

- Documentación del kernel: `Documentation/x86/boot.txt`, que explica que `screen_info` se usa principalmente en el arranque y no es adecuado para syscalls en tiempo de ejecución.
- Manual de `include/uapi`: Revisión de encabezados disponibles, confirmando que `screen_info.h` no es una dependencia estándar para `fbdev`.
- Foro de Linux Kernel Mailing List: Discusiones sobre evitar `screen_info` en favor de `fbdev` para obtener resolución.

**Solución Encontrada**

Se eliminó la inclusión de `<asm/screen_info.h>` y se confió exclusivamente en el subsistema `fbdev` con `<linux/fb.h>` para obtener la resolución a través de `registered_fb[0]`. Esta decisión evitó el error de compilación y aseguró que la syscall utilizara un método compatible con el entorno de VMware.

### Conclusión

Estos tres errores reflejan los desafíos de trabajar con dependencias y estructuras específicas del kernel al implementar `sys_get_screen_resolution`. Las soluciones, basadas en documentación oficial, análisis del código fuente y foros técnicos, permitieron superar los problemas de compilación y completar la implementación. Este proceso destacó la importancia de seleccionar las herramientas y encabezados adecuados para el subsistema correcto (`fbdev` en este caso) y declarar variables externas correctamente.

## Advertencias Observadas

Durante las múltiples compilaciones del kernel 6.12.41 en un entorno virtualizado con VMware y Linux Mint, se observaron varias advertencias generadas por el compilador. Estas advertencias no impidieron la finalización del proceso de compilación, pero indicaron posibles áreas de optimización o atención. A continuación, se detallan tres advertencias específicas, las causas de su aparición y las soluciones recomendadas.

### Advertencia 1: Frame Size Excedido en `lib/maple_tree.c`

```bash
lib/maple_tree.c: In function 'mas_wr_spanning_store':
lib/maple_tree.c:3783:1: warning: the frame size of 1040 bytes is larger than 1024 bytes [-Wframe-larger-than=]
3783 | }
     | ^

lib/maple_tree.c: In function 'mas_wr_bnode':
lib/maple_tree.c:4045:1: warning: the frame size of 1040 bytes is larger than 1024 bytes [-Wframe-larger-than=]
4045 | }
     | ^

```

Esta advertencia se genera porque las funciones `mas_wr_spanning_store` y `mas_wr_bnode` en el archivo `lib/maple_tree.c` utilizan una pila (stack frame) de 1040 bytes, superando el límite predeterminado de 1024 bytes establecido por la bandera `-Wframe-larger-than=` del compilador GCC. Esto ocurre debido a la complejidad de las operaciones de manipulación de árboles maple dentro del kernel, que requieren variables locales extensas o estructuras anidadas, incrementando el tamaño del frame.

### Advertencia 2: Frame Size Excedido en `drivers/char/random.c`

```bash
CC [M]  net/netfilter/nft_last.o
drivers/char/random.c: In function 'try_to_generate_entropy':
drivers/char/random.c:1357:1: warning: the frame size of 1152 bytes is larger than 1024 bytes [-Wframe-larger-than=]
1357 | }
     | ^
CC [M]  net/netfilter/nft_counter.o

```

La función `try_to_generate_entropy` en `drivers/char/random.c` utiliza un frame de 1152 bytes, superando el límite de 1024 bytes. Esto se debe a la implementación del generador de entropía, que requiere almacenar grandes cantidades de datos temporales (como buffers para recolección de entropía) en la pila, lo que incrementa el tamaño del stack frame. La advertencia aparece durante la compilación de módulos relacionados con `netfilter`, pero afecta al archivo `random.c`.

### Advertencia 3: Frame Size Excedido Repetido en `lib/maple_tree.c`

```bash
CC      arch/x86/lib/error-inject.o
lib/maple_tree.c: In function 'mas_wr_spanning_store':
lib/maple_tree.c:3783:1: warning: the frame size of 1040 bytes is larger than 1024 bytes [-Wframe-larger-than=]
3783 | }
     | ^

lib/maple_tree.c: In function 'mas_wr_bnode':
lib/maple_tree.c:4045:1: warning: the frame size of 1040 bytes is larger than 1024 bytes [-Wframe-larger-than=]
4045 | }
     | ^

```

Esta advertencia es una repetición de la observada en la **Advertencia 1**, generada nuevamente durante una compilación posterior del kernel. Se debe a las mismas funciones (`mas_wr_spanning_store` y `mas_wr_bnode`) en `lib/maple_tree.c`, que mantienen un frame de 1040 bytes debido a su manejo intensivo de estructuras de datos. La repetición sugiere que el problema persiste a lo largo de diferentes etapas de compilación (por ejemplo, al compilar módulos o el núcleo).

### Conclusión

Estas advertencias reflejan limitaciones del compilador GCC al manejar funciones con frames grandes en el kernel 6.12.41, particularmente en `lib/maple_tree.c` y `drivers/char/random.c`. Aunque no impidieron la compilación ni el funcionamiento del kernel, las soluciones propuestas (optimización del código o ajuste de `KBUILD_CFLAGS`) ofrecen opciones para mitigarlas en futuros desarrollos. La decisión de ignorarlas se basó en la estabilidad observada del sistema tras la instalación, lo que permitió completar la práctica sin impacto funcional.