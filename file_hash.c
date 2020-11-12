#include "file_hash.h"

static struct shash_desc *init_shash(struct crypto_shash *tfm) 
{
    struct shash_desc *shash;
    shash = kmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(tfm), GFP_KERNEL);
    if (!shash)
        return ERR_PTR(-ENOMEM);
    shash->tfm = tfm;
    return shash;
}

static int calc_hash(struct crypto_shash *tfm, const unsigned char *data, unsigned int datalen, unsigned char *digest) 
{
    int ret;
    struct shash_desc *shash;

    shash = init_shash(tfm);
    if (IS_ERR(shash)) {
        pr_info("can't alloc sdesc\n");
        return PTR_ERR(shash);
    }

    ret = crypto_shash_digest(shash, data, datalen, digest);
    kfree(shash);
    return ret;
}

static int check_hash(const unsigned char *data, unsigned int datalen, unsigned char *digest) 
{
    struct crypto_shash *tfm;
    int ret;

    tfm = crypto_alloc_shash("sha1", 0, 0);
    if (IS_ERR(tfm)) {
        pr_info("Can't alloc tfm %s\n", "sha1");
        return PTR_ERR(tfm);
    }
    ret = calc_hash(tfm, data, datalen, digest);
    crypto_free_shash(tfm);
    return ret;
}

int checkHash(const char *file_name, unsigned char * digest)
{
    int ret;
    mm_segment_t fs;
    struct file * fp;
    unsigned char * file_content;
    fp = filp_open(file_name, O_RDONLY, 0);
    if (IS_ERR(fp))
    {
        printk(KERN_WARNING "File_hash: Error occured while opening file %s\n", file_name);
        return -1;
    }
    
//    printk(KERN_INFO "File_hash: file name: %s, file size: %lld\n", file_name, fp->f_inode->i_size);
    file_content = (unsigned char *)kmalloc(fp->f_inode->i_size + 1, GFP_KERNEL);
    file_content[fp->f_inode->i_size] = '\0';
    if (file_content == NULL)
    {
        printk(KERN_WARNING "File_hash: Failed to alloc space.");
        filp_close(fp, NULL);
        return -1;
    }

//    printk(KERN_INFO "Hash Read Started\n");
    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = kernel_read(fp, file_content, fp->f_inode->i_size, &fp->f_pos);
    set_fs(fs);
    filp_close(fp, NULL);

//    printk(KERN_INFO "Hash Read Finished\n");
    if (ret < 0)
    {
        printk(KERN_WARNING "File_hash: Error occured while reading file %s\n", file_name);
        return -1;
    }
//    printk(KERN_INFO "/*%s*/\n", file_content);

    check_hash(file_content, fp->f_inode->i_size, digest);
    kfree(file_content);
    
    return 0;   
}

int hashcmpFile(const char * filename, const unsigned char * hash)
{
    unsigned char hashbuf[41] = {0};
    checkHash(filename, hashbuf);
    return strncmp((char *)hashbuf, (char *)hash, 20);
}

int hashcmp(const char* buf, unsigned int datalen, const unsigned char* hash)
{
    unsigned char bufhash[20] = {0};
    check_hash((unsigned char*)buf, datalen, bufhash);
    return strncmp((char *)bufhash, (char *)hash, 20);
}
