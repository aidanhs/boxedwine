#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

WINECRT void *memccpy(void * s1, const void * s2, int c, size_t n);
WINECRT void *memchr(const void *s, int c, size_t n);
WINECRT void * memcpy(void * destination, const void * source, size_t num);
WINECRT int memcmp(const void *s1, const void *s2, size_t n);
WINECRT void *memset(void *s, int c, size_t n);
WINECRT void *memmove(void *s1, const void *s2, size_t n);

WINECRT int strcmp(const char * str1, const char * str2);
WINECRT char *strdup(const char *s1);
WINECRT size_t strlen(const char *s);
WINECRT char *strcpy(char* s1, const char* s2);
WINECRT char *strcat(char* s1, const char* s2);
WINECRT char *strchr(const char *s, int c);
DOSHAL char *strerror(int errnum);
WINECRT int strncmp(const char *s1, const char *s2, size_t n);
WINECRT char *strrchr(const char *s, int c);
WINECRT char *strpbrk(const char *s1, const char *s2);
WINECRT size_t strcspn(const char *s1, const char *s2);
WINECRT char *strstr(const char *s1, const char *s2);
char *strncat(char * s1, const char * s2, size_t n);
char *strncpy(char *s1, const char *s2, size_t n);
WINECRT size_t strspn(const char *s1, const char *s2);


#endif