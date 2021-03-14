#ifndef _DATA_H
#define _DATA_H

#include <linux/slab.h>
#include <linux/skbuff.h>

#include "file.h"
#include "time.h"
#include "functions.h"

enum STATUS
{
    _WAIT = 0,
    _WAIT2,
    _SEND,
    _RECV,
    _CHEK,
    _FINI,
    _STOP,
    _NULL
};

enum TYPES
{
    TP_DATA = 0,
    TP_COMD,
    TP_SHFL,
    TP_ACKN,
    TP_RESD,
    TP_FINI
};

typedef struct _data_raw
{
    unsigned int ip;
    int type;
    int size;
    int cd;
    int r_state;
    int s_state;
    int cont_pos;
    char* content;
    unsigned int lastseq;
    unsigned short lastsnd;
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

Data *_append(Map *map, unsigned int);

Data *_insert(Map *map, unsigned int ip);

void free_map(Map *map);

Data *append_data(Map *map, unsigned int, int);

Data *insert_data(Map *map, unsigned int ip, int size);

int find_index(Map *map, unsigned int ip);

Data *find_data(Map *map, unsigned int ip);

void del_data(Map *map, unsigned int ip);

int resize_data(Data *pdata, int size);

int add_content(Data *pdata, const void *m, int size);

int get_content(Data *pdata, void *m, int size);

unsigned short check_chk(Data *pdata);

/* int get_data_content(Data *pdata, int size, void *buf); */

int get_rstate(Data *pdata);

int get_sstate(Data *pdata);

int set_rstate(Data *pdata, int state);

int set_sstate(Data *pdata, int state);

int set_cd(Data *pdata, int cd);

int set_type(Data *pdata, int type);

int save_to_file(const char *fname, Data *pdata);

int load_from_file(Map *map, const char *fname);

void print_data(Data *pdata);

int print_all_datas(Map *map);

int save_all_datas(Map *map, const char *fname);

int save_to_file_q(const char *fname, Data *pdata);

#endif
