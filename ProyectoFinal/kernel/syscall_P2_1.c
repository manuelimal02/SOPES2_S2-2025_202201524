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

SYSCALL_DEFINE2(ipc_channel_send, const void __user *, mensaje_usuario, size_t, longitud_mensaje) {
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

SYSCALL_DEFINE2(ipc_channel_receive, void __user *, mensaje_usuario, size_t __user *, longitud_usuario) {
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