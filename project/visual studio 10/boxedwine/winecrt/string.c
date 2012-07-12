#include <string.h>
#include <stdlib.h>

// public-domain implementations from http://en.wikibooks.org/wiki/C_Programming/Strings
#define NULL 0

WINECRT int memcmp(const void *s1, const void *s2, size_t n)
 {
     const unsigned char *us1 = (const unsigned char *) s1;
     const unsigned char *us2 = (const unsigned char *) s2;
     while (n-- != 0) {
         if (*us1 != *us2)
             return (*us1 < *us2) ? -1 : +1;
         us1++;
         us2++;
     }
     return 0;
 }

WINECRT void* memcpy(void* s1, const void * s2, size_t n)
 {
     char *dst = (char*)s1;
     const char *src = (char*)s2;
     /* Loop and copy.  */
     while (n-- != 0)
         *dst++ = *src++;
     return s1;
 }

WINECRT void * memmove(void *s1, const void *s2, size_t n) 
 {
    /* note: these don't have to point to unsigned chars */
    char *p1 = (char*)s1;
    const char *p2 = (const char*)s2;
    /* test for overlap that prevents an ascending copy */
    if (p2 < p1 && p1 < p2 + n) {
        /* do a descending copy */
        p2 += n;
        p1 += n;
        while (n-- != 0) 
            *--p1 = *--p2;
    } else 
        while (n-- != 0) 
            *p1++ = *p2++;
    return s1; 
 }

WINECRT void* memchr(const void *s, int c, size_t n)
 {
     const unsigned char *src = s;
     unsigned char uc = c;
     while (n-- != 0) {
         if (*src == uc)
             return (void *) src;
         src++;
     }
     return NULL;
 }

WINECRT void * memset (void *s, int c, size_t n)
 {
     unsigned char *us = (unsigned char*)s;
     unsigned char uc = c;
     while (n-- != 0)
         *us++ = uc;
     return s;
 }

WINECRT char* strcat(char * s1, const char * s2)
 {
     char *s = s1;
     /* Move s so that it points to the end of s1.  */
     while (*s != '\0')
         s++;
     /* Copy the contents of s2 into the space at the end of s1.  */
     strcpy(s, s2);
     return s1;
 }

WINECRT char* strchr(const char *s, int c)
 {
     /* Scan s for the character.  When this loop is finished,
        s will either point to the end of the string or the
        character we were looking for.  */
     while (*s != '\0' && *s != (char)c)
         s++;
     return ( (*s == c) ? (char *) s : NULL );
 }

WINECRT int strcmp(const char *s1, const char *s2)
 {
     unsigned char uc1, uc2;
     /* Move s1 and s2 to the first differing characters 
        in each string, or the ends of the strings if they
        are identical.  */
     while (*s1 != '\0' && *s1 == *s2) {
         s1++;
         s2++;
     }
     /* Compare the characters as unsigned char and
        return the difference.  */
     uc1 = (*(unsigned char *) s1);
     uc2 = (*(unsigned char *) s2);
     return ((uc1 < uc2) ? -1 : (uc1 > uc2));
 }

WINECRT char* strcpy(char * s1, const char * s2)
 {
     char *dst = s1;
     const char *src = s2;
     /* Do the copying in a loop.  */
     while ((*dst++ = *src++) != '\0')
         ;               /* The body of this loop is left empty. */
     /* Return the destination string.  */
     return s1;
 }

WINECRT char *strdup (const char *s) {
    char *d = malloc (strlen (s) + 1);
    if (d == NULL) return NULL;
    strcpy (d,s);
    return d;
}


WINECRT size_t strlen(const char *s)
 {
     const char *p = s;
     /* Loop over the data in s.  */
     while (*p != '\0')
         p++;
     return (size_t)(p - s);
 }

WINECRT int strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char uc1, uc2;
     /* Nothing to compare?  Return zero.  */
     if (n == 0)
         return 0;
     /* Loop, comparing bytes.  */
     while (n-- > 0 && *s1 == *s2) {
         /* If we've run out of bytes or hit a null, return zero
            since we already know *s1 == *s2.  */
         if (n == 0 || *s1 == '\0')
             return 0;
         s1++;
         s2++;
     }
     uc1 = (*(unsigned char *) s1);
     uc2 = (*(unsigned char *) s2);
     return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

WINECRT char* strrchr(const char *s, int c)
 {
     const char *last = NULL;
     /* If the character we're looking for is the terminating null,
        we just need to look for that character as there's only one
        of them in the string.  */
     if (c == '\0')
         return strchr(s, c);
     /* Loop through, finding the last match before hitting NULL.  */
     while ((s = strchr(s, c)) != NULL) {
         last = s;
         s++;
     }
     return (char *) last;
 }

WINECRT size_t strcspn(const char *s1, const char *s2)
 {
     const char *sc1;
     for (sc1 = s1; *sc1 != '\0'; sc1++)
         if (strchr(s2, *sc1) != NULL)
             return (sc1 - s1);
     return sc1 - s1;            /* terminating nulls match */
 }

WINECRT char *strstr(const char *haystack, const char *needle)
{
	size_t needlelen;
     /* Check for the null needle case.  */
     if (*needle == '\0')
         return (char *) haystack;
     needlelen = strlen(needle);
     for (; (haystack = strchr(haystack, *needle)) != NULL; haystack++)
         if (strncmp(haystack, needle, needlelen) == 0)
             return (char *) haystack;
     return NULL;
}

WINECRT char* strpbrk(const char *s1, const char *s2)
{
	register const char *s;
	register const char *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) return (char *) s; /* silence the warning */
		}
	}
	return NULL;
}