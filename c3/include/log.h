#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

extern FILE* logFile;

void kpanic(const char* msg, ...);
void kwarn(const char* msg, ...);
void klog(const char* msg, ...);
#endif