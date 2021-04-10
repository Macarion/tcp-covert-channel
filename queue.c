#include "queue.h"

bool InitQueue(LinkQueue *Q)
{
    *Q = (LinkQueue)kcalloc(1, sizeof(Queue), GFP_KERNEL);
    return *Q != NULL;
}

void DestoryQueue(LinkQueue *Q, void (*destory_data)(Data *))
{
    while (!QueueEmpty(*Q))
    {
        Pop(*Q, destory_data);
    }
}

bool QueueEmpty(LinkQueue Q)
{
    if (!Q)
        return true;
    return Q->front == NULL;
}

Data* GetHead(LinkQueue Q)
{
    return Q->front;
}

bool Pop(LinkQueue Q, void (*destory_data)(Data *))
{
    if (!Q || QueueEmpty(Q))
        return false;
    Data *p = Q->front;
    Q->front = Q->front->next;
    if (!Q->front)
        Q->rear = Q->front;
    destory_data(p);
    kfree(p);
    return true;
}

Data* Push(LinkQueue Q, void (*init_data)(Data *))
{
    if (!Q)
        return NULL;
    if (QueueEmpty(Q))
    {
        Q->front = Q->rear = (Data *)kcalloc(1, sizeof(Data), GFP_KERNEL);
    }
    else
    {
        Q->rear->next = (Data *)kcalloc(1, sizeof(Data), GFP_KERNEL);
        Q->rear = Q->rear->next;
    }
    if (!Q->rear)
        return NULL;
    init_data(Q->rear);
    return Q->rear;
}

Data* Insert(LinkQueue Q, void (*init_data)(Data *))
{
    if (!Q)
        return NULL;
    Data *p = Q->front;
    Q->front = (Data *)kcalloc(1, sizeof(Data), GFP_KERNEL);
    if (!Q->front)
        return NULL;
    Q->front->next = p;
    if (!Q->rear)
        Q->rear = Q->front;
    init_data(Q->front);
    return Q->front;
}

void TraverseQueue(LinkQueue Q, void(*visit)(Data *))
{
    if (!Q || QueueEmpty(Q))
        return;
    Data *p = Q->front;
    while (p)
    {
        visit(p);
        p = p->next;
    }
}

// static void __print_data(Data *p)
// {
//     printk(KERN_INFO "[%d] %s\n", p->size, p->content);
// }

// void PrintQueue(LinkQueue Q)
// {
//     TraverseQueue(Q, __print_data);
// }