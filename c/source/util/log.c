#include <stdio.h>
#include <stdarg.h>

void kpanic(const char* msg, ...) {
	int j = 0;
	int i = 1/j;
}

void kwarn(const char* msg, ...) {
	va_list argptr;
    va_start(argptr, msg);
    vfprintf(stderr, msg, argptr);
    va_end(argptr);
	fprintf(stderr, "\n");
}

void klog(const char* msg, ...) {
	va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    va_end(argptr);
	fprintf(stderr, "\n");
}
