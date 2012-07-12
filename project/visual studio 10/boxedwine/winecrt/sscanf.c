#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#define SDL_bool int
#define SDL_FALSE 0
#define SDL_TRUE 1
#define SDL_isspace isspace
#define SDL_strncmp strncmp
#define SDL_isdigit isdigit
#define SDL_tolower tolower
#define SDL_isupperhex(X)   (((X) >= 'A') && ((X) <= 'F'))
#define SDL_islowerhex(X)   (((X) >= 'a') && ((X) <= 'f'))
#define Sint64 long long
#define uintptr_t unsigned int
#define Uint64 unsigned long long
#define SDL_HAS_64BIT_TYPE

size_t SDL_ScanLong(const char *text, int radix, long *valuep)
{
    const char *textstart = text;
    long value = 0;
    SDL_bool negative = SDL_FALSE;

    if ( *text == '-' ) {
        negative = SDL_TRUE;
        ++text;
    }
    if ( radix == 16 && SDL_strncmp(text, "0x", 2) == 0 ) {
        text += 2;
    }
    for ( ; ; ) {
        int v;
        if ( SDL_isdigit((unsigned char) *text) ) {
            v = *text - '0';
        } else if ( radix == 16 && SDL_isupperhex(*text) ) {
            v = 10 + (*text - 'A');
        } else if ( radix == 16 && SDL_islowerhex(*text) ) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if ( valuep ) {
        if ( negative && value ) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}

size_t SDL_ScanUnsignedLong(const char *text, int radix, unsigned long *valuep)
{
    const char *textstart = text;
    unsigned long value = 0;

    if ( radix == 16 && SDL_strncmp(text, "0x", 2) == 0 ) {
        text += 2;
    }
    for ( ; ; ) {
        int v;
        if ( SDL_isdigit((unsigned char) *text) ) {
            v = *text - '0';
        } else if ( radix == 16 && SDL_isupperhex(*text) ) {
            v = 10 + (*text - 'A');
        } else if ( radix == 16 && SDL_islowerhex(*text) ) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if ( valuep ) {
        *valuep = value;
    }
    return (text - textstart);
}

static size_t SDL_ScanUintPtrT(const char *text, int radix, uintptr_t *valuep)
{
    const char *textstart = text;
    uintptr_t value = 0;

    if ( radix == 16 && SDL_strncmp(text, "0x", 2) == 0 ) {
        text += 2;
    }
    for ( ; ; ) {
        int v;
        if ( SDL_isdigit((unsigned char) *text) ) {
            v = *text - '0';
        } else if ( radix == 16 && SDL_isupperhex(*text) ) {
            v = 10 + (*text - 'A');
        } else if ( radix == 16 && SDL_islowerhex(*text) ) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if ( valuep ) {
        *valuep = value;
    }
    return (text - textstart);
}

static size_t SDL_ScanLongLong(const char *text, int radix, Sint64 *valuep)
{
    const char *textstart = text;
    Sint64 value = 0;
    SDL_bool negative = SDL_FALSE;

    if ( *text == '-' ) {
        negative = SDL_TRUE;
        ++text;
    }
    if ( radix == 16 && SDL_strncmp(text, "0x", 2) == 0 ) {
        text += 2;
    }
    for ( ; ; ) {
        int v;
        if ( SDL_isdigit((unsigned char) *text) ) {
            v = *text - '0';
        } else if ( radix == 16 && SDL_isupperhex(*text) ) {
            v = 10 + (*text - 'A');
        } else if ( radix == 16 && SDL_islowerhex(*text) ) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if ( valuep ) {
        if ( negative && value ) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}

static size_t SDL_ScanUnsignedLongLong(const char *text, int radix, Uint64 *valuep)
{
    const char *textstart = text;
    Uint64 value = 0;

    if ( radix == 16 && SDL_strncmp(text, "0x", 2) == 0 ) {
        text += 2;
    }
    for ( ; ; ) {
        int v;
        if ( SDL_isdigit((unsigned char) *text) ) {
            v = *text - '0';
        } else if ( radix == 16 && SDL_isupperhex(*text) ) {
            v = 10 + (*text - 'A');
        } else if ( radix == 16 && SDL_islowerhex(*text) ) {
            v = 10 + (*text - 'a');
        } else {
            break;
        }
        value *= radix;
        value += v;
        ++text;
    }
    if ( valuep ) {
        *valuep = value;
    }
    return (text - textstart);
}

static size_t SDL_ScanFloat(const char *text, double *valuep)
{
    const char *textstart = text;
    unsigned long lvalue = 0;
    double value = 0.0;
    SDL_bool negative = SDL_FALSE;

    if ( *text == '-' ) {
        negative = SDL_TRUE;
        ++text;
    }
    text += SDL_ScanUnsignedLong(text, 10, &lvalue);
    value += lvalue;
    if ( *text == '.' ) {
        int mult = 10;
        ++text;
        while ( SDL_isdigit((unsigned char) *text) ) {
            lvalue = *text - '0';
            value += (double)lvalue / mult;
            mult *= 10;
            ++text;
        }
    }
    if ( valuep ) {
        if ( negative && value ) {
            *valuep = -value;
        } else {
            *valuep = value;
        }
    }
    return (text - textstart);
}

WINECRT int vsscanf(const char *text, const char *fmt, va_list ap)
{    
    int retval = 0;
    
    while ( *fmt ) {
        if ( *fmt == ' ' ) {
            while ( SDL_isspace((unsigned char) *text) ) {
                ++text;
            }
            ++fmt;
            continue;
        }
        if ( *fmt == '%' ) {
            SDL_bool done = SDL_FALSE;
            long count = 0;
            int radix = 10;
            enum {
                DO_SHORT,
                DO_INT,
                DO_LONG,
                DO_LONGLONG
            } inttype = DO_INT;
            SDL_bool suppress = SDL_FALSE;

            ++fmt;
            if ( *fmt == '%' ) {
                if ( *text == '%' ) {
                    ++text;
                    ++fmt;
                    continue;
                }
                break;
            }
            if ( *fmt == '*' ) {
                suppress = SDL_TRUE;
                ++fmt;
            }
            fmt += SDL_ScanLong(fmt, 10, &count);

            if ( *fmt == 'c' ) {
                if ( ! count ) {
                    count = 1;
                }
                if ( suppress ) {
                    while ( count-- ) {
                        ++text;
                    }
                } else {
                    char *valuep = va_arg(ap, char*);
                    while ( count-- ) {
                        *valuep++ = *text++;
                    }
                    ++retval;
                }
                continue;
            }

            while ( SDL_isspace((unsigned char) *text) ) {
                ++text;
            }

            /* FIXME: implement more of the format specifiers */
            while (!done) {
                switch(*fmt) {
                    case '*':
                        suppress = SDL_TRUE;
                        break;
                    case 'h':
                        if ( inttype > DO_SHORT ) {
                            ++inttype;
                        }
                        break;
                    case 'l':
                        if ( inttype < DO_LONGLONG ) {
                            ++inttype;
                        }
                        break;
                    case 'I':
                        if ( SDL_strncmp(fmt, "I64", 3) == 0 ) {
                            fmt += 2;
                            inttype = DO_LONGLONG;
                        }
                        break;
                    case 'i':
                        {
                            int index = 0;
                            if ( text[index] == '-' ) {
                                ++index;
                            }
                            if ( text[index] == '0' ) {
                                if ( SDL_tolower((unsigned char) text[index+1]) == 'x' ) {
                                    radix = 16;
                                } else {
                                    radix = 8;
                                }
                            }
                        }
                        /* Fall through to %d handling */
                    case 'd':
#ifdef SDL_HAS_64BIT_TYPE
                        if ( inttype == DO_LONGLONG ) {
                            Sint64 value;
                            text += SDL_ScanLongLong(text, radix, &value);
                            if ( ! suppress ) {
                                Sint64 *valuep = va_arg(ap, Sint64*);
                                *valuep = value;
                                ++retval;
                            }
                        }
                        else
#endif /* SDL_HAS_64BIT_TYPE */
                        {
                            long value;
                            text += SDL_ScanLong(text, radix, &value);
                            if ( ! suppress ) {
                                switch (inttype) {
                                    case DO_SHORT:
                                        { short* valuep = va_arg(ap, short*);
                                            *valuep = (short)value;
                                        }
                                        break;
                                    case DO_INT:
                                        { int* valuep = va_arg(ap, int*);
                                            *valuep = (int)value;
                                        }
                                        break;
                                    case DO_LONG:
                                        { long* valuep = va_arg(ap, long*);
                                            *valuep = value;
                                        }
                                        break;
                                    case DO_LONGLONG:
                                        /* Handled above */
                                        break;
                                }
                                ++retval;
                            }
                        }
                        done = SDL_TRUE;
                        break;
                    case 'o':
                        if ( radix == 10 ) {
                            radix = 8;
                        }
                        /* Fall through to unsigned handling */
                    case 'x':
                    case 'X':
                        if ( radix == 10 ) {
                            radix = 16;
                        }
                        /* Fall through to unsigned handling */
                    case 'u':
#ifdef SDL_HAS_64BIT_TYPE
                        if ( inttype == DO_LONGLONG ) {
                            Uint64 value;
                            text += SDL_ScanUnsignedLongLong(text, radix, &value);
                            if ( ! suppress ) {
                                Uint64 *valuep = va_arg(ap, Uint64*);
                                *valuep = value;
                                ++retval;
                            }
                        }
                        else
#endif /* SDL_HAS_64BIT_TYPE */
                        {
                            unsigned long value;
                            text += SDL_ScanUnsignedLong(text, radix, &value);
                            if ( ! suppress ) {
                                switch (inttype) {
                                    case DO_SHORT:
                                        { short* valuep = va_arg(ap, short*);
                                            *valuep = (short)value;
                                        }
                                        break;
                                    case DO_INT:
                                        { int* valuep = va_arg(ap, int*);
                                            *valuep = (int)value;
                                        }
                                        break;
                                    case DO_LONG:
                                        { long* valuep = va_arg(ap, long*);
                                            *valuep = value;
                                        }
                                        break;
                                    case DO_LONGLONG:
                                        /* Handled above */
                                        break;
                                }
                                ++retval;
                            }
                        }
                        done = SDL_TRUE;
                        break;
                    case 'p':
                        {
                            uintptr_t value;
                            text += SDL_ScanUintPtrT(text, 16, &value);
                            if ( ! suppress ) {
                                void** valuep = va_arg(ap, void**);
                                *valuep = (void*)value;
                                ++retval;
                            }
                        }
                        done = SDL_TRUE;
                        break;
                    case 'f':
                        {
                            double value;
                            text += SDL_ScanFloat(text, &value);
                            if ( ! suppress ) {
                                float* valuep = va_arg(ap, float*);
                                *valuep = (float)value;
                                ++retval;
                            }
                        }
                        done = SDL_TRUE;
                        break;
                    case 's':
                        if ( suppress ) {
                            while ( !SDL_isspace((unsigned char) *text) ) {
                                ++text;
                                if ( count ) {
                                    if ( --count == 0 ) {
                                        break;
                                    }
                                }
                            }
                        } else {
                            char *valuep = va_arg(ap, char*);
                            while ( !SDL_isspace((unsigned char) *text) ) {
                                *valuep++ = *text++;
                                if ( count ) {
                                    if ( --count == 0 ) {
                                        break;
                                    }
                                }
                            }
                            *valuep = '\0';
                            ++retval;
                        }
                        done = SDL_TRUE;
                        break;
                    default:
                        done = SDL_TRUE;
                        break;
                }
                ++fmt;
            }
            continue;
        }
        if ( *text == *fmt ) {
            ++text;
            ++fmt;
            continue;
        }
        /* Text didn't match format specifier */
        break;
    }    

    return retval;
}

WINECRT int sscanf(const char *text, const char *fmt, ...)
{
	va_list ap;
	int retval = 0;
	va_start(ap, fmt);
	retval = vsscanf(text, fmt, ap);
	va_end(ap);
	return retval;
}

WINECRT int fscanf(FILE * stream, const char * format, ... ) {
	va_list ap;
	int retval = 0;	
	char buf[1024];
	int len = fread(buf, 1024, 1, stream);	
	if (len<=0)
		return 0;
	if (len>=1024)
		buf[1023]=0;
	va_start(ap, format);
	retval = vsscanf(buf, format, ap);
	va_end(ap);
	return retval;
}