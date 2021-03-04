#ifndef _DATA_H
#define _DATA_H

#include <linux/slab.h>
#include <linux/skbuff.h>

#include "file.h"
#include "time.h"
#include "functions.h"

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

void free_map(void);

Data *append_data(unsigned int, int);

Data *find_data(unsigned int ip);

void del_data(unsigned int ip);

int add_content(Data *pdata, const void *m, int size);

int get_content(Data *pdata, void *m, int size);

unsigned short check_chk(Data *pdata);

/* int get_data_content(Data *pdata, int size, void *buf); */

int get_rstate(Data *pdata);

int get_sstate(Data *pdata);

int set_rstate(Data *pdata, int state);

int set_sstate(Data *pdata, int state);

int save_to_file(const char *fname, Data *pdata);

int load_from_file(const char *fname);

void print_data(Data *pdata);

int print_all_datas(void);

int save_all_datas(const char *fname);

#endif
