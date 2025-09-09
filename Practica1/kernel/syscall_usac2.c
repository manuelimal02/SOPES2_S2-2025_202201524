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