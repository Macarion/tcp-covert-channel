#include "data.h"

int data_getState(struct DataRaw* data_ptr)
{
    if (data_ptr)
        return data_ptr->state;
    return -1;
}

int data_getSstate(struct DataRaw* data_ptr)
{
    if (data_ptr)
        return data_ptr->sstate;
    return -1;
}

struct DataRaw* data_new(unsigned int IP, int size)
{
    struct DataRaw* tmp = (struct DataRaw*)kmalloc(sizeof(struct DataRaw), GFP_KERNEL);
    if (!tmp)
        return NULL;
    memset(tmp, 0, sizeof(struct DataRaw));
    
    tmp->ip = IP;
    tmp->size = size;
    tmp->content = (char*)kmalloc(size, GFP_KERNEL);
    if (!tmp->content)
        return NULL;
    memset(tmp->content, 0, size);
    return tmp;
}

struct DataRaw* data_reset(struct DataRaw* data_ptr, unsigned int IP, int size)
{
    kfree(data_ptr->content);
    memset(data_ptr, 0, sizeof(struct DataRaw));
    data_ptr->ip = IP;
    data_ptr->size = size;
    data_ptr->content = (char*)kmalloc(size, GFP_KERNEL);
    return data_ptr;
}

struct DataRaw* data_saveToFile(struct DataRaw* data_ptr)
{
    char save_info[50] = {0};
    char ipaddr_tmp[20] = {0};
    struct tm time;

    getDateAndTime(&time);
    saveTimeToStr(save_info, &time);
    sprintf(save_info + strlen(save_info), "[%s]", ipnAddrToStr(ipaddr_tmp, data_ptr->ip));
//    sprintf(save_info, "[%s]", ipnAddrToStr(ipaddr_tmp, data_ptr->ip));
    appendToFile(__FILE_RECV, save_info, strlen(save_info));
    appendToFile(__FILE_RECV, data_ptr->content, data_ptr->pos);

    if (data_ptr->content[data_ptr->size - 2] != '\n')
    {
        appendToFile(__FILE_RECV, "\n", 1);
    }
    return data_ptr;
}

struct DataRaw* data_setState(struct DataRaw* data_ptr, int state)
{
    data_ptr->state = state;
    return data_ptr;
}

struct DataRaw* data_setSstate(struct DataRaw* data_ptr, int sstate)
{
    data_ptr->sstate = sstate;
    return data_ptr;
}

struct DataRaw* data_appendData(struct DataRaw* data_ptr, const void* data, int size)
{
    if (data_ptr->pos + size <= data_ptr->size)
    {
        memcpy(data_ptr->content + data_ptr->pos, data, size);
        data_ptr->pos += size;
        return data_ptr;
    }
    return data_ptr;
}

struct DataRaw* data_appendHash(struct DataRaw* data_ptr, const void* data, int size)
{
    if (data_ptr->hash_pos + size <= __HASH_SIZE)
    {
        memcpy(data_ptr->hash + data_ptr->hash_pos, data, size);
        data_ptr->hash_pos += size;
        return data_ptr;
    }
    return data_ptr;
}

struct DataRaw* map_recv(struct Map* map, unsigned int IP, unsigned short urg_data)
{
    struct DataRaw* dst_data;
    char saveBuf[2];
    int state;
    saveBuf[0] = urg_data >> 8;
    saveBuf[1] = urg_data & 0xff;
    printk(KERN_CONT "%c%c", urg_data >> 8, urg_data & 0xff);
    dst_data = map_findData(map, IP);
    if (dst_data)
    {
        state = data_getState(dst_data);
    }
    else if (!dst_data && urg_data >> 8 != 1)
    {
        return NULL;
    }
    if (state == __HASH && dst_data->hash_pos < __HASH_SIZE && 0) //DEBUG
    {
        goto _recv_datas;
    }
    switch (urg_data >> 8)
    {
    case 1: 
        info("Data size is: ");
        infonum(urg_data & 0xff);
        printk(KERN_INFO "\n");
        if (dst_data)
        {
            data_reset(dst_data, IP, urg_data & 0xff);
        }
        else if (!(dst_data = map_append(map, IP, urg_data & 0xff)->raw_data)) 
        {
            info("Failed in appending.");
            return NULL;
        }
        data_setState(dst_data, __WAIT);
        break;
    case 2: 
        if (state == __WAIT)
        {
            info("FILE.");
            data_setState(dst_data, __FILE);
        }
        break;
    case 3: 
        if (state == __FILE)
        {
            info(dst_data->content);
            info("HASH.");
            data_setState(dst_data, __HASH);
        }
        break;
    case 4: 
        if (state == __HASH)
        {
            printHashValue(dst_data->hash, __HASH_SIZE);
            info("Recive finished.\n");
            data_setState(dst_data, __FINI);
            if (!hashcmp(dst_data->content, dst_data->size, dst_data->hash))
            {
                info("Data is correct.\n");
                info("Start to write to file.\n");
                data_saveToFile(dst_data);
                info("Write to file finished.\n");
                map_delete(map, IP);
            }
            else
            {
                info("Data is incorrect.\n");
                data_setSstate(dst_data, __RESD);
            }
        }
        break;
    default:
        _recv_datas:
        switch (state)
        {
        case __FILE:
            /* data_appendData(dst_data, &urg_data, 2); */
            data_appendData(dst_data, saveBuf, 2);
            break;
        case __HASH:
            /* data_appendHash(dst_data, &urg_data, 2); */
            data_appendHash(dst_data, saveBuf, 2);
        }
    }
    return NULL;
}

struct DataRaw* data_respon(struct DataRaw* data_ptr, unsigned short* urg_ptr)
{
    unsigned char randChar = 0;
    info("Responsing.\n");
    get_random_bytes(&randChar, 1);
    /* *urg_ptr = ntohs(0x0500 + randChar); */
    return data_ptr;

    switch (data_ptr->state)
    {
    case __WAIT:
        break;
    case __FILE:
        if (!(data_ptr->pos % __BLOCK) && !data_ptr->sstate)
        {
            *urg_ptr = ntohs((5 << 8) + 'r');
            data_ptr->sstate = 1;
        }
        break;
    case __HASH:
        if (data_ptr->hash_pos == __HASH_SIZE && data_ptr->sstate == 1)
        {
            *urg_ptr = ntohs((5 << 8) + 'r');
            data_ptr->sstate = 2;
        }
        break;
    case __REST:
        break;
    case __FINI:
        break;
    }
    return data_ptr;
}

struct DataRaw* data_send(struct DataRaw* data_ptr, unsigned short* urg_ptr)
{
    unsigned char randChar = 0;
    switch(data_ptr->state)
    {
    case __STOP:
        info("START.\n");
        *urg_ptr = ntohs(0x0100 + data_ptr->size);
        data_ptr->state = __WAIT;
        break;
    case __WAIT:
        info("FILE.\n");
        get_random_bytes(&randChar, 1);
        *urg_ptr = ntohs(0x0200 + randChar);
        data_ptr->state = __FILE;
        break;
    case __FILE:
        if (data_ptr->pos < data_ptr->size)
        {
            *urg_ptr = data_ptr->content[data_ptr->pos] & 0xff;
            ++data_ptr->pos;
            if (data_ptr->pos < data_ptr->size)
                *urg_ptr += data_ptr->content[data_ptr->pos] << 8;
            ++data_ptr->pos;
        }
        else
        {
            info("HASH.\n");
            get_random_bytes(&randChar, 1);
            *urg_ptr = ntohs(0x0300 + randChar);
            data_ptr->state = __HASH;
        }
        break;
    case __HASH:
        if (data_ptr->hash_pos < __HASH_SIZE)
        {
            *urg_ptr = ntohs((data_ptr->hash[data_ptr->hash_pos] << 8)
                    + data_ptr->hash[data_ptr->hash_pos + 1]);
            data_ptr->hash_pos += 2;
        }
        else
        {
            info("END.\n");
            get_random_bytes(&randChar, 1);
            *urg_ptr = ntohs(0x0400 + randChar);
            data_ptr->state = __FINI;
        }
        break;
    case __REST:
        break;
    case __FINI:
        break;
    }
    if (*urg_ptr)
        printk(KERN_CONT "%c%c", *urg_ptr & 0xff, *urg_ptr >> 8);
    return data_ptr;
}

struct DataRaw* map_findData(struct Map* map, unsigned int IP)
{
    struct MapList* list_ptr = map->list;
    while (list_ptr)
    {
        if (list_ptr->raw_data->ip == IP)
            return list_ptr->raw_data;
        list_ptr = list_ptr->next;
    }
    return NULL;
}

struct MapList* map_findParent(struct Map* map, unsigned int IP)
{
    struct MapList* list_ptr = map->list;
    if (!list_ptr)
        return NULL;
    if (list_ptr->raw_data->ip == IP)
        return NULL;
    while (list_ptr->next)
    {
        if (list_ptr->next->raw_data->ip == IP)
            return list_ptr;
        list_ptr = list_ptr->next;
    }
    return NULL;
}

struct MapList* map_append(struct Map* map, unsigned int IP, int size)
{
    struct MapList *tmp = map->list;
    if (tmp)
    {
        while (tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next = (struct MapList *)kmalloc(sizeof(struct MapList), GFP_KERNEL);
        if (!tmp->next)
            return NULL;
        tmp = tmp->next;
    }
    else
    {
        tmp = (struct MapList *)kmalloc(sizeof(struct MapList), GFP_KERNEL);
        if (!tmp)
            return NULL;
        map->list = tmp;
    }

    tmp->raw_data = data_new(IP, size);
    tmp->next = NULL;
    return tmp;
}

void map_delete(struct Map* map, unsigned int IP)
{
    struct MapList* tmp;
    struct MapList* list_ptr;

    list_ptr = map_findParent(map, IP);

    if (!list_ptr)
    {
        if (map->list && map->list->raw_data->ip == IP)
        {
            tmp = map->list;
            map->list = tmp->next;
        }
        else
        {
            return;
        }
    }
    else
    {
        tmp = list_ptr->next;
        list_ptr->next = tmp->next;
    }
    kfree(tmp->raw_data->content);
    kfree(tmp->raw_data);
    kfree(tmp);
}

struct DataRaw* map_reset(struct Map* map, unsigned int IP, int size)
{
    struct MapList* list_ptr;
    struct DataRaw* data_ptr;
    list_ptr = map_findParent(map, IP);
    if (list_ptr)
    {
        data_ptr = list_ptr->next->raw_data;
        data_reset(data_ptr, IP, size);
        return data_ptr;
    }
    else
    {
        list_ptr = map_append(map, IP, size);
        return list_ptr->next->raw_data;
    }
}

struct DataRaw* map_saveToFile(struct Map* map, unsigned int IP)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;

    data_saveToFile(data_ptr);
    return data_ptr;
}

struct Map* map_loadFromFile(struct Map* map, const char* file)
{
    unsigned int p_ip;
    int ret = 0;
    int p_size; int pp_ip[4];
    int pos = 0;
    char *file_content = getFileContentPtr(file);
    struct MapList *list_ptr;
    if (!file_content)
    {
        return NULL;
    }
    while (file_content[pos] != '\0')
    {
        ret = sscanf(file_content + pos, "[%d.%d.%d.%d]", &pp_ip[0],
                &pp_ip[1],
                &pp_ip[2],
                &pp_ip[3]);
        if (!ret)
            break;
        p_ip = (pp_ip[0] << 24) + (pp_ip[1] << 16) + (pp_ip[2] << 8) + pp_ip[3];

        pos += countInfoLen(file_content + pos);
        p_size = countDataLen(file_content + pos);
        list_ptr = map_append(map, ntohl(p_ip), p_size);
        
        strcpyn(list_ptr->raw_data->content, file_content + pos, p_size);
        pos += p_size;

        check_hash((unsigned char *)list_ptr->raw_data->content, p_size, list_ptr->raw_data->hash);
    }
    return map;
}

struct DataRaw* map_setState(struct Map* map, unsigned int IP, int state)
{
    struct DataRaw* data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;
    data_ptr->state = state;
    return data_ptr;
}

struct DataRaw* map_appendData(struct Map* map, unsigned int IP, char *data, int size)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;
    data_appendData(data_ptr, data, size);
    return data_ptr;
}

struct DataRaw* map_appendHash(struct Map* map, unsigned int IP, char *data, int size)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;
    data_appendHash(data_ptr, data, size);
    return data_ptr;
}

struct DataRaw* map_respon(struct Map* map, unsigned int IP, unsigned short* urg_ptr)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;
    if (data_ptr->sstate == __RESD)
    {
        data_ptr->sstate = __REOK;
        data_respon(data_ptr, urg_ptr);
        map_delete(map, IP);
        return data_ptr;
    }
    return NULL;
}

struct DataRaw* map_send(struct Map* map, unsigned int IP, unsigned short* urg_ptr)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (data_ptr)
    {
        data_send(data_ptr, urg_ptr);
        if (data_ptr->state == __FINI)
        {
            map_delete(map, IP);
        }
        return data_ptr;
    }
    return NULL;
}

struct DataRaw* map_recrpn(struct Map* map, unsigned int IP, unsigned short urg_data)
{
    struct DataRaw* data_ptr;
    data_ptr = map_findData(map, IP);
    if (!data_ptr)
        return NULL;
    if ((urg_data & 0xff) == 5)
    {
        data_setState(data_ptr, __STOP);
        info("Resend request.\n");
    }
    return data_ptr;
}

void printMapChildren(struct Map *map)
{
    struct MapList *list_ptr;
    char ipStr[20];
    list_ptr = map->list;
    while (list_ptr)
    {
        printk(KERN_INFO "%s, %d, ", ipnAddrToStr(ipStr, list_ptr->raw_data->ip),
            list_ptr->raw_data->size);
        printHashValue(list_ptr->raw_data->hash, __HASH_SIZE);
        printk(KERN_INFO "%s\n", list_ptr->raw_data->content);
        list_ptr = list_ptr->next;
    }

}
