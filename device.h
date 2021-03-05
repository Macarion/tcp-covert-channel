#ifndef _DEVICE_H
#define _DEVICE_H

#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include "data.h"

#define DEVNAME "covert"
static int major;
static int minor;
static struct class *cls;
static dev_t devnum;
static Data *read_pdata;

extern Map send_map;
extern Map recv_map;

enum METHOD {
    M_READ,
    M_WRITE
};

int covert_dev_open(struct inode *inode, struct file *file);

int covert_dev_release(struct inode *inode, struct file *file);

loff_t covert_dev_llseek (struct file *file, loff_t offset, int where);

ssize_t covert_dev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);

ssize_t covert_dev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

int device_init(void);
void device_exit(void);

#endif
