#include <string.h>

int endsWith(const char *str, const char *suffix)
{
	int lenstr;
    int lensuffix;

    if (!str || !suffix)
        return 0;
    lenstr = strlen(str);
    lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}