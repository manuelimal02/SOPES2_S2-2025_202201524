# Proyecto Único: USACLinux - Escritorio Remoto Web

## Datos Personales

| Nombre | Carlos Manuel Lima Y Lima |
| --- | --- |
| Registro Académico | 202201524 |
| Curso | Sistemas Operativos 2, Sección A |

## Introducción

Esta documentación detalla el proceso seguido para implementar el proyecto USACLinux: Escritorio Remoto Web, el cual consiste en la creación de un sistema de administración remota para equipos Linux mediante la implementación de llamadas al sistema personalizadas en el kernel de Linux, versión [6.12.41]. El objetivo principal es desarrollar una solución integral que permita el acceso y control remoto de equipos Linux a través de una interfaz web intuitiva, integrando funcionalidades de monitorización de recursos (CPU y memoria RAM), visualización del escritorio en tiempo real y control remoto del mouse y teclado.

El proyecto se estructura en tres capas principales:

1. **Llamadas al Sistema Personalizadas**  
   Implementadas en el kernel (`sys_cpu_percentage`, `sys_ram_percentage`, `sys_control_mouse_click`), estas llamadas proporcionan acceso directo a los recursos del sistema y control sobre los botones del mouse.

2. **API Intermediaria**  
   Desarrollada en C, traduce las peticiones HTTP a llamadas al sistema y gestiona la autenticación mediante **PAM**.

3. **Aplicación Web**  
   Ofrece una interfaz gráfica para interactuar con el sistema remoto.  
   La seguridad se implementa mediante **autenticación PAM** y control de acceso basado en grupos (`remote_view` y `remote_control`).


Las llamadas al sistema se implementaron en archivos separados dentro del directorio `kernel/` para mantener una estructura organizada. Se modificó la tabla de syscalls (`arch/x86/entry/syscalls/syscall_64.tbl`) y el `Makefile` del kernel para integrar las nuevas funcionalidades. La API fue desarrollada en `C` y la aplicación web fue implementada utilizando `React + Vite + Tailwindcss`.

---

## Objetivos De Cada Nueva Syscall

### sys_cpu_percentage (syscall_pf_1.c)

- **Propósito:** Obtener el porcentaje de uso total de la CPU del sistema en tiempo real, proporcionando información agregada de todos los núcleos de procesamiento disponibles.
- **Objetivo logrado:** Se implementó una syscall que accede a las estadísticas de CPU del kernel mediante la estructura `kernel_cpustat`, agregando los tiempos de todos los núcleos (user, nice, system, idle, iowait, irq, softirq, steal) para calcular el porcentaje total de uso activo y ocioso del procesador. La implementación utiliza `for_each_possible_cpu` para iterar sobre todos los CPUs disponibles, convierte los tiempos de nanosegundos a ticks de reloj con `nsec_to_clock_t`, y calcula porcentajes precisos evitando divisiones por cero. La syscall transfiere los resultados de manera segura al espacio de usuario mediante `copy_to_user`, proporcionando tanto el porcentaje usado como el no usado en una estructura `informacion_cpu`, permitiendo al sistema de monitorización web mostrar el estado de la CPU en tiempo real.

### sys_ram_percentage (syscall_pf_2.c)

- **Propósito:** Obtener el porcentaje de uso de la memoria RAM del sistema en tiempo real, proporcionando información sobre la cantidad de memoria utilizada y disponible.
- **Objetivo logrado:** Se implementó una syscall que accede a las estadísticas de memoria del kernel mediante la función `si_meminfo()`, la cual llena una estructura `sysinfo` con información actualizada del sistema. La implementación calcula el porcentaje de memoria utilizada comparando la memoria total (`totalram`) con la memoria libre (`freeram`), obteniendo así el uso real de RAM. La syscall realiza validación contra división por cero y transfiere los resultados de manera segura al espacio de usuario mediante `copy_to_user()`, proporcionando tanto el porcentaje usado como el no usado en una estructura `informacion_ram`. Esta funcionalidad es esencial para el dashboard de monitorización del escritorio remoto, permitiendo visualizar el estado de la memoria en tiempo real.

### sys_control_mouse_click (syscall_pf_3.c)

- **Propósito:** Controlar el mouse del sistema de forma remota, permitiendo mover el cursor a coordenadas específicas y ejecutar clicks izquierdos o derechos de manera programática.
- **Objetivo logrado:** Se implementó una syscall que utiliza la API de dispositivos de entrada del kernel (`input subsystem`) para crear un dispositivo de mouse virtual que puede inyectar eventos de movimiento y clicks. La implementación incluye la inicialización automática del dispositivo mediante `device_initcall()`, detección dinámica de la resolución de pantalla leyendo `/sys/class/drm/card0-Virtual-1/modes`, configuración de límites absolutos para las coordenadas X e Y basados en la resolución detectada, y validación de coordenadas para prevenir valores fuera de rango. La syscall reporta eventos de movimiento absoluto con `input_report_abs()` y simula clicks mediante la secuencia de presionar (`input_report_key` con valor 1), sincronizar, esperar 20ms con `msleep()`, y liberar el botón (valor 0), emulando el comportamiento natural de un click físico. Esta funcionalidad es crítica para el control remoto del equipo, permitiendo a usuarios autorizados del grupo `remote_control` interactuar con el escritorio mediante la interfaz web.

---

## Detalle de los Pasos Seguidos para Modificar el Kernel, Compilarlo e Instalarlo

Los pasos descritos a continuación se siguieron para implementar las llamadas al sistema personalizadas en el kernel de Linux, versión [6.12.41]. Las modificaciones se integraron en el proceso de compilación e instalación, realizados en una máquina virtual VMware con Linux Mint, en el directorio `~/Documentos/LaboratorioSopes2/linux-6.12.41`.

### Agregado de Entradas en la Tabla de Syscalls

Se modificó el archivo `arch/x86/entry/syscalls/syscall_64.tbl` para registrar las syscalls con números secuenciales:

```bash
556 common cpu_percentage sys_cpu_percentage
557 common ram_percentage sys_ram_percentage
558 common control_mouse_click sys_control_mouse_click
```

### Creación de Archivos Para las Syscalls

Se crearon tres archivos en el directorio `kernel/`:

- **`syscall_pf_1.c`** para la implementación de `sys_cpu_percentage`, syscall que obtiene el porcentaje de uso de CPU del sistema agregando estadísticas de todos los núcleos disponibles mediante acceso a las estructuras `kernel_cpustat`.

- **`syscall_pf_2.c`** para la implementación de `sys_ram_percentage`, syscall que calcula el porcentaje de uso de memoria RAM utilizando la función `si_meminfo()` para acceder a las estadísticas de memoria del kernel.

- **`syscall_pf_3.c`** para la implementación de `sys_control_mouse_click`, syscall que controla el mouse del sistema mediante la creación de un dispositivo de entrada virtual, permitiendo movimiento del cursor y ejecución de clicks izquierdos y derechos.

Cada archivo contiene la implementación completa de la respectiva syscall, incluyendo las definiciones de estructuras para transferencia de datos entre kernel y espacio de usuario, la lógica de procesamiento de estadísticas del sistema o control de dispositivos, validaciones de seguridad para prevenir accesos inválidos, funciones auxiliares cuando son necesarias (como `obtener_resolucion_pantalla()` en `syscall_pf_3.c`), funciones de inicialización de dispositivos virtuales (en el caso de `syscall_pf_3.c` con `device_initcall()`), y mensajes de log con `pr_info()` para facilitar la depuración y monitoreo del comportamiento de las syscalls.

---

### Actualización del Makefile

Se modificó `kernel/Makefile` para incluir los nuevos archivos objeto en la compilación:

Fragmento de código (`kernel/Makefile`):

```makefile
obj-y = fork.o exec_domain.o panic.o \
	    cpu.o exit.o softirq.o resource.o \
	    sysctl.o capability.o ptrace.o user.o \
	    signal.o sys.o umh.o workqueue.o pid.o task_work.o \
	    extable.o params.o \
	    kthread.o sys_ni.o nsproxy.o \
	    notifier.o ksysfs.o cred.o reboot.o \
	    async.o range.o smpboot.o ucount.o regset.o ksyms_common.o \
		syscall_usac1.o syscall_usac2.o syscall_usac3.o \
		syscall_P2_1.o syscall_P2_2.o syscall_P2_3.o \
		syscall_pf_1.o syscall_pf_2.o syscall_pf_3.o \
```

### Compilación del Kernel

Se compiló el kernel utilizando todos los núcleos disponibles:

```bash
make -j$(nproc)
```

### Instalación de Módulos y Kernel

Se instalaron los módulos del kernel y el kernel compilado:

```bash
sudo make modules_install
sudo make install
```

### Reinicio y Verificación

Se reinició el sistema para cargar el nuevo kernel:

```bash
sudo reboot
```

Se verificó que el kernel correcto estuviera en uso:

```bash
uname -r
```

Salida esperada: `[6.12.41-carlos-lima-202201524]`.

---

## Implementación de sys_cpu_percentage

La syscall `sys_cpu_percentage` permite obtener información en tiempo real sobre el uso del procesador del sistema, agregando las estadísticas de todos los núcleos de CPU disponibles. Esta implementación se encuentra en el archivo `kernel/syscall_pf_1.c` y proporciona datos esenciales para el sistema de monitorización del escritorio remoto.

### Fragmento de Código Relevante

```c
struct informacion_cpu {
    int porcentaje_usado;
    int porcentaje_no_usado;
};

SYSCALL_DEFINE1(cpu_percentage, struct informacion_cpu __user *, info)
{
    struct informacion_cpu info_k;
    u64 user, nice, system, idle, iowait, irq, softirq, steal;
    u64 total_tiempo, total_idle, total_activo;
    int cpu;
    
    // INICIALIZAR VARIABLES DESDE CERO
    user = nice = system = idle = iowait = irq = softirq = steal = 0;
    
    // SUMAR LAS ESTADÍSTICAS DE TODOS LOS CPU
    for_each_possible_cpu(cpu) {
        struct kernel_cpustat *kcpustat = &kcpustat_cpu(cpu);
        user    += kcpustat->cpustat[CPUTIME_USER];
        nice    += kcpustat->cpustat[CPUTIME_NICE];
        system  += kcpustat->cpustat[CPUTIME_SYSTEM];
        idle    += kcpustat->cpustat[CPUTIME_IDLE];
        iowait  += kcpustat->cpustat[CPUTIME_IOWAIT];
        irq     += kcpustat->cpustat[CPUTIME_IRQ];
        softirq += kcpustat->cpustat[CPUTIME_SOFTIRQ];
        steal   += kcpustat->cpustat[CPUTIME_STEAL];
    }
    
    // CALCULAR TIEMPOS TOTALES
    total_tiempo = nsec_to_clock_t(user + nice + system + idle + iowait + irq + softirq + steal);
    total_idle = nsec_to_clock_t(idle + iowait + steal);
    total_activo = total_tiempo - total_idle;
    
    // CALCULAR PORCENTAJES DE USO
    if (total_tiempo > 0) {
        info_k.porcentaje_usado = (int)((total_activo * 100) / total_tiempo);
    } else {
        info_k.porcentaje_usado = 0;
    }
    info_k.porcentaje_no_usado = 100 - info_k.porcentaje_usado;
    
    if (copy_to_user(info, &info_k, sizeof(struct informacion_cpu))) {
        return -EFAULT;
    }
    
    return 0;
}
```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Define la estructura `informacion_cpu` para transferir los porcentajes de uso entre el kernel y el espacio de usuario, con campos para el porcentaje usado y no usado del CPU.
- **Funcionamiento:**
    - Se declaran variables de tipo `u64` (unsigned 64-bit) para almacenar los tiempos acumulados de CPU en diferentes estados: `user` (tiempo en modo usuario), `nice` (procesos de baja prioridad), `system` (tiempo en modo kernel), `idle` (CPU inactiva), `iowait` (esperando I/O), `irq` (manejando interrupciones hardware), `softirq` (interrupciones software), y `steal` (tiempo robado en entornos virtualizados).
    - Todas las variables se inicializan en cero para garantizar la correcta agregación de estadísticas.

**Lógica de la Syscall:**

- **Propósito:** Calcular el porcentaje agregado de uso de CPU de todos los núcleos del sistema.
- **Funcionamiento:**
    - **Agregación de Estadísticas:** Utiliza la macro `for_each_possible_cpu(cpu)` para iterar sobre todos los CPUs disponibles en el sistema (incluyendo CPUs offline que pueden activarse). Para cada CPU, accede a su estructura `kernel_cpustat` mediante `kcpustat_cpu(cpu)`, que contiene contadores de tiempo para cada estado del procesador.
    - **Acumulación de Tiempos:** Suma los valores de cada estado (`CPUTIME_USER`, `CPUTIME_NICE`, etc.) de todos los CPUs en las variables globales correspondientes, obteniendo tiempos totales del sistema completo.
    - **Conversión de Unidades:** Convierte los tiempos de nanosegundos (unidad interna del kernel) a ticks de reloj usando `nsec_to_clock_t()`, facilitando cálculos de porcentaje más precisos y compatibles con las convenciones del sistema.
    - **Cálculo de Totales:** Suma todos los tiempos para obtener `total_tiempo` (tiempo total transcurrido). Calcula `total_idle` sumando los estados de inactividad (`idle`, `iowait`, `steal`), y obtiene `total_activo` restando el tiempo idle del total.
    - **Cálculo de Porcentajes:** Realiza una división protegida verificando que `total_tiempo > 0` para evitar división por cero. Calcula el porcentaje usado como `(total_activo * 100) / total_tiempo` y el porcentaje no usado como `100 - porcentaje_usado`, garantizando que la suma sea exactamente 100%.
    - **Transferencia Segura:** Utiliza `copy_to_user()` para copiar la estructura `info_k` desde el espacio del kernel al buffer proporcionado por el usuario (`info`). Si la copia falla (por dirección inválida o permisos insuficientes), retorna `-EFAULT`.
    - **Retorno:** Devuelve `0` en caso de éxito, indicando que los porcentajes fueron calculados y transferidos correctamente.

---

## Implementación de sys_ram_percentage

La syscall `sys_ram_percentage` permite obtener información en tiempo real sobre el uso de memoria RAM del sistema. Esta implementación se encuentra en el archivo `kernel/syscall_pf_2.c` y complementa la monitorización de recursos del sistema junto con la syscall de CPU.

### Fragmento de Código Relevante

```c
struct informacion_ram {
    int porcentaje_usado;
    int porcentaje_no_usado;
};

SYSCALL_DEFINE1(ram_percentage, struct informacion_ram __user *, info)
{
    struct informacion_ram info_k;
    unsigned long total_mem, free_mem;
    struct sysinfo si;

    // OBTENER INFORMACIÓN DE MEMORIA DEL SISTEMA
    si_meminfo(&si);
    total_mem = si.totalram;
    free_mem = si.freeram;

    // CALCULAR PORCENTAJES DE USO
    if (total_mem > 0) {
        info_k.porcentaje_usado = (int)((((total_mem - free_mem) * 100) / total_mem));
    } else {
        info_k.porcentaje_usado = 0;
    }
    info_k.porcentaje_no_usado = 100 - info_k.porcentaje_usado;

    if (copy_to_user(info, &info_k, sizeof(struct informacion_ram))) {
        return -EFAULT;
    }

    return 0;
}
```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Define la estructura `informacion_ram` para transferir los porcentajes de uso de memoria entre el kernel y el espacio de usuario, con campos para el porcentaje usado y no usado.
- **Funcionamiento:**
    - Se declara una estructura `sysinfo` (`si`) que será llenada con información actualizada del sistema por la función `si_meminfo()`.
    - Las variables `total_mem` y `free_mem` de tipo `unsigned long` almacenarán la memoria total del sistema y la memoria libre disponible, respectivamente.
    - La estructura `informacion_ram info_k` se utiliza como buffer intermedio en el espacio del kernel antes de transferir los datos al usuario.

**Lógica de la Syscall:**

- **Propósito:** Calcular el porcentaje de uso de memoria RAM del sistema basándose en estadísticas actualizadas del kernel.
- **Funcionamiento:**
    - **Obtención de Estadísticas:** Llama a `si_meminfo(&si)`, función del kernel que actualiza la estructura `sysinfo` con información actual del sistema, incluyendo memoria total, memoria libre, buffers, cache, swap, y otras métricas relevantes.
    - **Extracción de Datos:** Obtiene `total_mem` desde `si.totalram` (memoria RAM total instalada en páginas) y `free_mem` desde `si.freeram` (memoria RAM disponible en páginas). Estas cantidades están en unidades de páginas de memoria.
    - **Cálculo de Memoria Usada:** Calcula la memoria utilizada restando la memoria libre del total: `(total_mem - free_mem)`. Esta operación proporciona la cantidad de memoria actualmente en uso por el sistema, incluyendo procesos, cache del kernel y buffers.
    - **Cálculo de Porcentajes:** Realiza una división protegida verificando que `total_mem > 0` para evitar división por cero (aunque en la práctica esto nunca debería ocurrir en un sistema funcional). Calcula el porcentaje usado mediante la fórmula: `((memoria_usada * 100) / total_mem)`, donde la multiplicación por 100 se realiza antes de la división para mantener precisión en aritmética entera.
    - **Complemento de Porcentaje:** Calcula el porcentaje no usado como `100 - porcentaje_usado`, garantizando que ambos porcentajes sumen exactamente 100% y proporcionando una visión completa del estado de la memoria.
    - **Transferencia Segura:** Utiliza `copy_to_user()` para copiar la estructura `info_k` desde el espacio del kernel al buffer proporcionado por el usuario (`info`). Si la copia falla debido a una dirección inválida o permisos insuficientes, retorna `-EFAULT`.
    - **Retorno:** Devuelve `0` en caso de éxito, indicando que los porcentajes fueron calculados y transferidos correctamente al espacio de usuario.

---

## Implementación de sys_control_mouse_click

La syscall `sys_control_mouse_click` permite controlar el mouse del sistema de forma remota, incluyendo movimiento del cursor y ejecución de clicks izquierdos y derechos. Esta implementación se encuentra en el archivo `kernel/syscall_pf_3.c` y utiliza el subsistema de entrada del kernel para crear un dispositivo de mouse virtual.

### Fragmento de Código Relevante

```c
static struct input_dev *mouse_click_202201524;
static int ancho_pantalla = 0, alto_pantalla = 0;

int obtener_resolucion_pantalla(int *ancho, int *alto)
{
    struct file *f;
    char buf[64];
    loff_t posicion = 0;
    ssize_t resultado;
    f = filp_open("/sys/class/drm/card0-Virtual-1/modes", O_RDONLY, 0);
    if (IS_ERR(f))
        return PTR_ERR(f);
    resultado = kernel_read(f, buf, sizeof(buf) - 1, &posicion);
    filp_close(f, NULL);
    if (resultado <= 0)
        return -EFAULT;
    buf[resultado] = '\0';
    if (sscanf(buf, "%dx%d", ancho, alto) != 2)
        return -EINVAL;
    return 0;
}

static int __init mouse_click_init(void)
{
    int error;
    mouse_click_202201524 = input_allocate_device();
    if (!mouse_click_202201524) {
        pr_err("Error: No se pudo alocar el dispositivo de mouse click.\n");
        return -ENOMEM;
    }
    mouse_click_202201524->name = "Mouse Click Virtual 202201524";
    mouse_click_202201524->phys = "202201524/virtual_mouse_click";
    mouse_click_202201524->id.bustype = BUS_VIRTUAL;

    __set_bit(INPUT_PROP_POINTER, mouse_click_202201524->propbit);
    __set_bit(INPUT_PROP_DIRECT, mouse_click_202201524->propbit);

    input_set_capability(mouse_click_202201524, EV_ABS, ABS_X);
    input_set_capability(mouse_click_202201524, EV_ABS, ABS_Y);

    input_set_capability(mouse_click_202201524, EV_KEY, BTN_LEFT);
    input_set_capability(mouse_click_202201524, EV_KEY, BTN_RIGHT);

    if (obtener_resolucion_pantalla(&ancho_pantalla, &alto_pantalla) == 0) {
        pr_info("Info: Resolucion De Pantalla Detectada: %dx%d\n", ancho_pantalla, alto_pantalla);
    }

    input_set_abs_params(mouse_click_202201524, ABS_X, 0, ancho_pantalla - 1, 0, 0);
    input_set_abs_params(mouse_click_202201524, ABS_Y, 0, alto_pantalla - 1, 0, 0);

    error = input_register_device(mouse_click_202201524);
    if (error) {
        pr_err("Error: No se pudo registrar el mouse click virtual: %d.\n", error);
        input_free_device(mouse_click_202201524);
        mouse_click_202201524 = NULL;
        return error;
    }
    pr_info("Info: Mouse click virtual inicializado correctamente.\n");
    return 0;
}

device_initcall(mouse_click_init);

SYSCALL_DEFINE3(control_mouse_click, int, pos_x_abs, int, pos_y_abs, int, tipo_clic)
{
    if (!mouse_click_202201524)
        return -ENODEV;

    if (pos_x_abs < 0) pos_x_abs = 0;
    if (pos_y_abs < 0) pos_y_abs = 0;
    if (pos_x_abs >= ancho_pantalla) pos_x_abs = ancho_pantalla - 1;
    if (pos_y_abs >= alto_pantalla) pos_y_abs = alto_pantalla - 1;

    pr_info("Info: Moviendo Mouse a X, Y: (%d,%d), Click: (%d).\n", pos_x_abs, pos_y_abs, tipo_clic);
    input_report_abs(mouse_click_202201524, ABS_X, pos_x_abs);
    input_report_abs(mouse_click_202201524, ABS_Y, pos_y_abs);
    //CLICK IZQUIERDO = 1, CLICK DERECHO = 2
    if (tipo_clic == 1) {
        input_report_key(mouse_click_202201524, BTN_LEFT, 1);
        input_sync(mouse_click_202201524);
        msleep(20);
        input_report_key(mouse_click_202201524, BTN_LEFT, 0);
    } else if (tipo_clic == 2) {
        input_report_key(mouse_click_202201524, BTN_RIGHT, 1);
        input_sync(mouse_click_202201524);
        msleep(20);
        input_report_key(mouse_click_202201524, BTN_RIGHT, 0);
    }
    input_sync(mouse_click_202201524);

    return 0;
}
```

### Explicación de la Implementación

**Inicialización y Estructura de Datos:**

- **Propósito:** Crear y registrar un dispositivo de entrada virtual que permita inyectar eventos de mouse en el sistema operativo de manera transparente para las aplicaciones gráficas.
- **Funcionamiento:**
    - `mouse_click_202201524` es un puntero estático global a la estructura `input_dev` que representa el dispositivo de mouse virtual.
    - Las variables estáticas `ancho_pantalla` y `alto_pantalla` almacenan la resolución detectada del display, con valores por defecto de 1718x888.
    - La función `obtener_resolucion_pantalla()` lee dinámicamente la resolución desde el sistema de archivos virtual DRM (`/sys/class/drm/card0-Virtual-1/modes`), abriendo el archivo con `filp_open()`, leyendo su contenido con `kernel_read()`, y parseando el formato "WIDTHxHEIGHT" con `sscanf()`.

**Inicialización del Dispositivo (mouse_click_init):**

- **Propósito:** Configurar y registrar el dispositivo de mouse virtual durante el arranque del kernel.
- **Funcionamiento:**
    - **Asignación:** Llama a `input_allocate_device()` para reservar memoria para la estructura del dispositivo. Si falla, retorna `-ENOMEM`.
    - **Configuración de Identidad:** Asigna nombre descriptivo ("Mouse Click Virtual 202201524"), identificador físico único, y tipo de bus (`BUS_VIRTUAL`) para identificarlo como dispositivo virtual.
    - **Propiedades:** Establece bits de propiedades con `__set_bit()` para indicar que es un dispositivo puntero (`INPUT_PROP_POINTER`) con entrada directa (`INPUT_PROP_DIRECT`).
    - **Capacidades:** Registra capacidades con `input_set_capability()` para eventos absolutos en ejes X e Y (`EV_ABS`, `ABS_X`, `ABS_Y`) y botones izquierdo y derecho (`EV_KEY`, `BTN_LEFT`, `BTN_RIGHT`).
    - **Detección de Resolución:** Invoca `obtener_resolucion_pantalla()` para actualizar los valores de ancho y alto desde el sistema DRM.
    - **Límites de Ejes:** Configura rangos válidos para coordenadas con `input_set_abs_params()`, estableciendo mínimo 0 y máximo `ancho_pantalla - 1` / `alto_pantalla - 1` sin valores de fuzz o flat.
    - **Registro:** Registra el dispositivo con `input_register_device()`. Si falla, libera la memoria y retorna el error.
    - **Inicialización Automática:** La macro `device_initcall(mouse_click_init)` asegura que la función se ejecute automáticamente durante la fase de inicialización de dispositivos del kernel.

**Lógica de la Syscall:**

- **Propósito:** Mover el cursor a coordenadas absolutas y opcionalmente ejecutar un click.
- **Funcionamiento:**
    - **Validación de Dispositivo:** Verifica que `mouse_click_202201524` esté inicializado. Si es NULL, retorna `-ENODEV` indicando que el dispositivo no está disponible.
    - **Normalización de Coordenadas:** Aplica clipping a las coordenadas recibidas para garantizar que estén dentro de los límites de la pantalla:
        - Si `pos_x_abs < 0`, se ajusta a 0.
        - Si `pos_x_abs >= ancho_pantalla`, se ajusta a `ancho_pantalla - 1`.
        - El mismo proceso se aplica para `pos_y_abs` con `alto_pantalla`.
    - **Registro de Movimiento:** Reporta las coordenadas absolutas al subsistema de entrada con `input_report_abs()` para los ejes X e Y, moviendo efectivamente el cursor a la posición especificada.
    - **Simulación de Click:** Evalúa el parámetro `tipo_clic`:
        - **Click Izquierdo (1):** Reporta presión del botón izquierdo (`BTN_LEFT`, valor 1), sincroniza con `input_sync()`, espera 20ms con `msleep()` para simular la duración física del click, y reporta liberación del botón (valor 0).
        - **Click Derecho (2):** Mismo proceso con `BTN_RIGHT`.
        - **Solo Movimiento (0 u otros):** No ejecuta ningún click, solo mueve el cursor.
    - **Sincronización Final:** Llama a `input_sync()` para notificar al subsistema de entrada que el paquete de eventos está completo y debe procesarse.
    - **Logging:** Utiliza `pr_info()` para registrar cada acción en el log del kernel, facilitando la depuración.
    - **Retorno:** Devuelve 0 en caso de éxito.

---

## Implementación del Backend (API)

La API desarrollada en C/C++ actúa como intermediaria entre la interfaz web y las llamadas al sistema del kernel, implementando autenticación PAM y control de acceso basado en grupos.

### Arquitectura del Backend

[DESCRIPCIÓN DE LA ARQUITECTURA]

### Sistema de Autenticación con PAM

[EXPLICACIÓN DE LA IMPLEMENTACIÓN PAM]

### Fragmento de Código Relevante

```c
// [CÓDIGO DEL BACKEND]
```

### Explicación de la Implementación

**Autenticación:**

- **Propósito:** [EXPLICACIÓN]
- **Funcionamiento:** [DETALLES]

**Control de Acceso:**

- **Propósito:** [EXPLICACIÓN]
- **Funcionamiento:** [DETALLES]

**Endpoints de la API:**

- [DESCRIPCIÓN DE ENDPOINTS]

---

## Implementación del Frontend (Aplicación Web)

La aplicación web proporciona una interfaz intuitiva para interactuar con el sistema remoto, permitiendo visualización del escritorio, control del mouse y teclado, y monitorización de recursos.

### Tecnologías Utilizadas

- [LISTA DE TECNOLOGÍAS]

### Arquitectura del Frontend

[DESCRIPCIÓN DE LA ARQUITECTURA]

### Funcionalidades Implementadas

**Visualización del Escritorio en Tiempo Real:**

- **Implementación:** [DETALLES]
- **Tasa de refresco:** [ESPECIFICACIONES]

**Control de Mouse:**

- **Implementación:** [DETALLES]
- **Funcionalidades:** Movimiento, click izquierdo, click derecho

**Control de Teclado:**

- **Implementación:** [DETALLES]
- **Funcionalidades:** Teclas alfanuméricas

**Monitorización de Recursos:**

- **CPU:** [DETALLES]
- **Memoria RAM:** [DETALLES]

### Fragmento de Código Relevante

```javascript
// [CÓDIGO DEL FRONTEND]
```

### Capturas de Pantalla

**Vista de Login:**

[DESCRIPCIÓN O IMAGEN]

**Vista de Dashboard:**

[DESCRIPCIÓN O IMAGEN]

**Vista de Control Remoto:**

[DESCRIPCIÓN O IMAGEN]

---

## Reporte de Errores

Durante el desarrollo del proyecto USACLinux, se encontraron diversos desafíos técnicos relacionados con la implementación de syscalls, la integración con PAM y el desarrollo de la interfaz web. A continuación, se detallan tres errores significativos, las fuentes consultadas y las soluciones aplicadas.

### Error 1: [TÍTULO DEL ERROR]

**Información del Error:**

[DESCRIPCIÓN DETALLADA DEL ERROR CON MENSAJE DE ERROR]

**Fuentes que Ayudaron a Resolverlo:**

- [FUENTE 1]
- [FUENTE 2]
- [FUENTE 3]

**Código Antes de la Solución:**

```c
// [CÓDIGO CON ERROR]
```

**Código Después de la Solución:**

```c
// [CÓDIGO CORREGIDO]
```

**Solución Encontrada:**

[EXPLICACIÓN DETALLADA DE LA SOLUCIÓN]

---

### Error 2: [TÍTULO DEL ERROR]

**Información del Error:**

[DESCRIPCIÓN DETALLADA DEL ERROR]

**Fuentes que Ayudaron a Resolverlo:**

- [FUENTE 1]
- [FUENTE 2]
- [FUENTE 3]

**Código Antes de la Solución:**

```c
// [CÓDIGO CON ERROR]
```

**Código Después de la Solución:**

```c
// [CÓDIGO CORREGIDO]
```

**Solución Encontrada:**

[EXPLICACIÓN DETALLADA DE LA SOLUCIÓN]

---

### Error 3: [TÍTULO DEL ERROR]

**Información del Error:**

[DESCRIPCIÓN DETALLADA DEL ERROR]

**Fuentes que Ayudaron a Resolverlo:**

- [FUENTE 1]
- [FUENTE 2]
- [FUENTE 3]

**Código Antes de la Solución:**

```c
// [CÓDIGO CON ERROR]
```

**Código Después de la Solución:**

```c
// [CÓDIGO CORREGIDO]
```

**Solución Encontrada:**

[EXPLICACIÓN DETALLADA DE LA SOLUCIÓN]

---

## Conclusiones

[CONCLUSIONES DEL PROYECTO]

---

Ahora puedes compartirme los detalles de cada syscall (syscall_pf_1.c, syscall_pf_2.c, syscall_pf_3.c) para que complete las secciones correspondientes con los objetivos y explicaciones de implementación.