#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/slab.h>

#define MYFS_MAGIC_NUMBER 0x13131313;
#define SLABNAME "my_cache"

static int sco = 0; 

static struct kmem_cache *cache = NULL;
static void* *line = NULL; 

static int size = 7;
module_param(size, int, 0); 
static int number = 31; 
module_param(number, int, 0); 


int free_allocated_inodes(struct inode *inode)
{
    kmem_cache_free(cache, inode->i_private);
    return 1;
}


// Деструктор суперблока; будет вызван внутри kill_block_super перед уничтожением структуры super_block (при размонтировании ФС)
static void myfs_put_super(struct super_block *sb)
{
    printk(KERN_DEBUG "MYFS super block destroyed\n");
}


static struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs, // заглушка из libfs
    .drop_inode = free_allocated_inodes,  
};


struct myfs_inode
{
    int i_mode;
    unsigned long i_ino;
} myfs_inode;


// Размещает новую структуру inode и заполняет ее значениями
static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
    struct inode *ret = new_inode(sb);
    
    if (ret)
    {
        inode_init_owner(ret, NULL, mode);
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        ret->i_private = &myfs_inode;
    }
    return ret;
}


// Выполняет построение корневого каталога ФС
static int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
    struct inode *root = NULL;

    // Заполняется структура super_block
    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC_NUMBER; // магическое число, по которому драйвер ФС может проверить, что на диске хранится именно та самая ФС, а не прочие данные
    sb->s_op = &myfs_super_ops; // операции для суперблока

    // Построение корневого каталога ФС
    root = myfs_make_inode(sb, S_IFDIR|0755); // создается inode корневого каталога; sb - указатель на суперблок; S_IFDIR значит, что создаем каталог
    if (!root)
    {
        printk(KERN_ERR "MYFS inode allocation failed\n");
        return -ENOMEM;
    }
    
    // файловые и inode операции взяты из libfs (предоставляются ядром)
    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    // Для корневого каталога создается структура dentry, через которую он помещается в directory-кэш
    sb->s_root = d_make_root(root); // устанавливаем указатель на dentry корневого каталога

    if (!sb->s_root)
    {
        printk(KERN_ERR "MYFS root creation failed\n");
        iput(root);
        return -ENOMEM;
    }
    return 0;
}


// Примонтирует устройство и возвращает структуру, описывающую корневой каталог ФС
static struct dentry* myfs_mount(struct file_system_type * type, int flags, char const *dev, void *data)
{
    struct dentry *const entry = mount_bdev(type, flags, dev, data, myfs_fill_sb); // myfs_fill_sb - ук. на ф-ю, которая будет вызвана из mount_bdev, чтобы проинициализировать суперблок
    // dentry представляет корневой каталог ФС
    if (IS_ERR(entry))
        printk(KERN_ERR "MYFS mounting failed!\n");
    else
        printk(KERN_DEBUG "MYFS mounted\n");
    return entry;
}


// Описывает создаваемую ФС
static struct file_system_type myfs_type = {
    .owner = THIS_MODULE, // отвечает за счетчик ссылок на модуль, чтобы его нельзя было случайно выгрузить
    .name = "myfs", // имя ФС; используется при монтировании
    .mount = myfs_mount, // указатель на ф-ю; исп. при монтировании
    .kill_sb = kill_block_super, // указатель на ф-ю; исп. при размонтировании; kill_block_super предоставляет ядро
};


void co (void *p)
{
    *(int *)p = (int)p;
    sco++;
}


// Инициализация модуля
static int __init myfs_init(void)
{
    int i, ret; 
        
    if(size < 0) 
    { 
        printk(KERN_ERR "MYFS invalid argument %d\n", size); 
        return -EINVAL; 
    } 
    
    line = kmalloc(sizeof(void*) * number, GFP_KERNEL); 
    if(!line) 
    { 
        printk(KERN_ERR "MYFS kmalloc error\n"); 
        kfree(line); 
        return -ENOMEM; 
    } 
    for(i = 0; i < number; i++) 
        line[i] = NULL; 
    
    cache = kmem_cache_create(SLABNAME, sizeof(struct myfs_inode), 0, 0, co); // создание кэша slab
	if (!cache)
    {
        printk(KERN_ERR "MYFS_MODULE cannot allocate cache\n");
        kmem_cache_destroy(cache); 
        return -ENOMEM;
    }
    for(i = 0; i < number; i++) 
    {
        if(NULL == (line[i] = kmem_cache_alloc(cache, GFP_KERNEL))) 
        { 
            printk(KERN_ERR "MYFS kmem_cache_alloc error\n"); 
            for(i = 0; i < number; i++) 
            kmem_cache_free(cache, line[i]);
        } 
    }
    
    ret = register_filesystem(&myfs_type);
    if (ret != 0)
    {
        printk(KERN_ERR "MYFS_MODULE cannot register filesystem\n");
        return ret;
    }
    
    printk(KERN_INFO "MYFS allocate %d objects into slab: %s\n", number, SLABNAME); 
    printk(KERN_INFO "MYFS object size %d bytes, full size %ld bytes\n", size, (long)size * number); 
    printk(KERN_INFO "MYFS constructor called %d times\n", sco); 
    printk(KERN_INFO "MYFS_MODULE filesystem loaded\n");
    return 0;
}


// Выгрузка модуля
static void __exit myfs_exit(void)
{
    int i, ret; 
    for(i = 0; i < number; i++) 
        kmem_cache_free(cache, line[i]); 
    kmem_cache_destroy(cache); 
    kfree(line); 
    
    ret = unregister_filesystem(&myfs_type);
    if (ret != 0)
        printk(KERN_ERR "MYFS_MODULE cannot unregister filesystem!\n");
    printk(KERN_INFO "MYFS_MODULE unloaded %d\n", sco);
}


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Obergan T.M");

module_init(myfs_init);
module_exit(myfs_exit); 
