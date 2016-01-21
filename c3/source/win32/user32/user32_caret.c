/*
 * Caret functions
 *
 * Copyright 1993 David Metcalfe
 * Copyright 1996 Frans van Dorsselaer
 * Copyright 2001 Eric Pouech
 * Copyright 2002 Alexandre Julliard
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
#include "../include/winbase.h"
#include "kthread.h"
#include "kprocess.h"

#define TIMERID 0xffff  /* system timer id for the caret */

/*****************************************************************
 *               CARET_DisplayCaret
 */
static void CARET_DisplayCaret( HWND hwnd, const RECT *r )
{
    HDC hdc;
    HDC hCompDC;

    /* do not use DCX_CACHE here, for x,y,width,height are in logical units */
    if (!(hdc = GetDCEx_k( hwnd, 0, DCX_USESTYLE /*| DCX_CACHE*/ ))) return;
    hCompDC = CreateCompatibleDC_k(hdc);
    if (hCompDC)
    {
	HBITMAP	hPrevBmp;

	hPrevBmp = SelectObject_k(hCompDC, (HBITMAP)currentProcess()->Caret.hBmp);
	BitBlt_k(hdc, r->left, r->top, r->right-r->left, r->bottom-r->top, hCompDC, 0, 0, SRCINVERT);
	SelectObject_k(hCompDC, hPrevBmp);
	DeleteDC_k(hCompDC);
    }
    ReleaseDC_k( hwnd, hdc );
}


/*****************************************************************
 *               CARET_Callback
 */
static void CALLBACK CARET_Callback( HWND hwnd, UINT msg, UINT_PTR id, DWORD ctime)
{
    RECT r;
    int hidden = 0;
    struct thread_input *input = currentThread()->input;

    input->caret_state = !input->caret_state;
    if (!input->caret_hide) CARET_DisplayCaret( hwnd, &r );
}


/*****************************************************************
 *		CreateCaret (USER32.@)
 */
BOOL WINAPI CreateCaret_k( HWND hwnd, HBITMAP bitmap, INT width, INT height )
{
    RECT r;
    int old_state = 0;
    int hidden = 0;
    HBITMAP hBmp = 0;
    HWND prev = 0;
    struct thread_input *input = currentThread()->input;
    struct KProcess* process = currentProcess();

    TRACE("hwnd=%p\n", hwnd);

    if (!hwnd) return FALSE;

    if (bitmap && (bitmap != (HBITMAP)1))
    {
        BITMAP bmp;
        if (!GetObjectA_k( bitmap, sizeof(bmp), &bmp )) return FALSE;
        width = bmp.bmWidth;
        height = bmp.bmHeight;
	    bmp.bmBits = NULL;
	    hBmp = CreateBitmapIndirect_k(&bmp);
	    if (hBmp)
	    {
	        /* copy the bitmap */
	        LPBYTE buf = HeapAlloc_k(GetProcessHeap_k(), 0, bmp.bmWidthBytes * bmp.bmHeight);
	        GetBitmapBits_k(bitmap, bmp.bmWidthBytes * bmp.bmHeight, buf);
	        SetBitmapBits_k(hBmp, bmp.bmWidthBytes * bmp.bmHeight, buf);
	        HeapFree_k(GetProcessHeap_k(), 0, buf);
	    }
    }
    else
    {
	    HDC hdc;

        if (!width) width = GetSystemMetrics_k(SM_CXBORDER);
        if (!height) height = GetSystemMetrics_k(SM_CYBORDER);

	    /* create the uniform bitmap on the fly */
	    hdc = GetDC_k(hwnd);
	    if (hdc)
	    {
	        HDC hMemDC = CreateCompatibleDC_k(hdc);
	        if (hMemDC)
	        {
		        if ((hBmp = CreateCompatibleBitmap_k(hMemDC, width, height )))
		        {
		            HBITMAP hPrevBmp = SelectObject_k(hMemDC, hBmp);
                    SetRect_k( &r, 0, 0, width, height );
		            FillRect_k(hMemDC, &r, bitmap ? GetStockObject_k(GRAY_BRUSH) : GetStockObject_k(WHITE_BRUSH));
		            SelectObject_k(hMemDC, hPrevBmp);
		        }
		        DeleteDC_k(hMemDC);
	        }
	        ReleaseDC_k(hwnd, hdc);
	    }
    }
    if (!hBmp) return FALSE;

    prev = input->caret;
    hidden = input->caret_hide;
    old_state = input->caret_state;
    r = input->caret_rect;

    if (input->caret!=hwnd) {
        input->caret_rect.left   = 0;
        input->caret_rect.top    = 0;
        input->caret_rect.right  = 0;
        input->caret_rect.bottom = 0;
    }
    input->caret = hwnd;
    input->caret_hide = 1;
    input->caret_state = 0;
    input->caret_rect.right  = input->caret_rect.left + width;
    input->caret_rect.bottom = input->caret_rect.top + height;

    if (prev && !hidden)  /* hide the previous one */
    {
        /* FIXME: won't work if prev belongs to a different process */
        KillSystemTimer_k( prev, TIMERID );
        if (old_state) CARET_DisplayCaret( prev, &r );
    }

    if (process->Caret.hBmp) DeleteObject_k( (HBITMAP)process->Caret.hBmp );
    process->Caret.hBmp = (S32)hBmp;
    //process->Caret.timeout = GetProfileIntA_k( "windows", "CursorBlinkRate", 500 );
    return TRUE;
}


/*****************************************************************
 *		DestroyCaret (USER32.@)
 */
BOOL WINAPI DestroyCaret_k(void)
{
    HWND prev = 0;
    RECT r;
    int old_state = 0;
    int hidden = 0;
    struct thread_input *input = currentThread()->input;
    struct KProcess* process = currentProcess();

    prev = input->caret;
    hidden = input->caret_hide;
    old_state = input->caret_state;
    r = input->caret_rect;
    
    input->caret = 0;
    input->caret_hide = 1;
    input->caret_state = 0;
    input->caret_rect.left   = 0;
    input->caret_rect.top    = 0;
    input->caret_rect.right  = 0;
    input->caret_rect.bottom = 0;

    if (prev && !hidden)
    {
        /* FIXME: won't work if prev belongs to a different process */
        KillSystemTimer_k( prev, TIMERID );
        if (old_state) CARET_DisplayCaret( prev, &r );
    }
    if (process->Caret.hBmp) DeleteObject_k( (HBITMAP)process->Caret.hBmp );
    process->Caret.hBmp = 0;
    return TRUE;
}


/*****************************************************************
 *		SetCaretPos (USER32.@)
 */
BOOL WINAPI SetCaretPos_k( INT x, INT y )
{
    HWND hwnd = 0;
    RECT r;
    int old_state = 0;
    int hidden = 0;
    struct thread_input *input = currentThread()->input;

    r = input->caret_rect;
    old_state = input->caret_state;
    hidden = input->caret_hide;
    hwnd = input->caret;

    input->caret_rect.right  += x - input->caret_rect.left;
    input->caret_rect.bottom += y - input->caret_rect.top;
    input->caret_rect.left = x;
    input->caret_rect.top  = y;
    input->caret_state = 1;

    if (!hidden && (x != r.left || y != r.top))
    {
        if (old_state) CARET_DisplayCaret( hwnd, &r );
        r.right += x - r.left;
        r.bottom += y - r.top;
        r.left = x;
        r.top = y;
        CARET_DisplayCaret( hwnd, &r );
        SetSystemTimer_k( hwnd, TIMERID, currentProcess()->Caret.timeout, CARET_Callback );
    }
    return TRUE;
}


/*****************************************************************
 *		HideCaret (USER32.@)
 */
BOOL WINAPI HideCaret_k( HWND hwnd )
{
    RECT r;
    int old_state = 0;
    int hidden = 0;
    struct thread_input *input = currentThread()->input;

    if (hwnd && hwnd != input->caret) {
        SetLastError_k(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    r = input->caret_rect;
    old_state = input->caret_state;
    hidden = input->caret_hide;
    hwnd = input->caret;
    input->caret_hide++;
    input->caret_state = 0;

    if (!hidden)
    {
        if (old_state) CARET_DisplayCaret( hwnd, &r );
        KillSystemTimer_k( hwnd, TIMERID );
    }
    return TRUE;
}


/*****************************************************************
 *		ShowCaret (USER32.@)
 */
BOOL WINAPI ShowCaret_k( HWND hwnd )
{
    struct thread_input *input = currentThread()->input;

    if (hwnd && hwnd != input->caret) {
        SetLastError_k(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    hwnd = input->caret;
    input->caret_hide--;
    input->caret_state = 1;

    if (input->caret_hide==0)
    {
        CARET_DisplayCaret( hwnd, &input->caret_rect );
        SetSystemTimer_k( hwnd, TIMERID, currentProcess()->Caret.timeout, CARET_Callback );
    }
    return TRUE;
}


/*****************************************************************
 *		GetCaretPos (USER32.@)
 */
BOOL WINAPI GetCaretPos_k( LPPOINT pt )
{
    struct thread_input *input = currentThread()->input;
    pt->x = input->caret_rect.left;
    pt->y = input->caret_rect.top;
    return TRUE;
}


/*****************************************************************
 *		SetCaretBlinkTime (USER32.@)
 */
BOOL WINAPI SetCaretBlinkTime_k( UINT msecs )
{
    TRACE("msecs=%d\n", msecs);

    currentProcess()->Caret.timeout = msecs;
/*    if (Caret.hwnd) CARET_SetTimer(); FIXME */
    return TRUE;
}


/*****************************************************************
 *		GetCaretBlinkTime (USER32.@)
 */
UINT WINAPI GetCaretBlinkTime_k(void)
{
    return currentProcess()->Caret.timeout;
}
