#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "memory/heap/kheap.h"
#include "disk/disk.h"
#include "io/io.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "fs/pparser.h"
#include "fs/file.h"
#include "disk/streamer.h"
#include "stringos/stringos.h"
uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;


static struct paging_4gb_chunk* kernel_chunk = 0;

void panic(const char* msg)
{
    print(msg);
    while(1) {}
}

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
static struct paging_4gb_chunk *kernel_chunk =0;
void kernel_main()
{
    terminal_initialize();
    print("Hello World!\nTest\n");

    //panic("The system cannot continue error\n");
    
    kheap_init();

    fs_init();

    disk_search_and_init();
    
    idt_init();
    
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    
    paging_switch(kernel_chunk);

    enable_paging();
    
    enable_interrupts();

    // struct disk_stream* stream = diskstreamer_new(0);
    // unsigned char c = 0;

    // diskstreamer_seek(stream,0x201);
    
    // diskstreamer_read(stream,&c,1); 

    // char buf[20];
    // strcpy(buf,"Hello World!");

    int fd = fopen("0:/test1.txt","r");
    if(fd)
    {
        print("open test1.txt successfull\n");
        char buf[14];
        fseek(fd,3,SEEK_SET);
        fread(buf,11,1,fd);
        buf[13] = 0x00;
        print(buf);
        fclose(fd);
    }
    while(1){}
}
