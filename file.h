#ifndef __FILE_IO_H
#define __FILE_IO_H

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

int append_to_file(const char *filename, const void *data, int datasize);
long long get_file_length(const char* filename);
char* get_file_content(const char* filename, char* digest, int size);
char* get_file_content_ptr(const char* filename);

#endif
