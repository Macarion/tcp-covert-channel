#ifndef __FILE_IO_H
#define __FILE_IO_H

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

int appendToFile(const char *filename, const void *data, int datasize);
long long getFileLength(const char* filename);
char* getFileContent(const char* filename, char* digest, int size);
char* getFileContentPtr(const char* filename);

#include "file_io.c"
#endif
