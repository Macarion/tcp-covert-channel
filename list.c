#include "list.h"

bool InitList(LinkList *L)
{
    *L = (LinkList)kmalloc(sizeof(List), GFP_KERNEL);
    if (!(*L))
        return false;
    (*L)->head = (*L)->tail = (Node *)kcalloc(1, sizeof(Node), GFP_KERNEL);
    if (!(*L)->head)
        return false;
    (*L)->head->next = NULL;
    return true;
}

bool DestroyList(LinkList *L, void (*destory_node)(Node *))
{
    if (!(*L) || !(*L)->head)
    {
        return false;
    }
    while (DeleteAfter((*L)->head, destory_node) == true);
    
    kfree((*L)->head);
    kfree(*L);
    *L = NULL;
    return true;
}

Node* AppendNode(LinkList L, void (*init_node)(Node *))
{
    if (!L || !L->head)
        return false;
    L->tail->next = (Node *)kcalloc(1, sizeof(Node), GFP_KERNEL);
    L->tail = L->tail->next;
    if (!L->tail)
        return NULL;
    init_node(L->tail);
    return L->tail;
}

static void __clear_freq(Node *p)
{
    p->freq = 0;
}

Node* FindNode(LinkList L, bool (*match)(Node *))
{
    if (!L || !L->head)
        return NULL;
    Node *p = L->head->next;
    while (p && !match(p))
        p = p->next;
    if (!p)
        return NULL;
    if (++p->freq == UINT_MAX)
        TraverseList(L, __clear_freq);
    else
        MoveNode(L, p);
    return p;
}

bool MoveNode(LinkList L, Node *p)
{
    if (!L || !L->head)
        return false;
    Node *q = L->head;
    Node *t;
    while (q->next && q->next->freq > p->freq)
        q = q->next;
    if (q->next && q->next != p)
    {
        t = q;
        while (t->next != p)
            t = t->next;
        t->next = p->next;
        if (!t->next)
            L->tail = t;
        t = q->next;
        q->next = p;
        p->next = t;
    }
    return true;
}

bool DeleteNode(LinkList L, bool (*match)(Node *), void (*destory_node)(Node *))
{
    if (!L || !L->head)
        return false;
    Node *p;
    for (p = L->head; p->next; p = p->next)
    {
        if (match(p->next))
        {
            if (p->next == L->tail)
                L->tail = p;
            DeleteAfter(p, destory_node);
            return true;
        }
    }
    return false;
}

bool DeleteAfter(Node *p, void (*destory_node)(Node *))
{
    if (!p || !p->next)
        return false;
    Node *q = p->next;
    p->next = q->next;
    destory_node(q);
    kfree(q);
    return true;
}

void TraverseList(LinkList L, void(*visit)(Node *))
{
    if (!L || !L->head)
        return;
    Node *p = L->head->next;
    while (p)
    {
        visit(p);
        p = p->next;
    }
}

// static void __print_node(Node *p)
// {
//     printk(KERN_INFO "IP: %d, freq: %d\n", p->ip, p->freq);
// }

// void PrintList(LinkList L)
// {
//     TraverseList(L, __print_node);
// }

