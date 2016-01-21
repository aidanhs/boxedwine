/*
 * Icontitle window class.
 *
 * Copyright 1997 Alex Korobka
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
#include "../include/unicode.h"
#include "controls.h"
#include "win.h"

static BOOL bMultiLineTitle;
static HFONT hIconTitleFont;

/*********************************************************************
 * icon title class descriptor
 */
const struct builtin_class_descr ICONTITLE_builtin_class =
{
    (LPCWSTR)ICONTITLE_CLASS_ATOM, /* name */
    0,                    /* style */
    WINPROC_ICONTITLE,    /* proc */
    0,                    /* extra */
    IDC_ARROW,            /* cursor */
    0                     /* brush */
};



/***********************************************************************
 *           ICONTITLE_Create
 */
HWND ICONTITLE_Create( HWND owner )
{
    HWND hWnd;
    HINSTANCE instance = (HINSTANCE)GetWindowLongPtrA_k( owner, GWLP_HINSTANCE );
    LONG style = WS_CLIPSIBLINGS;

    if (!IsWindowEnabled_k(owner)) style |= WS_DISABLED;
    if( GetWindowLongA_k( owner, GWL_STYLE ) & WS_CHILD )
	hWnd = CreateWindowExA_k( 0, (LPCSTR)ICONTITLE_CLASS_ATOM, NULL,
                                style | WS_CHILD, 0, 0, 1, 1,
                                GetParent_k(owner), 0, instance, NULL );
    else
	hWnd = CreateWindowExA_k( 0, (LPCSTR)ICONTITLE_CLASS_ATOM, NULL,
                                style, 0, 0, 1, 1,
                                owner, 0, instance, NULL );
    WIN_SetOwner( hWnd, owner );  /* MDI depends on this */
    SetWindowLongW_k( hWnd, GWL_STYLE,
                    GetWindowLongW_k( hWnd, GWL_STYLE ) & ~(WS_CAPTION | WS_BORDER) );
    return hWnd;
}

/***********************************************************************
 *           ICONTITLE_SetTitlePos
 */
static BOOL ICONTITLE_SetTitlePos( HWND hwnd, HWND owner )
{
    static const WCHAR emptyTitleText[] = {'<','.','.','.','>',0};
    WCHAR str[80];
    HDC hDC;
    HFONT hPrevFont;
    RECT rect;
    INT cx, cy;
    POINT pt;

    int length = GetWindowTextW_k( owner, str, sizeof(str)/sizeof(WCHAR) );

    while (length && str[length - 1] == ' ') /* remove trailing spaces */
        str[--length] = 0;

    if( !length )
    {
        strcpyW( str, emptyTitleText );
        length = strlenW( str );
    }

    if (!(hDC = GetDC_k( hwnd ))) return FALSE;

    hPrevFont = SelectObject_k( hDC, hIconTitleFont );

    SetRect_k( &rect, 0, 0, GetSystemMetrics_k(SM_CXICONSPACING) -
             GetSystemMetrics_k(SM_CXBORDER) * 2,
             GetSystemMetrics_k(SM_CYBORDER) * 2 );

    DrawTextW_k( hDC, str, length, &rect, DT_CALCRECT | DT_CENTER | DT_NOPREFIX | DT_WORDBREAK |
               (( bMultiLineTitle ) ? 0 : DT_SINGLELINE) );

    SelectObject_k( hDC, hPrevFont );
    ReleaseDC_k( hwnd, hDC );

    cx = rect.right - rect.left +  4 * GetSystemMetrics_k(SM_CXBORDER);
    cy = rect.bottom - rect.top;

    pt.x = (GetSystemMetrics_k(SM_CXICON) - cx) / 2;
    pt.y = GetSystemMetrics_k(SM_CYICON);

    /* point is relative to owner, make it relative to parent */
    MapWindowPoints_k( owner, GetParent_k(hwnd), &pt, 1 );

    SetWindowPos_k( hwnd, owner, pt.x, pt.y, cx, cy, SWP_NOACTIVATE );
    return TRUE;
}

/***********************************************************************
 *           ICONTITLE_Paint
 */
static BOOL ICONTITLE_Paint( HWND hwnd, HWND owner, HDC hDC, BOOL bActive )
{
    RECT rect;
    HFONT hPrevFont;
    HBRUSH hBrush = 0;
    COLORREF textColor = 0;

    if( bActive )
    {
	hBrush = GetSysColorBrush_k(COLOR_ACTIVECAPTION);
	textColor = GetSysColor_k(COLOR_CAPTIONTEXT);
    }
    else
    {
        if( GetWindowLongA_k( hwnd, GWL_STYLE ) & WS_CHILD )
	{
	    hBrush = (HBRUSH) GetClassLongPtrW_k(hwnd, GCLP_HBRBACKGROUND);
	    if( hBrush )
	    {
		INT level;
		LOGBRUSH logBrush;
		GetObjectA_k( hBrush, sizeof(logBrush), &logBrush );
		level = GetRValue(logBrush.lbColor) +
			   GetGValue(logBrush.lbColor) +
			      GetBValue(logBrush.lbColor);
		if( level < (0x7F * 3) )
		    textColor = RGB( 0xFF, 0xFF, 0xFF );
	    }
	    else
		hBrush = GetStockObject_k( WHITE_BRUSH );
	}
	else
	{
	    hBrush = GetStockObject_k( BLACK_BRUSH );
	    textColor = RGB( 0xFF, 0xFF, 0xFF );
	}
    }

    GetClientRect_k( hwnd, &rect );
    DPtoLP_k( hDC, (LPPOINT)&rect, 2 );
    FillRect_k( hDC, &rect, hBrush );

    hPrevFont = SelectObject_k( hDC, hIconTitleFont );
    if( hPrevFont )
    {
	WCHAR buffer[80];

        INT length = GetWindowTextW_k( owner, buffer, sizeof(buffer)/sizeof(buffer[0]) );
        SetTextColor_k( hDC, textColor );
        SetBkMode_k( hDC, TRANSPARENT );

        DrawTextW_k( hDC, buffer, length, &rect, DT_CENTER | DT_NOPREFIX |
                   DT_WORDBREAK | ((bMultiLineTitle) ? 0 : DT_SINGLELINE) );

	    SelectObject_k( hDC, hPrevFont );
    }
    return (hPrevFont != 0);
}

/***********************************************************************
 *           IconTitleWndProc
 */
LRESULT WINAPI IconTitleWndProc_k( HWND hWnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam )
{
    HWND owner = GetWindow_k( hWnd, GW_OWNER );

    if (!IsWindow_k(hWnd)) return 0;

    switch( msg )
    {
        case WM_CREATE:
            if (!hIconTitleFont)
            {
                LOGFONTA logFont;
                SystemParametersInfoA_k( SPI_GETICONTITLELOGFONT, 0, &logFont, 0 );
                SystemParametersInfoA_k( SPI_GETICONTITLEWRAP, 0, &bMultiLineTitle, 0 );
                hIconTitleFont = CreateFontIndirectA_k( &logFont );
            }
            return (hIconTitleFont ? 0 : -1);
	case WM_NCHITTEST:
	     return HTCAPTION;
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDBLCLK:
	     return SendMessageW_k( owner, msg, wParam, lParam );
	case WM_ACTIVATE:
	     if( wParam ) SetActiveWindow_k( owner );
             return 0;
	case WM_CLOSE:
	     return 0;
	case WM_SHOWWINDOW:
             if (wParam) ICONTITLE_SetTitlePos( hWnd, owner );
	     return 0;
	case WM_ERASEBKGND:
            if( GetWindowLongW_k( owner, GWL_STYLE ) & WS_CHILD )
                lParam = SendMessageW_k( owner, WM_ISACTIVEICON, 0, 0 );
            else
                lParam = (owner == GetActiveWindow_k());
            if( ICONTITLE_Paint( hWnd, owner, (HDC)wParam, (BOOL)lParam ) )
                ValidateRect_k( hWnd, NULL );
            return 1;
    }
    return DefWindowProcW_k( hWnd, msg, wParam, lParam );
}
