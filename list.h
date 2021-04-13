#ifndef _LIST_H
#define _LIST_H

#include "comp.h"
#include "queue.h"

typedef struct _node
{
    unsigned int ip;
    unsigned int freq; // 访问频率
    LinkQueue queue; // 数据队列，存放真正要发送的数据
    struct _node *next;
} Node;

typedef struct _ip_list
{
    Node *head;
    Node *tail;
} List, *LinkList;

bool InitList(LinkList *L);

bool DestroyList(LinkList *L, void (*destory_node)(Node *));

Node* AppendNode(LinkList L, void (*init_node)(Node *));

Node* FindNode(LinkList L, bool (*match)(Node *));

bool MoveNode(LinkList L, Node *p);

bool DeleteNode(LinkList L, bool (*match)(Node *), void (*destory_node)(Node *));

bool DeleteAfter(Node *p, void (*destory_node)(Node *));

void TraverseList(LinkList L, void (*visit)(Node *));

void PrintList(LinkList L);

#endif
