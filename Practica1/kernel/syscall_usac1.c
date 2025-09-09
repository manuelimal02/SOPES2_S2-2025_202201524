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