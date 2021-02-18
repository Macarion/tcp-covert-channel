#ifndef _DATA_H
#define _DATA_H

#include <linux/slab.h>
#include <linux/skbuff.h>

enum STATUS
{
    _WAIT,
    _SEND,
    _RECV,
    _CHEK,
    _FINI,
    _STOP,
    _NULL
};

typedef struct _data_raw
{
    unsigned int ip;
    int size;
    int r_state;
    int s_state;
    int cont_pos;
    char* content;
} Data;

struct _map_node
{
    unsigned int ip;
    Data *data;
};

typedef struct _map
{
    int count;
    int size;
    struct _map_node *maps;
} Map;

static Map map;

Data *_append(unsigned int);

void append_data(unsigned int, int);

Data *find_data(unsigned int ip);

void del_data(unsigned int ip);

int add_content(unsigned int ip, const void *m, int size);

void *get_content(unsigned int ip, void *m, int size);

unsigned short check_chk(unsigned int ip);

int get_data_content(unsigned int ip, int size, void *buf);

int get_rstate(unsigned int ip);

int get_sstate(unsigned int ip);

int set_rstate(unsigned int ip, int state);

int set_sstate(unsigned int ip, int state);

#endif
