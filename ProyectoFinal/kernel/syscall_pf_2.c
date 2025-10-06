#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

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