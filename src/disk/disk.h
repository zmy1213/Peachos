#ifndef DISK_H
#define DISK_H
#include "fs/file.h"

typedef unsigned int PEACHOS_DISK_TYPE;

#define PEACHOS_DISK_TYPE_REAL 0

struct disk
{
    PEACHOS_DISK_TYPE type;//磁盘类型
    int sector_size;//扇区大小
    int id;//磁盘编号
    struct filesystem* filesystem;//文件系统
    void* fs_private;//文件系统私有数据
};
int disk_read_sector(int lba,int total,void*buf);
void disk_search_and_init();
struct disk *disk_get(int index);
int disk_read_block(struct disk*idisk,unsigned int lba,int total,void*buf);
#endif