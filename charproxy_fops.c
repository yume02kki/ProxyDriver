#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "charproxy_fops.h"

static char buffer[BUF_SIZE] = "hello from charproxy\n";
static size_t buffer_len = 21;

static int open(struct inode *inode, struct file *file) {
    return 0;
}

static int release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t read(struct file *f, char __user *buf, size_t size, loff_t *off) {
    if (*off >= buffer_len) return 0;
    size_t to_copy = min(size, buffer_len - (size_t)*off);

    if (copy_to_user(buf, buffer + *off, to_copy))
        return -EFAULT;

    *off += to_copy;
    return to_copy;
}

static ssize_t write(struct file *f, const char __user *buf, size_t size, loff_t *off) {
    if (*off >= buffer_len) return 0;
    size_t to_copy = min(size, buffer_len - (size_t)*off);

    if (copy_from_user(buffer + *off, buf, to_copy))
        return -EFAULT;

    *off += to_copy;
    return to_copy;
}


const struct file_operations charproxy_fops = {
    .owner = THIS_MODULE,
    .open = open,
    .release = release,
    .read = read,
    .write = write,
    .unlocked_ioctl = charproxy_ioctl,
};
