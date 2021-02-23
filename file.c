#include "file.h"

int append_to_file(const char *filename, const void *data, int datasize)
{
    int ret;
    mm_segment_t fs;
    struct file *fp;

    fp = filp_open(filename, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (IS_ERR(fp))
    {
        return -1;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = kernel_write(fp, data, datasize, &fp->f_pos);
    set_fs(fs);
    filp_close(fp, NULL);

    return ret;
}

long long get_file_length(const char* filename)
{
    long long size;
    struct file* fp;
    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(fp))
        return -1;
    size = fp->f_inode->i_size;
    filp_close(fp, NULL);
    return size;
}

char* get_file_content(const char* filename, char* digest, int size)
{
    mm_segment_t fs;
    struct file *fp;

    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(fp))
    {
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    kernel_read(fp, digest, size, &fp->f_pos);
    set_fs(fs);
    filp_close(fp, NULL);
    return digest;
}

/* int load_from_file(const char *fname) */
/* { */
    /* int cont_size = get_file_length(fname); */
    /* if (cont_size++ == -1) */
        /* return -1; */
    /* char *cont = kmalloc(cont_size * sizeof(char), GFP_KERNEL); */
    /* if (!cont) */
        /* return -1; */

    /* if (!get_file_content(fname, cont, cont_size)) */
        /* return -1; */

    /* kfree(cont); */
    // /* sscanf(cont, "[%d.%d.%d.%d][%s]%s", ); */
    /* return 0; // */
/* } */

char* get_file_content_ptr(const char* filename)
{
    mm_segment_t fs;
    struct file *fp;
    char* content;
    long long size;

    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(fp))
    {
        return NULL;
    }

    size = fp->f_inode->i_size;
    content = (char*)kmalloc(size + 1, GFP_KERNEL);
    fs = get_fs();
    set_fs(KERNEL_DS);
    kernel_read(fp, content, size, &fp->f_pos);
    set_fs(fs);
    filp_close(fp, NULL);
    return content;
}
