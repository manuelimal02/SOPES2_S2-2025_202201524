#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/delay.h>

static struct input_dev *mouse_click_202201524;
static int ancho_pantalla = 1718, alto_pantalla = 888;

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