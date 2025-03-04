#include "paging.h"
#include "memory/heap/kheap.h"
#include "status.h"
void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;
struct paging_4gb_chunk *paging_new_4gb(uint8_t flags)
{
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        {
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk *chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

void paging_switch(struct paging_4gb_chunk *directory)
{
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
}

void  paging_free_4gb(struct paging_4gb_chunk *chunk)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xfffff000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

uint32_t *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void *addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVAGR;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

void* paging_align_address(void* ptr)
{
    if ((uint32_t)ptr % PAGING_PAGE_SIZE)
    {
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }
    
    return ptr;
}

void* paging_align_to_lower_page(void* addr)
{
    uint32_t _addr = (uint32_t) addr;
    _addr -= (_addr % PAGING_PAGE_SIZE);
    return (void*) _addr;
}

int paging_map(struct paging_4gb_chunk* directory, void* virt, void* phys, int flags)
{
    if (((unsigned int)virt % PAGING_PAGE_SIZE) || ((unsigned int) phys % PAGING_PAGE_SIZE))
    {
        return -EINVAGR;
    }

    return paging_set(directory->directory_entry, virt, (uint32_t) phys | flags);
}

int paging_map_range(struct paging_4gb_chunk* directory, void* virt, void* phys, int count, int flags)
{
    int res = 0;
    for (int i = 0; i < count; i++)
    {
        res = paging_map(directory, virt, phys, flags);
        if (res < 0)
            break;
        virt += PAGING_PAGE_SIZE;
        phys += PAGING_PAGE_SIZE;
    }

    return res;
}
int paging_map_to(struct paging_4gb_chunk *directory, void *virt, void *phys, void *phys_end, int flags)
{
    int res = 0; // 初始化返回值为0，表示成功
    // 检查虚拟地址是否按页对齐
    if ((uint32_t)virt % PAGING_PAGE_SIZE)
    {
        res = -EINVAGR; // 如果虚拟地址未对齐，设置返回值为错误码-EINVARG
        goto out; // 跳转到函数末尾返回结果
    }
    // 检查物理地址是否按页对齐
    if ((uint32_t)phys % PAGING_PAGE_SIZE)
    {
        res = -EINVAGR; // 如果物理地址未对齐，设置返回值为错误码-EINVARG
        goto out; // 跳转到函数末尾返回结果
    }
    // 检查物理结束地址是否按页对齐
    if ((uint32_t)phys_end % PAGING_PAGE_SIZE)
    {
        res = -EINVAGR; // 如果物理结束地址未对齐，设置返回值为错误码-EINVARG
        goto out; // 跳转到函数末尾返回结果
    }

    // 检查物理结束地址是否大于或等于物理起始地址
    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        res = -EINVAGR; // 如果物理结束地址小于物理起始地址，设置返回值为错误码-EINVARG
        goto out; // 跳转到函数末尾返回结果
    }

    // 计算物理地址范围的总字节数
    uint32_t total_bytes = phys_end - phys;
    // 计算总页数
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    // 调用paging_map_range函数将地址范围映射到页目录
    res = paging_map_range(directory, virt, phys, total_pages, flags);
out:
    return res;
}

int paging_set(uint32_t *directory, void *virt, uint32_t val)
{
    // 检查虚拟地址是否按页对齐
    if (!paging_is_aligned(virt))
    {
        // 如果未对齐，返回错误码 -EINVAGR
        return -EINVAGR;
    }

    // 初始化目录索引和表索引
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    // 获取虚拟地址对应的目录索引和表索引
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if (res < 0)
    {
        // 如果获取索引失败，返回错误码
        return res;
    }

    // 获取目录项
    uint32_t entry = directory[directory_index];
    // 从目录项中提取页表基地址
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    // 设置页表项的值
    table[table_index] = val;

    // 返回成功码 0
    return 0;
}

void* paging_get_physical_address(uint32_t* directory, void* virt)
{
    void* virt_addr_new = (void*) paging_align_to_lower_page(virt);
    void* difference = (void*)((uint32_t) virt - (uint32_t) virt_addr_new);
    return (void*)((paging_get(directory, virt_addr_new) & 0xfffff000) + difference);
}

uint32_t paging_get(uint32_t* directory, void* virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virt, &directory_index, &table_index);
    
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    return table[table_index];
} 