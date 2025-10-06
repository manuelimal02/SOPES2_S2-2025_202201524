#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/tick.h>

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