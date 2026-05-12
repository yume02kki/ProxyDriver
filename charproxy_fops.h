#ifndef CHARPROXY_H
#define CHARPROXY_H

#include <linux/fs.h>

#define DEVICE_NAME "charproxy"
#define BUF_SIZE 128

extern const struct file_operations charproxy_fops;
long charproxy_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif
