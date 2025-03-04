#ifndef KERNEL_H
#define KERNEL_H

void kernel_main();
void print(const char*str);
void panic(const char* msg);
void kernel_page();
void kernel_registers();

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define PEACHOS_MAXPATH 108
#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)
#endif