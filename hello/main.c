#include <linux/module.h>
#include <linux/kernel.h>

int __init driver_init(void)
{
  printk(KERN_INFO "Hello world, loaded!");
  return 0;
}

void __exit driver_exit(void)
{
  printk(KERN_INFO "Goodby world!");
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");