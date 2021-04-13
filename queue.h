#ifndef _QUEUE_H
#define _QUEUE_H

#include "comp.h"

typedef struct _data
{
    unsigned int ip;
    int type;       // 数据类型
    int size;       // 数据大小
    int cd;         // 重发冷却时间
    int r_state;    // 接收状态
    int s_state;    // 发送状态
    int cont_pos;   //数据内容当前位置
    char* content;  // 数据内容
    unsigned int lastseq;   // 上次发送的seq码
    unsigned short lastsnd; // 上次发送的内容
    struct _data *next;
} Data;

typedef struct _queue
{
    Data *front;
    Data *rear;
} Queue, *LinkQueue;

bool InitQueue(LinkQueue *Q);

void DestoryQueue(LinkQueue *Q, void (*destory_data)(Data *));

bool QueueEmpty(LinkQueue Q);

Data* GetHead(LinkQueue Q);

bool Pop(LinkQueue Q, void (*destory_data)(Data *));

Data* Push(LinkQueue Q, void (*init_data)(Data *));

Data* Insert(LinkQueue Q, void (*init_data)(Data *));

void TraverseQueue(LinkQueue Q, void(*visit)(Data *));

void PrintQueue(LinkQueue Q);

#endif
