#ifndef MY_IOCTL_H
#define MY_IOCTL_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#endif

#define IOCTL_MAGIC 'k'

struct proxy_request {
    int pid;
    uint64_t addr;
    uint64_t size;
    void *buf;
};

#define IOCTL_READ  _IOR(IOCTL_MAGIC, 1, struct proxy_request)
#define IOCTL_WRITE _IOW(IOCTL_MAGIC, 2, struct proxy_request)

#endif
