#include <linux/init.h> 
#include <linux/module.h> 
#include "md.h" 

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M");

static int __init md_init(void) 
{ 
   printk("LAB3: module md2 start!\n"); 
   printk("LAB3: data string exported from md1 : %s\n", md1_data); 
   printk("LAB3: string returned md1_proc() is : %s\n", md1_proc()); 
   return 0; 
} 

static void __exit md_exit(void) 
{ 
   printk("LAB3: module md2 unloaded!\n"); 
} 

module_init(md_init);
module_exit(md_exit);

