/*
 * Desktop window class.
 *
 * Copyright 1994 Alexandre Julliard
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
#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "../include/winbase.h"
#include "controls.h"

static HBRUSH hbrushPattern;
static HBITMAP hbitmapWallPaper;
static SIZE bitmapSize;
static BOOL fTileWallPaper;

POINT desktopCursor;

/*********************************************************************
 * desktop class descriptor
 */
const struct builtin_class_descr DESKTOP_builtin_class =
{
    (LPCWSTR)DESKTOP_CLASS_ATOM, /* name */
    CS_DBLCLKS,           /* style */
    WINPROC_DESKTOP,      /* proc */
    0,                    /* extra */
    IDC_ARROW,            /* cursor */
    (HBRUSH)(COLOR_BACKGROUND+1)    /* brush */
};


/***********************************************************************
 *           DESKTOP_LoadBitmap
 */
static HBITMAP DESKTOP_LoadBitmap( const WCHAR *filename )
{
    HBITMAP hbitmap;

    if (!filename[0]) return 0;
    hbitmap = LoadImageW_k( 0, filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );
    if (!hbitmap)
    {
        WCHAR buffer[MAX_PATH];
        UINT len = GetWindowsDirectoryW_k( buffer, MAX_PATH - 2 );
        if (buffer[len - 1] != '\\') buffer[len++] = '\\';
        lstrcpynW( buffer + len, filename, MAX_PATH - len );
        hbitmap = LoadImageW_k( 0, buffer, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );
    }
    return hbitmap;
}

/***********************************************************************
 *           init_wallpaper
 */
static void init_wallpaper( const WCHAR *wallpaper )
{
    HBITMAP hbitmap = DESKTOP_LoadBitmap( wallpaper );

    if (hbitmapWallPaper) DeleteObject_k( hbitmapWallPaper );
    hbitmapWallPaper = hbitmap;
    if (hbitmap)
    {
	BITMAP bmp;
	GetObjectA_k( hbitmap, sizeof(bmp), &bmp );
	bitmapSize.cx = (bmp.bmWidth != 0) ? bmp.bmWidth : 1;
	bitmapSize.cy = (bmp.bmHeight != 0) ? bmp.bmHeight : 1;
        //fTileWallPaper = GetProfileIntA_k( "desktop", "TileWallPaper", 0 );
    }
}

/***********************************************************************
 *           DesktopWndProc
 */
LRESULT WINAPI DesktopWndProc_k( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if (message == WM_NCCREATE) return TRUE;
    return 0;  /* all other messages are ignored */
}

/***********************************************************************
 *           PaintDesktop   (USER32.@)
 *
 */
BOOL WINAPI PaintDesktop_k(HDC hdc)
{
    HWND hwnd = GetDesktopWindow_k();

    /* check for an owning thread; otherwise don't paint anything (non-desktop mode) */
    if (GetWindowThreadProcessId_k( hwnd, NULL ))
    {
        RECT rect;

        GetClientRect_k( hwnd, &rect );

        /* Paint desktop pattern (only if wall paper does not cover everything) */

        if (!hbitmapWallPaper ||
            (!fTileWallPaper && ((bitmapSize.cx < rect.right) || (bitmapSize.cy < rect.bottom))))
        {
            HBRUSH brush = hbrushPattern;
            if (!brush) brush = (HBRUSH)GetClassLongPtrW_k( hwnd, GCLP_HBRBACKGROUND );
            /* Set colors in case pattern is a monochrome bitmap */
            SetBkColor_k( hdc, RGB(0,0,0) );
            SetTextColor_k( hdc, GetSysColor_k(COLOR_BACKGROUND) );
            FillRect_k( hdc, &rect, brush );
        }

        /* Paint wall paper */

        if (hbitmapWallPaper)
        {
            INT x, y;
            HDC hMemDC = CreateCompatibleDC_k( hdc );

            SelectObject_k( hMemDC, hbitmapWallPaper );

            if (fTileWallPaper)
            {
                for (y = 0; y < rect.bottom; y += bitmapSize.cy)
                    for (x = 0; x < rect.right; x += bitmapSize.cx)
                        BitBlt_k( hdc, x, y, bitmapSize.cx, bitmapSize.cy, hMemDC, 0, 0, SRCCOPY );
            }
            else
            {
                x = (rect.left + rect.right - bitmapSize.cx) / 2;
                y = (rect.top + rect.bottom - bitmapSize.cy) / 2;
                if (x < 0) x = 0;
                if (y < 0) y = 0;
                BitBlt_k( hdc, x, y, bitmapSize.cx, bitmapSize.cy, hMemDC, 0, 0, SRCCOPY );
            }
            DeleteDC_k( hMemDC );
        }
    }
    return TRUE;
}

/***********************************************************************
 *           SetDeskWallPaper   (USER32.@)
 *
 * FIXME: is there a unicode version?
 */
BOOL WINAPI SetDeskWallPaper_k( LPCSTR filename )
{
    return SystemParametersInfoA_k( SPI_SETDESKWALLPAPER, MAX_PATH, (void *)filename, SPIF_UPDATEINIFILE );
}


/***********************************************************************
 *           update_wallpaper
 */
BOOL update_wallpaper( const WCHAR *wallpaper, const WCHAR *pattern )
{
    int pat[8];

    if (hbrushPattern) DeleteObject_k( hbrushPattern );
    hbrushPattern = 0;
    memset( pat, 0, sizeof(pat) );
    if (pattern)
    {
        char buffer[64];
        WideCharToMultiByte_k( CP_ACP, 0, pattern, -1, buffer, sizeof(buffer), NULL, NULL );
        if (sscanf( buffer, " %d %d %d %d %d %d %d %d",
                    &pat[0], &pat[1], &pat[2], &pat[3],
                    &pat[4], &pat[5], &pat[6], &pat[7] ))
        {
            WORD ptrn[8];
            HBITMAP hbitmap;
            int i;

            for (i = 0; i < 8; i++) ptrn[i] = pat[i] & 0xffff;
            hbitmap = CreateBitmap_k( 8, 8, 1, 1, ptrn );
            hbrushPattern = CreatePatternBrush_k( hbitmap );
            DeleteObject_k( hbitmap );
        }
    }
    init_wallpaper( wallpaper );
    RedrawWindow_k( GetDesktopWindow_k(), 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_NOCHILDREN );
    return TRUE;
}