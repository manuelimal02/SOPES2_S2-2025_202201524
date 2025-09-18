#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <drm/drm_device.h>
#include <drm/drm_crtc.h>
#include <drm/drm_plane.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_modeset_lock.h>
#include <linux/iosys-map.h>

struct datos_captura_pantalla {
    int ancho;
    int alto;
    int bits_pixel;
    char __user *buffer_datos;
    size_t tamano_buffer;
};

extern struct fb_info *registered_fb[];
extern int num_registered_fb;

SYSCALL_DEFINE1(capture_screen, struct datos_captura_pantalla __user *, datos_usuario) {
    struct datos_captura_pantalla captura;
    struct drm_modeset_acquire_ctx bloqueo;
    struct drm_device *dispositivo;
    struct drm_plane *plano;
    struct drm_framebuffer *marco = NULL;
    struct iosys_map mapeo[DRM_FORMAT_MAX_PLANES]; 
    size_t tamano;
    u32 ancho, alto;
    int error;

    if (copy_from_user(&captura, datos_usuario, sizeof(captura)))
        return -EFAULT;
    if (num_registered_fb <= 0 || !registered_fb[0]) return -ENODEV;
    struct drm_fb_helper *ayudante = (struct drm_fb_helper *)registered_fb[0]->par;
    dispositivo = ayudante ? ayudante->dev : NULL;
    if (!dispositivo) return -ENODEV;

    drm_modeset_acquire_init(&bloqueo, 0);
    error = drm_modeset_lock_all_ctx(dispositivo, &bloqueo);
    if (!error) {
        drm_for_each_plane(plano, dispositivo) {
            if (plano->type == DRM_PLANE_TYPE_PRIMARY && plano->state && plano->state->fb) {
                marco = plano->state->fb;
                drm_framebuffer_get(marco);
                ancho = marco->width;
                alto = marco->height;
                break;
            }
        }
        drm_modeset_drop_locks(&bloqueo);
    }
    drm_modeset_acquire_fini(&bloqueo);
    
    if (!marco) return -ENODEV;

    tamano = (size_t)marco->pitches[0] * marco->height;
    if (captura.tamano_buffer < tamano) {
        drm_framebuffer_put(marco);
        return -ENOSPC;
    }

    error = drm_gem_fb_vmap(marco, mapeo, NULL);
    if (!error && !copy_to_user(captura.buffer_datos, mapeo[0].vaddr, tamano)) {
        captura.ancho = ancho;
        captura.alto = alto;
        captura.bits_pixel = marco->format->cpp[0] * 8;
        captura.tamano_buffer = tamano;
        error = copy_to_user(datos_usuario, &captura, sizeof(captura));
    }

    drm_gem_fb_vunmap(marco, mapeo);
    drm_framebuffer_put(marco);
    
    return error ? -EFAULT : 0;
}