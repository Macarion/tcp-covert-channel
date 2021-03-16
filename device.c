#include "device.h"

static int major;
static int minor;
static struct class *cls;
static dev_t devnum;
static Data *read_pdata;
static struct cdev covert_dev;

extern Map send_map;
extern Map recv_map;

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = covert_dev_open,
    .release = covert_dev_release,
    .read = covert_dev_read,
    .write = covert_dev_write,
    .llseek = covert_dev_llseek,
};

int covert_dev_open(struct inode *inode, struct file *file)
{
    return 0;
}

int covert_dev_release(struct inode *inode, struct file *file)
{
    return 0;
}

loff_t covert_dev_llseek(struct file *file, loff_t offset, int where)
{
    if (offset == 0)
    {
        print_all_datas(&send_map);
        return 0;
    }
    read_pdata = find_data(&send_map, offset);
    if (!read_pdata)
    {
        return 0;
    }
    return read_pdata->size;
}

ssize_t covert_dev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
    if (!read_pdata)
    {
        return 0;
    }
    if (read_pdata->size > count)
    {
        return 0;
    }
    raw_copy_to_user(buf, read_pdata->content, read_pdata->size);
    read_pdata = NULL;
    return 0;
}

ssize_t covert_dev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
    Data *pdata;
    unsigned long ret;
    unsigned int ip;
    int size, type;
    
    if (sizeof(unsigned int) + sizeof(int) * 2 > count)
    {
        return 0;
    }
    ret = raw_copy_from_user(&ip, buf + *offset, sizeof(unsigned int));
    if (ret)
    {
        return -1;
    }
    *offset += sizeof(unsigned int);
    ret = raw_copy_from_user(&size, buf + *offset, sizeof(int));
    if (ret)
    {
        return -1;
    }
    *offset += sizeof(int);
    ret = raw_copy_from_user(&type, buf + *offset, sizeof(int));
    if (ret)
    {
        return -1;
    }
    *offset += sizeof(int);
    /* printk(KERN_INFO "ip: %u, size: %d\n", ip, size); */

    if (sizeof(unsigned int) + sizeof(int) * 2 + size > count)
    {
        return 0;
    }

    pdata = append_data(&send_map, ip, size);
    if (!pdata)
    {
        return -1;
    }
    set_type(pdata, type);
    /* printk(KERN_INFO "pdata = %p\n", pdata); */
    /* printk(KERN_INFO "&map = %p, map.count = %d\n", &send_map, send_map.count); */
    
    ret = raw_copy_from_user(pdata->content, buf + *offset, size);
    if (ret)
    {
        del_data(&send_map, ip);
        return -1;
    }
    *offset += size;

    /* print_data(pdata); */

    return 0;
}

int device_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&devnum, 0, 1, DEVNAME);
    if (!ret) {
        major = MAJOR(devnum);
        minor = MINOR(devnum);
        /* printk("major = %d; minor = %d\n", major, minor); */
    }

    cdev_init(&covert_dev, &fops);
    ret = cdev_add(&covert_dev, devnum, 1);

    if (ret) {
        printk(KERN_ERR "Unable to cdev_add\n");
        return -EINVAL;
    }

    cls = class_create(THIS_MODULE, DEVNAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVNAME);
    return 0;
}

void device_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    cdev_del(&covert_dev);
    unregister_chrdev_region(MKDEV(major, minor), 1);

    return;
}
