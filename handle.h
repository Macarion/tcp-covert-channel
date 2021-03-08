#ifndef _HANDLE_H
#define _HANDLE_H

#include "config.h"
#include "data.h"

extern Map send_map;
extern Map recv_map;

int send_data(unsigned int ip, unsigned short *buf, int size, unsigned int seq);

void recv_data(unsigned int ip, const unsigned short *buf, int size, unsigned int seq);

#endif
