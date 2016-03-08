#ifndef __CTYPE_H__
#define __CTYPE_H__

int winecrt_isalpha(int c);
#define isalpha winecrt_isalpha

int winecrt_islower(int c);
#define islower winecrt_islower

int winecrt_tolower(int c);
#define tolower winecrt_tolower

int winecrt_toupper(int c);
#define toupper winecrt_toupper

int winecrt_isalnum(int c);
#define isalnum winecrt_isalnum

int winecrt_isdigit(int c);
#define isdigit winecrt_isdigit

int winecrt_iscntrl(int c);
#define iscntrl winecrt_iscntrl

int winecrt_isgraph(int c);
#define isgraph winecrt_isgraph

int winecrt_isprint(int c);
#define isprint winecrt_isprint

int winecrt_ispunct(int c);
#define ispunct winecrt_ispunct

int winecrt_isspace(int c);
#define isspace winecrt_isspace

int winecrt_isupper(int c);
#define isupper winecrt_isupper

int winecrt_isxdigit(int c);
#define isxdigit winecrt_isxdigit

#endif