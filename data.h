#ifndef _DATA_H
#define _DATA_H

#include "list.h"
#include "queue.h"

#include "file.h"
#include "time.h"
#include "functions.h"

enum STATUS
{
    _WAIT = 0, // 等待开始发送或接收
    _WAIT2, // 等待发送或接收数据大小
    _SEND, // 发送中
    _RECV, // 接收中
    _CHEK, // 校验码
    _FINI, // 完成，等待应答
    _STOP, // 结束
    _NULL
}; // 数据发送或接收中的状态

enum TYPES
{
    TP_DATA = 0, // 单数据
    TP_COMD, // 命令
    TP_SHFL, // 脚本
    TP_ACKN, // 应答
    TP_RESD, // 重发
    TP_FINI  // 结束
}; // 数据的类型

Node* append_ip_node(LinkList L, unsigned int ip); // 添加对应ip的队列

void delete_ip_node(LinkList L, unsigned int ip); // 删除对应ip的队列

Data* append_data(LinkList L, unsigned int ip, int size); // 添加数据到队列尾

Data* insert_data(LinkList L, unsigned int ip, int size); // 插入数据到队列头

Data* find_data(LinkList L, unsigned int ip); // 定位数据地址

void delete_data(LinkList L, unsigned int ip); // 删除数据

void destory_all(LinkList *L); // 删除所有数据

int resize_data(Data *p, int size); // 重设数据大小

int add_content(Data *p, const void *m, int size); // 添加特定大小的数据

int get_content(Data *p, void *m, int size); // 获取特定大小的数据

unsigned short check_chk(Data *pdata); // 检查校验码

// unsigned short check_chk(Data *pdata);

int get_rstate(Data *pdata);

int get_sstate(Data *pdata);

int set_rstate(Data *pdata, int state);

int set_sstate(Data *pdata, int state);

int set_cd(Data *pdata, int cd);

int set_type(Data *pdata, int type);

int save_to_file(const char *fname, Data *pdata); // 将时间，ip，数据信息写入文件

int load_from_file(LinkList L, const char *fname); // 从文件获取数据

void print_data(Data *pdata); // 打印到log文件

// int print_all_datas(LinkList L);

void print_all(LinkList L); // 打印所有数据到log文件

void save_all_datas(LinkList L, const char *fname); // 保存所有数据到文件

int save_to_file_q(const char *fname, Data *pdata); // 仅将数据写入文件

#endif
