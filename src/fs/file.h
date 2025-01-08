#ifndef FILE_H
#define FILE_H
#include "pparser.h"
#include <stdint.h>
typedef unsigned int FILE_SEEK_MODE;
enum 
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};
typedef unsigned int FILE_MODE;
enum 
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

enum
{
    FILE_STAT_READ_ONLY = 0b00000001
};

typedef unsigned int FILE_STAT_FLAGS;

struct disk;
typedef void* (*FS_OPEN_FUNCTION)(struct disk *disk,struct path_part * path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk *disk,void* private ,uint32_t size,uint32_t nmemb,char *out);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk * disk);
typedef int (*FS_SEEK_FUNCTION)(void *private,uint32_t offset,FILE_SEEK_MODE seek_mode);
typedef int (*FS_CLOSE_FUNCTION)(void* private);

struct file_stat
{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);

struct filesystem
{
    FS_OPEN_FUNCTION open;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    FS_RESOLVE_FUNCTION resolve;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    FS_READ_FUNCTION read;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    FS_SEEK_FUNCTION seek;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    FS_STAT_FUNCTION stat;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    FS_CLOSE_FUNCTION close;//如果提供的磁盘正在使用它的文件系统，文件系统应该从解析返回零
    char name[20];
};
struct file_descriptor
{
    int index;//文件描述符的索引
    struct filesystem *filesystem;
    void *private;//初始的文件描述符数据
    struct disk *disk;//被文件描述符使用的那块磁盘
};

void fs_init();
int fopen(const char *filename,const char *mode);
void fs_insert_filesystem(struct filesystem *filesystem);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd);
int fstat(int fd, struct file_stat* stat);
int fclose(int fd);
struct filesystem *fs_resolve(struct disk *disk);


#endif