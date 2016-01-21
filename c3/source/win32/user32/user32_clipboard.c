/*
 * WIN32 clipboard implementation
 *
 * Copyright 1994 Martin Ayotte
 *	     1996 Alex Korobka
 *	     1999 Noel Borthwick
 *	     2003 Ulrich Czekalla for CodeWeavers
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
 *
 * NOTES:
 *    This file contains the implementation for the WIN32 Clipboard API
 * and Wine's internal clipboard cache.
 * The actual contents of the clipboard are held in the clipboard cache.
 * The internal implementation talks to a "clipboard driver" to fill or
 * expose the cache to the native device. (Currently only the X11 and
 * TTY clipboard  driver are available)
 */

#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <string.h>

#include "../include/winbase.h"
#include "../include/unicode.h"
#include "user_private.h"

#define  CF_REGFORMATBASE  0xC000

typedef struct
{
    HWND hWndOpen;
    HWND hWndOwner;
    HWND hWndViewer;
    UINT seqno;
    UINT flags;
} CLIPBOARDINFO, *LPCLIPBOARDINFO;


/**************************************************************************
 *	CLIPBOARD_ReleaseOwner
 */
BOOL CLIPBOARD_ReleaseOwner(void)
{
    return FALSE;
}




/**************************************************************************
 *                WIN32 Clipboard implementation
 **************************************************************************/

/**************************************************************************
 *		RegisterClipboardFormatW (USER32.@)
 */
UINT WINAPI RegisterClipboardFormatW_k( LPCWSTR name )
{
    return GlobalAddAtomW_k( name );
}


/**************************************************************************
 *		RegisterClipboardFormatA (USER32.@)
 */
UINT WINAPI RegisterClipboardFormatA_k( LPCSTR name )
{
    return GlobalAddAtomA_k( name );
}


/**************************************************************************
 *		GetClipboardFormatNameW (USER32.@)
 */
INT WINAPI GetClipboardFormatNameW_k(UINT wFormat, LPWSTR retStr, INT maxlen)
{
    if (wFormat < MAXINTATOM) return 0;
    return GlobalGetAtomNameW_k( wFormat, retStr, maxlen );
}


/**************************************************************************
 *		GetClipboardFormatNameA (USER32.@)
 */
INT WINAPI GetClipboardFormatNameA_k(UINT wFormat, LPSTR retStr, INT maxlen)
{
    if (wFormat < MAXINTATOM) return 0;
    return GlobalGetAtomNameA_k( wFormat, retStr, maxlen );
}


/**************************************************************************
 *		OpenClipboard (USER32.@)
 *
 * Note: Netscape uses NULL hWnd to open the clipboard.
 */
BOOL WINAPI OpenClipboard_k( HWND hWnd )
{
    return FALSE;
}


/**************************************************************************
 *		CloseClipboard (USER32.@)
 */
BOOL WINAPI CloseClipboard_k(void)
{
    return TRUE;
}


/**************************************************************************
 *		EmptyClipboard (USER32.@)
 * Empties and acquires ownership of the clipboard
 */
BOOL WINAPI EmptyClipboard(void)
{    
    return TRUE;
}


/**************************************************************************
 *		GetClipboardOwner (USER32.@)
 *  FIXME: Can't return the owner if the clipboard is owned by an external X-app
 */
HWND WINAPI GetClipboardOwner_k(void)
{
    return 0;
}


/**************************************************************************
 *		GetOpenClipboardWindow (USER32.@)
 */
HWND WINAPI GetOpenClipboardWindow_k(void)
{
    return 0;
}


/**************************************************************************
 *		SetClipboardViewer (USER32.@)
 */
HWND WINAPI SetClipboardViewer_k( HWND hWnd )
{
    return 0;
}


/**************************************************************************
 *              GetClipboardViewer (USER32.@)
 */
HWND WINAPI GetClipboardViewer_k(void)
{
    return 0;
}


/**************************************************************************
 *              ChangeClipboardChain (USER32.@)
 */
BOOL WINAPI ChangeClipboardChain_k(HWND hWnd, HWND hWndNext)
{
    return TRUE;
}


/**************************************************************************
 *		SetClipboardData (USER32.@)
 */
HANDLE WINAPI SetClipboardData_k(UINT wFormat, HANDLE hData)
{
    return 0;
}


/**************************************************************************
 *		CountClipboardFormats (USER32.@)
 */
INT WINAPI CountClipboardFormats_k(void)
{
    return 0;
}


/**************************************************************************
 *		EnumClipboardFormats (USER32.@)
 */
UINT WINAPI EnumClipboardFormats_k(UINT wFormat)
{
    return 0;
}


/**************************************************************************
 *		IsClipboardFormatAvailable (USER32.@)
 */
BOOL WINAPI IsClipboardFormatAvailable_k(UINT wFormat)
{
    return FALSE;
}


/**************************************************************************
 *		GetClipboardData (USER32.@)
 */
HANDLE WINAPI GetClipboardData_k(UINT wFormat)
{
    return 0;
}


/**************************************************************************
 *		GetPriorityClipboardFormat (USER32.@)
 */
INT WINAPI GetPriorityClipboardFormat_k(UINT *list, INT nCount)
{
    return -1;
}


/**************************************************************************
 *		GetClipboardSequenceNumber (USER32.@)
 * Supported on Win2k/Win98
 * MSDN: Windows clipboard code keeps a serial number for the clipboard
 * for each window station.  The number is incremented whenever the
 * contents change or are emptied.
 * If you do not have WINSTA_ACCESSCLIPBOARD then the function returns 0
 */
DWORD WINAPI GetClipboardSequenceNumber_k(VOID)
{
    return 0;
}
