#ifndef __STDLIB_H__
#define __STDLIB_H__

#include "sys/types.h"

void winecrt_exit(int status);
#define exit winecrt_exit

char* winecrt_getenv(const char * name);
#define getenv winecrt_getenv

int winecrt_putenv(char *string);
#define putenv winecrt_putenv

void *winecrt_malloc(size_t size);
#define malloc winecrt_malloc

void *winecrt_calloc(size_t nelem, size_t elsize);
#define calloc winecrt_calloc

void winecrt_free(void *ptr);
#define free winecrt_free

unsigned long winecrt_strtoul(const char * str, char ** endptr, int base);
#define strtoul winecrt_strtoul

void *winecrt_bsearch(const void *key, const void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
#define bsearch winecrt_bsearch

void *winecrt_realloc(void *ptr, size_t size);
#define realloc winecrt_realloc

void winecrt_abort(void);
#define abort winecrt_abort

int winecrt_atoi(const char *str);
#define atoi winecrt_atoi

int winecrt_system(const char *command);
#define system winecrt_system

void winecrt_qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
#define qsort winecrt_qsort

int winecrt_abs(int i);
#define abs winecrt_abs

long winecrt_labs(long i);
#define labs winecrt_labs

long winecrt_strtol(const char * str, char ** endptr, int base);
#define strtol winecrt_strtol

int winecrt_unsetenv(const char *name);
#define unsetenv winecrt_unsetenv

int winecrt_atexit(void (*func)());
#define atexit winecrt_atexit

char** winecrt_getEnviron();

int winecrt_mkstemps(char *template, int suffixlen);
#define mkstemps winecrt_mkstemps

int winecrt_rand(void);
#define rand winecrt_rand

//WINECRT void _allmul();
//WINECRT void _alldiv();
//WINECRT void _aulldiv();
//WINECRT void _allrem();
//WINECRT void _aullrem();
void _alldvrm();
void _aulldvrm();
void _allshl();
void _allshr();
void _aullshr();

#endif