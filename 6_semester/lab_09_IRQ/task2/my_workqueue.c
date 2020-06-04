#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define SHARED_IRQ 1

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M");

static int my_dev_id;
static int irq_call_n = 0;

struct workqueue_struct *wq;

void my_workqueue_function(struct work_struct *work) 
{
	printk(KERN_INFO "LAB9 workqueue: counter %d\n", ++irq_call_n);
}

DECLARE_WORK(my_workqueue, my_workqueue_function);

static irqreturn_t my_interrupt(int irq, void *dev_id) 
{
	if (irq == SHARED_IRQ) 
	{
		queue_work(wq, &my_workqueue); 
		printk(KERN_INFO "LAB9 workqueue in my_interrupt\n");
		return IRQ_HANDLED;
	} 
	else 
		return IRQ_NONE;
}

static int __init my_workqueue_init(void) 
{
	if (request_irq(SHARED_IRQ, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
	{
		printk(KERN_ERR "LAB9 error: can't register handler\n");
		return -1;
	}
	
	wq = create_workqueue("workqueue");
	if (wq) 
		printk(KERN_INFO "LAB9 Workqueue created!\n"); 
	else
	{
		free_irq(SHARED_IRQ, &my_dev_id);
		printk(KERN_ERR "LAB9 error: can't create workqueue\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "LAB9 Module loaded!\n");

	return 0; 
}

static void __exit my_workqueue_exit(void) 
{
	flush_workqueue(wq);
	destroy_workqueue(wq);
	free_irq(SHARED_IRQ, &my_dev_id); 
	printk(KERN_INFO "LAB9 Module unloaded\n");
}

module_init(my_workqueue_init)
module_exit(my_workqueue_exit)
