#ifndef __CTYPE_H__
#define __CTYPE_H__

WINECRT int isalpha(int c);
WINECRT int islower(int c);
WINECRT int tolower(int c);
WINECRT int toupper(int c);
WINECRT int isalnum(int c);
WINECRT int isdigit(int c);
WINECRT int iscntrl(int c);
WINECRT int isgraph(int c);
WINECRT int isprint(int c);
WINECRT int ispunct(int c);
WINECRT int isspace(int c);
WINECRT int isupper(int c);
WINECRT int isxdigit(int c);

#endif