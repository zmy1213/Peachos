#ifndef IO_H
#define IO_H

unsigned char insb(unsigned shor port);
unsigned short insw(unsigned shor port);

void outb(unsigned short port,unsigned char val);
void outw(unsigned short port,unsigned short val);

#endif 