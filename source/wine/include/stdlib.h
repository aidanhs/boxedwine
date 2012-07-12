#ifndef __STDLIB_H__
#define __STDLIB_H__

#include "sys/types.h"

DOSHAL void exit(int status);
DOSHAL char* getenv(const char * name);
DOSHAL int putenv(char *string);
DOSHAL void *malloc(size_t size);
DOSHAL void free(void *ptr);
DOSHAL unsigned long strtoul(const char * str, char ** endptr, int base);
WINECRT void *bsearch(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
DOSHAL void *realloc(void *ptr, size_t size);
DOSHAL void abort(void);
WINECRT int atoi(const char *str);
DOSHAL int system(const char *command);
WINECRT void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
WINECRT int abs(int i);
WINECRT long labs(long i);
WINECRT long strtol(const char * str, char ** endptr, int base);
DOSHAL int unsetenv(const char *name);
WINECRT int atexit(void (*func)());
DOSHAL char** getEnviron();

WINECRT void _allmul();
WINECRT void _alldiv();
WINECRT void _aulldiv();
WINECRT void _allrem();
WINECRT void _aullrem();
WINECRT void _alldvrm();
WINECRT void _aulldvrm();
WINECRT void _allshl();
WINECRT void _allshr();
WINECRT void _aullshr();

#endif