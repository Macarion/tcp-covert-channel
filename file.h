#ifndef __FILE_IO_H
#define __FILE_IO_H

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

int append_to_file(const char *filename, const void *data, int datasize);   // 添加数据到文件
long long get_file_length(const char* filename);                            // 获取文件大小
char* get_file_content(const char* filename, char* digest, int size);       // 获取文件内容
char* get_file_content_ptr(const char* filename);                           // 获取文件内容的指针，需手动kfree

#endif
