#ifndef __STRING_H__
#define __STRING_H__

#include <sys/types.h>

void *winecrt_memccpy(void * s1, const void * s2, int c, size_t n);
#define memccpy winecrt_memccpy

void *winecrt_memchr(const void *s, int c, size_t n);
#define memchr winecrt_memchr

void *winecrt_memcpy(void * destination, const void * source, size_t num);
#define memcpy winecrt_memcpy

int winecrt_memcmp(const void *s1, const void *s2, size_t n);
#define memcmp winecrt_memcmp

void *winecrt_memset(void *s, int c, size_t n);
#define memset winecrt_memset

void *winecrt_memmove(void *s1, const void *s2, size_t n);
#define memmove winecrt_memmove

int winecrt_strcmp(const char * str1, const char * str2);
#define strcmp winecrt_strcmp

char *winecrt_strdup(const char *s1);
#define strdup winecrt_strdup

size_t winecrt_strlen(const char *s);
#define strlen winecrt_strlen

char *winecrt_strcpy(char* s1, const char* s2);
#define strcpy winecrt_strcpy

char *winecrt_strcat(char* s1, const char* s2);
#define strcat winecrt_strcat

char *winecrt_strchr(const char *s, int c);
#define strchr winecrt_strchr

char *winecrt_strerror(int errnum);
#define strerror winecrt_strerror

int winecrt_strncmp(const char *s1, const char *s2, size_t n);
#define strncmp winecrt_strncmp

char *winecrt_strrchr(const char *s, int c);
#define strrchr winecrt_strrchr

char *winecrt_strpbrk(const char *s1, const char *s2);
#define strpbrk winecrt_strpbrk

size_t winecrt_strcspn(const char *s1, const char *s2);
#define strcspn winecrt_strcspn

char *winecrt_strstr(const char *s1, const char *s2);
#define strstr winecrt_strstr

char *winecrt_strncat(char * s1, const char * s2, size_t n);
#define strncat winecrt_strncat

char *winecrt_strncpy(char *s1, const char *s2, size_t n);
#define strncpy winecrt_strncpy

size_t winecrt_strspn(const char *s1, const char *s2);
#define strspn winecrt_strspn

int winecrt_strcasecmp(const char *s1, const char *s2);
#define strcasecmp winecrt_strcasecmp

int winecrt_strncasecmp(const char *s1, const char *s2, size_t n);
#define strncasecmp winecrt_strncasecmp


#endif