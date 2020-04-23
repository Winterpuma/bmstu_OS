#include <linux/module.h>
#include <init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>

#define COOKIE_BUF_SIZE PAGE_SIZE

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M.");

int fortune_init(void);
ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos);
ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos);
void fortune_exit(void);

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = fortune_read,
    .write = fortune_write,
};

char *cookie_buf;
struct proc_dir_entry *proc_file;
unsigned int read_index;
unsigned int write_index;


int fortune_init(void)
{
    cookie_buf = vmalloc(COOKIE_BUF_SIZE);

    if (!cookie_buf)
    {
        printk(KERN_INFO "Error: can't malloc cookie buffer\n");
        return -ENOMEM;
    }

    memset(cookie_buf, 0, COOKIE_BUF_SIZE);
    proc_file = proc_create("fortune", 0666, NULL, &fops);

    if (!proc_file)
    {
        vfree(cookie_buf);
        printk(KERN_INFO "Error: can't create fortune file\n");
        return -ENOMEM;
    }

    read_index = 0;
    write_index = 0;

    proc_mkdir("Dir_fort", NULL);
    proc_symlink("Symbolic_fort", NULL, "/proc/fortune");

    printk(KERN_INFO "Fortune module loaded successfully\n");
    return 0;
}


ssize_t fortune_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
    int len;

    if (write_index == 0 || *f_pos > 0)
        return 0;

    if (read_index >= write_index)
        read_index = 0;

    len = sprintf(buf, "%s\n", &cookie_buf[read_index]);
    read_index += len;
    *f_pos += len;

    return len;
}


ssize_t fortune_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
    int free_space = (COOKIE_BUF_SIZE - write_index) + 1;

    if (count > free_space)
    {
        printk(KERN_INFO "Error: cookie pot is full\n");
        return -ENOSPC;
    }

    if (copy_from_user(&cookie_buf[write_index], buf, count))
        return -EFAULT;

    write_index += count;
    cookie_buf[write_index-1] = 0;

    return count;
}


void fortune_exit(void)
{
    remove_proc_entry("fortune", NULL);

    if (cookie_buf)
        vfree(cookie_buf);

    printk(KERN_INFO "Fortune module unloaded\n");
}

module_init(fortune_init);
module_exit(fortune_exit);


