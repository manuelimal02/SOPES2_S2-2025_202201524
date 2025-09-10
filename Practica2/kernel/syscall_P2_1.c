#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>

struct ipc_msg {
    struct list_head list;
    char *data;
    size_t len;
};

static struct list_head msg_queue = LIST_HEAD_INIT(msg_queue);
static DEFINE_MUTEX(ipc_mutex);
static DECLARE_WAIT_QUEUE_HEAD(ipc_wait);

SYSCALL_DEFINE2(ipc_channel_send, const void __user *, msg, size_t, len) {
    struct ipc_msg *new_msg = kmalloc(sizeof(*new_msg), GFP_KERNEL);
    if (!new_msg) return -ENOMEM;

    new_msg->data = kmalloc(len, GFP_KERNEL);
    if (!new_msg->data) {
        kfree(new_msg);
        return -ENOMEM;
    }

    if (copy_from_user(new_msg->data, msg, len)) {
        kfree(new_msg->data);
        kfree(new_msg);
        return -EFAULT;
    }
    new_msg->len = len;

    mutex_lock(&ipc_mutex);
    list_add_tail(&new_msg->list, &msg_queue);
    mutex_unlock(&ipc_mutex);

    wake_up_interruptible(&ipc_wait);
    return 0;
}

SYSCALL_DEFINE2(ipc_channel_receive, void __user *, msg, size_t __user *, len) {
    struct ipc_msg *m;
    int ret = 0;

    wait_event_interruptible(ipc_wait, !list_empty(&msg_queue));

    mutex_lock(&ipc_mutex);
    if (list_empty(&msg_queue)) {
        ret = -EAGAIN;
        goto out;
    }

    m = list_first_entry(&msg_queue, struct ipc_msg, list);
    list_del(&m->list);

    if (copy_to_user(len, &m->len, sizeof(size_t))) {
        ret = -EFAULT;
        goto free;
    }
    if (copy_to_user(msg, m->data, m->len)) {
        ret = -EFAULT;
        goto free;
    }

free:
    kfree(m->data);
    kfree(m);
out:
    mutex_unlock(&ipc_mutex);
    return ret;
}