#ifndef _HANDLE_H
#define _HANDLE_H

#include "data.h"

int send_data(unsigned int ip, void *buf, int size);

void recv_data(unsigned int ip, const void *buf, int size);

#include "handle.c"

#endif
