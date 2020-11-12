#ifndef __FILE_HASH_H
#define __FILE_HASH_H

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <crypto/hash.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

static struct shash_desc *init_shash(struct crypto_shash *tfm);
static int calc_hash(struct crypto_shash *tfm, const unsigned char *data, unsigned int datalen, unsigned char *digest);
static int check_hash(const unsigned char *data, unsigned int datalen, unsigned char *digest);
int checkHash(const char *file_name, unsigned char * digest);
int hashcmp(const char* buf, unsigned int datalen, const unsigned char* hash);
int hashcmpFile(const char * filename, const unsigned char * hash);

#include "file_hash.c"
#endif
