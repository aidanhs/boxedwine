#ifndef __LOG_H__
#define __LOG_H__

void kpanic(const char* msg, ...);
void kwarn(const char* msg, ...);
void klog(const char* msg, ...);
#endif