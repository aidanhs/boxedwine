/*
 * Window properties
 *
 * Copyright 1995, 1996, 2001 Alexandre Julliard
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

#include <stdarg.h>
#include <string.h>
#include "../include/winbase.h"
#include "../include/unicode.h"
#include "win.h"

/* size of buffer needed to store an atom string */
#define ATOM_BUFFER_SIZE 256


/***********************************************************************
 *              EnumPropsA_relay
 *
 * relay to call the EnumProps callback function from EnumPropsEx
 */
static BOOL CALLBACK EnumPropsA_relay( HWND hwnd, LPSTR str, HANDLE handle, ULONG_PTR lparam )
{
    PROPENUMPROCA func = (PROPENUMPROCA)lparam;
    return func( hwnd, str, handle );
}


/***********************************************************************
 *              EnumPropsW_relay
 *
 * relay to call the EnumProps callback function from EnumPropsEx
 */
static BOOL CALLBACK EnumPropsW_relay( HWND hwnd, LPWSTR str, HANDLE handle, ULONG_PTR lparam )
{
    PROPENUMPROCW func = (PROPENUMPROCW)lparam;
    return func( hwnd, str, handle );
}


/***********************************************************************
 *              EnumPropsA   (USER32.@)
 */
INT WINAPI EnumPropsA_k( HWND hwnd, PROPENUMPROCA func )
{
    return EnumPropsExA_k( hwnd, EnumPropsA_relay, (LPARAM)func );
}


/***********************************************************************
 *              EnumPropsW   (USER32.@)
 */
INT WINAPI EnumPropsW_k( HWND hwnd, PROPENUMPROCW func )
{
    return EnumPropsExW_k( hwnd, EnumPropsW_relay, (LPARAM)func );
}


/***********************************************************************
 *              GetPropA   (USER32.@)
 */
HANDLE WINAPI GetPropA_k( HWND hwnd, LPCSTR str )
{
    WCHAR buffer[ATOM_BUFFER_SIZE];

    if (IS_INTRESOURCE(str)) return GetPropW_k( hwnd, (LPCWSTR)str );
    if (!MultiByteToWideChar_k( CP_ACP, 0, str, -1, buffer, ATOM_BUFFER_SIZE )) return 0;
    return GetPropW_k( hwnd, buffer );
}


/***********************************************************************
 *              GetPropW   (USER32.@)
 */
HANDLE WINAPI GetPropW_k( HWND hwnd, LPCWSTR str )
{
    WND* win = WIN_GetPtr(hwnd);    

    if (!win) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    }    
    if (IS_INTRESOURCE(str)) {
        WCHAR string[ATOM_BUFFER_SIZE];
        if (!GlobalGetAtomNameW_k( (ATOM)str, string, ATOM_BUFFER_SIZE )) return 0;
        return pblMapGet(win->properties, string, strlenW(string)+1, NULL);
    } else {
        return pblMapGet(win->properties, (void*)str, strlenW(str)+1, NULL);
    }    
}

/***********************************************************************
 *              SetPropA   (USER32.@)
 */
BOOL WINAPI SetPropA_k( HWND hwnd, LPCSTR str, HANDLE handle )
{
    WCHAR buffer[ATOM_BUFFER_SIZE];

    if (IS_INTRESOURCE(str)) return SetPropW_k( hwnd, (LPCWSTR)str, handle );
    if (!MultiByteToWideChar_k( CP_ACP, 0, str, -1, buffer, ATOM_BUFFER_SIZE )) return FALSE;
    return SetPropW_k( hwnd, buffer, handle );
}


/***********************************************************************
 *              SetPropW   (USER32.@)
 */
BOOL WINAPI SetPropW_k( HWND hwnd, LPCWSTR str, HANDLE handle )
{
    WND* win = WIN_GetPtr(hwnd);    

    if (!win) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    }    
    if (IS_INTRESOURCE(str)) {
        WCHAR string[ATOM_BUFFER_SIZE];
        if (!GlobalGetAtomNameW_k( (ATOM)str, string, ATOM_BUFFER_SIZE )) return FALSE;
        pblMapPut(win->properties, string, strlenW(string)+1, handle, sizeof(HANDLE), NULL);
    } else {
        pblMapPut(win->properties, (void*)str, strlenW(str)+1, handle, sizeof(HANDLE), NULL);
    } 
    return TRUE;
}


/***********************************************************************
 *              RemovePropA   (USER32.@)
 */
HANDLE WINAPI RemovePropA_k( HWND hwnd, LPCSTR str )
{
    WCHAR buffer[ATOM_BUFFER_SIZE];

    if (IS_INTRESOURCE(str)) return RemovePropW_k( hwnd, (LPCWSTR)str );
    if (!MultiByteToWideChar_k( CP_ACP, 0, str, -1, buffer, ATOM_BUFFER_SIZE )) return 0;
    return RemovePropW_k( hwnd, buffer );
}


/***********************************************************************
 *              RemovePropW   (USER32.@)
 */
HANDLE WINAPI RemovePropW_k( HWND hwnd, LPCWSTR str )
{
    WND* win = WIN_GetPtr(hwnd);    

    if (!win) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    }    
    if (IS_INTRESOURCE(str)) {
        WCHAR string[ATOM_BUFFER_SIZE];
        if (!GlobalGetAtomNameW_k( (ATOM)str, string, ATOM_BUFFER_SIZE )) return 0;
        return pblMapRemove(win->properties, string, strlenW(string)+1, NULL);
    } else {
        return pblMapRemove(win->properties, (void*)str, strlenW(str)+1, NULL);
    } 
}


/***********************************************************************
 *              EnumPropsExA   (USER32.@)
 */
INT WINAPI EnumPropsExA_k(HWND hwnd, PROPENUMPROCEXA func, LPARAM lParam)
{
    WND* win = WIN_GetPtr(hwnd);    
    PblIterator* it;
    char string[ATOM_BUFFER_SIZE];
    INT ret = -1;

    if (!win) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    } 

    it = pblMapIteratorNew(win->properties);
     while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        HANDLE* handle = pblMapEntryValue(entry);
        WCHAR* str = pblMapEntryKey(entry);

        if (IS_INTRESOURCE(str)) {
            if (!GlobalGetAtomNameA_k( (ATOM)str, string, ATOM_BUFFER_SIZE )) continue;
        } else {
            WideCharToMultiByte_k(CP_ACP, 0, str, -1, string, ATOM_BUFFER_SIZE, NULL, NULL);
        }
        if (!(ret = func( hwnd, string, *handle, lParam ))) break;
     }
    pblIteratorFree(it);
    return ret;
}


/***********************************************************************
 *              EnumPropsExW   (USER32.@)
 */
INT WINAPI EnumPropsExW_k(HWND hwnd, PROPENUMPROCEXW func, LPARAM lParam)
{
    WND* win = WIN_GetPtr(hwnd);    
    PblIterator* it;
    WCHAR string[ATOM_BUFFER_SIZE];
    INT ret = -1;

    if (!win) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    } 

    it = pblMapIteratorNew(win->properties);
     while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        HANDLE* handle = pblMapEntryValue(entry);
        WCHAR* str = pblMapEntryKey(entry);

        if (IS_INTRESOURCE(str)) {
            if (!GlobalGetAtomNameW_k( (ATOM)str, string, ATOM_BUFFER_SIZE )) continue;
            str = string;
        }
        if (!(ret = func( hwnd, str, *handle, lParam ))) break;
     }
    pblIteratorFree(it);
    return ret;
}
