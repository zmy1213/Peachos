#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "io/io.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "fs/pparser.h"
#include "fs/file.h"
#include "disk/streamer.h"
#include "stringos/stringos.h"
#include "gdt/gdt.h"
#include "task/tss.h"
#include "task/task.h"
#include "task/process.h"
#include "config.h"
#include "status.h"
uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

static struct paging_4gb_chunk* kernel_chunk = 0;

void panic(const char* msg)
{
    print(msg);
    while(1) {}
}

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[PEACHOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[PEACHOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9} 
};
// 位 7（1）：存在位（P）：设置为 1 表示该段是存在的。 内核code 段

// 位 6-5（00）：特权级别（DPL）：设置为 00 表示最高特权级别（Ring 0），通常用于内核。

// 位 4（1）：描述符类型位（S）：设置为 1 表示这是一个代码或数据段，而不是系统段。

// 位 3-0：类型位：

// 位 3（1）：可执行位（E）：设置为 1 表示这是一个代码段。
// 位 2（0）：方向位/一致性位（DC）：对于代码段，设置为 0 表示代码可以从低地址执行到高地址。
// 位 1（1）：可读位（R）：设置为 1 表示代码段是可读的。
// 位 0（0）：已访问位（A）：一般由CPU设置，初始化时通常为0。


uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}
void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }
    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}
void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y,' ',0);
        }
    }   
}
void print(const char* str)
{
    
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        terminal_writechar(str[i],15);
    }
}
extern void problem();
void kernel_main()
{
    terminal_initialize();
    print("Hello World!\nTest\n");

    //panic("The system cannot continue error\n");

    memset(gdt_real,0x00,sizeof(gdt_real));

    gdt_structured_to_gdt(gdt_real,gdt_structured,PEACHOS_TOTAL_GDT_SEGMENTS);

    gdt_load(gdt_real,sizeof(gdt_real));
    
    kheap_init();

    fs_init();

    disk_search_and_init();
    
    idt_init();

    memset(&tss, 0x00, sizeof(tss));//
    tss.esp0 = 0x600000;//0x600000 is the stack address of the kernel stack
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28); // 0x28 is the offset of the TSS in the GDT
    
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    paging_switch(kernel_chunk);

    enable_paging();
    
 

    // struct disk_stream* stream = diskstreamer_new(0);
    // unsigned char c = 0;

    // diskstreamer_seek(stream,0x201);
    
    // diskstreamer_read(stream,&c,1); 

    // char buf[20];
    // strcpy(buf,"Hello World!");

    // int fd = fopen("0:/test1.txt","r");
    // if(fd)
    // {
    //     print("open test1.txt successfull\n");
    //     char buf[14];
    //     fseek(fd,3,SEEK_SET);
        
    //     fread(buf,11,1,fd);
    //     buf[13] = 0x00;
    //     print(buf);
    //     fclose(fd);
    // }

    struct process * process = 0;
    int res = process_load("0:/blank.bin",&process);
    if(res != PEACHOS_ALL_OK)
    {
        panic("process load failed\n");
    }else
    {
        print("process load successfull\n");

    }
    task_run_first_ever_task();
    while(1){}
}
