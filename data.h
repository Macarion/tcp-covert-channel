#ifndef __RECV_DATA_H
#define __RECV_DATA_H

#include <linux/slab.h>
#include <linux/skbuff.h>
#include "file_io.h"
#include "functions.h"
#include "file_hash.h"

#define __BLOCK 20
#define __HASH_SIZE 20
#define __FILE_RECV "/etc/covert/recvdata"
#define __FILE_NAME "/etc/covert/data"

enum STATUS
{
    __STOP,
    __WAIT,
    __FILE,
    __HASH,
    __REST,
    __FINI
};

struct DataRaw
{
    unsigned int ip;
    int time; //
    int type;
    int size;
    int state;
    int pos;
    int hash_pos;
    unsigned char hash[__HASH_SIZE];
    char* content;
    char sstate;
};

struct MapList
{
    struct DataRaw* raw_data;
    struct MapList* next;
};

struct Map
{
    int count;
    struct MapList* list;
};

int data_getState(struct DataRaw* data_ptr);
int data_getSstate(struct DataRaw* data_ptr);

struct DataRaw* data_new(unsigned int IP, int size);
struct DataRaw* data_reset(struct DataRaw* data_ptr, unsigned int IP, int size);
struct DataRaw* data_saveToFile(struct DataRaw* data_ptr);
struct DataRaw* data_setState(struct DataRaw* data_ptr, int state);
struct DataRaw* data_appendData(struct DataRaw* data_ptr, const void* data, int size);
struct DataRaw* data_appendHash(struct DataRaw* data_ptr, const void* data, int size);
struct DataRaw* data_respon(struct DataRaw* data_ptr, unsigned short* urg_ptr);
struct DataRaw* data_send(struct DataRaw* data_ptr, unsigned short* urg_ptr);

struct MapList* map_append(struct Map* map, unsigned int IP, int size);
void map_delete(struct Map* map, unsigned int IP);
struct DataRaw* map_reset(struct Map* map, unsigned int IP, int size);
struct DataRaw* map_saveToFile(struct Map* map, unsigned int IP);
struct DataRaw* map_setState(struct Map* map, unsigned int IP, int state);
struct Map* map_loadFromFile(struct Map* map, const char* file);
struct DataRaw* map_appendData(struct Map* map, unsigned int IP, char *data, int size);
struct DataRaw* map_appendHash(struct Map* map, unsigned int IP, char *data, int size);
struct DataRaw* map_findData(struct Map* map, unsigned int IP);
struct MapList* map_findParent(struct Map* map, unsigned int IP);

struct DataRaw* map_recv(struct Map* map, unsigned int IP, const unsigned short urg_data);
struct DataRaw* map_respon(struct Map* map, unsigned int IP, unsigned short* urg_ptr);
struct DataRaw* map_respon(struct Map* map, unsigned int IP, unsigned short* urg_ptr);
struct DataRaw* map_send(struct Map* map, unsigned int IP, unsigned short* urg_ptr);

void printMapChildren(struct Map *map);

#include "data.c"
#endif
