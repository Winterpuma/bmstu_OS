#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M");


static int __init md_init(void) 
{ 
   printk("LAB3: Hello world!\n"); 
   return 0; 
}

static void __exit md_exit(void) 
{ 
   printk("LAB3: Goodbye\n"); 
} 

module_init(md_init); 
module_exit(md_exit); 
                         