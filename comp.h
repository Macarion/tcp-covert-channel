#ifndef _COMP_H
#define _COMP_H

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define kmalloc(arg1, arg2) malloc(arg1)
#define kcalloc(arg1, arg2, arg3) calloc(arg1, arg2)
#define kfree free
#define GFP_KERNEL
#define KERN_INFO
#define printk printf
#else
#include <linux/slab.h>
#include <linux/skbuff.h>
#endif

#ifndef bool
#define bool _Bool
#define true 1
#define false 0
#endif

#define print_all_datas print_all

#endif