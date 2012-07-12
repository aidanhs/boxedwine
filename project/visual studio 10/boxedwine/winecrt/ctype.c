#include <ctype.h>

WINECRT int isalpha(int c)
{
	return islower(c) || isupper(c);
}

WINECRT int islower(int c)
{
	return c>='a' && c<='z';
}

WINECRT int tolower(int c)
{
	if (isupper(c))
		c = 'A' + (c - 'a');
	return c;
}

WINECRT int toupper(int c)
{
	if (islower(c))
		c = 'a' + (c - 'A');
	return c;
}

WINECRT int isalnum(int c)
{
	return isdigit(c) || isalpha(c);
}

WINECRT int isdigit(int c)
{
	return (c>='0' && c<='9');
}

WINECRT int iscntrl(int c)
{
	return ((c>=0 && c<=0x1F) || c==0x7F);
}

WINECRT int isgraph(int c)
{
	return (c != ' ' && isprint(c));
}

WINECRT int isprint(int c)
{
	return (c>=0x20 && c<0x7e);
}

WINECRT int ispunct(int c) 
{
	return ((c > ' ' && c <= '~') && !isalnum(c));
}

WINECRT int isspace(int c)
{
	return (c ==  ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

WINECRT int isupper(int c)
{
	return (c>='A' && c<='Z');
}

WINECRT int isxdigit(int c)
{
	return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F');
}

