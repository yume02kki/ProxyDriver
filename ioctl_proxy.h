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

struct vma_entry {
    uint64_t start;
    uint64_t end;
    uint32_t prot; /* VM_READ=1, VM_WRITE=2, VM_EXEC=4 */
};

struct dump_maps_request {
    int pid;
    uint32_t max_entries; /* max entries buf can hold */
    uint32_t count;       /* filled by kernel: actual number of VMAs */
    struct vma_entry *entries;
};

#define IOCTL_READ      _IOR(IOCTL_MAGIC, 1, struct proxy_request)
#define IOCTL_WRITE     _IOW(IOCTL_MAGIC, 2, struct proxy_request)
#define IOCTL_DUMP_MAPS _IOWR(IOCTL_MAGIC, 3, struct dump_maps_request)

#endif
