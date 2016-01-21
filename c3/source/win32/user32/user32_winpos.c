/*
 * Window position related functions.
 *
 * Copyright 1993, 1994, 1995 Alexandre Julliard
 *                       1995, 1996, 1999 Alex Korobka
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
#include "../include/ntstatus.h"
#include "controls.h"
#include "user_private.h"
#include "../include/gdi_driver.h"
#include "win.h"

#define SWP_AGG_NOGEOMETRYCHANGE \
    (SWP_NOSIZE | SWP_NOCLIENTSIZE | SWP_NOZORDER)
#define SWP_AGG_NOPOSCHANGE \
    (SWP_NOSIZE | SWP_NOMOVE | SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE | SWP_NOZORDER)
#define SWP_AGG_STATUSFLAGS \
    (SWP_AGG_NOPOSCHANGE | SWP_FRAMECHANGED | SWP_HIDEWINDOW | SWP_SHOWWINDOW)

#define HAS_DLGFRAME(style,exStyle) \
    (((exStyle) & WS_EX_DLGMODALFRAME) || \
     (((style) & WS_DLGFRAME) && !((style) & WS_BORDER)))

#define HAS_THICKFRAME(style) \
    (((style) & WS_THICKFRAME) && \
     !(((style) & (WS_DLGFRAME|WS_BORDER)) == WS_DLGFRAME))

#define EMPTYPOINT(pt) ((pt).x == -1 && (pt).y == -1)

#define ON_LEFT_BORDER(hit) \
 (((hit) == HTLEFT) || ((hit) == HTTOPLEFT) || ((hit) == HTBOTTOMLEFT))
#define ON_RIGHT_BORDER(hit) \
 (((hit) == HTRIGHT) || ((hit) == HTTOPRIGHT) || ((hit) == HTBOTTOMRIGHT))
#define ON_TOP_BORDER(hit) \
 (((hit) == HTTOP) || ((hit) == HTTOPLEFT) || ((hit) == HTTOPRIGHT))
#define ON_BOTTOM_BORDER(hit) \
 (((hit) == HTBOTTOM) || ((hit) == HTBOTTOMLEFT) || ((hit) == HTBOTTOMRIGHT))

#define PLACE_MIN		0x0001
#define PLACE_MAX		0x0002
#define PLACE_RECT		0x0004

typedef struct
{
    struct user_object obj;
    INT       actualCount;
    INT       suggestedCount;
    HWND      hwndParent;
    WINDOWPOS *winPos;
} DWP;

/***********************************************************************
 *		SwitchToThisWindow (USER32.@)
 */
void WINAPI SwitchToThisWindow_k( HWND hwnd, BOOL alt_tab )
{
    if (IsIconic_k( hwnd )) ShowWindow_k( hwnd, SW_RESTORE );
    else BringWindowToTop_k( hwnd );
}


/***********************************************************************
 *		GetWindowRect (USER32.@)
 */
BOOL WINAPI GetWindowRect_k( HWND hwnd, LPRECT rect )
{
    BOOL ret = WIN_GetRectangles( hwnd, COORDS_SCREEN, rect, NULL );
    //if (ret) TRACE( "hwnd %p %s\n", hwnd, wine_dbgstr_rect(rect) );
    return ret;
}


/***********************************************************************
 *		GetWindowRgn (USER32.@)
 */
int WINAPI GetWindowRgn_k( HWND hwnd, HRGN hrgn )
{
    int nRet = ERROR;
    HRGN win_rgn = 0;
    WND* wnd = WIN_GetPtr(hwnd);

    if (!wnd) {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return ERROR;
    }
    if (!wnd->win_region) {
        return ERROR;
    }
    return CombineRgn_k( hrgn, wnd->win_region, 0, RGN_COPY );
}

/***********************************************************************
 *		GetWindowRgnBox (USER32.@)
 */
int WINAPI GetWindowRgnBox_k( HWND hwnd, LPRECT prect )
{
    int ret = ERROR;
    HRGN hrgn;

    if (!prect)
        return ERROR;

    if ((hrgn = CreateRectRgn_k(0, 0, 0, 0)))
    {
        if ((ret = GetWindowRgn_k( hwnd, hrgn )) != ERROR )
            ret = GetRgnBox_k( hrgn, prect );
        DeleteObject_k(hrgn);
    }

    return ret;
}

/***********************************************************************
 *		SetWindowRgn (USER32.@)
 */
int WINAPI SetWindowRgn_k( HWND hwnd, HRGN hrgn, BOOL bRedraw )
{    
    WND* wnd = WIN_GetPtr(hwnd);
    UINT swp_flags = SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED|SWP_NOCLIENTSIZE|SWP_NOCLIENTMOVE;

    if (!wnd) {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return ERROR;
    }
    if (hrgn) {
        if (wnd->win_region) {
            DeleteObject_k(wnd->win_region);
        }
        wnd->win_region = hrgn;
    } else {
        if (!wnd->win_region) {
            DeleteObject_k(wnd->win_region);
            wnd->win_region = 0;
        }
    }
        
    if (!bRedraw) swp_flags |= SWP_NOREDRAW;
    SetWindowPos_k( hwnd, 0, 0, 0, 0, 0, swp_flags );
    USER_Driver->pSetWindowRgn( hwnd, hrgn, bRedraw );
    return TRUE;
}


/***********************************************************************
 *		GetClientRect (USER32.@)
 */
BOOL WINAPI GetClientRect_k( HWND hwnd, LPRECT rect )
{
    return WIN_GetRectangles( hwnd, COORDS_CLIENT, NULL, rect );
}

static INLINE int is_point_in_window( WND *win, int x, int y )
{
    if (!(win->dwStyle & WS_VISIBLE)) return 0; /* not visible */
    if ((win->dwStyle & (WS_POPUP|WS_CHILD|WS_DISABLED)) == (WS_CHILD|WS_DISABLED))
        return 0;  /* disabled child */
    if ((win->dwExStyle & (WS_EX_LAYERED|WS_EX_TRANSPARENT)) == (WS_EX_LAYERED|WS_EX_TRANSPARENT))
        return 0;  /* transparent */
    if (x < win->rectWindow.left || x >= win->rectWindow.right ||
        y < win->rectWindow.top || y >= win->rectWindow.bottom)
        return 0;  /* not in window */
    if (win->win_region && !PtInRegion_k(win->win_region, x, y))
        return 0;  /* not in window region */
    return 1;
}

/* find all children of 'parent' that contain the given point */
static void get_window_children_from_point(WND *parent, int x, int y, PblList* results )
{
    WND *ptr;
    PblIterator* it = pblListIterator(parent->children);

    while (it && pblIteratorHasNext(it)) {    
        PblMapEntry* entry = pblIteratorNext(it);
        ptr = pblMapEntryValue(entry);
        if (!is_point_in_window( ptr, x, y )) continue;  /* skip it */

        /* if point is in client area, and window is not minimized or disabled, check children */
        if (!(ptr->dwStyle & (WS_MINIMIZE|WS_DISABLED)) &&
            x >= ptr->rectClient.left && x < ptr->rectClient.right &&
            y >= ptr->rectClient.top && y < ptr->rectClient.bottom)
        {
            get_window_children_from_point( ptr, x - ptr->rectClient.left, y - ptr->rectClient.top, results );
        }

        /* now add window to the array */
        pblListAdd(results, ptr->handle);
    }
    pblIteratorFree(it);
}

/* return list of all windows containing point (in absolute coords) */
static void all_windows_from_point( WND *top, int x, int y, PblList* results)
{
    WND *ptr;

    /* make point relative to top window */
    for (ptr = WIN_GetPtr(top->parent); ptr && !is_desktop_window(ptr->handle); ptr = WIN_GetPtr(ptr->parent))
    {
        x -= ptr->rectClient.left;
        y -= ptr->rectClient.top;
    }

    if (!is_point_in_window( top, x, y )) return;

    /* if point is in client area, and window is not minimized or disabled, check children */
    if (!(top->dwStyle & (WS_MINIMIZE|WS_DISABLED)) &&
        x >= top->rectClient.left && x < top->rectClient.right &&
        y >= top->rectClient.top && y < top->rectClient.bottom)
    {
        if (!is_desktop_window(top->handle))
        {
            x -= top->rectClient.left;
            y -= top->rectClient.top;
        }
        get_window_children_from_point( top, x, y, results );
    }
    /* now add window to the array */
    pblListAdd(results, top->handle);
}

/***********************************************************************
 *           WINPOS_WindowFromPoint
 *
 * Find the window and hittest for a given point.
 */
HWND WINPOS_WindowFromPoint( HWND hwndScope, POINT pt, INT *hittest )
{
    int res;
    HWND ret;
    PblList* results = pblListNewLinkedList();
    PblIterator* it;

    if (!hwndScope) hwndScope = GetDesktopWindow_k();

    *hittest = HTNOWHERE;

    all_windows_from_point( WIN_GetPtr(hwndScope), pt.x, pt.y, results);

    /* now determine the hittest */
    
    it = pblListIterator(results);
    while (it && pblIteratorHasNext(it)) {   
        PblMapEntry* entry = pblIteratorNext(it);
        ret = pblMapEntryValue(entry);

        LONG style = GetWindowLongW_k(ret, GWL_STYLE );

        /* If window is minimized or disabled, return at once */
        if (style & WS_MINIMIZE)
        {
            *hittest = HTCAPTION;
            break;
        }
        if (style & WS_DISABLED)
        {
            *hittest = HTERROR;
            break;
        }
        /* Send WM_NCCHITTEST (if same thread) */
        if (!WIN_IsCurrentThread( ret ))
        {
            *hittest = HTCLIENT;
            break;
        }
        res = SendMessageW_k( ret, WM_NCHITTEST, 0, MAKELONG(pt.x,pt.y) );
        if (res != HTTRANSPARENT)
        {
            *hittest = res;  /* Found the window */
            break;
        }
        /* continue search with next window in z-order */
    }
    pblIteratorFree(it);
    pblListFree(results);
    TRACE( "scope %p (%d,%d) returning %p\n", hwndScope, pt.x, pt.y, ret );
    return ret;
}


/*******************************************************************
 *		WindowFromPoint (USER32.@)
 */
HWND WINAPI WindowFromPoint_k( POINT pt )
{
    INT hittest;
    return WINPOS_WindowFromPoint( 0, pt, &hittest );
}


/*******************************************************************
 *		ChildWindowFromPoint (USER32.@)
 */
HWND WINAPI ChildWindowFromPoint_k( HWND hwndParent, POINT pt )
{
    return ChildWindowFromPointEx_k( hwndParent, pt, CWP_ALL );
}

/*******************************************************************
 *		RealChildWindowFromPoint (USER32.@)
 */
HWND WINAPI RealChildWindowFromPoint_k( HWND hwndParent, POINT pt )
{
    return ChildWindowFromPointEx_k( hwndParent, pt, CWP_SKIPTRANSPARENT | CWP_SKIPINVISIBLE );
}

/*******************************************************************
 *		ChildWindowFromPointEx (USER32.@)
 */
HWND WINAPI ChildWindowFromPointEx_k( HWND hwndParent, POINT pt, UINT uFlags)
{
    /* pt is in the client coordinates */
    HWND *list;
    int i;
    RECT rect;
    HWND retvalue;

    GetClientRect_k( hwndParent, &rect );
    if (!PtInRect_k( &rect, pt )) return 0;
    if (!(list = WIN_ListChildren( hwndParent ))) return hwndParent;

    for (i = 0; list[i]; i++)
    {
        if (!WIN_GetRectangles( list[i], COORDS_PARENT, &rect, NULL )) continue;
        if (!PtInRect_k( &rect, pt )) continue;
        if (uFlags & (CWP_SKIPINVISIBLE|CWP_SKIPDISABLED))
        {
            LONG style = GetWindowLongW_k( list[i], GWL_STYLE );
            if ((uFlags & CWP_SKIPINVISIBLE) && !(style & WS_VISIBLE)) continue;
            if ((uFlags & CWP_SKIPDISABLED) && (style & WS_DISABLED)) continue;
        }
        if (uFlags & CWP_SKIPTRANSPARENT)
        {
            if (GetWindowLongW_k( list[i], GWL_EXSTYLE ) & WS_EX_TRANSPARENT) continue;
        }
        break;
    }
    retvalue = list[i];
    HeapFree_k( GetProcessHeap_k(), 0, list );
    if (!retvalue) retvalue = hwndParent;
    return retvalue;
}


/*******************************************************************
 *         WINPOS_GetWinOffset
 *
 * Calculate the offset between the origin of the two windows. Used
 * to implement MapWindowPoints.
 */
static BOOL WINPOS_GetWinOffset( HWND hwndFrom, HWND hwndTo, BOOL *mirrored, POINT *ret_offset )
{
    WND * wndPtr;
    POINT offset;
    BOOL mirror_from, mirror_to;
    HWND hwnd;

    offset.x = offset.y = 0;
    *mirrored = mirror_from = mirror_to = FALSE;

    /* Translate source window origin to screen coords */
    if (hwndFrom)
    {
        if (!(wndPtr = WIN_GetPtr( hwndFrom )))
        {
            SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
            return FALSE;
        }
        if (wndPtr != WND_DESKTOP)
        {
            if (wndPtr->dwExStyle & WS_EX_LAYOUTRTL)
            {
                mirror_from = TRUE;
                offset.x += wndPtr->rectClient.right - wndPtr->rectClient.left;
            }
            while (wndPtr->parent)
            {
                offset.x += wndPtr->rectClient.left;
                offset.y += wndPtr->rectClient.top;
                hwnd = wndPtr->parent;
                if (!(wndPtr = WIN_GetPtr( hwnd ))) break;
                if (wndPtr == WND_DESKTOP) break;
            }
        }
    }

    /* Translate origin to destination window coords */
    if (hwndTo)
    {
        if (!(wndPtr = WIN_GetPtr( hwndTo )))
        {
            SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
            return FALSE;
        }
        if (wndPtr != WND_DESKTOP)
        {
            if (wndPtr->dwExStyle & WS_EX_LAYOUTRTL)
            {
                mirror_to = TRUE;
                offset.x -= wndPtr->rectClient.right - wndPtr->rectClient.left;
            }
            while (wndPtr->parent)
            {
                offset.x -= wndPtr->rectClient.left;
                offset.y -= wndPtr->rectClient.top;
                hwnd = wndPtr->parent;
                if (!(wndPtr = WIN_GetPtr( hwnd ))) break;
                if (wndPtr == WND_DESKTOP) break;
            }
        }
    }

    *mirrored = mirror_from ^ mirror_to;
    if (mirror_from) offset.x = -offset.x;
    *ret_offset = offset;
    return TRUE;
}

/* map coordinates of a window region */
void map_window_region( HWND from, HWND to, HRGN hrgn )
{
    BOOL mirrored;
    POINT offset;
    UINT i, size;
    RGNDATA *data;
    HRGN new_rgn;
    RECT *rect;

    if (!WINPOS_GetWinOffset( from, to, &mirrored, &offset )) return;

    if (!mirrored)
    {
        OffsetRgn_k( hrgn, offset.x, offset.y );
        return;
    }
    if (!(size = GetRegionData_k( hrgn, 0, NULL ))) return;
    if (!(data = HeapAlloc_k( GetProcessHeap_k(), 0, size ))) return;
    GetRegionData_k( hrgn, size, data );
    rect = (RECT *)data->Buffer;
    for (i = 0; i < data->rdh.nCount; i++)
    {
        int tmp = -(rect[i].left + offset.x);
        rect[i].left    = -(rect[i].right + offset.x);
        rect[i].right   = tmp;
        rect[i].top    += offset.y;
        rect[i].bottom += offset.y;
    }
    if ((new_rgn = ExtCreateRegion_k( NULL, data->rdh.nCount, data )))
    {
        CombineRgn_k( hrgn, new_rgn, 0, RGN_COPY );
        DeleteObject_k( new_rgn );
    }
    HeapFree_k( GetProcessHeap_k(), 0, data );
}


/*******************************************************************
 *		MapWindowPoints (USER32.@)
 */
INT WINAPI MapWindowPoints_k( HWND hwndFrom, HWND hwndTo, LPPOINT lppt, UINT count )
{
    BOOL mirrored;
    POINT offset;
    UINT i;

    if (!WINPOS_GetWinOffset( hwndFrom, hwndTo, &mirrored, &offset )) return 0;

    for (i = 0; i < count; i++)
    {
        lppt[i].x += offset.x;
        lppt[i].y += offset.y;
        if (mirrored) lppt[i].x = -lppt[i].x;
    }
    if (mirrored && count == 2)  /* special case for rectangle */
    {
        int tmp = lppt[0].x;
        lppt[0].x = lppt[1].x;
        lppt[1].x = tmp;
    }
    return MAKELONG( LOWORD(offset.x), LOWORD(offset.y) );
}


/*******************************************************************
 *		ClientToScreen (USER32.@)
 */
BOOL WINAPI ClientToScreen_k( HWND hwnd, LPPOINT lppnt )
{
    POINT offset;
    BOOL mirrored;

    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    if (!WINPOS_GetWinOffset( hwnd, 0, &mirrored, &offset )) return FALSE;
    lppnt->x += offset.x;
    lppnt->y += offset.y;
    if (mirrored) lppnt->x = -lppnt->x;
    return TRUE;
}


/*******************************************************************
 *		ScreenToClient (USER32.@)
 */
BOOL WINAPI ScreenToClient_k( HWND hwnd, LPPOINT lppnt )
{
    POINT offset;
    BOOL mirrored;

    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    if (!WINPOS_GetWinOffset( 0, hwnd, &mirrored, &offset )) return FALSE;
    lppnt->x += offset.x;
    lppnt->y += offset.y;
    if (mirrored) lppnt->x = -lppnt->x;
    return TRUE;
}


/***********************************************************************
 *		IsIconic_k (USER32.@)
 */
BOOL WINAPI IsIconic_k(HWND hWnd)
{
    return (GetWindowLongW_k( hWnd, GWL_STYLE ) & WS_MINIMIZE) != 0;
}


/***********************************************************************
 *		IsZoomed (USER32.@)
 */
BOOL WINAPI IsZoomed_k(HWND hWnd)
{
    return (GetWindowLongW_k( hWnd, GWL_STYLE ) & WS_MAXIMIZE) != 0;
}


/*******************************************************************
 *		AllowSetForegroundWindow (USER32.@)
 */
BOOL WINAPI AllowSetForegroundWindow_k( DWORD procid )
{
    /* FIXME: If Win98/2000 style SetForegroundWindow behavior is
     * implemented, then fix this function. */
    return TRUE;
}


/*******************************************************************
 *		LockSetForegroundWindow (USER32.@)
 */
BOOL WINAPI LockSetForegroundWindow_k( UINT lockcode )
{
    /* FIXME: If Win98/2000 style SetForegroundWindow behavior is
     * implemented, then fix this function. */
    return TRUE;
}


/***********************************************************************
 *		BringWindowToTop (USER32.@)
 */
BOOL WINAPI BringWindowToTop_k( HWND hwnd )
{
    return SetWindowPos_k( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
}


/***********************************************************************
 *		MoveWindow (USER32.@)
 */
BOOL WINAPI MoveWindow_k( HWND hwnd, INT x, INT y, INT cx, INT cy,
                            BOOL repaint )
{
    int flags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (!repaint) flags |= SWP_NOREDRAW;
    TRACE("%p %d,%d %dx%d %d\n", hwnd, x, y, cx, cy, repaint );
    return SetWindowPos_k( hwnd, 0, x, y, cx, cy, flags );
}


/***********************************************************************
 *           WINPOS_RedrawIconTitle
 */
BOOL WINPOS_RedrawIconTitle( HWND hWnd )
{
    HWND icon_title = 0;
    WND *win = WIN_GetPtr( hWnd );

    if (win && win != WND_DESKTOP)
    {
	    icon_title = win->icon_title;
    }
    if (!icon_title) return FALSE;
    SendMessageW_k( icon_title, WM_SHOWWINDOW, TRUE, 0 );
    InvalidateRect_k( icon_title, NULL, TRUE );
    return TRUE;
}

/***********************************************************************
 *           WINPOS_ShowIconTitle
 */
static void WINPOS_ShowIconTitle( HWND hwnd, BOOL bShow )
{
    WND *win = WIN_GetPtr( hwnd );
    HWND title = 0;

    TRACE("%p %i\n", hwnd, (bShow != 0) );

    if (!win || win == WND_DESKTOP) return;
    if (win->rectWindow.left == -32000 || win->rectWindow.top == -32000)
    {
        TRACE( "not showing title for hidden icon %p\n", hwnd );
        bShow = FALSE;
    }
    else title = win->icon_title;

    if (bShow)
    {
        if (!title)
        {
            title = ICONTITLE_Create( hwnd );
            if (!(win = WIN_GetPtr( hwnd )))
            {
                DestroyWindow_k( title );
                return;
            }
            win->icon_title = title;
        }
        if (!IsWindowVisible_k(title))
        {
            SendMessageW_k( title, WM_SHOWWINDOW, TRUE, 0 );
            SetWindowPos_k( title, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE |
                          SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW );
        }
    }
    else if (title) ShowWindow_k( title, SW_HIDE );
}

/*******************************************************************
 *           WINPOS_GetMinMaxInfo
 *
 * Get the minimized and maximized information for a window.
 */
void WINPOS_GetMinMaxInfo( HWND hwnd, POINT *maxSize, POINT *maxPos,
			   POINT *minTrack, POINT *maxTrack )
{
    MINMAXINFO MinMax;
    HMONITOR monitor;
    INT xinc, yinc;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    LONG adjustedStyle;
    LONG exstyle = GetWindowLongW_k( hwnd, GWL_EXSTYLE );
    RECT rc;
    WND *win;

    /* Compute default values */

    GetWindowRect_k(hwnd, &rc);
    MinMax.ptReserved.x = rc.left;
    MinMax.ptReserved.y = rc.top;

    if ((style & WS_CAPTION) == WS_CAPTION)
        adjustedStyle = style & ~WS_BORDER; /* WS_CAPTION = WS_DLGFRAME | WS_BORDER */
    else
        adjustedStyle = style;

    GetClientRect_k(GetAncestor_k(hwnd,GA_PARENT), &rc);
    AdjustWindowRectEx_k(&rc, adjustedStyle, ((style & WS_POPUP) && GetMenu_k(hwnd)), exstyle);

    xinc = -rc.left;
    yinc = -rc.top;

    MinMax.ptMaxSize.x = rc.right - rc.left;
    MinMax.ptMaxSize.y = rc.bottom - rc.top;
    if (style & (WS_DLGFRAME | WS_BORDER))
    {
        MinMax.ptMinTrackSize.x = GetSystemMetrics_k(SM_CXMINTRACK);
        MinMax.ptMinTrackSize.y = GetSystemMetrics_k(SM_CYMINTRACK);
    }
    else
    {
        MinMax.ptMinTrackSize.x = 2 * xinc;
        MinMax.ptMinTrackSize.y = 2 * yinc;
    }
    MinMax.ptMaxTrackSize.x = GetSystemMetrics_k(SM_CXMAXTRACK);
    MinMax.ptMaxTrackSize.y = GetSystemMetrics_k(SM_CYMAXTRACK);
    MinMax.ptMaxPosition.x = -xinc;
    MinMax.ptMaxPosition.y = -yinc;

    if ((win = WIN_GetPtr( hwnd )) && win != WND_DESKTOP)
    {
        if (!EMPTYPOINT(win->max_pos)) MinMax.ptMaxPosition = win->max_pos;
    }

    SendMessageW_k( hwnd, WM_GETMINMAXINFO, 0, (LPARAM)&MinMax );

    /* if the app didn't change the values, adapt them for the current monitor */

    if ((monitor = MonitorFromWindow_k( hwnd, MONITOR_DEFAULTTOPRIMARY )))
    {
        RECT rc_work;
        MONITORINFO mon_info;

        mon_info.cbSize = sizeof(mon_info);
        GetMonitorInfoW_k( monitor, &mon_info );

        rc_work = mon_info.rcMonitor;

        if (style & WS_MAXIMIZEBOX)
        {
            if ((style & WS_CAPTION) == WS_CAPTION || !(style & (WS_CHILD | WS_POPUP)))
                rc_work = mon_info.rcWork;
        }

        if (MinMax.ptMaxSize.x == GetSystemMetrics_k(SM_CXSCREEN) + 2 * xinc &&
            MinMax.ptMaxSize.y == GetSystemMetrics_k(SM_CYSCREEN) + 2 * yinc)
        {
            MinMax.ptMaxSize.x = (rc_work.right - rc_work.left) + 2 * xinc;
            MinMax.ptMaxSize.y = (rc_work.bottom - rc_work.top) + 2 * yinc;
        }
        if (MinMax.ptMaxPosition.x == -xinc && MinMax.ptMaxPosition.y == -yinc)
        {
            MinMax.ptMaxPosition.x = rc_work.left - xinc;
            MinMax.ptMaxPosition.y = rc_work.top - yinc;
        }
    }

      /* Some sanity checks */

    TRACE("%d %d / %d %d / %d %d / %d %d\n",
                      MinMax.ptMaxSize.x, MinMax.ptMaxSize.y,
                      MinMax.ptMaxPosition.x, MinMax.ptMaxPosition.y,
                      MinMax.ptMaxTrackSize.x, MinMax.ptMaxTrackSize.y,
                      MinMax.ptMinTrackSize.x, MinMax.ptMinTrackSize.y);
    MinMax.ptMaxTrackSize.x = max( MinMax.ptMaxTrackSize.x,
                                   MinMax.ptMinTrackSize.x );
    MinMax.ptMaxTrackSize.y = max( MinMax.ptMaxTrackSize.y,
                                   MinMax.ptMinTrackSize.y );

    if (maxSize) *maxSize = MinMax.ptMaxSize;
    if (maxPos) *maxPos = MinMax.ptMaxPosition;
    if (minTrack) *minTrack = MinMax.ptMinTrackSize;
    if (maxTrack) *maxTrack = MinMax.ptMaxTrackSize;
}


/***********************************************************************
 *           WINPOS_FindIconPos
 *
 * Find a suitable place for an iconic window.
 */
static POINT WINPOS_FindIconPos( HWND hwnd, POINT pt )
{
    RECT rect, rectParent;
    HWND parent, child;
    HRGN hrgn, tmp;
    int xspacing, yspacing;

    parent = GetAncestor_k( hwnd, GA_PARENT );
    GetClientRect_k( parent, &rectParent );
    if ((pt.x >= rectParent.left) && (pt.x + GetSystemMetrics_k(SM_CXICON) < rectParent.right) &&
        (pt.y >= rectParent.top) && (pt.y + GetSystemMetrics_k(SM_CYICON) < rectParent.bottom))
        return pt;  /* The icon already has a suitable position */

    xspacing = GetSystemMetrics_k(SM_CXICONSPACING);
    yspacing = GetSystemMetrics_k(SM_CYICONSPACING);

    /* Check if another icon already occupies this spot */
    /* FIXME: this is completely inefficient */

    hrgn = CreateRectRgn_k( 0, 0, 0, 0 );
    tmp = CreateRectRgn_k( 0, 0, 0, 0 );
    for (child = GetWindow_k( parent, GW_HWNDFIRST ); child; child = GetWindow_k( child, GW_HWNDNEXT ))
    {
        if (child == hwnd) continue;
        if ((GetWindowLongW_k( child, GWL_STYLE ) & (WS_VISIBLE|WS_MINIMIZE)) != (WS_VISIBLE|WS_MINIMIZE))
            continue;
        if (WIN_GetRectangles( child, COORDS_PARENT, &rect, NULL ))
        {
            SetRectRgn_k( tmp, rect.left, rect.top, rect.right, rect.bottom );
            CombineRgn_k( hrgn, hrgn, tmp, RGN_OR );
        }
    }
    DeleteObject_k( tmp );

    for (rect.bottom = rectParent.bottom; rect.bottom >= yspacing; rect.bottom -= yspacing)
    {
        for (rect.left = rectParent.left; rect.left <= rectParent.right - xspacing; rect.left += xspacing)
        {
            rect.right = rect.left + xspacing;
            rect.top = rect.bottom - yspacing;
            if (!RectInRegion_k( hrgn, &rect ))
            {
                /* No window was found, so it's OK for us */
                pt.x = rect.left + (xspacing - GetSystemMetrics_k(SM_CXICON)) / 2;
                pt.y = rect.top + (yspacing - GetSystemMetrics_k(SM_CYICON)) / 2;
                DeleteObject_k( hrgn );
                return pt;
            }
        }
    }
    DeleteObject_k( hrgn );
    pt.x = pt.y = 0;
    return pt;
}


/***********************************************************************
 *           WINPOS_MinMaximize
 */
UINT WINPOS_MinMaximize( HWND hwnd, UINT cmd, LPRECT rect )
{
    WND *wndPtr;
    UINT swpFlags = 0;
    POINT size;
    LONG old_style;
    WINDOWPLACEMENT wpl;

    TRACE("%p %u\n", hwnd, cmd );

    wpl.length = sizeof(wpl);
    GetWindowPlacement_k( hwnd, &wpl );

    if (HOOK_CallHooks( WH_CBT, HCBT_MINMAX, (WPARAM)hwnd, cmd, TRUE ))
        return SWP_NOSIZE | SWP_NOMOVE;

    if (IsIconic_k( hwnd ))
    {
        switch (cmd)
        {
        case SW_SHOWMINNOACTIVE:
        case SW_SHOWMINIMIZED:
        case SW_FORCEMINIMIZE:
        case SW_MINIMIZE:
            return SWP_NOSIZE | SWP_NOMOVE;
        }
        if (!SendMessageW_k( hwnd, WM_QUERYOPEN, 0, 0 )) return SWP_NOSIZE | SWP_NOMOVE;
        swpFlags |= SWP_NOCOPYBITS;
    }

    switch( cmd )
    {
    case SW_SHOWMINNOACTIVE:
    case SW_SHOWMINIMIZED:
    case SW_FORCEMINIMIZE:
    case SW_MINIMIZE:
        if (!(wndPtr = WIN_GetPtr( hwnd ))) return 0;
        if( wndPtr->dwStyle & WS_MAXIMIZE) wndPtr->flags |= WIN_RESTORE_MAX;
        else wndPtr->flags &= ~WIN_RESTORE_MAX;

        old_style = WIN_SetStyle( hwnd, WS_MINIMIZE, WS_MAXIMIZE );

        wpl.ptMinPosition = WINPOS_FindIconPos( hwnd, wpl.ptMinPosition );

        if (!(old_style & WS_MINIMIZE)) swpFlags |= SWP_STATECHANGED;
        SetRect_k( rect, wpl.ptMinPosition.x, wpl.ptMinPosition.y,
                 wpl.ptMinPosition.x + GetSystemMetrics_k(SM_CXICON),
                 wpl.ptMinPosition.y + GetSystemMetrics_k(SM_CYICON) );
        swpFlags |= SWP_NOCOPYBITS;
        break;

    case SW_MAXIMIZE:
        old_style = GetWindowLongW_k( hwnd, GWL_STYLE );
        if ((old_style & WS_MAXIMIZE) && (old_style & WS_VISIBLE)) return SWP_NOSIZE | SWP_NOMOVE;

        WINPOS_GetMinMaxInfo( hwnd, &size, &wpl.ptMaxPosition, NULL, NULL );

        old_style = WIN_SetStyle( hwnd, WS_MAXIMIZE, WS_MINIMIZE );
        if (old_style & WS_MINIMIZE)
        {
            if ((wndPtr = WIN_GetPtr( hwnd )))
            {
                wndPtr->flags |= WIN_RESTORE_MAX;
            }
            WINPOS_ShowIconTitle( hwnd, FALSE );
        }

        if (!(old_style & WS_MAXIMIZE)) swpFlags |= SWP_STATECHANGED;
        SetRect_k( rect, wpl.ptMaxPosition.x, wpl.ptMaxPosition.y,
                 wpl.ptMaxPosition.x +  size.x, wpl.ptMaxPosition.y + size.y );
        break;

    case SW_SHOWNOACTIVATE:
        if ((wndPtr = WIN_GetPtr( hwnd )))
        {
            wndPtr->flags &= ~WIN_RESTORE_MAX;
        }
        /* fall through */
    case SW_SHOWNORMAL:
    case SW_RESTORE:
    case SW_SHOWDEFAULT: /* FIXME: should have its own handler */
        old_style = WIN_SetStyle( hwnd, 0, WS_MINIMIZE | WS_MAXIMIZE );
        if (old_style & WS_MINIMIZE)
        {
            BOOL restore_max;

            WINPOS_ShowIconTitle( hwnd, FALSE );

            if (!(wndPtr = WIN_GetPtr( hwnd ))) return 0;
            restore_max = (wndPtr->flags & WIN_RESTORE_MAX) != 0;
            if (restore_max)
            {
                /* Restore to maximized position */
                WINPOS_GetMinMaxInfo( hwnd, &size, &wpl.ptMaxPosition, NULL, NULL);
                WIN_SetStyle( hwnd, WS_MAXIMIZE, 0 );
                swpFlags |= SWP_STATECHANGED;
                SetRect_k( rect, wpl.ptMaxPosition.x, wpl.ptMaxPosition.y,
                         wpl.ptMaxPosition.x + size.x, wpl.ptMaxPosition.y + size.y );
                break;
            }
        }
        else if (!(old_style & WS_MAXIMIZE)) break;

        swpFlags |= SWP_STATECHANGED;

        /* Restore to normal position */

        *rect = wpl.rcNormalPosition;
        break;
    }

    return swpFlags;
}


/***********************************************************************
 *              show_window
 *
 * Implementation of ShowWindow and ShowWindowAsync.
 */
static BOOL show_window( HWND hwnd, INT cmd )
{
    WND *wndPtr;
    HWND parent;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    BOOL wasVisible = (style & WS_VISIBLE) != 0;
    BOOL showFlag = TRUE;
    RECT newPos = {0, 0, 0, 0};
    UINT swp = 0;

    TRACE("hwnd=%p, cmd=%d, wasVisible %d\n", hwnd, cmd, wasVisible);

    switch(cmd)
    {
        case SW_HIDE:
            if (!wasVisible) return FALSE;
            showFlag = FALSE;
            swp |= SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE;
            if (style & WS_CHILD) swp |= SWP_NOACTIVATE | SWP_NOZORDER;
	    break;

	case SW_SHOWMINNOACTIVE:
        case SW_MINIMIZE:
        case SW_FORCEMINIMIZE: /* FIXME: Does not work if thread is hung. */
            swp |= SWP_NOACTIVATE | SWP_NOZORDER;
            /* fall through */
	case SW_SHOWMINIMIZED:
            swp |= SWP_SHOWWINDOW | SWP_FRAMECHANGED;
            swp |= WINPOS_MinMaximize( hwnd, cmd, &newPos );
            if ((style & WS_MINIMIZE) && wasVisible) return TRUE;
	    break;

	case SW_SHOWMAXIMIZED: /* same as SW_MAXIMIZE */
            if (!wasVisible) swp |= SWP_SHOWWINDOW;
            swp |= SWP_FRAMECHANGED;
            swp |= WINPOS_MinMaximize( hwnd, SW_MAXIMIZE, &newPos );
            if ((style & WS_MAXIMIZE) && wasVisible) return TRUE;
            break;

	case SW_SHOWNA:
            swp |= SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE;
            if (style & WS_CHILD) swp |= SWP_NOZORDER;
            break;
	case SW_SHOW:
            if (wasVisible) return TRUE;
	    swp |= SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE;
            if (style & WS_CHILD) swp |= SWP_NOACTIVATE | SWP_NOZORDER;
	    break;

	case SW_SHOWNOACTIVATE:
            swp |= SWP_NOACTIVATE | SWP_NOZORDER;
            /* fall through */
	case SW_RESTORE:
            /* fall through */
	case SW_SHOWNORMAL:  /* same as SW_NORMAL: */
	case SW_SHOWDEFAULT: /* FIXME: should have its own handler */
            if (!wasVisible) swp |= SWP_SHOWWINDOW;
            if (style & (WS_MINIMIZE | WS_MAXIMIZE))
            {
                swp |= SWP_FRAMECHANGED;
                swp |= WINPOS_MinMaximize( hwnd, cmd, &newPos );
            }
            else
            {
                if (wasVisible) return TRUE;
                swp |= SWP_NOSIZE | SWP_NOMOVE;
            }
            if (style & WS_CHILD && !(swp & SWP_STATECHANGED)) swp |= SWP_NOACTIVATE | SWP_NOZORDER;
	    break;
        default:
            return wasVisible;
    }

    if ((showFlag != wasVisible || cmd == SW_SHOWNA) && cmd != SW_SHOWMAXIMIZED && !(swp & SWP_STATECHANGED))
    {
        SendMessageW_k( hwnd, WM_SHOWWINDOW, showFlag, 0 );
        if (!IsWindow_k( hwnd )) return wasVisible;
    }

    swp = USER_Driver->pShowWindow( hwnd, cmd, &newPos, swp );

    parent = GetAncestor_k( hwnd, GA_PARENT );
    if (parent && !IsWindowVisible_k( parent ) && !(swp & SWP_STATECHANGED))
    {
        /* if parent is not visible simply toggle WS_VISIBLE and return */
        if (showFlag) WIN_SetStyle( hwnd, WS_VISIBLE, 0 );
        else WIN_SetStyle( hwnd, 0, WS_VISIBLE );
    }
    else
        SetWindowPos_k( hwnd, HWND_TOP, newPos.left, newPos.top,
                      newPos.right - newPos.left, newPos.bottom - newPos.top, swp );

    if (cmd == SW_HIDE)
    {
        HWND hFocus;

        WINPOS_ShowIconTitle( hwnd, FALSE );

        /* FIXME: This will cause the window to be activated irrespective
         * of whether it is owned by the same thread. Has to be done
         * asynchronously.
         */

        if (hwnd == GetActiveWindow_k())
            WINPOS_ActivateOtherWindow(hwnd);

        /* Revert focus to parent */
        hFocus = GetFocus_k();
        if (hwnd == hFocus)
        {
            HWND parent = GetAncestor_k(hwnd, GA_PARENT);
            if (parent == GetDesktopWindow_k()) parent = 0;
            SetFocus_k(parent);
        }
        return wasVisible;
    }

    if (IsIconic_k(hwnd)) WINPOS_ShowIconTitle( hwnd, TRUE );

    if (!(wndPtr = WIN_GetPtr( hwnd ))) return wasVisible;

    if (wndPtr->flags & WIN_NEED_SIZE)
    {
        /* should happen only in CreateWindowEx() */
	int wParam = SIZE_RESTORED;
        RECT client;
        LPARAM lparam;

        WIN_GetRectangles( hwnd, COORDS_PARENT, NULL, &client );
        lparam = MAKELONG( client.right - client.left, client.bottom - client.top );
	wndPtr->flags &= ~WIN_NEED_SIZE;
	if (wndPtr->dwStyle & WS_MAXIMIZE) wParam = SIZE_MAXIMIZED;
        else if (wndPtr->dwStyle & WS_MINIMIZE)
        {
            wParam = SIZE_MINIMIZED;
            lparam = 0;
        }

        SendMessageW_k( hwnd, WM_SIZE, wParam, lparam );
        SendMessageW_k( hwnd, WM_MOVE, 0, MAKELONG( client.left, client.top ));
    }

    /* if previous state was minimized Windows sets focus to the window */
    if (style & WS_MINIMIZE) SetFocus_k( hwnd );

    return wasVisible;
}


/***********************************************************************
 *		ShowWindowAsync (USER32.@)
 *
 * doesn't wait; returns immediately.
 * used by threads to toggle windows in other (possibly hanging) threads
 */
BOOL WINAPI ShowWindowAsync_k( HWND hwnd, INT cmd )
{
    HWND full_handle;

    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if ((full_handle = WIN_IsCurrentThread( hwnd )))
        return show_window( full_handle, cmd );

    return SendNotifyMessageW_k( hwnd, WM_WINE_SHOWWINDOW, cmd, 0 );
}


/***********************************************************************
 *		ShowWindow (USER32.@)
 */
BOOL WINAPI ShowWindow_k( HWND hwnd, INT cmd )
{
    HWND full_handle;

    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    if ((full_handle = WIN_IsCurrentThread( hwnd )))
        return show_window( full_handle, cmd );

    return SendMessageW_k( hwnd, WM_WINE_SHOWWINDOW, cmd, 0 );
}


/***********************************************************************
 *		GetInternalWindowPos (USER32.@)
 */
UINT WINAPI GetInternalWindowPos_k( HWND hwnd, LPRECT rectWnd,
                                      LPPOINT ptIcon )
{
    WINDOWPLACEMENT wndpl;
    if (GetWindowPlacement_k( hwnd, &wndpl ))
    {
	if (rectWnd) *rectWnd = wndpl.rcNormalPosition;
	if (ptIcon)  *ptIcon = wndpl.ptMinPosition;
	return wndpl.showCmd;
    }
    return 0;
}


/***********************************************************************
 *		GetWindowPlacement (USER32.@)
 *
 * Win95:
 * Fails if wndpl->length of Win95 (!) apps is invalid.
 */
BOOL WINAPI GetWindowPlacement_k( HWND hwnd, WINDOWPLACEMENT *wndpl )
{
    WND *pWnd = WIN_GetPtr( hwnd );

    if (!pWnd) return FALSE;

    if (pWnd == WND_DESKTOP)
    {
        wndpl->length  = sizeof(*wndpl);
        wndpl->showCmd = SW_SHOWNORMAL;
        wndpl->flags = 0;
        wndpl->ptMinPosition.x = -1;
        wndpl->ptMinPosition.y = -1;
        wndpl->ptMaxPosition.x = -1;
        wndpl->ptMaxPosition.y = -1;
        GetWindowRect_k( hwnd, &wndpl->rcNormalPosition );
        return TRUE;
    }

    /* update the placement according to the current style */
    if (pWnd->dwStyle & WS_MINIMIZE)
    {
        pWnd->min_pos.x = pWnd->rectWindow.left;
        pWnd->min_pos.y = pWnd->rectWindow.top;
    }
    else if (pWnd->dwStyle & WS_MAXIMIZE)
    {
        pWnd->max_pos.x = pWnd->rectWindow.left;
        pWnd->max_pos.y = pWnd->rectWindow.top;
    }
    else
    {
        pWnd->normal_rect = pWnd->rectWindow;
    }

    wndpl->length  = sizeof(*wndpl);
    if( pWnd->dwStyle & WS_MINIMIZE )
        wndpl->showCmd = SW_SHOWMINIMIZED;
    else
        wndpl->showCmd = ( pWnd->dwStyle & WS_MAXIMIZE ) ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL ;
    if( pWnd->flags & WIN_RESTORE_MAX )
        wndpl->flags = WPF_RESTORETOMAXIMIZED;
    else
        wndpl->flags = 0;
    wndpl->ptMinPosition    = pWnd->min_pos;
    wndpl->ptMaxPosition    = pWnd->max_pos;
    wndpl->rcNormalPosition = pWnd->normal_rect;

    //TRACE( "%p: returning min %d,%d max %d,%d normal %s\n", hwnd, wndpl->ptMinPosition.x, wndpl->ptMinPosition.y, wndpl->ptMaxPosition.x, wndpl->ptMaxPosition.y, wine_dbgstr_rect(&wndpl->rcNormalPosition) );
    return TRUE;
}

/* make sure the specified rect is visible on screen */
static void make_rect_onscreen( RECT *rect )
{
    MONITORINFO info;
    HMONITOR monitor = MonitorFromRect_k( rect, MONITOR_DEFAULTTONEAREST );

    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW_k( monitor, &info )) return;
    /* FIXME: map coordinates from rcWork to rcMonitor */
    if (rect->right <= info.rcWork.left)
    {
        rect->right += info.rcWork.left - rect->left;
        rect->left = info.rcWork.left;
    }
    else if (rect->left >= info.rcWork.right)
    {
        rect->left += info.rcWork.right - rect->right;
        rect->right = info.rcWork.right;
    }
    if (rect->bottom <= info.rcWork.top)
    {
        rect->bottom += info.rcWork.top - rect->top;
        rect->top = info.rcWork.top;
    }
    else if (rect->top >= info.rcWork.bottom)
    {
        rect->top += info.rcWork.bottom - rect->bottom;
        rect->bottom = info.rcWork.bottom;
    }
}

/* make sure the specified point is visible on screen */
static void make_point_onscreen( POINT *pt )
{
    RECT rect;

    SetRect_k( &rect, pt->x, pt->y, pt->x + 1, pt->y + 1 );
    make_rect_onscreen( &rect );
    pt->x = rect.left;
    pt->y = rect.top;
}


/***********************************************************************
 *           WINPOS_SetPlacement
 */
static BOOL WINPOS_SetPlacement( HWND hwnd, const WINDOWPLACEMENT *wndpl, UINT flags )
{
    DWORD style;
    WND *pWnd = WIN_GetPtr( hwnd );
    WINDOWPLACEMENT wp = *wndpl;

    if (flags & PLACE_MIN) make_point_onscreen( &wp.ptMinPosition );
    if (flags & PLACE_MAX) make_point_onscreen( &wp.ptMaxPosition );
    if (flags & PLACE_RECT) make_rect_onscreen( &wp.rcNormalPosition );

    //TRACE( "%p: setting min %d,%d max %d,%d normal %s flags %x ajusted to min %d,%d max %d,%d normal %s\n", hwnd, wndpl->ptMinPosition.x, wndpl->ptMinPosition.y, wndpl->ptMaxPosition.x, wndpl->ptMaxPosition.y, wine_dbgstr_rect(&wndpl->rcNormalPosition), flags, wp.ptMinPosition.x, wp.ptMinPosition.y, wp.ptMaxPosition.x, wp.ptMaxPosition.y, wine_dbgstr_rect(&wp.rcNormalPosition) );

    if (!pWnd || pWnd == WND_DESKTOP) return FALSE;

    if( flags & PLACE_MIN ) pWnd->min_pos = wp.ptMinPosition;
    if( flags & PLACE_MAX ) pWnd->max_pos = wp.ptMaxPosition;
    if( flags & PLACE_RECT) pWnd->normal_rect = wp.rcNormalPosition;

    style = pWnd->dwStyle;

    if( style & WS_MINIMIZE )
    {
        if (flags & PLACE_MIN)
        {
            WINPOS_ShowIconTitle( hwnd, FALSE );
            SetWindowPos_k( hwnd, 0, wp.ptMinPosition.x, wp.ptMinPosition.y, 0, 0,
                          SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
        }
    }
    else if( style & WS_MAXIMIZE )
    {
        if (flags & PLACE_MAX)
            SetWindowPos_k( hwnd, 0, wp.ptMaxPosition.x, wp.ptMaxPosition.y, 0, 0,
                          SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
    }
    else if( flags & PLACE_RECT )
        SetWindowPos_k( hwnd, 0, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                      wp.rcNormalPosition.right - wp.rcNormalPosition.left,
                      wp.rcNormalPosition.bottom - wp.rcNormalPosition.top,
                      SWP_NOZORDER | SWP_NOACTIVATE );

    ShowWindow_k( hwnd, wndpl->showCmd );

    if (IsIconic_k( hwnd ))
    {
        if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_VISIBLE) WINPOS_ShowIconTitle( hwnd, TRUE );

        /* SDK: ...valid only the next time... */
        if( wndpl->flags & WPF_RESTORETOMAXIMIZED )
        {
            pWnd = WIN_GetPtr( hwnd );
            if (pWnd)
            {
                pWnd->flags |= WIN_RESTORE_MAX;
            }
        }
    }
    return TRUE;
}


/***********************************************************************
 *		SetWindowPlacement (USER32.@)
 *
 * Win95:
 * Fails if wndpl->length of Win95 (!) apps is invalid.
 */
BOOL WINAPI SetWindowPlacement_k( HWND hwnd, const WINDOWPLACEMENT *wpl )
{
    UINT flags = PLACE_MAX | PLACE_RECT;
    if (!wpl) return FALSE;
    if (wpl->flags & WPF_SETMINPOSITION) flags |= PLACE_MIN;
    return WINPOS_SetPlacement( hwnd, wpl, flags );
}


/***********************************************************************
 *		AnimateWindow (USER32.@)
 *		Shows/Hides a window with an animation
 *		NO ANIMATION YET
 */
BOOL WINAPI AnimateWindow_k(HWND hwnd, DWORD dwTime, DWORD dwFlags)
{
	FIXME("partial stub\n");

	/* If trying to show/hide and it's already   *
	 * shown/hidden or invalid window, fail with *
	 * invalid parameter                         */
	if(!IsWindow_k(hwnd) ||
	   (IsWindowVisible_k(hwnd) && !(dwFlags & AW_HIDE)) ||
	   (!IsWindowVisible_k(hwnd) && (dwFlags & AW_HIDE)))
	{
		SetLastError_k(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ShowWindow_k(hwnd, (dwFlags & AW_HIDE) ? SW_HIDE : ((dwFlags & AW_ACTIVATE) ? SW_SHOW : SW_SHOWNA));

	return TRUE;
}

/***********************************************************************
 *		SetInternalWindowPos (USER32.@)
 */
void WINAPI SetInternalWindowPos_k( HWND hwnd, UINT showCmd,
                                    LPRECT rect, LPPOINT pt )
{
    WINDOWPLACEMENT wndpl;
    UINT flags;

    wndpl.length  = sizeof(wndpl);
    wndpl.showCmd = showCmd;
    wndpl.flags = flags = 0;

    if( pt )
    {
        flags |= PLACE_MIN;
        wndpl.flags |= WPF_SETMINPOSITION;
        wndpl.ptMinPosition = *pt;
    }
    if( rect )
    {
        flags |= PLACE_RECT;
        wndpl.rcNormalPosition = *rect;
    }
    WINPOS_SetPlacement( hwnd, &wndpl, flags );
}


/*******************************************************************
 *         can_activate_window
 *
 * Check if we can activate the specified window.
 */
static BOOL can_activate_window( HWND hwnd )
{
    LONG style;

    if (!hwnd) return FALSE;
    style = GetWindowLongW_k( hwnd, GWL_STYLE );
    if (!(style & WS_VISIBLE)) return FALSE;
    if ((style & (WS_POPUP|WS_CHILD)) == WS_CHILD) return FALSE;
    return !(style & WS_DISABLED);
}


/*******************************************************************
 *         WINPOS_ActivateOtherWindow
 *
 *  Activates window other than pWnd.
 */
void WINPOS_ActivateOtherWindow(HWND hwnd)
{
    HWND hwndTo, fg;

    if ((GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_POPUP) && (hwndTo = GetWindow_k( hwnd, GW_OWNER )))
    {
        hwndTo = GetAncestor_k( hwndTo, GA_ROOT );
        if (can_activate_window( hwndTo )) goto done;
    }

    hwndTo = hwnd;
    for (;;)
    {
        if (!(hwndTo = GetWindow_k( hwndTo, GW_HWNDNEXT ))) break;
        if (can_activate_window( hwndTo )) break;
    }

 done:
    fg = GetForegroundWindow_k();
    TRACE("win = %p fg = %p\n", hwndTo, fg);
    if (!fg || (hwnd == fg))
    {
        if (SetForegroundWindow_k( hwndTo )) return;
    }
    if (!SetActiveWindow_k( hwndTo )) SetActiveWindow_k(0);
}


/***********************************************************************
 *           WINPOS_HandleWindowPosChanging
 *
 * Default handling for a WM_WINDOWPOSCHANGING. Called from DefWindowProc().
 */
LONG WINPOS_HandleWindowPosChanging( HWND hwnd, WINDOWPOS *winpos )
{
    POINT minTrack, maxTrack;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );

    if (winpos->flags & SWP_NOSIZE) return 0;
    if ((style & WS_THICKFRAME) || ((style & (WS_POPUP | WS_CHILD)) == 0))
    {
	WINPOS_GetMinMaxInfo( hwnd, NULL, NULL, &minTrack, &maxTrack );
	if (winpos->cx > maxTrack.x) winpos->cx = maxTrack.x;
	if (winpos->cy > maxTrack.y) winpos->cy = maxTrack.y;
	if (!(style & WS_MINIMIZE))
	{
	    if (winpos->cx < minTrack.x ) winpos->cx = minTrack.x;
	    if (winpos->cy < minTrack.y ) winpos->cy = minTrack.y;
	}
    }
    return 0;
}


/***********************************************************************
 *           dump_winpos_flags
 */
static void dump_winpos_flags(UINT flags)
{
    static const DWORD dumped_flags = (SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW |
                                       SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW |
                                       SWP_HIDEWINDOW | SWP_NOCOPYBITS | SWP_NOOWNERZORDER |
                                       SWP_NOSENDCHANGING | SWP_DEFERERASE | SWP_ASYNCWINDOWPOS |
                                       SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE | SWP_STATECHANGED);
    TRACE("flags:");
    if(flags & SWP_NOSIZE) TRACE(" SWP_NOSIZE");
    if(flags & SWP_NOMOVE) TRACE(" SWP_NOMOVE");
    if(flags & SWP_NOZORDER) TRACE(" SWP_NOZORDER");
    if(flags & SWP_NOREDRAW) TRACE(" SWP_NOREDRAW");
    if(flags & SWP_NOACTIVATE) TRACE(" SWP_NOACTIVATE");
    if(flags & SWP_FRAMECHANGED) TRACE(" SWP_FRAMECHANGED");
    if(flags & SWP_SHOWWINDOW) TRACE(" SWP_SHOWWINDOW");
    if(flags & SWP_HIDEWINDOW) TRACE(" SWP_HIDEWINDOW");
    if(flags & SWP_NOCOPYBITS) TRACE(" SWP_NOCOPYBITS");
    if(flags & SWP_NOOWNERZORDER) TRACE(" SWP_NOOWNERZORDER");
    if(flags & SWP_NOSENDCHANGING) TRACE(" SWP_NOSENDCHANGING");
    if(flags & SWP_DEFERERASE) TRACE(" SWP_DEFERERASE");
    if(flags & SWP_ASYNCWINDOWPOS) TRACE(" SWP_ASYNCWINDOWPOS");
    if(flags & SWP_NOCLIENTSIZE) TRACE(" SWP_NOCLIENTSIZE");
    if(flags & SWP_NOCLIENTMOVE) TRACE(" SWP_NOCLIENTMOVE");
    if(flags & SWP_STATECHANGED) TRACE(" SWP_STATECHANGED");

    if(flags & ~dumped_flags) TRACE(" %08x", flags & ~dumped_flags);
    TRACE("\n");
}

/***********************************************************************
 *           SWP_DoWinPosChanging
 */
static BOOL SWP_DoWinPosChanging( WINDOWPOS* pWinpos, RECT* pNewWindowRect, RECT* pNewClientRect )
{
    WND *wndPtr;
    RECT window_rect, client_rect;

    /* Send WM_WINDOWPOSCHANGING message */

    if (!(pWinpos->flags & SWP_NOSENDCHANGING))
        SendMessageW_k( pWinpos->hwnd, WM_WINDOWPOSCHANGING, 0, (LPARAM)pWinpos );

    if (!(wndPtr = WIN_GetPtr( pWinpos->hwnd )) || wndPtr == WND_DESKTOP) return FALSE;

    /* Calculate new position and size */

    WIN_GetRectangles( pWinpos->hwnd, COORDS_PARENT, &window_rect, &client_rect );
    *pNewWindowRect = window_rect;
    *pNewClientRect = (wndPtr->dwStyle & WS_MINIMIZE) ? window_rect : client_rect;

    if (!(pWinpos->flags & SWP_NOSIZE))
    {
        if (wndPtr->dwStyle & WS_MINIMIZE)
        {
            pNewWindowRect->right  = pNewWindowRect->left + GetSystemMetrics_k(SM_CXICON);
            pNewWindowRect->bottom = pNewWindowRect->top + GetSystemMetrics_k(SM_CYICON);
        }
        else
        {
            pNewWindowRect->right  = pNewWindowRect->left + pWinpos->cx;
            pNewWindowRect->bottom = pNewWindowRect->top + pWinpos->cy;
        }
    }
    if (!(pWinpos->flags & SWP_NOMOVE))
    {
        pNewWindowRect->left    = pWinpos->x;
        pNewWindowRect->top     = pWinpos->y;
        pNewWindowRect->right  += pWinpos->x - window_rect.left;
        pNewWindowRect->bottom += pWinpos->y - window_rect.top;

        OffsetRect_k( pNewClientRect, pWinpos->x - window_rect.left,
                                    pWinpos->y - window_rect.top );
    }
    pWinpos->flags |= SWP_NOCLIENTMOVE | SWP_NOCLIENTSIZE;

    TRACE( "hwnd %p, after %p, swp %d,%d %dx%d flags %08x\n",
           pWinpos->hwnd, pWinpos->hwndInsertAfter, pWinpos->x, pWinpos->y,
           pWinpos->cx, pWinpos->cy, pWinpos->flags );
    //TRACE( "current %s style %08x new %s\n", wine_dbgstr_rect( &window_rect ), wndPtr->dwStyle, wine_dbgstr_rect( pNewWindowRect ));
    return TRUE;
}

/***********************************************************************
 *           get_valid_rects
 *
 * Compute the valid rects from the old and new client rect and WVR_* flags.
 * Helper for WM_NCCALCSIZE handling.
 */
static INLINE void get_valid_rects( const RECT *old_client, const RECT *new_client, UINT flags,
                                    RECT *valid )
{
    int cx, cy;

    if (flags & WVR_REDRAW)
    {
        SetRectEmpty_k( &valid[0] );
        SetRectEmpty_k( &valid[1] );
        return;
    }

    if (flags & WVR_VALIDRECTS)
    {
        if (!IntersectRect_k( &valid[0], &valid[0], new_client ) ||
            !IntersectRect_k( &valid[1], &valid[1], old_client ))
        {
            SetRectEmpty_k( &valid[0] );
            SetRectEmpty_k( &valid[1] );
            return;
        }
        flags = WVR_ALIGNLEFT | WVR_ALIGNTOP;
    }
    else
    {
        valid[0] = *new_client;
        valid[1] = *old_client;
    }

    /* make sure the rectangles have the same size */
    cx = min( valid[0].right - valid[0].left, valid[1].right - valid[1].left );
    cy = min( valid[0].bottom - valid[0].top, valid[1].bottom - valid[1].top );

    if (flags & WVR_ALIGNBOTTOM)
    {
        valid[0].top = valid[0].bottom - cy;
        valid[1].top = valid[1].bottom - cy;
    }
    else
    {
        valid[0].bottom = valid[0].top + cy;
        valid[1].bottom = valid[1].top + cy;
    }
    if (flags & WVR_ALIGNRIGHT)
    {
        valid[0].left = valid[0].right - cx;
        valid[1].left = valid[1].right - cx;
    }
    else
    {
        valid[0].right = valid[0].left + cx;
        valid[1].right = valid[1].left + cx;
    }
}


/***********************************************************************
 *           SWP_DoOwnedPopups
 *
 * fix Z order taking into account owned popups -
 * basically we need to maintain them above the window that owns them
 *
 * FIXME: hide/show owned popups when owner visibility changes.
 */
static HWND SWP_DoOwnedPopups(HWND hwnd, HWND hwndInsertAfter)
{
    HWND owner, *list = NULL;
    unsigned int i;

    TRACE("(%p) hInsertAfter = %p\n", hwnd, hwndInsertAfter );

    if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD) return hwndInsertAfter;

    if ((owner = GetWindow_k( hwnd, GW_OWNER )))
    {
        /* make sure this popup stays above the owner */

        if (hwndInsertAfter != HWND_TOPMOST)
        {
            if (!(list = WIN_ListChildren( GetDesktopWindow_k() ))) return hwndInsertAfter;

            for (i = 0; list[i]; i++)
            {
                BOOL topmost = (GetWindowLongW_k( list[i], GWL_EXSTYLE ) & WS_EX_TOPMOST) != 0;

                if (list[i] == owner)
                {
                    if (i > 0) hwndInsertAfter = list[i-1];
                    else hwndInsertAfter = topmost ? HWND_TOPMOST : HWND_TOP;
                    break;
                }

                if (hwndInsertAfter == HWND_TOP || hwndInsertAfter == HWND_NOTOPMOST)
                {
                    if (!topmost) break;
                }
                else if (list[i] == hwndInsertAfter) break;
            }
        }
    }

    if (hwndInsertAfter == HWND_BOTTOM) goto done;
    if (!list && !(list = WIN_ListChildren( GetDesktopWindow_k() ))) goto done;

    i = 0;
    if (hwndInsertAfter == HWND_TOP || hwndInsertAfter == HWND_NOTOPMOST)
    {
        if (hwndInsertAfter == HWND_NOTOPMOST || !(GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_TOPMOST))
        {
            /* skip all the topmost windows */
            while (list[i] && (GetWindowLongW_k( list[i], GWL_EXSTYLE ) & WS_EX_TOPMOST)) i++;
        }
    }
    else if (hwndInsertAfter != HWND_TOPMOST)
    {
        /* skip windows that are already placed correctly */
        for (i = 0; list[i]; i++)
        {
            if (list[i] == hwndInsertAfter) break;
            if (list[i] == hwnd) goto done;  /* nothing to do if window is moving backwards in z-order */
        }
    }

    for ( ; list[i]; i++)
    {
        if (list[i] == hwnd) break;
        if (GetWindow_k( list[i], GW_OWNER ) != hwnd) continue;
        TRACE( "moving %p owned by %p after %p\n", list[i], hwnd, hwndInsertAfter );
        SetWindowPos_k( list[i], hwndInsertAfter, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_DEFERERASE );
        hwndInsertAfter = list[i];
    }

done:
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return hwndInsertAfter;
}

/***********************************************************************
 *           SWP_DoNCCalcSize
 */
static UINT SWP_DoNCCalcSize( WINDOWPOS* pWinpos, const RECT* pNewWindowRect, RECT* pNewClientRect,
                              RECT *validRects )
{
    UINT wvrFlags = 0;
    RECT window_rect, client_rect;

    WIN_GetRectangles( pWinpos->hwnd, COORDS_PARENT, &window_rect, &client_rect );

      /* Send WM_NCCALCSIZE message to get new client area */
    if( (pWinpos->flags & (SWP_FRAMECHANGED | SWP_NOSIZE)) != SWP_NOSIZE )
    {
        NCCALCSIZE_PARAMS params;
        WINDOWPOS winposCopy;

        params.rgrc[0] = *pNewWindowRect;
        params.rgrc[1] = window_rect;
        params.rgrc[2] = client_rect;
        params.lppos = &winposCopy;
        winposCopy = *pWinpos;

        wvrFlags = SendMessageW_k( pWinpos->hwnd, WM_NCCALCSIZE, TRUE, (LPARAM)&params );

        *pNewClientRect = params.rgrc[0];

        // TRACE( "hwnd %p old win %s old client %s new win %s new client %s\n", pWinpos->hwnd, wine_dbgstr_rect(&window_rect), wine_dbgstr_rect(&client_rect), wine_dbgstr_rect(pNewWindowRect), wine_dbgstr_rect(pNewClientRect) );

        if( pNewClientRect->left != client_rect.left ||
            pNewClientRect->top != client_rect.top )
            pWinpos->flags &= ~SWP_NOCLIENTMOVE;

        if( (pNewClientRect->right - pNewClientRect->left !=
             client_rect.right - client_rect.left))
            pWinpos->flags &= ~SWP_NOCLIENTSIZE;
        else
            wvrFlags &= ~WVR_HREDRAW;

        if (pNewClientRect->bottom - pNewClientRect->top !=
             client_rect.bottom - client_rect.top)
            pWinpos->flags &= ~SWP_NOCLIENTSIZE;
        else
            wvrFlags &= ~WVR_VREDRAW;

        validRects[0] = params.rgrc[1];
        validRects[1] = params.rgrc[2];
    }
    else
    {
        if (!(pWinpos->flags & SWP_NOMOVE) &&
            (pNewClientRect->left != client_rect.left ||
             pNewClientRect->top != client_rect.top))
            pWinpos->flags &= ~SWP_NOCLIENTMOVE;
    }

    if (pWinpos->flags & (SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_SHOWWINDOW | SWP_HIDEWINDOW))
    {
        SetRectEmpty_k( &validRects[0] );
        SetRectEmpty_k( &validRects[1] );
    }
    else get_valid_rects( &client_rect, pNewClientRect, wvrFlags, validRects );

    return wvrFlags;
}

/* fix redundant flags and values in the WINDOWPOS structure */
static BOOL fixup_flags( WINDOWPOS *winpos )
{
    HWND parent;
    RECT window_rect;
    POINT pt;
    WND *wndPtr = WIN_GetPtr( winpos->hwnd );
    BOOL ret = TRUE;

    if (!wndPtr)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    winpos->hwnd = wndPtr->obj.handle;  /* make it a full handle */

    /* Finally make sure that all coordinates are valid */
    if (winpos->x < -32768) winpos->x = -32768;
    else if (winpos->x > 32767) winpos->x = 32767;
    if (winpos->y < -32768) winpos->y = -32768;
    else if (winpos->y > 32767) winpos->y = 32767;

    if (winpos->cx < 0) winpos->cx = 0;
    else if (winpos->cx > 32767) winpos->cx = 32767;
    if (winpos->cy < 0) winpos->cy = 0;
    else if (winpos->cy > 32767) winpos->cy = 32767;

    parent = GetAncestor_k( winpos->hwnd, GA_PARENT );
    if (!IsWindowVisible_k( parent )) winpos->flags |= SWP_NOREDRAW;

    if (winpos->flags & SWP_HIDEWINDOW) wndPtr->flags |= WIN_HIDDEN;
    else if (winpos->flags & SWP_SHOWWINDOW) wndPtr->flags &= ~WIN_HIDDEN;

    if (wndPtr->dwStyle & WS_VISIBLE) winpos->flags &= ~SWP_SHOWWINDOW;
    else
    {
        winpos->flags &= ~SWP_HIDEWINDOW;
        if (!(winpos->flags & SWP_SHOWWINDOW)) winpos->flags |= SWP_NOREDRAW;
    }

    WIN_GetRectangles( winpos->hwnd, COORDS_SCREEN, &window_rect, NULL );
    if ((window_rect.right - window_rect.left == winpos->cx) &&
        (window_rect.bottom - window_rect.top == winpos->cy))
        winpos->flags |= SWP_NOSIZE;    /* Already the right size */

    pt.x = winpos->x;
    pt.y = winpos->y;
    ClientToScreen_k( parent, &pt );
    if ((window_rect.left == pt.x) && (window_rect.top == pt.y))
        winpos->flags |= SWP_NOMOVE;    /* Already the right position */

    if ((wndPtr->dwStyle & (WS_POPUP | WS_CHILD)) != WS_CHILD)
    {
        if (!(winpos->flags & (SWP_NOACTIVATE|SWP_HIDEWINDOW)) && /* Bring to the top when activating */
            (winpos->flags & SWP_NOZORDER ||
             (winpos->hwndInsertAfter != HWND_TOPMOST && winpos->hwndInsertAfter != HWND_NOTOPMOST)))
        {
            winpos->flags &= ~SWP_NOZORDER;
            winpos->hwndInsertAfter = HWND_TOP;
        }
    }

    /* Check hwndInsertAfter */
    if (winpos->flags & SWP_NOZORDER) goto done;

    if (winpos->hwndInsertAfter == HWND_TOP)
    {
        if (GetWindow_k(winpos->hwnd, GW_HWNDFIRST) == winpos->hwnd)
            winpos->flags |= SWP_NOZORDER;
    }
    else if (winpos->hwndInsertAfter == HWND_BOTTOM)
    {
        if (!(wndPtr->dwExStyle & WS_EX_TOPMOST) && GetWindow_k(winpos->hwnd, GW_HWNDLAST) == winpos->hwnd)
            winpos->flags |= SWP_NOZORDER;
    }
    else if (winpos->hwndInsertAfter == HWND_TOPMOST)
    {
        if ((wndPtr->dwExStyle & WS_EX_TOPMOST) && GetWindow_k(winpos->hwnd, GW_HWNDFIRST) == winpos->hwnd)
            winpos->flags |= SWP_NOZORDER;
    }
    else if (winpos->hwndInsertAfter == HWND_NOTOPMOST)
    {
        if (!(wndPtr->dwExStyle & WS_EX_TOPMOST))
            winpos->flags |= SWP_NOZORDER;
    }
    else
    {
        if ((winpos->hwnd == winpos->hwndInsertAfter) ||
            (winpos->hwnd == GetWindow_k( winpos->hwndInsertAfter, GW_HWNDNEXT )))
            winpos->flags |= SWP_NOZORDER;
    }
 done:
    return ret;
}


/***********************************************************************
 *		set_window_pos
 *
 * Backend implementation of SetWindowPos.
 */
BOOL set_window_pos( HWND hwnd, HWND insert_after, UINT swp_flags,
                     const RECT *window_rect, const RECT *client_rect, const RECT *valid_rects )
{
    WND *win;
    HWND parent = GetAncestor_k( hwnd, GA_PARENT );
    int old_width;
    RECT visible_rect, old_visible_rect, old_window_rect;
    struct window_surface *old_surface, *new_surface = NULL;

    visible_rect = *window_rect;
    USER_Driver->pWindowPosChanging( hwnd, insert_after, swp_flags, window_rect, client_rect, &visible_rect, &new_surface );

    WIN_GetRectangles( hwnd, COORDS_SCREEN, &old_window_rect, NULL );

    if (!(win = WIN_GetPtr( hwnd )) || win == WND_DESKTOP)
    {
        if (new_surface) window_surface_release( new_surface );
        return FALSE;
    }
    old_width = win->rectClient.right - win->rectClient.left;
    old_visible_rect = win->visible_rect;
    old_surface = win->surface;
    if (old_surface != new_surface) swp_flags |= SWP_FRAMECHANGED;  /* force refreshing non-client area */

    if (!(swp_flags & SWP_NOZORDER) && win->parent) {
        WND* parent = WIN_GetPtr(win->parent);
        WND* after = WIN_GetPtr(insert_after);
        if (parent && after) {
            int index = pblListIndexOf(parent->children, after);
            if (index>=0) {
                pblListRemoveElement(parent->children, win);
                index = pblListIndexOf(parent->children, after);
                pblListAddAt(parent->children, index+1, win);
            }
        }
    }
    if (swp_flags & SWP_SHOWWINDOW) win->dwStyle |= WS_VISIBLE;
    else if (swp_flags & SWP_HIDEWINDOW) win->dwStyle &= ~WS_VISIBLE;

    if (((swp_flags & SWP_AGG_NOPOSCHANGE) != SWP_AGG_NOPOSCHANGE) || (swp_flags & (SWP_HIDEWINDOW | SWP_SHOWWINDOW | SWP_STATECHANGED | SWP_FRAMECHANGED))) {
        invalidate_dce( win, &old_window_rect );
    }

    {
        TRACE( "win %p surface %p -> %p\n", hwnd, old_surface, new_surface );
        register_window_surface( old_surface, new_surface );
        if (old_surface)
        {
            if (!IsRectEmpty_k( valid_rects ))
            {
                move_window_bits( hwnd, old_surface, new_surface, &visible_rect,
                                  &old_visible_rect, client_rect, valid_rects );
                valid_rects = NULL;  /* prevent the driver from trying to also move the bits */
            }
            window_surface_release( old_surface );
        }
        USER_Driver->pWindowPosChanged( hwnd, insert_after, swp_flags, window_rect,
                                        client_rect, &visible_rect, valid_rects, new_surface );
    }

    return TRUE;
}


/***********************************************************************
 *		USER_SetWindowPos
 *
 *     User32 internal function
 */
BOOL USER_SetWindowPos_k( WINDOWPOS * winpos )
{
    RECT newWindowRect, newClientRect, valid_rects[2];
    UINT orig_flags;
    
    orig_flags = winpos->flags;

    /* First, check z-order arguments.  */
    if (!(winpos->flags & SWP_NOZORDER))
    {
        /* fix sign extension */
        if (winpos->hwndInsertAfter == (HWND)0xffff) winpos->hwndInsertAfter = HWND_TOPMOST;
        else if (winpos->hwndInsertAfter == (HWND)0xfffe) winpos->hwndInsertAfter = HWND_NOTOPMOST;

        if (!(winpos->hwndInsertAfter == HWND_TOP ||
              winpos->hwndInsertAfter == HWND_BOTTOM ||
              winpos->hwndInsertAfter == HWND_TOPMOST ||
              winpos->hwndInsertAfter == HWND_NOTOPMOST))
        {
            HWND parent = GetAncestor_k( winpos->hwnd, GA_PARENT );
            HWND insertafter_parent = GetAncestor_k( winpos->hwndInsertAfter, GA_PARENT );

            /* hwndInsertAfter must be a sibling of the window */
            if (!insertafter_parent) return FALSE;
            if (insertafter_parent != parent) return TRUE;
        }
    }

    /* Make sure that coordinates are valid for WM_WINDOWPOSCHANGING */
    if (!(winpos->flags & SWP_NOMOVE))
    {
        if (winpos->x < -32768) winpos->x = -32768;
        else if (winpos->x > 32767) winpos->x = 32767;
        if (winpos->y < -32768) winpos->y = -32768;
        else if (winpos->y > 32767) winpos->y = 32767;
    }
    if (!(winpos->flags & SWP_NOSIZE))
    {
        if (winpos->cx < 0) winpos->cx = 0;
        else if (winpos->cx > 32767) winpos->cx = 32767;
        if (winpos->cy < 0) winpos->cy = 0;
        else if (winpos->cy > 32767) winpos->cy = 32767;
    }

    if (!SWP_DoWinPosChanging( winpos, &newWindowRect, &newClientRect )) return FALSE;

    /* Fix redundant flags */
    if (!fixup_flags( winpos )) return FALSE;

    if((winpos->flags & (SWP_NOZORDER | SWP_HIDEWINDOW | SWP_SHOWWINDOW)) != SWP_NOZORDER)
    {
        if (GetAncestor_k( winpos->hwnd, GA_PARENT ) == GetDesktopWindow_k())
            winpos->hwndInsertAfter = SWP_DoOwnedPopups( winpos->hwnd, winpos->hwndInsertAfter );
    }

    /* Common operations */

    SWP_DoNCCalcSize( winpos, &newWindowRect, &newClientRect, valid_rects );

    if (!set_window_pos( winpos->hwnd, winpos->hwndInsertAfter, winpos->flags,
                         &newWindowRect, &newClientRect, valid_rects ))
        return FALSE;

    /* erase parent when hiding or resizing child */
    if (!(orig_flags & SWP_DEFERERASE) &&
        ((orig_flags & SWP_HIDEWINDOW) ||
         (!(orig_flags & SWP_SHOWWINDOW) &&
          (winpos->flags & SWP_AGG_STATUSFLAGS) != SWP_AGG_NOGEOMETRYCHANGE)))
    {
        HWND parent = GetAncestor_k( winpos->hwnd, GA_PARENT );
        if (!parent || parent == GetDesktopWindow_k()) parent = winpos->hwnd;
        erase_now( parent, 0 );
    }

    if( winpos->flags & SWP_HIDEWINDOW )
        HideCaret_k(winpos->hwnd);
    else if (winpos->flags & SWP_SHOWWINDOW)
        ShowCaret_k(winpos->hwnd);

    if (!(winpos->flags & (SWP_NOACTIVATE|SWP_HIDEWINDOW)))
    {
        /* child windows get WM_CHILDACTIVATE message */
        if ((GetWindowLongW_k( winpos->hwnd, GWL_STYLE ) & (WS_CHILD | WS_POPUP)) == WS_CHILD)
            SendMessageW_k( winpos->hwnd, WM_CHILDACTIVATE, 0, 0 );
        else
            SetForegroundWindow_k( winpos->hwnd );
    }

      /* And last, send the WM_WINDOWPOSCHANGED message */

    TRACE("\tstatus flags = %04x\n", winpos->flags & SWP_AGG_STATUSFLAGS);

    if (((winpos->flags & SWP_AGG_STATUSFLAGS) != SWP_AGG_NOPOSCHANGE))
    {
        /* WM_WINDOWPOSCHANGED is sent even if SWP_NOSENDCHANGING is set
           and always contains final window position.
         */
        winpos->x = newWindowRect.left;
        winpos->y = newWindowRect.top;
        winpos->cx = newWindowRect.right - newWindowRect.left;
        winpos->cy = newWindowRect.bottom - newWindowRect.top;
        SendMessageW_k( winpos->hwnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)winpos );
    }
    return TRUE;
}

/***********************************************************************
 *		SetWindowPos (USER32.@)
 */
BOOL WINAPI SetWindowPos_k( HWND hwnd, HWND hwndInsertAfter,
                          INT x, INT y, INT cx, INT cy, UINT flags )
{
    WINDOWPOS winpos;

    TRACE("hwnd %p, after %p, %d,%d (%dx%d), flags %08x\n",
          hwnd, hwndInsertAfter, x, y, cx, cy, flags);

    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    winpos.hwnd = WIN_GetFullHandle(hwnd);
    winpos.hwndInsertAfter = WIN_GetFullHandle(hwndInsertAfter);
    winpos.x = x;
    winpos.y = y;
    winpos.cx = cx;
    winpos.cy = cy;
    winpos.flags = flags;
    
    if (WIN_IsCurrentThread( hwnd ))
        return USER_SetWindowPos_k(&winpos);

    return SendMessageW_k( winpos.hwnd, WM_WINE_SETWINDOWPOS, 0, (LPARAM)&winpos );
}


/***********************************************************************
 *		BeginDeferWindowPos (USER32.@)
 */
HDWP WINAPI BeginDeferWindowPos_k( INT count )
{
    HDWP handle = 0;
    DWP *pDWP;

    TRACE("%d\n", count);

    if (count < 0)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }
    /* Windows allows zero count, in which case it allocates context for 8 moves */
    if (count == 0) count = 8;

    if (!(pDWP = HeapAlloc_k( GetProcessHeap_k(), 0, sizeof(DWP)))) return 0;

    pDWP->actualCount    = 0;
    pDWP->suggestedCount = count;
    pDWP->hwndParent     = 0;

    if (!(pDWP->winPos = HeapAlloc_k( GetProcessHeap_k(), 0, count * sizeof(WINDOWPOS) )) ||
        !(handle = alloc_user_handle( &pDWP->obj, USER_DWP )))
    {
        HeapFree_k( GetProcessHeap_k(), 0, pDWP->winPos );
        HeapFree_k( GetProcessHeap_k(), 0, pDWP );
    }

    TRACE("returning hdwp %p\n", handle);
    return handle;
}


/***********************************************************************
 *		DeferWindowPos (USER32.@)
 */
HDWP WINAPI DeferWindowPos_k( HDWP hdwp, HWND hwnd, HWND hwndAfter,
                                INT x, INT y, INT cx, INT cy,
                                UINT flags )
{
    DWP *pDWP;
    int i;
    HDWP retvalue = hdwp;

    TRACE("hdwp %p, hwnd %p, after %p, %d,%d (%dx%d), flags %08x\n",
          hdwp, hwnd, hwndAfter, x, y, cx, cy, flags);

    hwnd = WIN_GetFullHandle( hwnd );
    if (is_desktop_window( hwnd )) return 0;

    if (!(pDWP = get_user_handle_ptr( hdwp, USER_DWP ))) return 0;
    if (pDWP == OBJ_OTHER_PROCESS)
    {
        FIXME( "other process handle %p?\n", hdwp );
        return 0;
    }

    for (i = 0; i < pDWP->actualCount; i++)
    {
        if (pDWP->winPos[i].hwnd == hwnd)
        {
              /* Merge with the other changes */
            if (!(flags & SWP_NOZORDER))
            {
                pDWP->winPos[i].hwndInsertAfter = WIN_GetFullHandle(hwndAfter);
            }
            if (!(flags & SWP_NOMOVE))
            {
                pDWP->winPos[i].x = x;
                pDWP->winPos[i].y = y;
            }
            if (!(flags & SWP_NOSIZE))
            {
                pDWP->winPos[i].cx = cx;
                pDWP->winPos[i].cy = cy;
            }
            pDWP->winPos[i].flags &= flags | ~(SWP_NOSIZE | SWP_NOMOVE |
                                               SWP_NOZORDER | SWP_NOREDRAW |
                                               SWP_NOACTIVATE | SWP_NOCOPYBITS|
                                               SWP_NOOWNERZORDER);
            pDWP->winPos[i].flags |= flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW |
                                              SWP_FRAMECHANGED);
            goto END;
        }
    }
    if (pDWP->actualCount >= pDWP->suggestedCount)
    {
        WINDOWPOS *newpos = HeapReAlloc_k( GetProcessHeap_k(), 0, pDWP->winPos,
                                         pDWP->suggestedCount * 2 * sizeof(WINDOWPOS) );
        if (!newpos)
        {
            retvalue = 0;
            goto END;
        }
        pDWP->suggestedCount *= 2;
        pDWP->winPos = newpos;
    }
    pDWP->winPos[pDWP->actualCount].hwnd = hwnd;
    pDWP->winPos[pDWP->actualCount].hwndInsertAfter = hwndAfter;
    pDWP->winPos[pDWP->actualCount].x = x;
    pDWP->winPos[pDWP->actualCount].y = y;
    pDWP->winPos[pDWP->actualCount].cx = cx;
    pDWP->winPos[pDWP->actualCount].cy = cy;
    pDWP->winPos[pDWP->actualCount].flags = flags;
    pDWP->actualCount++;
END:
    release_user_handle_ptr( pDWP );
    return retvalue;
}


/***********************************************************************
 *		EndDeferWindowPos (USER32.@)
 */
BOOL WINAPI EndDeferWindowPos_k( HDWP hdwp )
{
    DWP *pDWP;
    WINDOWPOS *winpos;
    BOOL res = TRUE;
    int i;

    TRACE("%p\n", hdwp);

    if (!(pDWP = free_user_handle( hdwp, USER_DWP ))) return FALSE;
    if (pDWP == OBJ_OTHER_PROCESS)
    {
        FIXME( "other process handle %p?\n", hdwp );
        return FALSE;
    }

    for (i = 0, winpos = pDWP->winPos; res && i < pDWP->actualCount; i++, winpos++)
    {
        TRACE("hwnd %p, after %p, %d,%d (%dx%d), flags %08x\n",
               winpos->hwnd, winpos->hwndInsertAfter, winpos->x, winpos->y,
               winpos->cx, winpos->cy, winpos->flags);

        if (WIN_IsCurrentThread( winpos->hwnd ))
            res = USER_SetWindowPos_k( winpos );
        else
            res = SendMessageW_k( winpos->hwnd, WM_WINE_SETWINDOWPOS, 0, (LPARAM)winpos );
    }
    HeapFree_k( GetProcessHeap_k(), 0, pDWP->winPos );
    HeapFree_k( GetProcessHeap_k(), 0, pDWP );
    return res;
}


/***********************************************************************
 *		ArrangeIconicWindows (USER32.@)
 */
UINT WINAPI ArrangeIconicWindows_k( HWND parent )
{
    RECT rectParent;
    HWND hwndChild;
    INT x, y, xspacing, yspacing;

    GetClientRect_k( parent, &rectParent );
    x = rectParent.left;
    y = rectParent.bottom;
    xspacing = GetSystemMetrics_k(SM_CXICONSPACING);
    yspacing = GetSystemMetrics_k(SM_CYICONSPACING);

    hwndChild = GetWindow_k( parent, GW_CHILD );
    while (hwndChild)
    {
        if( IsIconic_k( hwndChild ) )
        {
            WINPOS_ShowIconTitle( hwndChild, FALSE );

            SetWindowPos_k( hwndChild, 0, x + (xspacing - GetSystemMetrics_k(SM_CXICON)) / 2,
                            y - yspacing - GetSystemMetrics_k(SM_CYICON)/2, 0, 0,
                            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
	    if( IsWindow_k(hwndChild) )
                WINPOS_ShowIconTitle(hwndChild , TRUE );

            if (x <= rectParent.right - xspacing) x += xspacing;
            else
            {
                x = rectParent.left;
                y -= yspacing;
            }
        }
        hwndChild = GetWindow_k( hwndChild, GW_HWNDNEXT );
    }
    return yspacing;
}


/***********************************************************************
 *           draw_moving_frame
 *
 * Draw the frame used when moving or resizing window.
 */
static void draw_moving_frame( HWND parent, HDC hdc, RECT *screen_rect, BOOL thickframe )
{
    RECT rect = *screen_rect;

    if (parent) MapWindowPoints_k( 0, parent, (POINT *)&rect, 2 );
    if (thickframe)
    {
        const int width = GetSystemMetrics_k(SM_CXFRAME);
        const int height = GetSystemMetrics_k(SM_CYFRAME);

        HBRUSH hbrush = SelectObject_k( hdc, GetStockObject_k( GRAY_BRUSH ) );
        PatBlt_k( hdc, rect.left, rect.top,
                rect.right - rect.left - width, height, PATINVERT );
        PatBlt_k( hdc, rect.left, rect.top + height, width,
                rect.bottom - rect.top - height, PATINVERT );
        PatBlt_k( hdc, rect.left + width, rect.bottom - 1,
                rect.right - rect.left - width, -height, PATINVERT );
        PatBlt_k( hdc, rect.right - 1, rect.top, -width,
                rect.bottom - rect.top - height, PATINVERT );
        SelectObject_k( hdc, hbrush );
    }
    else DrawFocusRect_k( hdc, &rect );
}


/***********************************************************************
 *           start_size_move
 *
 * Initialization of a move or resize, when initiated from a menu choice.
 * Return hit test code for caption or sizing border.
 */
static LONG start_size_move( HWND hwnd, WPARAM wParam, POINT *capturePoint, LONG style )
{
    LONG hittest = 0;
    POINT pt;
    MSG msg;
    RECT rectWindow;

    GetWindowRect_k( hwnd, &rectWindow );

    if ((wParam & 0xfff0) == SC_MOVE)
    {
        /* Move pointer at the center of the caption */
        RECT rect = rectWindow;
        /* Note: to be exactly centered we should take the different types
         * of border into account, but it shouldn't make more than a few pixels
         * of difference so let's not bother with that */
        rect.top += GetSystemMetrics_k(SM_CYBORDER);
        if (style & WS_SYSMENU)
            rect.left += GetSystemMetrics_k(SM_CXSIZE) + 1;
        if (style & WS_MINIMIZEBOX)
            rect.right -= GetSystemMetrics_k(SM_CXSIZE) + 1;
        if (style & WS_MAXIMIZEBOX)
            rect.right -= GetSystemMetrics_k(SM_CXSIZE) + 1;
        pt.x = (rect.right + rect.left) / 2;
        pt.y = rect.top + GetSystemMetrics_k(SM_CYSIZE)/2;
        hittest = HTCAPTION;
        *capturePoint = pt;
    }
    else  /* SC_SIZE */
    {
        SetCursor_k( LoadCursorW_k( 0, (LPWSTR)IDC_SIZEALL ) );
        pt.x = pt.y = 0;
        while(!hittest)
        {
            if (!GetMessageW_k( &msg, 0, 0, 0 )) return 0;
            if (CallMsgFilterW_k( &msg, MSGF_SIZE )) continue;

            switch(msg.message)
            {
            case WM_MOUSEMOVE:
                pt.x = min( max( msg.pt.x, rectWindow.left ), rectWindow.right - 1 );
                pt.y = min( max( msg.pt.y, rectWindow.top ), rectWindow.bottom - 1 );
                hittest = SendMessageW_k( hwnd, WM_NCHITTEST, 0, MAKELONG( pt.x, pt.y ) );
                if ((hittest < HTLEFT) || (hittest > HTBOTTOMRIGHT)) hittest = 0;
                break;

            case WM_LBUTTONUP:
                return 0;

            case WM_KEYDOWN:
                switch(msg.wParam)
                {
                case VK_UP:
                    hittest = HTTOP;
                    pt.x =(rectWindow.left+rectWindow.right)/2;
                    pt.y = rectWindow.top + GetSystemMetrics_k(SM_CYFRAME) / 2;
                    break;
                case VK_DOWN:
                    hittest = HTBOTTOM;
                    pt.x =(rectWindow.left+rectWindow.right)/2;
                    pt.y = rectWindow.bottom - GetSystemMetrics_k(SM_CYFRAME) / 2;
                    break;
                case VK_LEFT:
                    hittest = HTLEFT;
                    pt.x = rectWindow.left + GetSystemMetrics_k(SM_CXFRAME) / 2;
                    pt.y =(rectWindow.top+rectWindow.bottom)/2;
                    break;
                case VK_RIGHT:
                    hittest = HTRIGHT;
                    pt.x = rectWindow.right - GetSystemMetrics_k(SM_CXFRAME) / 2;
                    pt.y =(rectWindow.top+rectWindow.bottom)/2;
                    break;
                case VK_RETURN:
                case VK_ESCAPE:
                    return 0;
                }
                break;
            default:
                TranslateMessage_k( &msg );
                DispatchMessageW_k( &msg );
                break;
            }
        }
        *capturePoint = pt;
    }
    SetCursorPos_k( pt.x, pt.y );
    SendMessageW_k( hwnd, WM_SETCURSOR, (WPARAM)hwnd, MAKELONG( hittest, WM_MOUSEMOVE ));
    return hittest;
}


/***********************************************************************
 *           WINPOS_SysCommandSizeMove
 *
 * Perform SC_MOVE and SC_SIZE commands.
 */
void WINPOS_SysCommandSizeMove( HWND hwnd, WPARAM wParam )
{
    MSG msg;
    RECT sizingRect, mouseRect, origRect;
    HDC hdc;
    HWND parent;
    LONG hittest = (LONG)(wParam & 0x0f);
    WPARAM syscommand = wParam & 0xfff0;
    HCURSOR hDragCursor = 0, hOldCursor = 0;
    POINT minTrack, maxTrack;
    POINT capturePoint, pt;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    BOOL    thickframe = HAS_THICKFRAME( style );
    BOOL    iconic = style & WS_MINIMIZE;
    BOOL    moved = FALSE;
    DWORD     dwPoint = GetMessagePos_k();
    BOOL DragFullWindows = TRUE;

    if (IsZoomed_k(hwnd) || !IsWindowVisible_k(hwnd)) return;

    pt.x = (short)LOWORD(dwPoint);
    pt.y = (short)HIWORD(dwPoint);
    capturePoint = pt;
    ClipCursor_k( NULL );

    TRACE("hwnd %p command %04lx, hittest %d, pos %d,%d\n",
          hwnd, syscommand, hittest, pt.x, pt.y);

    if (syscommand == SC_MOVE)
    {
        if (!hittest) hittest = start_size_move( hwnd, wParam, &capturePoint, style );
        if (!hittest) return;
    }
    else  /* SC_SIZE */
    {
        if ( hittest && (syscommand != SC_MOUSEMENU) )
            hittest += (HTLEFT - WMSZ_LEFT);
        else
        {
            set_capture_window( hwnd, GUI_INMOVESIZE, NULL );
            hittest = start_size_move( hwnd, wParam, &capturePoint, style );
            if (!hittest)
            {
                set_capture_window( 0, GUI_INMOVESIZE, NULL );
                return;
            }
        }
    }

      /* Get min/max info */

    WINPOS_GetMinMaxInfo( hwnd, NULL, NULL, &minTrack, &maxTrack );
    WIN_GetRectangles( hwnd, COORDS_PARENT, &sizingRect, NULL );
    origRect = sizingRect;
    if (style & WS_CHILD)
    {
        parent = GetParent_k(hwnd);
        GetClientRect_k( parent, &mouseRect );
        MapWindowPoints_k( parent, 0, (LPPOINT)&mouseRect, 2 );
        MapWindowPoints_k( parent, 0, (LPPOINT)&sizingRect, 2 );
    }
    else
    {
        parent = 0;
        mouseRect = get_virtual_screen_rect();
    }

    if (ON_LEFT_BORDER(hittest))
    {
        mouseRect.left  = max( mouseRect.left, sizingRect.right-maxTrack.x+capturePoint.x-sizingRect.left );
        mouseRect.right = min( mouseRect.right, sizingRect.right-minTrack.x+capturePoint.x-sizingRect.left );
    }
    else if (ON_RIGHT_BORDER(hittest))
    {
        mouseRect.left  = max( mouseRect.left, sizingRect.left+minTrack.x+capturePoint.x-sizingRect.right );
        mouseRect.right = min( mouseRect.right, sizingRect.left+maxTrack.x+capturePoint.x-sizingRect.right );
    }
    if (ON_TOP_BORDER(hittest))
    {
        mouseRect.top    = max( mouseRect.top, sizingRect.bottom-maxTrack.y+capturePoint.y-sizingRect.top );
        mouseRect.bottom = min( mouseRect.bottom,sizingRect.bottom-minTrack.y+capturePoint.y-sizingRect.top);
    }
    else if (ON_BOTTOM_BORDER(hittest))
    {
        mouseRect.top    = max( mouseRect.top, sizingRect.top+minTrack.y+capturePoint.y-sizingRect.bottom );
        mouseRect.bottom = min( mouseRect.bottom, sizingRect.top+maxTrack.y+capturePoint.y-sizingRect.bottom );
    }

    /* Retrieve a default cache DC (without using the window style) */
    hdc = GetDCEx_k( parent, 0, DCX_CACHE );

    if( iconic ) /* create a cursor for dragging */
    {
        hDragCursor = (HCURSOR)GetClassLongPtrW_k( hwnd, GCLP_HICON);
        if( !hDragCursor ) hDragCursor = (HCURSOR)SendMessageW_k( hwnd, WM_QUERYDRAGICON, 0, 0L);
        if( !hDragCursor ) iconic = FALSE;
    }

    /* we only allow disabling the full window drag for child windows */
    if (parent) SystemParametersInfoW_k( SPI_GETDRAGFULLWINDOWS, 0, &DragFullWindows, 0 );

    /* repaint the window before moving it around */
    RedrawWindow_k( hwnd, NULL, 0, RDW_UPDATENOW | RDW_ALLCHILDREN );

    SendMessageW_k( hwnd, WM_ENTERSIZEMOVE, 0, 0 );
    set_capture_window( hwnd, GUI_INMOVESIZE, NULL );

    while(1)
    {
        int dx = 0, dy = 0;

        if (!GetMessageW_k( &msg, 0, 0, 0 )) break;
        if (CallMsgFilterW_k( &msg, MSGF_SIZE )) continue;

        /* Exit on button-up, Return, or Esc */
        if ((msg.message == WM_LBUTTONUP) ||
            ((msg.message == WM_KEYDOWN) &&
             ((msg.wParam == VK_RETURN) || (msg.wParam == VK_ESCAPE)))) break;

        if ((msg.message != WM_KEYDOWN) && (msg.message != WM_MOUSEMOVE))
        {
            TranslateMessage_k( &msg );
            DispatchMessageW_k( &msg );
            continue;  /* We are not interested in other messages */
        }

        pt = msg.pt;

        if (msg.message == WM_KEYDOWN) switch(msg.wParam)
        {
        case VK_UP:    pt.y -= 8; break;
        case VK_DOWN:  pt.y += 8; break;
        case VK_LEFT:  pt.x -= 8; break;
        case VK_RIGHT: pt.x += 8; break;
        }

        pt.x = max( pt.x, mouseRect.left );
        pt.x = min( pt.x, mouseRect.right );
        pt.y = max( pt.y, mouseRect.top );
        pt.y = min( pt.y, mouseRect.bottom );

        dx = pt.x - capturePoint.x;
        dy = pt.y - capturePoint.y;

        if (dx || dy)
        {
            if( !moved )
            {
                moved = TRUE;

                if( iconic ) /* ok, no system popup tracking */
                {
                    hOldCursor = SetCursor_k(hDragCursor);
                    ShowCursor_k( TRUE );
                    WINPOS_ShowIconTitle( hwnd, FALSE );
                }
                else if(!DragFullWindows)
                    draw_moving_frame( parent, hdc, &sizingRect, thickframe );
            }

            if (msg.message == WM_KEYDOWN) SetCursorPos_k( pt.x, pt.y );
            else
            {
                if(!iconic && !DragFullWindows) draw_moving_frame( parent, hdc, &sizingRect, thickframe );
                if (hittest == HTCAPTION) OffsetRect_k( &sizingRect, dx, dy );
                if (ON_LEFT_BORDER(hittest)) sizingRect.left += dx;
                else if (ON_RIGHT_BORDER(hittest)) sizingRect.right += dx;
                if (ON_TOP_BORDER(hittest)) sizingRect.top += dy;
                else if (ON_BOTTOM_BORDER(hittest)) sizingRect.bottom += dy;
                capturePoint = pt;

                /* determine the hit location */
                if (syscommand == SC_SIZE)
                {
                    WPARAM wpSizingHit = 0;

                    if (hittest >= HTLEFT && hittest <= HTBOTTOMRIGHT)
                        wpSizingHit = WMSZ_LEFT + (hittest - HTLEFT);
                    SendMessageW_k( hwnd, WM_SIZING, wpSizingHit, (LPARAM)&sizingRect );
                }
                else
                    SendMessageW_k( hwnd, WM_MOVING, 0, (LPARAM)&sizingRect );

                if (!iconic)
                {
                    if(!DragFullWindows)
                        draw_moving_frame( parent, hdc, &sizingRect, thickframe );
                    else
                    {
                        RECT rect = sizingRect;
                        MapWindowPoints_k( 0, parent, (POINT *)&rect, 2 );
                        SetWindowPos_k( hwnd, 0, rect.left, rect.top,
                                      rect.right - rect.left, rect.bottom - rect.top,
                                      ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
                    }
                }
            }
        }
    }

    if( iconic )
    {
        if( moved ) /* restore cursors, show icon title later on */
        {
            ShowCursor_k( FALSE );
            SetCursor_k( hOldCursor );
        }
    }
    else if (moved && !DragFullWindows)
    {
        draw_moving_frame( parent, hdc, &sizingRect, thickframe );
    }

    set_capture_window( 0, GUI_INMOVESIZE, NULL );
    ReleaseDC_k( parent, hdc );
    if (parent) MapWindowPoints_k( 0, parent, (POINT *)&sizingRect, 2 );

    if (HOOK_CallHooks( WH_CBT, HCBT_MOVESIZE, (WPARAM)hwnd, (LPARAM)&sizingRect, TRUE ))
        moved = FALSE;

    SendMessageW_k( hwnd, WM_EXITSIZEMOVE, 0, 0 );
    SendMessageW_k( hwnd, WM_SETVISIBLE, !IsIconic_k(hwnd), 0L);

    /* window moved or resized */
    if (moved)
    {
        /* if the moving/resizing isn't canceled call SetWindowPos
         * with the new position or the new size of the window
         */
        if (!((msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE)) )
        {
            /* NOTE: SWP_NOACTIVATE prevents document window activation in Word 6 */
            if(!DragFullWindows || iconic)
                SetWindowPos_k( hwnd, 0, sizingRect.left, sizingRect.top,
                              sizingRect.right - sizingRect.left,
                              sizingRect.bottom - sizingRect.top,
                              ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
        }
        else
        { /* restore previous size/position */
            if(DragFullWindows)
                SetWindowPos_k( hwnd, 0, origRect.left, origRect.top,
                              origRect.right - origRect.left,
                              origRect.bottom - origRect.top,
                              ( hittest == HTCAPTION ) ? SWP_NOSIZE : 0 );
        }
    }

    if (IsIconic_k(hwnd))
    {
        /* Single click brings up the system menu when iconized */

        if( !moved )
        {
            if(style & WS_SYSMENU )
                SendMessageW_k( hwnd, WM_SYSCOMMAND,
                              SC_MOUSEMENU + HTSYSMENU, MAKELONG(pt.x,pt.y));
        }
        else WINPOS_ShowIconTitle( hwnd, TRUE );
    }
}
