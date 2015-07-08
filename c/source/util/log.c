#include <stdio.h>
#include <stdarg.h>

FILE* logFile;

void kpanic(const char* msg, ...) {
	int j = 0;
	int i;

	va_list argptr;
    va_start(argptr, msg);
    vfprintf(stderr, msg, argptr);
	if (logFile) {
		vfprintf(logFile, msg, argptr);
	}
    va_end(argptr);
	fprintf(stderr, "\n");
	if (logFile) {
		fprintf(logFile, "\n");
		fflush(logFile);
		fclose(logFile);
	}
	i=1/j;
}

void kwarn(const char* msg, ...) {
#ifdef _DEBUG
	va_list argptr;
    va_start(argptr, msg);
    vfprintf(stderr, msg, argptr);
	if (logFile) {
		vfprintf(logFile, msg, argptr);
	}
    va_end(argptr);
	fprintf(stderr, "\n");
	if (logFile) {
		fprintf(logFile, "\n");
	}
#endif
}

void klog(const char* msg, ...) {
	va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
	if (logFile) {
		vfprintf(logFile, msg, argptr);
	}
    va_end(argptr);
	fprintf(stderr, "\n");
	if (logFile) {
		fprintf(logFile, "\n");
	}
}
