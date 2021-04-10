#include "data.h"

static unsigned __match_ip;
static unsigned __init_ip;
static int __init_size;
static const char *__fname;

static bool __match_node(Node *p)
{
    return p->ip == __match_ip;
}

static void __init_node(Node *p)
{
    p->ip = __init_ip;
}

static void __init_data(Data *p)
{
    p->ip = __init_ip;
    p->size = __init_size;
    if (__init_size)
    {
        p->content = (char *)kmalloc((__init_size + 1) * sizeof(char), GFP_KERNEL);
    }
}

static void __destory_data(Data *p)
{
    kfree(p->content);
}

static void __destory_queue(LinkQueue *Q)
{
    DestoryQueue(Q, __destory_data);
}

static void __destory_node(Node *p)
{
    __destory_queue(&p->queue);
}

Node* append_ip_node(LinkList L, unsigned int ip)
{
    __init_ip = ip;
    return AppendNode(L, __init_node);
}

void delete_ip_node(LinkList L, unsigned int ip)
{
    __match_ip = ip;
    DeleteNode(L, __match_node, __destory_node);
}

Data* append_data(LinkList L, unsigned int ip, int size)
{
    __match_ip = __init_ip = ip;
    __init_size = size;
    Node *p = FindNode(L, __match_node);
    if (!p && !(p = append_ip_node(L, ip)))
        return NULL;
    if (!p->queue && !InitQueue(&p->queue))
        return NULL;
    return Push(p->queue, __init_data);
}

Data* insert_data(LinkList L, unsigned int ip, int size)
{
    __match_ip = __init_ip = ip;
    __init_size = size;
    Node *p = FindNode(L, __match_node);
    if (!p && !(p = append_ip_node(L, ip)))
        return NULL;
    if (!p->queue && !InitQueue(&p->queue))
        return NULL;
    return Insert(p->queue, __init_data);
}

Data* find_data(LinkList L, unsigned int ip)
{
    __match_ip = ip;
    Node *p = FindNode(L, __match_node);
    if (!p)
        return NULL;
    return GetHead(p->queue);
}

void delete_data(LinkList L, unsigned int ip)
{
    __match_ip = ip;
    Node *p = FindNode(L, __match_node);
    if (!p)
        return;
    Pop(p->queue, __destory_data);
    if (QueueEmpty(p->queue))
    {
        DeleteNode(L, __match_node, __destory_node);
    }
}

void destory_all(LinkList *L)
{
    DestroyList(L, __destory_node);
}

int resize_data(Data *p, int size)
{
    char *cont;
    if (!p->size)
    {
        p->content = (char *)kmalloc(size + 1, GFP_KERNEL);
        p->size = size;
    }
    if (p->size < size)
    {
        cont = kmalloc(size + 1, GFP_KERNEL);
        memcpy(cont, p->content, p->size + 1);
        kfree(p->content);
        p->size = size;
        p->content = cont;
    }
    return p->size;
}

int add_content(Data *p, const void *m, int size)
{
    if (p->cont_pos + size <= p->size)
    {
        memcpy(p->content + p->cont_pos, m, size);
        p->cont_pos += size;
        return size;
    }
    if (p->cont_pos == p->size)
    {
        return 0;
    }
    size = p->size - p->cont_pos;
    memcpy(p->content + p->cont_pos, m, size);
    p->cont_pos += size;
    return size;
}

int get_content(Data *p, void *m, int size)
{
    if (p->cont_pos + size <= p->size)
    {
        memcpy(m, p->content + p->cont_pos, size);
        p->cont_pos += size;
        return size;
    }
    if (p->cont_pos == p->size)
    {
        return 0;
    }
    size = p->size - p->cont_pos;
    memcpy(m, p->content + p->cont_pos, size);
    p->cont_pos += size;
    return size;
}

static unsigned short __checksum(const char *cont, int size)
{
    unsigned int sum = 0;
    if (size & 1)
    {
        size--;
        sum += cont[size] << 8;
    }
    while (size >= 2)
    {
        size -= 2;
        sum += (cont[size] << 8) + cont[size + 1];
    }
    while (sum >> 16)
    {
        sum = (sum >> 16) + (sum & 0xffff);
    }
    return ~sum;
}

unsigned short check_chk(Data *pdata)
{
    return __checksum(pdata->content, pdata->size);
}

int get_rstate(Data *pdata)
{
    return pdata->r_state;
}

int get_sstate(Data *pdata)
{
    return pdata->s_state;
}

int set_rstate(Data *pdata, int state)
{
    return pdata->r_state = state;
}

int set_sstate(Data *pdata, int state)
{
    return pdata->s_state = state;
}

int set_type(Data *pdata, int type)
{
    return pdata->type = type;
}

int set_cd(Data *pdata, int cd)
{
    return pdata->cd = cd;
}

int save_to_file(const char *fname, Data *pdata)
{
    char save_info[50] = {0};
    char ipaddr_tmp[20] = {0};
    struct tm time;

    getDateAndTime(&time);
    saveTimeToStr(save_info, &time);
    sprintf(save_info + strlen(save_info), "[%s] ", ipnAddrToStr(ipaddr_tmp, pdata->ip));

    append_to_file(fname, save_info, strlen(save_info));
    append_to_file(fname, pdata->content, pdata->size);

    if (pdata->content[pdata->size - 2] != '\n')
    {
        append_to_file(fname, "\n", 1);
    }
    return pdata->size;
}

int load_from_file(LinkList L, const char *fname)
{
    unsigned int p_ip;
    int ret = 0;
    int p_size; int pp_ip[4];
    int pos = 0;
    int count = 0;
    Data *pdata;
    char *cont;

    int cont_size = get_file_length(fname);
    if (cont_size++ == -1)
        return -1;
    cont = kmalloc(cont_size * sizeof(char), GFP_KERNEL);
    if (!cont)
        return -1;

    if (!get_file_content(fname, cont, cont_size))
        return -1;

    while (cont[pos] != '\0')
    {
        ret = sscanf(cont + pos, "[%d.%d.%d.%d]", &pp_ip[0],
                &pp_ip[1],
                &pp_ip[2],
                &pp_ip[3]);
        if (!ret)
            break;
        p_ip = (pp_ip[0] << 24) + (pp_ip[1] << 16) + (pp_ip[2] << 8) + pp_ip[3];

        pos += countInfoLen(cont + pos);
        p_size = countDataLen(cont + pos);
        pdata = append_data(L, ntohl(p_ip), p_size);
        count++;
        
        strcpyn(pdata->content, cont + pos, p_size);
        pos += p_size + 1;
    }
    kfree(cont);
    return count;
}

void print_data(Data *pdata)
{
    printk(KERN_INFO "[%d][%d][%d] %s\n", pdata->size, pdata->type, pdata->s_state, pdata->content);
}

// int print_all_datas(LinkList L)
// {
//     char ip_str[20];
//     int i;
//     if (L->count > L->size)
//     {
//         printk(KERN_INFO "Error: count >= size.\n");
//         return -1;
//     }
//     printk(KERN_INFO "count: %d, size: %d\n", L->count, L->size);
//     for (i = 0; i < L->count && L->maps[i].ip; ++i)
//     {
//         ipnAddrToStr(ip_str, L->maps[i].data->ip);
//         printk(KERN_INFO "%d. [%s][%d][%d][%d] %s\n", i + 1, ip_str, L->maps[i].data->size,
//                 L->maps[i].data->type, L->maps[i].data->s_state, L->maps[i].data->content);
//     }
//     return i;
// }

static void __save_queue(Data *p)
{
    save_to_file(__fname, p);
}

static void __save_node(Node *p)
{
    TraverseQueue(p->queue, __save_queue);
}

void save_all_datas(LinkList L, const char *fname)
{
    __fname = fname;
    TraverseList(L, __save_node);
}

int save_to_file_q(const char *fname, Data *pdata)
{
    append_to_file(fname, pdata->content, pdata->size);

    return pdata->size;
}

static void __print_data(Data *p)
{
    print_data(p);
}

static void __print_node(Node *p)
{
    char ip_str[20];
    ipnAddrToStr(ip_str, p->ip);
    printk(KERN_INFO "IP: %s, freq: %d\n", ip_str, p->freq);
}

static void __print_all(Node *p)
{
    __print_node(p);
    TraverseQueue(p->queue, __print_data);
}

void print_all(LinkList L)
{
    TraverseList(L, __print_all);
}
