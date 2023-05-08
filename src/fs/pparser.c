#include "pparser.h"
#include "kernel.h"
#include "stringos/stringos.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "status.h"
static int pathparser_path_valid_format(const char * filename)
{
    int len = strnlen(filename,PEACHOS_MAXPATH);
    return (len >=3 && isdigit(filename[0]) && memcmp((void*)&filename[1],":/",2)==0);
}
static int pathparser_get_drive_by_path(const char ** path)
{
    if(!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    *path+=3;
    return drive_no;
}

static struct path_root* pathparser_create_root(int drive_number)
{
    struct path_root *path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}
static const char * pathparser_get_path_part(const char ** path)
{
    char * result_path_part = kzalloc(PEACHOS_MAXPATH);
    while(**path != '/' && **path!=0x00)
    {
        result_path_part[i] = **path
        *path +=1;
        i++;
    }

    if(**path=='/')
    {
        *path+=1;
    }

    if(i==0)
    {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

struct path_part*pathparser_parse_path_part(struct path*last_part,const char **path)
{
    const char *path_part_str = pathparser_get_path_part(path);
    if(!path_part_str)
    {
        return 0;
    }

    struct path_part * part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;
    if(last_part)
    {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct path_root*root)
{
    struct path_part *part = root->first;
    while(part)
    {
        struct path_part *next_part = part->next;
        kfree((void*)part->part);
        kfree(part);
        part = part->next;
    }
    kfree(root);
}