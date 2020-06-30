#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M");

static int __init md_init(void) 
{
	printk("LAB3: module is loaded.\n");
	struct task_struct *task = &init_task;
 	do 
	{
 		printk("LAB3:---%s-%d, parent %s-%d", task->comm,
 		task->pid, task->parent->comm, task->parent->pid);
 	} while ((task = next_task(task)) != &init_task);
 	printk("LAB3:---%s-%d, parent %s-%d", current->comm,
 	current->pid, current->parent->comm, current->parent->pid);
 	return 0;
}

static void __exit md_exit(void) 
{
	printk("LAB3: module is unloaded.\n");
}

module_init(md_init);
module_exit(md_exit);