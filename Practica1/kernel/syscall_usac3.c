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