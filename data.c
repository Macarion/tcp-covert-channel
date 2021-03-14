#include "data.h"

Data *_append(Map *map, unsigned int ip)
{
    Data *t;
    if (map->count == map->size)
    {
        struct _map_node *t = map->maps;
        map->maps = kcalloc(map->size == 0 ? 1 : map->size * 2, sizeof(struct _map_node), GFP_KERNEL);
        memcpy(map->maps, t, sizeof(struct _map_node) * map->size);
        map->size = map->size == 0 ? 1 : map->size * 2;
        kfree(t);
    }
    t = kcalloc(1, sizeof(Data), GFP_KERNEL);
    map->maps[map->count].ip = t->ip = ip;
    map->maps[map->count].data = t;
    map->count++;
    return t;
}

Data *_insert(Map *map, unsigned int ip)
{
    int locate;
    Data *t;
    if (map->count == map->size)
    {
        struct _map_node *t = map->maps;
        map->maps = kcalloc(map->size == 0 ? 1 : map->size * 2, sizeof(struct _map_node), GFP_KERNEL);
        memcpy(map->maps, t, sizeof(struct _map_node) * map->size);
        map->size = map->size == 0 ? 1 : map->size * 2;
        kfree(t);
    }
    locate = find_index(map, ip);
    t = kcalloc(1, sizeof(Data), GFP_KERNEL);
    if (locate == -1)
    {
        map->maps[map->count].ip = t->ip = ip;
        map->maps[map->count].data = t;
    }
    else
    {
        int i;
        for (i = map->count - 1; i >= locate; --i)
        {
            memcpy(map->maps + i + 1, map->maps + i, sizeof(struct _map_node));
        }
        map->maps[locate].ip = t->ip = ip;
        map->maps[locate].data = t;
    }
    map->count++;
    return t;
}

void free_map(Map *map)
{
    int i;
    for (i = 0; i < map->count; ++i)
    {
        kfree(map->maps[i].data->content);
        kfree(map->maps[i].data);
    }
    kfree(map->maps);
}

Data *append_data(Map *map, unsigned int ip, int size)
{
    Data *d = _append(map, ip);
    d->size = size;
    d->content = kcalloc(1, size + 1, GFP_KERNEL);
    d->s_state = d->r_state = _WAIT;
    return d;
}

Data *insert_data(Map *map, unsigned int ip, int size)
{
    Data *d = _insert(map, ip);
    d->size = size;
    d->content = kcalloc(1, size + 1, GFP_KERNEL);
    d->s_state = d->r_state = _WAIT;
    return d;
}

int find_index(Map *map, unsigned int ip)
{
    int i;
    for (i = 0; i < map->count; ++i)
    {
        if (map->maps[i].ip == ip)
        {
            return i;
        }
    }
    return -1;
}

Data *find_data(Map *map, unsigned int ip)
{
    int i = find_index(map, ip);
    if (i == -1)
        return NULL;
    return map->maps[i].data;
}

void del_data(Map *map, unsigned int ip)
{
    int i = find_index(map, ip);
    if (i == -1) return;

    kfree(map->maps[i].data->content);
    kfree(map->maps[i].data);

    for (; i < map->count - 1; ++i)
    {
        memcpy(map->maps + i, map->maps + i + 1, sizeof(struct _map_node));
    }
    map->maps[map->count].ip = 0;
    map->maps[map->count].data = NULL;
    map->count > 0 ? map->count-- : (map->count = 0);

    /* if (map->count * 2 <= map->size) */
    /* { */
        /* struct _map_node *t = map->maps; */
        /* map->maps = kcalloc(map->size == 1 ? 1 : map->size / 2, sizeof(struct _map_node), GFP_KERNEL); */
        /* memcpy(map->maps, t, sizeof(struct _map_node) * map->count); */
        /* map->size = map->size == 1 ? 1 : map->size / 2; */
        /* kfree(t); */
    /* } */
}

int resize_data(Data *pdata, int size)
{
    char *cont;
    cont = kcalloc(1, size + 1, GFP_KERNEL);
    if (pdata->size <= size)
    {
        memcpy(cont, pdata->content, pdata->size + 1);
    }
    pdata->size = size;
    kfree(pdata->content);
    pdata->content = cont;
    return size;
}

int add_content(Data *pdata, const void *m, int size)
{
    if (pdata->cont_pos + size <= pdata->size)
    {
        memcpy(pdata->content + pdata->cont_pos, m, size);
        pdata->cont_pos += size;
        return size;
    }
    if (pdata->cont_pos == pdata->size)
    {
        return 0;
    }
    else
    {
        size = pdata->size - pdata->cont_pos;
        memcpy(pdata->content + pdata->cont_pos, m, size);
        pdata->cont_pos += size;
        return size;
    }
}

int get_content(Data *pdata, void *m, int size)
{
    if (pdata->cont_pos + size <= pdata->size)
    {
        memcpy(m, pdata->content + pdata->cont_pos, size);
        pdata->cont_pos += size;
        return size;
    }
    if (pdata->cont_pos == pdata->size)
    {
        return 0;
    }
    else
    {
        size = pdata->size - pdata->cont_pos;
        memcpy(m, pdata->content + pdata->cont_pos, size);
        pdata->cont_pos += size;
        return size;
    }
}

unsigned short _checksum(const char *cont, int size)
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
    /* unsigned int sum; */
    return _checksum(pdata->content, pdata->size);
    /* for (int i = 0; i * 2 < t->size; ++i) */
    /* { */
        /* sum += (t->content[i * 2] << 8) + t->content[i * 2 + 1]; */
    /* } */
    /* while (sum >> 16) */
    /* { */
        /* sum = (sum >> 16) + sum & 0xffff; */
    /* } */
    /* return ~sum; */
}

/* {{{ */

/* int get_data_content(unsigned int ip, int size, void *buf) */
/* { */
    /* Data *d = find_data(ip); */
    /* if (!d) */
        /* return -1; */
    /* if (d->cont_pos + size >= d->size) */
    /* { */
        /* return 1; */
    /* } */
    /* memcpy(buf, d->content + d->cont_pos, size); */
    /* d->cont_pos += size; */
    /* return 0; */
/* } */

/* }}} */

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

int load_from_file(Map *map, const char *fname)
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
        pdata = append_data(map, ntohl(p_ip), p_size);
        count++;
        
        strcpyn(pdata->content, cont + pos, p_size);
        pos += p_size + 1;
    }
    kfree(cont);
    return count;
}

void print_data(Data *pdata)
{
    char ip_str[20];
    ipnAddrToStr(ip_str, pdata->ip);
    printk(KERN_INFO "[%s][%d][%d] %s\n", ip_str, pdata->size, pdata->type, pdata->content);
}

int print_all_datas(Map *map)
{
    char ip_str[20];
    int i;
    if (map->count > map->size)
    {
        printk(KERN_INFO "Error: count >= size.\n");
        return -1;
    }
    for (i = 0; i < map->count && map->maps[i].ip; ++i)
    {
        ipnAddrToStr(ip_str, map->maps[i].data->ip);
        /* printk(KERN_INFO "%d. [%s][%d] %s\n", i + 1, ip_str, map->maps[i].data->content); */
        printk(KERN_INFO "%d. [%u][%d][%d] %s\n", i + 1, map->maps[i].ip, map->maps[i].data->size,
                map->maps[i].data->s_state, map->maps[i].data->content);
    }
    return i;
}

int save_all_datas(Map *map, const char *fname)
{
    int i;
    for (i = 0; i < map->count; ++i)
    {
        save_to_file(fname, map->maps[i].data);
    }
    return i;
}

int save_to_file_q(const char *fname, Data *pdata)
{
    append_to_file(fname, pdata->content, pdata->size);

    return pdata->size;
}

