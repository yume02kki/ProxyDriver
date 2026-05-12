#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/pid.h>

#include "charproxy_fops.h"
#include "ioctl_proxy.h"

static dev_t dev_num;
static struct cdev charproxy_cdev;
static struct class *charproxy_class;

long charproxy_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct proxy_request req;
    struct pid *pid_struct;
    struct task_struct *task;
    u8 *kbuf;
    ssize_t ret;

    if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
        return -EFAULT;

    kbuf = kmalloc(req.size, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    pid_struct = find_get_pid(req.pid);
    if (!pid_struct) {
        kfree(kbuf);
        return -ESRCH;
    }

    task = get_pid_task(pid_struct, PIDTYPE_PID);
    put_pid(pid_struct);
    if (!task) {
        kfree(kbuf);
        return -ESRCH;
    }

    switch (cmd) {
        case IOCTL_READ:
            ret = access_process_vm(task, req.addr, kbuf, req.size, 0);
            if (ret > 0 && copy_to_user(req.buf, kbuf, ret))
                ret = -EFAULT;
            break;
        case IOCTL_WRITE:
            if (copy_from_user(kbuf, req.buf, req.size))
                ret = -EFAULT;
            else
                ret = access_process_vm(task, req.addr, kbuf, req.size, FOLL_WRITE);
            break;
        default:
            ret = -ENOTTY;
    }

    put_task_struct(task);
    kfree(kbuf);
    return ret;
}

static int __init charproxy_init(void)
{
    int err;

    err = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (err < 0)
        return err;

    cdev_init(&charproxy_cdev, &charproxy_fops);
    err = cdev_add(&charproxy_cdev, dev_num, 1);
    if (err < 0) {
        unregister_chrdev_region(dev_num, 1);
        return err;
    }

    charproxy_class = class_create(DEVICE_NAME);
    if (IS_ERR(charproxy_class)) {
        cdev_del(&charproxy_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(charproxy_class);
    }

    device_create(charproxy_class, NULL, dev_num, NULL, DEVICE_NAME);
    printk(KERN_INFO "charproxy: loaded (major %d)\n", MAJOR(dev_num));
    return 0;
}

static void __exit charproxy_exit(void)
{
    device_destroy(charproxy_class, dev_num);
    class_destroy(charproxy_class);
    cdev_del(&charproxy_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "charproxy: unloaded\n");
}

module_init(charproxy_init);
module_exit(charproxy_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("charproxy driver");
