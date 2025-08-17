#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/fb.h>

extern struct fb_info *registered_fb[FB_MAX];
extern int num_registered_fb;

SYSCALL_DEFINE2(get_screen_resolution, int __user *, width, int __user *, height)
{
    int w = 0, h = 0;

    if (!width || !height) {
        pr_err("Error: Punteros width o height invalidos.\n");
        return -EINVAL;
    }

#if IS_ENABLED(CONFIG_FB)
    if (num_registered_fb > 0 && registered_fb[0]) {
        struct fb_info *info = registered_fb[0];
        w = info->var.xres;
        h = info->var.yres;
    } else {
        pr_err("Error: No se encontró dispositivo framebuffer activo.\n");
        return -ENODEV;
    }
#else
    pr_err("Error: Subsistema framebuffer no habilitado en el kernel.\n");
    return -ENODEV;
#endif

    if (w <= 0 || h <= 0) {
        pr_err("Error: Resolucion invalida obtenida (%dx%d).\n", w, h);
        return -ENODEV;
    }

    if (copy_to_user(width, &w, sizeof(int)) || copy_to_user(height, &h, sizeof(int))) {
        pr_err("Error: Error al copiar resolución al espacio de usuario.\n");
        return -EFAULT;
    }

    pr_info("Info: Resolucion obtenida: %dx%d\n", w, h);
    return 0;
}