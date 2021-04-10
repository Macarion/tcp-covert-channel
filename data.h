#ifndef _DATA_H
#define _DATA_H

#include "list.h"
#include "queue.h"

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

Node* append_ip_node(LinkList L, unsigned int ip);

void delete_ip_node(LinkList L, unsigned int ip);

Data* append_data(LinkList L, unsigned int ip, int size);

Data* insert_data(LinkList L, unsigned int ip, int size);

Data* find_data(LinkList L, unsigned int ip);

void delete_data(LinkList L, unsigned int ip);

void destory_all(LinkList *L);

int resize_data(Data *p, int size);

int add_content(Data *p, const void *m, int size);

int get_content(Data *p, void *m, int size);

unsigned short check_chk(Data *pdata);

int resize_data(Data *pdata, int size);

int add_content(Data *pdata, const void *m, int size);

int get_content(Data *pdata, void *m, int size);

// unsigned short check_chk(Data *pdata);

int get_rstate(Data *pdata);

int get_sstate(Data *pdata);

int set_rstate(Data *pdata, int state);

int set_sstate(Data *pdata, int state);

int set_cd(Data *pdata, int cd);

int set_type(Data *pdata, int type);

int save_to_file(const char *fname, Data *pdata);

int load_from_file(LinkList L, const char *fname);

void print_data(Data *pdata);

// int print_all_datas(LinkList L);

void print_all(LinkList L);

void save_all_datas(LinkList L, const char *fname);

int save_to_file_q(const char *fname, Data *pdata);

#endif