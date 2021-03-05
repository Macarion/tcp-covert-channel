#ifndef _HANDLE_H
#define _HANDLE_H

#include "config.h"
#include "data.h"

int send_data(Map *map, unsigned int ip, void *buf, int size);

void recv_data(Map *map, unsigned int ip, const void *buf, int size);

#endif
