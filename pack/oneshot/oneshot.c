#include <asm/unistd.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/wait.h>

#define MODULE_NAME "oneshot"
MODULE_LICENSE("GPL");

extern atomic_t oneshot_interval;
extern atomic_t oneshot_repeat;
extern atomic_t oneshot_delay;
extern atomic_t oneshot_finish;
extern wait_queue_head_t wq;
extern u64 res_steps, res_energy;

ssize_t oneshot_proc_write(struct file *fp, const char __user *src,
                           size_t sz, loff_t *offset)
{
        char buf[100] = {0};
        ulong copy = min(sz, sizeof(buf));
        ulong interval = 0, delay = 0, repeat = 0;

        if (copy_from_user(buf, src, sz)){
                pr_err("copy_from_user failed\n");
                return -EFAULT;
        }

        buf[copy] = 0;
        if(3 != sscanf(buf, "%lu %lu %lu", &interval, &delay, &repeat))
                return -EFAULT;
        if (interval < 0)
                interval = 0;

        pr_info("oneshot starts with interval=%lu delay=%lu repeat=%lu\n", interval, delay, repeat);
        atomic_set(&oneshot_repeat, repeat);
        atomic_set(&oneshot_delay, delay);
        atomic_set(&oneshot_interval, interval);
        atomic_set(&oneshot_finish, 0);

        return copy;
}

ssize_t oneshot_proc_read(struct file *fp, char __user *to,
                           size_t sz, loff_t *offset)
{
        u8 buf[100] = {0};

        if(sz < 2 * sizeof(u64))
                return -EFAULT;

        wait_event_interruptible(wq, atomic_read(&oneshot_finish) != 0);
        atomic_set(&oneshot_finish, 0);

        *(u64*)buf = res_steps;
        *(u64*)(buf + sizeof(u64)) = res_energy;
        if(copy_to_user(to, buf, 2 * sizeof(u64))){
                pr_err("copy_to_user failed\n");
                return -EFAULT;
        }

        return 2 * sizeof(u64);
}

static const struct file_operations my_proc_fops = {
        .owner = THIS_MODULE,
        .write = oneshot_proc_write,
        .read = oneshot_proc_read
};

int __init oneshot_init(void) {
        struct proc_dir_entry *oneshot_proc = NULL;
        oneshot_proc = proc_create("oneshot", 0x777, NULL, &my_proc_fops);

        if (oneshot_proc == NULL)
                return -ENOMEM;

        pr_info("oneshot init\n");
        return 0;
}

void __exit oneshot_exit(void) {
        remove_proc_entry("oneshot", NULL);
        pr_info("oneshot exit\n");
}

module_init(oneshot_init);
module_exit(oneshot_exit);
