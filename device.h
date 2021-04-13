#ifndef _DEVICE_H
#define _DEVICE_H

#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include "data.h"
#include "config.h"

enum METHOD {
    M_READ,
    M_WRITE
};

// 设备操作函数
int covert_dev_open(struct inode *inode, struct file *file);

int covert_dev_release(struct inode *inode, struct file *file);

loff_t covert_dev_llseek (struct file *file, loff_t offset, int where);

ssize_t covert_dev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);

ssize_t covert_dev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);

// 字符设备初始化，用于与用户空间通信，实现用户调用内核程序，动态发送数据
int device_init(void);

// 字符设备卸载
void device_exit(void);

#endif
