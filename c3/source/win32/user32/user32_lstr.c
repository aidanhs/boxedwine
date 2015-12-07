/*
 * USER string functions
 *
 * Copyright 1993 Yngvi Sigurjonsson (yngvi@hafro.is)
 * Copyright 1996 Alexandre Julliard
 * Copyright 1996 Marcus Meissner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/winbase.h"
#include "../include/unicode.h"


/***********************************************************************
 *           CharNextA   (USER32.@)
 */
LPSTR WINAPI CharNextA_k( LPCSTR ptr )
{
    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByte_k( ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}


/***********************************************************************
 *           CharNextExA   (USER32.@)
 */
LPSTR WINAPI CharNextExA_k( WORD codepage, LPCSTR ptr, DWORD flags )
{
    if (!*ptr) return (LPSTR)ptr;
    if (IsDBCSLeadByteEx_k( codepage, ptr[0] ) && ptr[1]) return (LPSTR)(ptr + 2);
    return (LPSTR)(ptr + 1);
}


/***********************************************************************
 *           CharNextExW   (USER32.@)
 */
LPWSTR WINAPI CharNextExW_k( WORD codepage, LPCWSTR ptr, DWORD flags )
{
    /* doesn't make sense, there are no codepages for Unicode */
    return NULL;
}


/***********************************************************************
 *           CharNextW   (USER32.@)
 */
LPWSTR WINAPI CharNextW_k(LPCWSTR x)
{
    if (*x) x++;

    return (LPWSTR)x;
}


/***********************************************************************
 *           CharPrevA   (USER32.@)
 */
LPSTR WINAPI CharPrevA_k( LPCSTR start, LPCSTR ptr )
{
    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextA_k( start );
        if (next >= ptr) break;
        start = next;
    }
    return (LPSTR)start;
}


/***********************************************************************
 *           CharPrevExA   (USER32.@)
 */
LPSTR WINAPI CharPrevExA_k( WORD codepage, LPCSTR start, LPCSTR ptr, DWORD flags )
{
    while (*start && (start < ptr))
    {
        LPCSTR next = CharNextExA_k( codepage, start, flags );
        if (next >= ptr) break;
        start = next;
    }
    return (LPSTR)start;
}


/***********************************************************************
 *           CharPrevExW   (USER32.@)
 */
LPSTR WINAPI CharPrevExW_k( WORD codepage, LPCWSTR start, LPCWSTR ptr, DWORD flags )
{
    /* doesn't make sense, there are no codepages for Unicode */
    return NULL;
}


/***********************************************************************
 *           CharPrevW   (USER32.@)
 */
LPWSTR WINAPI CharPrevW_k(LPCWSTR start,LPCWSTR x)
{
    if (x>start) return (LPWSTR)(x-1);
    else return (LPWSTR)x;
}


/***********************************************************************
 *           CharToOemA   (USER32.@)
 */
BOOL WINAPI CharToOemA_k( LPCSTR s, LPSTR d )
{
    if ( !s || !d ) return TRUE;
    return CharToOemBuffA_k( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharToOemBuffA   (USER32.@)
 */
BOOL WINAPI CharToOemBuffA_k( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    bufW = HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar_k( CP_ACP, 0, s, len, bufW, len );
	WideCharToMultiByte_k( CP_OEMCP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}


/***********************************************************************
 *           CharToOemBuffW   (USER32.@)
 */
BOOL WINAPI CharToOemBuffW_k( LPCWSTR s, LPSTR d, DWORD len )
{
    if ( !s || !d ) return TRUE;
    WideCharToMultiByte_k( CP_OEMCP, 0, s, len, d, len, NULL, NULL );
    return TRUE;
}


/***********************************************************************
 *           CharToOemW   (USER32.@)
 */
BOOL WINAPI CharToOemW_k( LPCWSTR s, LPSTR d )
{
    return CharToOemBuffW_k( s, d, strlenW( s ) + 1 );
}


/***********************************************************************
 *           OemToCharA   (USER32.@)
 */
BOOL WINAPI OemToCharA_k( LPCSTR s, LPSTR d )
{
    return OemToCharBuffA_k( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           OemToCharBuffA   (USER32.@)
 */
BOOL WINAPI OemToCharBuffA_k( LPCSTR s, LPSTR d, DWORD len )
{
    WCHAR *bufW;

    bufW = HeapAlloc( GetProcessHeap(), 0, len * sizeof(WCHAR) );
    if( bufW )
    {
	MultiByteToWideChar_k( CP_OEMCP, 0, s, len, bufW, len );
	WideCharToMultiByte_k( CP_ACP, 0, bufW, len, d, len, NULL, NULL );
	HeapFree( GetProcessHeap(), 0, bufW );
    }
    return TRUE;
}


/***********************************************************************
 *           OemToCharBuffW   (USER32.@)
 */
BOOL WINAPI OemToCharBuffW_k( LPCSTR s, LPWSTR d, DWORD len )
{
    MultiByteToWideChar_k( CP_OEMCP, 0, s, len, d, len );
    return TRUE;
}


/***********************************************************************
 *           OemToCharW   (USER32.@)
 */
BOOL WINAPI OemToCharW_k( LPCSTR s, LPWSTR d )
{
    return OemToCharBuffW_k( s, d, strlen( s ) + 1 );
}


/***********************************************************************
 *           CharLowerA   (USER32.@)
 */
LPSTR WINAPI CharLowerA_k(LPSTR str)
{
    if (IS_INTRESOURCE(str))
    {
        char ch = LOWORD(str);
        CharLowerBuffA_k( &ch, 1 );
        return (LPSTR)(UINT_PTR)(BYTE)ch;
    }

    //__TRY
    {
        CharLowerBuffA_k( str, strlen(str) );
    }
    /*
    __EXCEPT_PAGE_FAULT
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return NULL;
    }
    __ENDTRY
    */
    return str;
}


/***********************************************************************
 *           CharUpperA   (USER32.@)
 */
LPSTR WINAPI CharUpperA_k(LPSTR str)
{
    if (IS_INTRESOURCE(str))
    {
        char ch = LOWORD(str);
        CharUpperBuffA_k( &ch, 1 );
        return (LPSTR)(UINT_PTR)(BYTE)ch;
    }

    //__TRY
    {
        CharUpperBuffA_k( str, strlen(str) );
    }
    /*
    __EXCEPT_PAGE_FAULT
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return NULL;
    }
    __ENDTRY
    */
    return str;
}


/***********************************************************************
 *           CharLowerW   (USER32.@)
 */
LPWSTR WINAPI CharLowerW_k(LPWSTR x)
{
    if (!IS_INTRESOURCE(x)) return strlwrW(x);
    else return (LPWSTR)((UINT_PTR)tolowerW(LOWORD(x)));
}


/***********************************************************************
 *           CharUpperW   (USER32.@)
 */
LPWSTR WINAPI CharUpperW_k(LPWSTR x)
{
    if (!IS_INTRESOURCE(x)) return struprW(x);
    else return (LPWSTR)((UINT_PTR)toupperW(LOWORD(x)));
}


/***********************************************************************
 *           CharLowerBuffA   (USER32.@)
 */
DWORD WINAPI CharLowerBuffA_k( LPSTR str, DWORD len )
{
    DWORD lenW;
    WCHAR buffer[32];
    WCHAR *strW = buffer;

    if (!str) return 0; /* YES */

    lenW = MultiByteToWideChar_k(CP_ACP, 0, str, len, NULL, 0);
    if (lenW > sizeof(buffer)/sizeof(WCHAR))
    {
        strW = HeapAlloc(GetProcessHeap(), 0, lenW * sizeof(WCHAR));
        if (!strW) return 0;
    }
    MultiByteToWideChar_k(CP_ACP, 0, str, len, strW, lenW);
    CharLowerBuffW_k(strW, lenW);
    len = WideCharToMultiByte_k(CP_ACP, 0, strW, lenW, str, len, NULL, NULL);
    if (strW != buffer) HeapFree(GetProcessHeap(), 0, strW);
    return len;
}


/***********************************************************************
 *           CharLowerBuffW   (USER32.@)
 */
DWORD WINAPI CharLowerBuffW_k( LPWSTR str, DWORD len )
{
    DWORD ret = len;
    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = tolowerW(*str);
    return ret;
}


/***********************************************************************
 *           CharUpperBuffA   (USER32.@)
 */
DWORD WINAPI CharUpperBuffA_k( LPSTR str, DWORD len )
{
    DWORD lenW;
    WCHAR buffer[32];
    WCHAR *strW = buffer;

    if (!str) return 0; /* YES */

    lenW = MultiByteToWideChar_k(CP_ACP, 0, str, len, NULL, 0);
    if (lenW > sizeof(buffer)/sizeof(WCHAR))
    {
        strW = HeapAlloc(GetProcessHeap(), 0, lenW * sizeof(WCHAR));
        if (!strW) return 0;
    }
    MultiByteToWideChar_k(CP_ACP, 0, str, len, strW, lenW);
    CharUpperBuffW_k(strW, lenW);
    len = WideCharToMultiByte_k(CP_ACP, 0, strW, lenW, str, len, NULL, NULL);
    if (strW != buffer) HeapFree(GetProcessHeap(), 0, strW);
    return len;
}


/***********************************************************************
 *           CharUpperBuffW   (USER32.@)
 */
DWORD WINAPI CharUpperBuffW_k( LPWSTR str, DWORD len )
{
    DWORD ret = len;
    if (!str) return 0; /* YES */
    for (; len; len--, str++) *str = toupperW(*str);
    return ret;
}


/***********************************************************************
 *           IsCharLower    (USER.436)
 *           IsCharLowerA   (USER32.@)
 */
BOOL WINAPI IsCharLowerA_k(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar_k(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharLowerW_k(wch);
}


/***********************************************************************
 *           IsCharLowerW   (USER32.@)
 */
BOOL WINAPI IsCharLowerW_k(WCHAR x)
{
    return (get_char_typeW(x) & C1_LOWER) != 0;
}


/***********************************************************************
 *           IsCharUpper    (USER.435)
 *           IsCharUpperA   (USER32.@)
 */
BOOL WINAPI IsCharUpperA_k(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar_k(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharUpperW_k(wch);
}


/***********************************************************************
 *           IsCharUpperW   (USER32.@)
 */
BOOL WINAPI IsCharUpperW_k(WCHAR x)
{
    return (get_char_typeW(x) & C1_UPPER) != 0;
}


/***********************************************************************
 *           IsCharAlphaNumeric    (USER.434)
 *           IsCharAlphaNumericA   (USER32.@)
 */
BOOL WINAPI IsCharAlphaNumericA_k(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar_k(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharAlphaNumericW_k(wch);
}


/***********************************************************************
 *           IsCharAlphaNumericW   (USER32.@)
 */
BOOL WINAPI IsCharAlphaNumericW_k(WCHAR x)
{
    return (get_char_typeW(x) & (C1_ALPHA|C1_DIGIT)) != 0;
}


/***********************************************************************
 *           IsCharAlpha    (USER.433)
 *           IsCharAlphaA   (USER32.@)
 */
BOOL WINAPI IsCharAlphaA_k(CHAR x)
{
    WCHAR wch;
    MultiByteToWideChar_k(CP_ACP, 0, &x, 1, &wch, 1);
    return IsCharAlphaW_k(wch);
}


/***********************************************************************
 *           IsCharAlphaW   (USER32.@)
 */
BOOL WINAPI IsCharAlphaW_k(WCHAR x)
{
    return (get_char_typeW(x) & C1_ALPHA) != 0;
}
