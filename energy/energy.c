#include <asm/unistd.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/delay.h>

#define MODULE_NAME "energy"
MODULE_LICENSE("GPL");

#define CORE_ENERGY_MSR 0x0C001029AULL

static u64 start = 0, stop = 0;

int __init energy_init(void) {
        rdmsrl(CORE_ENERGY_MSR, start);
        mb();

        mdelay(3000);

        rdmsrl(CORE_ENERGY_MSR, stop);

        pr_info("energy: %016llx %016llx %llu\n",
                start, stop, stop-start);

        return 0;
}

void __exit energy_exit(void) {
        pr_info("energy exit\n");
}

module_init(energy_init);
module_exit(energy_exit);
