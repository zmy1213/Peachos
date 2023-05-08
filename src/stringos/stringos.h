#ifndef STRINGOS_H
#define STRINGOS_H
#include<stdbool.h>
int strlen(const char*ptr);
int strnlen(const char *ptr,int max);
bool isdigit(char c);
int tonumericdigit(char c);

#endif