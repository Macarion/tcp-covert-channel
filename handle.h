#ifndef _HANDLE_H
#define _HANDLE_H

#include "config.h"
#include "data.h"
#include "command.h"

extern LinkList send_map;
extern LinkList recv_map;

// 数据发送处理函数
int send_data(unsigned int ip, unsigned short *buf, int size, unsigned int seq);

// 数据接收处理函数
void recv_data(unsigned int ip, const unsigned short *buf, int size, unsigned int seq);

#endif
