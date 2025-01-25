#ifndef TASKSWITCHSEGMENT_H
#define TASKSWITCHSEGMENT_H


#include <stdint.h>
struct tss
{
    uint32_t link;// previous tss
    uint32_t esp0; // Kernel stack pointer
    uint32_t ss0; // Kernel stack segment
    uint32_t esp1;// Ring 1 stack pointer
    uint32_t ss1;// Ring 1 stack segment
    uint32_t esp2;// Ring 2 stack pointer
    uint32_t ss2;// Ring 2 stack segment
    uint32_t sr3;// reserved
    uint32_t eip;// Instruction pointer
    uint32_t eflags;// Flags
    uint32_t eax;// General purpose registers
    uint32_t ecx;// General purpose registers
    uint32_t edx;// General purpose registers
    uint32_t ebx;// General purpose registers
    uint32_t esp;// Stack pointer
    uint32_t ebp;// Base pointer
    uint32_t esi;// Source index
    uint32_t edi;// Destination index
    uint32_t es;// Extra segment
    uint32_t cs;// Code segment
    uint32_t ss;// Stack segment
    uint32_t ds;// Data segment
    uint32_t fs;// Extra segment
    uint32_t gs;// Extra segment
    uint32_t ldtr;// Local descriptor table register
    uint32_t iopb;// I/O port bitmap
    uint32_t ssp;// Stack segment pointer
} __attribute__((packed));

void tss_load(int tss_segment);

#endif