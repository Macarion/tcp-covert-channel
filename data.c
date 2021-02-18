#include "data.h"

Data *_append(unsigned int ip)
{
    if (map.count <= map.size)
    {
        struct _map_node *t = map.maps;
        map.maps = kcalloc(map.size * 2, sizeof(struct _map_node), GFP_KERNEL);
        memcpy(map.maps, t, sizeof(struct _map_node) * map.size);
        map.size *= 2;
        kfree(t);
    }
    Data *t = kcalloc(1, sizeof(Data), GFP_KERNEL);
    map.maps[map.count].ip = ip;
    map.maps[map.count].data = t;
    map.count++;
    return t;
}

void append_data(unsigned int ip, int size)
{
    Data *d = _append(ip);
    d->size = size;
    d->content = kcalloc(1, sizeof(char) * size, GFP_KERNEL);
}

int find_index(unsigned int ip)
{
    int i;
    for (i = 0; i < map.count; ++i)
    {
        if (map.maps[i].ip == ip)
        {
            return i;
        }
    }
    return -1;
}

Data *find_data(unsigned int ip)
{
    int i = find_index(ip);
    if (i == -1)
        return NULL;
    return map.maps[i].data;
}

void del_data(unsigned int ip)
{
    int i = find_index(ip);
    for (; i < map.count - 1; ++i)
    {
        memcpy(map.maps + i, map.maps + i + 1, sizeof(struct _map_node));
    }
    map.maps[map.count].ip = 0;
    map.maps[map.count].data = NULL;
    map.count--;
}

int add_content(unsigned int ip, const void *m, int size)
{
    Data *t = find_data(ip);
    if (!t)
        return -1;
    if (t->cont_pos + size < t->size)
    {
        memcpy(t->content + t->cont_pos, m, size);
        return size;
    }
    return 0;
}

void *get_content(unsigned int ip, void *m, int size)
{
    Data *t = find_data(ip);
    if (!t)
        return NULL;
    if (t->cont_pos + size < t->size)
    {
        memcpy(m, t->content + t->cont_pos, size);
        t->cont_pos += size;
        return m;
    }
    return NULL;
}

unsigned short _checksum(const char *cont, int size)
{
    unsigned int sum;
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
        sum = (sum >> 16) + sum & 0xffff;
    }
    return ~sum;

}

unsigned short check_chk(unsigned int ip)
{
    /* unsigned int sum; */
    Data *t = find_data(ip);
    if (!t)
        return 0;
    return _checksum(t->content, t->size);
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

int get_rstate(unsigned int ip)
{
    Data *d = find_data(ip);
    if (!d)
        return _NULL;
    return d->r_state;
}

int get_sstate(unsigned int ip)
{
    Data *d = find_data(ip);
    if (!d)
        return _NULL;
    return d->s_state;
}

int set_rstate(unsigned int ip, int state)
{
    Data *d = find_data(ip);
    if (!d)
        return _NULL;
    return d->r_state = state;
}

int set_sstate(unsigned int ip, int state)
{
    Data *d = find_data(ip);
    if (!d)
        return _NULL;
    return d->s_state = state;
}
