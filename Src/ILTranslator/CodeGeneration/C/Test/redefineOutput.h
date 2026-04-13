#ifndef _REPRINTF_H
#define _REPRINTF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#define va_list char* //参数指针
#define va_start(ap, arg) (ap = (va_list)&arg + sizeof(arg))//指向可变参数的第一个参数地址
#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))//指向可变参数的下一个参数地址
#define va_end(ap) (ap = (va_list)0)//指空 防止野指�?#else
#include <Windows.h>
#endif


int refputc(char c, FILE* stream);
int refputs(char* str, FILE* stream);
int reputc(char c, FILE *stream);
int reputchar(char c);
int reputs(char* str);
int revfprintf(FILE* stream, const char* format, ...);
int reprintf(const char* format, ...);
int refprintf(FILE* stream, const char* format, ...);
FILE* refopen(const char *filename, const char *mode);
int refclose(FILE *fp);


#endif
