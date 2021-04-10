#ifndef _QUEUE_H
#define _QUEUE_H

#include "comp.h"

typedef struct _data
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