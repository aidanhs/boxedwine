/*
 * Window painting functions
 *
 * Copyright 1993, 1994, 1995, 2001, 2004, 2005, 2008 Alexandre Julliard
 * Copyright 1996, 1997, 1999 Alex Korobka
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

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "../include/winbase.h"
#include "../include/ntstatus.h"
#include "../include/list.h"
#include "win.h"
#include "user_private.h"
#include "controls.h"
#include "../include/gdi_driver.h"
#include "kthread.h"

struct dce
{
    struct list entry;         /* entry in global DCE list */
    HDC         hdc;
    HWND        hwnd;
    HRGN        clip_rgn;
    DWORD       flags;
    LONG        count;         /* usage count; 0 or 1 for cache DCEs, always 1 for window DCEs,
                                  always >= 1 for class DCEs */
};

static struct list dce_list = LIST_INIT(dce_list);

static BOOL CALLBACK dc_hook( HDC hDC, WORD code, DWORD_PTR data, LPARAM lParam );

static const WCHAR displayW[] = { 'D','I','S','P','L','A','Y',0 };


/***********************************************************************
 *           dump_rdw_flags
 */
static void dump_rdw_flags(UINT flags)
{
    TRACE("flags:");
    if (flags & RDW_INVALIDATE) TRACE(" RDW_INVALIDATE");
    if (flags & RDW_INTERNALPAINT) TRACE(" RDW_INTERNALPAINT");
    if (flags & RDW_ERASE) TRACE(" RDW_ERASE");
    if (flags & RDW_VALIDATE) TRACE(" RDW_VALIDATE");
    if (flags & RDW_NOINTERNALPAINT) TRACE(" RDW_NOINTERNALPAINT");
    if (flags & RDW_NOERASE) TRACE(" RDW_NOERASE");
    if (flags & RDW_NOCHILDREN) TRACE(" RDW_NOCHILDREN");
    if (flags & RDW_ALLCHILDREN) TRACE(" RDW_ALLCHILDREN");
    if (flags & RDW_UPDATENOW) TRACE(" RDW_UPDATENOW");
    if (flags & RDW_ERASENOW) TRACE(" RDW_ERASENOW");
    if (flags & RDW_FRAME) TRACE(" RDW_FRAME");
    if (flags & RDW_NOFRAME) TRACE(" RDW_NOFRAME");

#define RDW_FLAGS \
    (RDW_INVALIDATE | \
    RDW_INTERNALPAINT | \
    RDW_ERASE | \
    RDW_VALIDATE | \
    RDW_NOINTERNALPAINT | \
    RDW_NOERASE | \
    RDW_NOCHILDREN | \
    RDW_ALLCHILDREN | \
    RDW_UPDATENOW | \
    RDW_ERASENOW | \
    RDW_FRAME | \
    RDW_NOFRAME)

    if (flags & ~RDW_FLAGS) TRACE(" %04x", flags & ~RDW_FLAGS);
    TRACE("\n");
#undef RDW_FLAGS
}

/* get the top-level window to clip against for a given window */
static INLINE WND *get_top_clipping_window( WND *win )
{
    while (!(win->paint_flags & PAINT_HAS_SURFACE) && win->parent && !is_desktop_window(win->parent))
        win = WIN_GetPtr(win->parent);
    return win;
}

/* set the region to the client rect clipped by the window rect, in parent-relative coordinates */
static void set_region_client_rect( HRGN region, WND* win )
{
    RECT rect;

    IntersectRect_k( &rect, &win->rectWindow, &win->rectClient );
    SetRectRgn_k(region, rect.left, rect.top, rect.right, rect.bottom);
}

/* intersect the window region with the specified region, relative to the window parent */
static HRGN intersect_window_region( HRGN region, WND* win )
{
    /* make region relative to window rect */
    OffsetRgn_k( region, -win->rectWindow.left, -win->rectWindow.top );
    if (CombineRgn_k(region, region, win->win_region, RGN_AND)==NULLREGION) {
        return NULL;
    }
    /* make region relative to parent again */
    OffsetRgn_k( region, win->rectWindow.left, win->rectWindow.top );
    return region;
}

/* check if window and all its ancestors are visible */
static int is_visible(WND *win )
{
    while (win)
    {
        if (!(win->dwStyle & WS_VISIBLE)) return 0;
        win = WIN_GetPtr(win->parent);
        /* if parent is minimized children are not visible */
        if (win && (win->dwStyle & WS_MINIMIZE)) return 0;
    }
    return 1;
}

/* clip all children of a given window out of the visible region */
static HRGN clip_children( WND* parent, WND* last, HRGN region, int offset_x, int offset_y )
{
    WND* ptr;
    HRGN tmp = CreateRectRgn_k(0, 0, 0, 0);
    PblIterator* it = pblListIterator(parent->children);

    while (it && pblIteratorHasNext(it)) {    
        PblMapEntry* entry = pblIteratorNext(it);
        ptr = pblMapEntryValue(entry);
    
        if (ptr == last) break;
        if (!(ptr->dwStyle & WS_VISIBLE)) continue;
        if (ptr->dwExStyle & WS_EX_TRANSPARENT) continue;
        SetRectRgn_k(tmp, ptr->visible_rect.left, ptr->visible_rect.top, ptr->visible_rect.right, ptr->visible_rect.bottom);
        if (ptr->win_region && !intersect_window_region( tmp, ptr ))
        {
            region = NULL;
            break;
        }
        OffsetRgn_k( tmp, offset_x, offset_y );
        if (CombineRgn_k(region, region, tmp, RGN_DIFF)==NULLREGION) break;
    }
    pblIteratorFree(it);
    DeleteObject_k( tmp );
    return region;
}

/* compute the visible region of a window, in window coordinates */
HRGN get_visible_region_server( WND *win, unsigned int flags )
{
    HRGN tmp = NULL, region;
    int offset_x, offset_y;

    region = CreateRectRgn_k(0, 0, 0, 0);

    /* first check if all ancestors are visible */

    if (!is_visible( win )) return region;  /* empty region */

    /* create a region relative to the window itself */

    if ((flags & DCX_PARENTCLIP) && win->parent && !is_desktop_window(win->parent))
    {
        WND* parent = WIN_GetPtr(win->parent);
        set_region_client_rect( region, WIN_GetPtr(win->parent) );
        OffsetRgn_k( region, -parent->rectClient.left, -parent->rectClient.top );
    }
    else if (flags & DCX_WINDOW)
    {
        SetRectRgn_k(region, win->visible_rect.left, win->visible_rect.top, win->visible_rect.right, win->visible_rect.bottom);
        if (win->win_region && !intersect_window_region( region, win )) goto error;
    }
    else
    {
        set_region_client_rect( region, win );
        if (win->win_region && !intersect_window_region( region, win )) goto error;
    }

    /* clip children */

    if (flags & DCX_CLIPCHILDREN)
    {
        if (is_desktop_window(win->handle)) offset_x = offset_y = 0;
        else
        {
            offset_x = win->rectClient.left;
            offset_y = win->rectClient.top;
        }
        if (!clip_children( win, NULL, region, offset_x, offset_y )) goto error;
    }

    /* clip siblings of ancestors */

    if (is_desktop_window(win->handle)) offset_x = offset_y = 0;
    else
    {
        offset_x = win->rectWindow.left;
        offset_y = win->rectWindow.top;
    }
    tmp = CreateRectRgn_k(0, 0, 0, 0);

    while (win->parent)
    {
        /* we don't clip out top-level siblings as that's up to the native windowing system */
        if ((win->dwStyle & WS_CLIPSIBLINGS) && !is_desktop_window( win->parent ))
        {
            if (!clip_children(WIN_GetPtr(win->parent), win, region, 0, 0 )) goto error;
            if (OffsetRgn_k( region, 0, 0 )==NULLREGION) break;
        }
        /* clip to parent client area */
        win = WIN_GetPtr(win->parent);
        if (!is_desktop_window(win->handle))
        {
            offset_x += win->rectClient.left;
            offset_y += win->rectClient.top;
            OffsetRgn_k( region, win->rectClient.left, win->rectClient.top );
        }
        set_region_client_rect( tmp, win );
        if (win->win_region && !intersect_window_region( tmp, win )) goto error;
        if (CombineRgn_k(region, region, tmp, RGN_AND)==NULLREGION) break;
    }
    DeleteObject_k( tmp );
    OffsetRgn_k( region, -offset_x, -offset_y );  /* make it relative to target window */
    return region;

error:
    if (tmp) DeleteObject_k( tmp );
    DeleteObject_k( region );
    return NULL;
}

/* convert coordinates from client to screen coords */
static INLINE void client_to_screen_rect( WND *win, RECT* rect )
{
    for ( ; win && !is_desktop_window(win->handle); win = WIN_GetPtr(win->parent))
    {
        rect->left   += win->rectClient.left;
        rect->right  += win->rectClient.left;
        rect->top    += win->rectClient.top;
        rect->bottom += win->rectClient.top;
    }
}

/* convert coordinates from client to screen coords */
static INLINE void client_to_screen( WND *win, int *x, int *y )
{
    for ( ; win && !is_desktop_window(win->handle); win = WIN_GetPtr(win->parent))
    {
        *x += win->rectClient.left;
        *y += win->rectClient.top;
    }
}

/* map the region from window to screen coordinates */
static INLINE void map_win_region_to_screen( WND *win, HRGN region )
{
    if (!is_desktop_window(win->handle))
    {
        int x = win->rectWindow.left;
        int y = win->rectWindow.top;
        client_to_screen(WIN_GetPtr(win->parent), &x, &y );
        OffsetRgn_k( region, x, y );
    }
}

HRGN get_visible_region_server( WND *win, unsigned int flags );
HRGN get_visible_region(HWND hwnd, DWORD flags, HWND* topWin, RECT* topRect, RECT* winRect, DWORD* paintFlags)
{
    HRGN region;
    WND *top, *win = WIN_GetPtr( hwnd );

    if (!win) return 0;

    top = get_top_clipping_window( win );
    if ((region = get_visible_region_server( win, flags )))
    {
        map_win_region_to_screen( win, region );
    }
    *topWin  = top->handle;
    *topRect = top->visible_rect;

    if (!is_desktop_window(win->handle))
    {
        *winRect = (flags & DCX_WINDOW) ? win->rectWindow : win->rectClient;
        client_to_screen_rect( WIN_GetPtr(top->parent), topRect );
        client_to_screen_rect( WIN_GetPtr(win->parent), winRect );
    }
    else
    {
        winRect->left   = 0;
        winRect->top    = 0;
        winRect->right  = win->rectClient.right - win->rectClient.left;
        winRect->bottom = win->rectClient.bottom - win->rectClient.top;
    }
    *paintFlags = win->paint_flags & PAINT_CLIENT_FLAGS;
    return region;
}

/***********************************************************************
 *		update_visible_region
 *
 * Set the visible region and X11 drawable for the DC associated to
 * a given window.
 */
static void update_visible_region( struct dce *dce )
{
    struct window_surface *surface = NULL;
    HRGN vis_rgn = 0;
    HWND top_win = 0;
    DWORD flags = dce->flags;
    DWORD paint_flags = 0;
    size_t size = 256;
    RECT win_rect, top_rect;
    WND *win;

    /* don't clip siblings if using parent clip region */
    if (flags & DCX_PARENTCLIP) flags &= ~DCX_CLIPSIBLINGS;

    vis_rgn = get_visible_region(dce->hwnd, flags, &top_win, &top_rect, &win_rect, &paint_flags);


    if (!vis_rgn) return;

    USER_Driver->pGetDC( dce->hdc, dce->hwnd, top_win, &win_rect, &top_rect, flags );

    if (dce->clip_rgn) CombineRgn_k( vis_rgn, vis_rgn, dce->clip_rgn,
                                   (flags & DCX_INTERSECTRGN) ? RGN_AND : RGN_DIFF );

    /* don't use a surface to paint the client area of OpenGL windows */
    if (!(paint_flags & SET_WINPOS_PIXEL_FORMAT) || (flags & DCX_WINDOW))
    {
        win = WIN_GetPtr( top_win );
        if (win && win != WND_DESKTOP)
        {
            surface = win->surface;
            if (surface) window_surface_add_ref( surface );
        }
    }

    if (!surface) top_rect = get_virtual_screen_rect();
    __wine_set_visible_region( dce->hdc, vis_rgn, &win_rect, &top_rect, surface );
    if (surface) window_surface_release( surface );
}

/***********************************************************************
 *		release_dce
 */
static void release_dce( struct dce *dce )
{
    if (!dce->hwnd) return;  /* already released */

    __wine_set_visible_region( dce->hdc, 0, &dummy_surface.rect, &dummy_surface.rect, &dummy_surface );
    USER_Driver->pReleaseDC( dce->hwnd, dce->hdc );

    if (dce->clip_rgn) DeleteObject_k( dce->clip_rgn );
    dce->clip_rgn = 0;
    dce->hwnd     = 0;
    dce->flags   &= DCX_CACHE;
}


/***********************************************************************
 *   delete_clip_rgn
 */
static void delete_clip_rgn( struct dce *dce )
{
    if (!dce->clip_rgn) return;  /* nothing to do */

    dce->flags &= ~(DCX_EXCLUDERGN | DCX_INTERSECTRGN);
    DeleteObject_k( dce->clip_rgn );
    dce->clip_rgn = 0;

    /* make it dirty so that the vis rgn gets recomputed next time */
    SetHookFlags_k( dce->hdc, DCHF_INVALIDATEVISRGN );
}


/***********************************************************************
 *           alloc_dce
 *
 * Allocate a new DCE.
 */
static struct dce *alloc_dce(void)
{
    struct dce *dce;

    if (!(dce = HeapAlloc_k( GetProcessHeap_k(), 0, sizeof(*dce) ))) return NULL;
    if (!(dce->hdc = CreateDCW_k( displayW, NULL, NULL, NULL )))
    {
        HeapFree_k( GetProcessHeap_k(), 0, dce );
        return 0;
    }
    dce->hwnd      = 0;
    dce->clip_rgn  = 0;
    dce->flags     = 0;
    dce->count     = 1;

    /* store DCE handle in DC hook data field */
    SetDCHook_k( dce->hdc, dc_hook, (DWORD_PTR)dce );
    SetHookFlags_k( dce->hdc, DCHF_INVALIDATEVISRGN );
    return dce;
}


/***********************************************************************
 *		get_window_dce
 */
static struct dce *get_window_dce( HWND hwnd )
{
    struct dce *dce;
    WND *win = WIN_GetPtr( hwnd );

    if (!win || win == WND_DESKTOP) return NULL;

    dce = win->dce;
    if (!dce && (dce = get_class_dce( win->class )))
    {
        win->dce = dce;
        dce->count++;
    }

    if (!dce)  /* try to allocate one */
    {
        struct dce *dce_to_free = NULL;
        LONG class_style = GetClassLongW_k( hwnd, GCL_STYLE );

        if (class_style & CS_CLASSDC)
        {
            if (!(dce = alloc_dce())) return NULL;

            win = WIN_GetPtr( hwnd );
            if (win && win != WND_DESKTOP)
            {
                if (win->dce)  /* another thread beat us to it */
                {
                    dce_to_free = dce;
                    dce = win->dce;
                }
                else if ((win->dce = set_class_dce( win->class, dce )) != dce)
                {
                    dce_to_free = dce;
                    dce = win->dce;
                    dce->count++;
                }
                else
                {
                    dce->count++;
                    list_add_tail( &dce_list, &dce->entry );
                }
            }
            else dce_to_free = dce;
        }
        else if (class_style & CS_OWNDC)
        {
            if (!(dce = alloc_dce())) return NULL;

            win = WIN_GetPtr( hwnd );
            if (win && win != WND_DESKTOP)
            {
                if (win->dwStyle & WS_CLIPCHILDREN) dce->flags |= DCX_CLIPCHILDREN;
                if (win->dwStyle & WS_CLIPSIBLINGS) dce->flags |= DCX_CLIPSIBLINGS;
                if (win->dce)  /* another thread beat us to it */
                {
                    dce_to_free = dce;
                    dce = win->dce;
                }
                else
                {
                    win->dce = dce;
                    dce->hwnd = hwnd;
                    dce->count++;
                    list_add_tail( &dce_list, &dce->entry );
                }
            }
            else dce_to_free = dce;
        }

        if (dce_to_free)
        {
            SetDCHook_k( dce_to_free->hdc, NULL, 0 );
            DeleteDC_k( dce_to_free->hdc );
            HeapFree_k( GetProcessHeap_k(), 0, dce_to_free );
            if (dce_to_free == dce)
                dce = NULL;
        }
    }
    return dce;
}


/***********************************************************************
 *           free_dce
 *
 * Free a class or window DCE.
 */
void free_dce( struct dce *dce, HWND hwnd )
{
    USER_Lock();

    if (dce)
    {
        if (!--dce->count)
        {
            /* turn it into a cache entry */
            SetHookFlags_k( dce->hdc, DCHF_RESETDC );
            release_dce( dce );
            dce->flags |= DCX_CACHE;
        }
        else if (dce->hwnd == hwnd)
        {
            release_dce( dce );
        }
    }

    /* now check for cache DCEs */

    if (hwnd)
    {
        LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
        {
            if (dce->hwnd != hwnd) continue;
            if (!(dce->flags & DCX_CACHE)) continue;

            if (dce->count) WARN( "GetDC() without ReleaseDC() for window %p\n", hwnd );
            dce->count = 0;
            release_dce( dce );
        }
    }

    USER_Unlock();
}


/***********************************************************************
 *           make_dc_dirty
 *
 * Mark the associated DC as dirty to force a refresh of the visible region
 */
static void make_dc_dirty( struct dce *dce )
{
    if (!dce->count)
    {
        /* Don't bother with visible regions of unused DCEs */
        TRACE("\tpurged %p dce [%p]\n", dce, dce->hwnd);
        release_dce( dce );
    }
    else
    {
        /* Set dirty bits in the hDC and DCE structs */
        TRACE("\tfixed up %p dce [%p]\n", dce, dce->hwnd);
        SetHookFlags_k( dce->hdc, DCHF_INVALIDATEVISRGN );
    }
}


/***********************************************************************
 *   invalidate_dce
 *
 * It is called from SetWindowPos() - we have to
 * mark as dirty all busy DCEs for windows that have pWnd->parent as
 * an ancestor and whose client rect intersects with specified update
 * rectangle. In addition, pWnd->parent DCEs may need to be updated if
 * DCX_CLIPCHILDREN flag is set.
 */
void invalidate_dce( WND *win, const RECT *extra_rect )
{
    RECT window_rect;
    struct dce *dce;

    if (!win->parent) return;

    GetWindowRect_k( win->obj.handle, &window_rect );

    //TRACE("%p parent %p %s (%s)\n", win->obj.handle, win->parent, wine_dbgstr_rect(&window_rect), wine_dbgstr_rect(extra_rect) );

    /* walk all DCEs and fixup non-empty entries */

    LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
    {
        TRACE( "%p: hwnd %p dcx %08x %s %s\n", dce, dce->hwnd, dce->flags,
               (dce->flags & DCX_CACHE) ? "Cache" : "Owned", dce->count ? "InUse" : "" );

        if (!dce->hwnd) continue;
        if ((dce->hwnd == win->parent) && !(dce->flags & DCX_CLIPCHILDREN))
            continue;  /* child window positions don't bother us */

        /* if DCE window is a child of hwnd, it has to be invalidated */
        if (dce->hwnd == win->obj.handle || IsChild_k( win->obj.handle, dce->hwnd ))
        {
            make_dc_dirty( dce );
            continue;
        }

        /* otherwise check if the window rectangle intersects this DCE window */
        if (win->parent == dce->hwnd || IsChild_k( win->parent, dce->hwnd ))
        {
            RECT dce_rect, tmp;
            GetWindowRect_k( dce->hwnd, &dce_rect );
            if (IntersectRect_k( &tmp, &dce_rect, &window_rect ) ||
                (extra_rect && IntersectRect_k( &tmp, &dce_rect, extra_rect )))
                make_dc_dirty( dce );
        }
    }
}

/***********************************************************************
 *		release_dc
 *
 * Implementation of ReleaseDC.
 */
static INT release_dc( HWND hwnd, HDC hdc, BOOL end_paint )
{
    struct dce *dce;
    BOOL ret = FALSE;

    TRACE("%p %p\n", hwnd, hdc );

    USER_Lock();
    dce = (struct dce *)GetDCHook_k( hdc, NULL );
    if (dce && dce->count)
    {
        if (!(dce->flags & DCX_NORESETATTRS)) SetHookFlags_k( dce->hdc, DCHF_RESETDC );
        if (end_paint || (dce->flags & DCX_CACHE)) delete_clip_rgn( dce );
        if (dce->flags & DCX_CACHE) dce->count = 0;
        ret = TRUE;
    }
    USER_Unlock();
    return ret;
}


/***********************************************************************
 *		dc_hook
 *
 * See "Undoc. Windows" for hints (DC, SetDCHook, SetHookFlags)..
 */
static BOOL CALLBACK dc_hook( HDC hDC, WORD code, DWORD_PTR data, LPARAM lParam )
{
    BOOL retv = TRUE;
    struct dce *dce = (struct dce *)data;

    TRACE("hDC = %p, %u\n", hDC, code);

    if (!dce) return 0;
    assert( dce->hdc == hDC );

    switch( code )
    {
    case DCHC_INVALIDVISRGN:
        /* GDI code calls this when it detects that the
         * DC is dirty (usually after SetHookFlags()). This
         * means that we have to recompute the visible region.
         */
        if (dce->count) update_visible_region( dce );
        else /* non-fatal but shouldn't happen */
            WARN("DC is not in use!\n");
        break;
    case DCHC_DELETEDC:
        /*
         * Windows will not let you delete a DC that is busy
         * (between GetDC and ReleaseDC)
         */
        USER_Lock();
        if (dce->count > 1)
        {
            WARN("Application trying to delete a busy DC %p\n", dce->hdc);
            retv = FALSE;
        }
        else
        {
            list_remove( &dce->entry );
            if (dce->clip_rgn) DeleteObject_k( dce->clip_rgn );
            HeapFree_k( GetProcessHeap_k(), 0, dce );
        }
        USER_Unlock();
        break;
    }
    return retv;
}

void inc_queue_paint_count( struct KThread* thread, int incr );

/* increment (or decrement) the window paint count */
static INLINE void inc_window_paint_count( WND* win, int incr )
{
    if (win->tid) inc_queue_paint_count( getThread(win->tid), incr );
}

/* set a region as new update region for the window */
static void set_update_region( WND *win, HRGN region )
{
    if (region)
    {
        if (!win->update_region) inc_window_paint_count( win, 1 );
        else DeleteObject_k( win->update_region );
        win->update_region = region;
    }
    else
    {
        if (win->update_region)
        {
            inc_window_paint_count( win, -1 );
            DeleteObject_k( win->update_region );
        }
        win->paint_flags &= ~(PAINT_ERASE | PAINT_DELAYED_ERASE | PAINT_NONCLIENT);
        win->update_region = NULL;
    }
}

/* validate the non client area of a window */
static void validate_non_client( WND *win )
{
    HRGN tmp;
    RECT rect;

    if (!win->update_region) return;  /* nothing to do */

    /* get client rect in window coords */
    rect.left   = win->rectClient.left - win->rectWindow.left;
    rect.top    = win->rectClient.top - win->rectWindow.top;
    rect.right  = win->rectClient.right - win->rectWindow.left;
    rect.bottom = win->rectClient.bottom - win->rectWindow.top;

    tmp = CreateRectRgnIndirect_k(&rect);
    if (CombineRgn_k(tmp, win->update_region, tmp, RGN_AND)==NULLREGION) {
        DeleteObject_k(tmp);
    } else {
        CombineRgn_k(tmp, win->update_region, tmp, RGN_AND);
        set_update_region( win, tmp );
    }
    win->paint_flags &= ~PAINT_NONCLIENT;
}

/* validate a window completely so that we don't get any further paint messages for it */
static void validate_whole_window( WND *win )
{
    set_update_region( win, NULL );

    if (win->paint_flags & PAINT_INTERNAL)
    {
        win->paint_flags &= ~PAINT_INTERNAL;
        inc_window_paint_count( win, -1 );
    }
}

/* check if any area of a window needs repainting */
static INLINE int win_needs_repaint( WND *win )
{
    return win->update_region || (win->paint_flags & PAINT_INTERNAL);
}

/* retrieve the update flags for a window depending on the state of the update region */
static unsigned int get_update_flags_server( WND *win, unsigned int flags )
{
    unsigned int ret = 0;

    if (flags & UPDATE_NONCLIENT)
    {
        if ((win->paint_flags & PAINT_NONCLIENT) && win->update_region) ret |= UPDATE_NONCLIENT;
    }
    if (flags & UPDATE_ERASE)
    {
        if ((win->paint_flags & PAINT_ERASE) && win->update_region) ret |= UPDATE_ERASE;
    }
    if (flags & UPDATE_PAINT)
    {
        if (win->update_region)
        {
            if (win->paint_flags & PAINT_DELAYED_ERASE) ret |= UPDATE_DELAYED_ERASE;
            ret |= UPDATE_PAINT;
        }
    }
    if (flags & UPDATE_INTERNALPAINT)
    {
        if (win->paint_flags & PAINT_INTERNAL)
        {
            ret |= UPDATE_INTERNALPAINT;
            if (win->paint_flags & PAINT_DELAYED_ERASE) ret |= UPDATE_DELAYED_ERASE;
        }
    }
    return ret;
}

/* iterate through the children of the given window until we find one with some update flags */
static unsigned int get_child_update_flags( WND *win, WND *from_child,
                                            unsigned int flags, WND **child )
{
    WND *ptr;
    unsigned int ret = 0;
    PblIterator* it = pblListIterator(win->children);

    /* first make sure we want to iterate children at all */

    if (win->dwStyle & WS_MINIMIZE) return 0;

    /* note: the WS_CLIPCHILDREN test is the opposite of the invalidation case,
     * here we only want to repaint children of windows that clip them, others
     * need to wait for WM_PAINT to be done in the parent first.
     */
    if (!(flags & UPDATE_ALLCHILDREN) && !(win->dwStyle & WS_CLIPCHILDREN)) return 0;

    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        ptr = pblMapEntryValue(entry);

        if (from_child)  /* skip all children until from_child is found */
        {
            if (ptr == from_child) from_child = NULL;
            continue;
        }
        if (!(ptr->dwStyle & WS_VISIBLE)) continue;
        if ((ret = get_update_flags_server( ptr, flags )) != 0)
        {
            *child = ptr;
            break;
        }
        if ((ret = get_child_update_flags( ptr, NULL, flags, child ))) break;
    }
    pblIteratorFree(it);
    return ret;
}

/* iterate through children and siblings of the given window until we find one with some update flags */
static unsigned int get_window_update_flags( WND *win, WND *from_child, unsigned int flags, WND **child )
{
    unsigned int ret;
    WND *ptr, *from_sibling = NULL;

    /* if some parent is not visible start from the next sibling */

    if (!is_visible( win )) return 0;
    for (ptr = from_child; ptr; ptr = WIN_GetPtr(ptr->parent))
    {
        if (!(ptr->dwStyle & WS_VISIBLE) || (ptr->dwStyle & WS_MINIMIZE)) from_sibling = ptr;
        if (ptr == win) break;
    }

    /* non-client painting must be delayed if one of the parents is going to
     * be repainted and doesn't clip children */

    if ((flags & UPDATE_NONCLIENT) && !(flags & (UPDATE_PAINT|UPDATE_INTERNALPAINT)))
    {
        for (ptr = WIN_GetPtr(win->parent); ptr; ptr = WIN_GetPtr(ptr->parent))
        {
            if (!(ptr->dwStyle & WS_CLIPCHILDREN) && win_needs_repaint( ptr ))
                return 0;
        }
        if (from_child && !(flags & UPDATE_ALLCHILDREN))
        {
            for (ptr = from_sibling ? from_sibling : from_child; ptr; ptr = WIN_GetPtr(ptr->parent))
            {
                if (!(ptr->dwStyle & WS_CLIPCHILDREN) && win_needs_repaint( ptr )) from_sibling = ptr;
                if (ptr == win) break;
            }
        }
    }


    /* check window itself (only if not restarting from a child) */

    if (!from_child)
    {
        if ((ret = get_update_flags_server( win, flags )))
        {
            *child = win;
            return ret;
        }
        from_child = win;
    }

    /* now check children */

    if (flags & UPDATE_NOCHILDREN) return 0;
    if (!from_sibling)
    {
        if ((ret = get_child_update_flags( from_child, NULL, flags, child ))) return ret;
        from_sibling = from_child;
    }

    /* then check siblings and parent siblings */

    while (from_sibling->parent && from_sibling != win)
    {
        if ((ret = get_child_update_flags( WIN_GetPtr(from_sibling->parent), from_sibling, flags, child )))
            return ret;
        from_sibling = WIN_GetPtr(from_sibling->parent);
    }
    return 0;
}

/* validate the update region of a window on all parents; helper for get_update_region */
static void validate_parents( WND *child )
{
    int offset_x = 0, offset_y = 0;
    WND *win = child;
    HRGN tmp = NULL;

    if (!child->update_region) return;

    while (win->parent)
    {
        /* map to parent client coords */
        offset_x += win->rectWindow.left;
        offset_y += win->rectWindow.top;

        win = WIN_GetPtr(win->parent);

        /* and now map to window coords */
        offset_x += win->rectClient.left - win->rectClient.left;
        offset_y += win->rectClient.top - win->rectClient.top;

        if (win->update_region && !(win->dwStyle & WS_CLIPCHILDREN))
        {
            if (!tmp)
                tmp = CreateRectRgn_k(0, 0, 0, 0);
            OffsetRgn_k( child->update_region, offset_x, offset_y );
            CombineRgn_k(tmp, win->update_region, child->update_region, RGN_DIFF);
            set_update_region( win, tmp );
            /* restore child coords */
            OffsetRgn_k( child->update_region, -offset_x, -offset_y );
        }
    }
    if (tmp) DeleteObject_k( tmp );
}

/* get a window update region */
HRGN get_update_region(HWND hwnd, HWND *child, UINT *flags)
{
    HRGN result = 0;
    WND *from_child = NULL;
    WND *win = WIN_GetPtr(hwnd);
    DWORD resultFlags = 0;

    if (!win) return 0;

    if (*child)
    {
        WND *ptr;

        if (!(from_child = WIN_GetPtr(*child))) return 0;

        /* make sure from_child is a child of win */
        ptr = from_child;
        while (ptr && ptr != win) ptr = WIN_GetPtr(ptr->parent);
        if (!ptr)
        {
            SetLastError_k( STATUS_INVALID_PARAMETER );
            return 0;
        }
    }

    if (*flags & UPDATE_DELAYED_ERASE)  /* this means that the previous call didn't erase */
    {
        if (from_child) from_child->paint_flags |= PAINT_DELAYED_ERASE;
        else win->paint_flags |= PAINT_DELAYED_ERASE;
    }

    resultFlags = get_window_update_flags( win, from_child, *flags, &win );
    *child = win->handle;

    if (*flags & UPDATE_NOREGION) {
        *flags = resultFlags;
        return 0;
    }
    *flags = resultFlags;
    

    if (win->update_region)
    {
        /* convert update region to screen coordinates */
        result = CreateRectRgn_k( 0, 0, 0, 0);
        CombineRgn_k( result, win->update_region, 0, RGN_COPY);

        map_win_region_to_screen( win, result );
    }

    if (*flags & (UPDATE_PAINT|UPDATE_INTERNALPAINT)) /* validate everything */
    {
        validate_parents( win );
        validate_whole_window( win );
    }
    else
    {
        if (*flags & UPDATE_NONCLIENT) validate_non_client( win );
        if (*flags & UPDATE_ERASE)
        {
            win->paint_flags &= ~(PAINT_ERASE | PAINT_DELAYED_ERASE);
            /* desktop window only gets erased, not repainted */
            if (is_desktop_window(win->handle)) validate_whole_window( win );
        }
    }
    return result;
}

/***********************************************************************
 *           get_update_flags
 *
 * Get only the update flags, not the update region.
 */
static BOOL get_update_flags( HWND hwnd, HWND *child, UINT *flags )
{
    *flags |= UPDATE_NOREGION;
    get_update_region(hwnd, child, flags);
    return TRUE;
}

/* determine the window visible rectangle, i.e. window or client rect cropped by parent rects */
/* the returned rectangle is in window coordinates; return 0 if rectangle is empty */
static int get_window_visible_rect( WND* win, RECT* rect, int frame )
{
    int offset_x = 0, offset_y = 0;

    if (!(win->dwStyle & WS_VISIBLE)) return 0;

    *rect = frame ? win->rectWindow : win->rectClient;
    if (!is_desktop_window(win->handle))
    {
        offset_x = win->rectWindow.left;
        offset_y = win->rectWindow.top;
    }

    while (win->parent)
    {
        win = WIN_GetPtr(win->parent);
        if (!(win->dwStyle & WS_VISIBLE) || win->dwStyle & WS_MINIMIZE) return 0;
        if (!is_desktop_window(win->handle))
        {
            offset_x += win->rectClient.left;
            offset_y += win->rectClient.top;
            OffsetRect_k( rect, win->rectClient.left, win->rectClient.top );
        }
        
        if (!IntersectRect_k( rect, rect, &win->rectClient )) return 0;
        if (!IntersectRect_k( rect, rect, &win->rectWindow )) return 0;
    }
    OffsetRect_k( rect, -offset_x, -offset_y );
    return 1;
}

/* return a copy of the specified region cropped to the window client or frame rectangle, */
/* and converted from client to window coordinates. Helper for (in)validate_window. */
static HRGN crop_region_to_win_rect( WND* win, HRGN region, int frame )
{
    RECT rect;
    HRGN tmp;

    if (!get_window_visible_rect( win, &rect, frame )) return NULL;
    tmp = CreateRectRgnIndirect_k(&rect);

    if (region)
    {
        /* map it to client coords */
        OffsetRgn_k( tmp, win->rectWindow.left - win->rectClient.left, win->rectWindow.top - win->rectClient.top );
        
        /* intersect specified region with bounding rect */
        if (CombineRgn_k(tmp, region, tmp, RGN_AND)==NULLREGION) goto done;
        
        /* map it back to window coords */
        OffsetRgn_k( tmp, win->rectClient.left - win->rectWindow.left, win->rectClient.top - win->rectWindow.top );
    }
    return tmp;

done:
    DeleteObject_k( tmp );
    return NULL;
}

/* add a region to the update region; the passed region is freed or reused */
static int add_update_region( WND *win, HRGN region )
{
    if (win->update_region)
        CombineRgn_k(region, win->update_region, region, RGN_AND);
    set_update_region( win, region );
    return 1;
}

/* add/subtract a region (in client coordinates) to the update region of the window */
static void redraw_window_internal(WND* win, HRGN region, int frame, unsigned int flags )
{
    HRGN tmp;
    WND *child;
    PblIterator* it;

    if (flags & RDW_INVALIDATE)
    {
        if (!(tmp = crop_region_to_win_rect( win, region, frame ))) return;

        if (!add_update_region( win, tmp )) return;

        if (flags & RDW_FRAME) win->paint_flags |= PAINT_NONCLIENT;
        if (flags & RDW_ERASE) win->paint_flags |= PAINT_ERASE;
    }
    else if (flags & RDW_VALIDATE)
    {
        if (!region && (flags & RDW_NOFRAME))  /* shortcut: validate everything */
        {
            set_update_region( win, NULL );
        }
        else if (win->update_region)
        {
            if ((tmp = crop_region_to_win_rect( win, region, frame )))
            {
                CombineRgn_k(tmp, win->update_region, tmp, RGN_DIFF);
                set_update_region( win, tmp );
            }
            if (flags & RDW_NOFRAME) validate_non_client( win );
            if (flags & RDW_NOERASE) win->paint_flags &= ~(PAINT_ERASE | PAINT_DELAYED_ERASE);
        }
    }

    if ((flags & RDW_INTERNALPAINT) && !(win->paint_flags & PAINT_INTERNAL))
    {
        win->paint_flags |= PAINT_INTERNAL;
        inc_window_paint_count( win, 1 );
    }
    else if ((flags & RDW_NOINTERNALPAINT) && (win->paint_flags & PAINT_INTERNAL))
    {
        win->paint_flags &= ~PAINT_INTERNAL;
        inc_window_paint_count( win, -1 );
    }

    /* now process children recursively */

    if (flags & RDW_NOCHILDREN) return;
    if (win->dwStyle & WS_MINIMIZE) return;
    if ((win->dwStyle & WS_CLIPCHILDREN) && !(flags & RDW_ALLCHILDREN)) return;

    if (!(tmp = crop_region_to_win_rect( win, region, 0 ))) return;

    /* map to client coordinates */
    OffsetRgn_k( tmp, win->rectWindow.left - win->rectClient.left, win->rectWindow.top - win->rectClient.top );

    if (flags & RDW_INVALIDATE) flags |= RDW_FRAME | RDW_ERASE;

    it = pblListIterator(win->children);

    while (it && pblIteratorHasNext(it)) {    
        PblMapEntry* entry = pblIteratorNext(it);
        child = pblMapEntryValue(entry);
        if (!(child->dwStyle & WS_VISIBLE)) continue;
        if (!RectInRegion_k( tmp, &child->rectWindow )) continue;
        OffsetRgn_k( tmp, -child->rectClient.left, -child->rectClient.top );
        redraw_window_internal( child, tmp, 1, flags );
        OffsetRgn_k( tmp, child->rectClient.left, child->rectClient.top );
    }
    pblIteratorFree(it);
    DeleteObject_k( tmp );
}

void redraw_window(HWND hWnd, DWORD flags, const RECT *rect, HRGN rgn)
{
    WND *win = WIN_GetPtr(hWnd);

    if (!win) return;
    if (!is_visible( win )) return;  /* nothing to do */

    if (flags & (RDW_VALIDATE|RDW_INVALIDATE))
    {
        if (rect || rgn)  /* no data means whole rectangle */
        {
            rgn = CreateRectRgnIndirect_k(rect);
            if (win->dwStyle & WS_EX_LAYOUTRTL) MirrorRgn_k(hWnd, rgn );
        }
    }

    redraw_window_internal( win, rgn, (flags & RDW_INVALIDATE) && (flags & RDW_FRAME), flags );
    if (rect) DeleteObject_k( rgn );
}

/***********************************************************************
 *           redraw_window_rect_or_region
 *
 * Redraw part of a window.
 */
static BOOL redraw_window_rect_or_region( HWND hwnd, UINT flags, const RECT *rect, HRGN rgn)
{
    if (!(flags & (RDW_INVALIDATE|RDW_VALIDATE|RDW_INTERNALPAINT|RDW_NOINTERNALPAINT)))
        return TRUE;  /* nothing to do */
    redraw_window(hwnd, flags, rect, rgn);    
    return TRUE;
}

/***********************************************************************
 *           send_ncpaint
 *
 * Send a WM_NCPAINT message if needed, and return the resulting update region (in screen coords).
 * Helper for erase_now and BeginPaint.
 */
static HRGN send_ncpaint( HWND hwnd, HWND *child, UINT *flags )
{
    HRGN whole_rgn = get_update_region( hwnd, child, flags );
    HRGN client_rgn = 0;

    if (child) hwnd = *child;

    if (hwnd == GetDesktopWindow_k()) return whole_rgn;

    if (whole_rgn)
    {
        RECT client, update;
        INT type;

        /* check if update rgn overlaps with nonclient area */
        type = GetRgnBox_k( whole_rgn, &update );
        WIN_GetRectangles( hwnd, COORDS_SCREEN, 0, &client );

        if ((*flags & UPDATE_NONCLIENT) ||
            update.left < client.left || update.top < client.top ||
            update.right > client.right || update.bottom > client.bottom)
        {
            client_rgn = CreateRectRgnIndirect_k( &client );
            CombineRgn_k( client_rgn, client_rgn, whole_rgn, RGN_AND );

            /* check if update rgn contains complete nonclient area */
            if (type == SIMPLEREGION)
            {
                RECT window;
                GetWindowRect_k( hwnd, &window );
                if (EqualRect_k( &window, &update ))
                {
                    DeleteObject_k( whole_rgn );
                    whole_rgn = (HRGN)1;
                }
            }
        }
        else
        {
            client_rgn = whole_rgn;
            whole_rgn = 0;
        }

        if (whole_rgn) /* NOTE: WM_NCPAINT allows wParam to be 1 */
        {
            if (*flags & UPDATE_NONCLIENT) SendMessageW_k( hwnd, WM_NCPAINT, (WPARAM)whole_rgn, 0 );
            if (whole_rgn > (HRGN)1) DeleteObject_k( whole_rgn );
        }
    }
    return client_rgn;
}


/***********************************************************************
 *           send_erase
 *
 * Send a WM_ERASEBKGND message if needed, and optionally return the DC for painting.
 * If a DC is requested, the region is selected into it. In all cases the region is deleted.
 * Helper for erase_now and BeginPaint.
 */
static BOOL send_erase( HWND hwnd, UINT flags, HRGN client_rgn,
                        RECT *clip_rect, HDC *hdc_ret )
{
    BOOL need_erase = (flags & UPDATE_DELAYED_ERASE) != 0;
    HDC hdc = 0;
    RECT dummy;

    if (!clip_rect) clip_rect = &dummy;
    if (hdc_ret || (flags & UPDATE_ERASE))
    {
        UINT dcx_flags = DCX_INTERSECTRGN | DCX_USESTYLE;
        if (IsIconic_k(hwnd)) dcx_flags |= DCX_WINDOW;

        if ((hdc = GetDCEx_k( hwnd, client_rgn, dcx_flags )))
        {
            INT type = GetClipBox_k( hdc, clip_rect );

            if (flags & UPDATE_ERASE)
            {
                /* don't erase if the clip box is empty */
                if (type != NULLREGION)
                    need_erase = !SendMessageW_k( hwnd, WM_ERASEBKGND, (WPARAM)hdc, 0 );
            }
            if (!hdc_ret) release_dc( hwnd, hdc, TRUE );
        }

        if (hdc_ret) *hdc_ret = hdc;
    }
    if (!hdc) DeleteObject_k( client_rgn );
    return need_erase;
}


/***********************************************************************
 *           erase_now
 *
 * Implementation of RDW_ERASENOW behavior.
 */
void erase_now( HWND hwnd, UINT rdw_flags )
{
    HWND child = 0;
    HRGN hrgn;
    BOOL need_erase = FALSE;

    /* loop while we find a child to repaint */
    for (;;)
    {
        UINT flags = UPDATE_NONCLIENT | UPDATE_ERASE;

        if (rdw_flags & RDW_NOCHILDREN) flags |= UPDATE_NOCHILDREN;
        else if (rdw_flags & RDW_ALLCHILDREN) flags |= UPDATE_ALLCHILDREN;
        if (need_erase) flags |= UPDATE_DELAYED_ERASE;

        if (!(hrgn = send_ncpaint( hwnd, &child, &flags ))) break;
        need_erase = send_erase( child, flags, hrgn, NULL, NULL );

        if (!flags) break;  /* nothing more to do */
        if ((rdw_flags & RDW_NOCHILDREN) && !need_erase) break;
    }
}


/***********************************************************************
 *           move_window_bits
 *
 * Move the window bits when a window is resized or its surface recreated.
 */
void move_window_bits( HWND hwnd, struct window_surface *old_surface,
                       struct window_surface *new_surface,
                       const RECT *visible_rect, const RECT *old_visible_rect,
                       const RECT *client_rect, const RECT *valid_rects )
{
    RECT dst = valid_rects[0];
    RECT src = valid_rects[1];

    if (new_surface != old_surface ||
        src.left - old_visible_rect->left != dst.left - visible_rect->left ||
        src.top - old_visible_rect->top != dst.top - visible_rect->top)
    {
        char buffer[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
        BITMAPINFO *info = (BITMAPINFO *)buffer;
        void *bits;
        UINT flags = UPDATE_NOCHILDREN;
        HRGN rgn = get_update_region( hwnd, NULL, &flags );
        HDC hdc = GetDCEx_k( hwnd, rgn, DCX_CACHE | DCX_EXCLUDERGN );

        OffsetRect_k( &dst, -client_rect->left, -client_rect->top );
        //TRACE( "copying  %s -> %s\n", wine_dbgstr_rect(&src), wine_dbgstr_rect(&dst) );
        bits = old_surface->funcs->get_info( old_surface, info );
        old_surface->funcs->lock( old_surface );
        SetDIBitsToDevice_k( hdc, dst.left, dst.top, dst.right - dst.left, dst.bottom - dst.top,
                           src.left - old_visible_rect->left - old_surface->rect.left,
                           old_surface->rect.bottom - (src.bottom - old_visible_rect->top),
                           0, old_surface->rect.bottom - old_surface->rect.top,
                           bits, info, DIB_RGB_COLORS );
        old_surface->funcs->unlock( old_surface );
        ReleaseDC_k( hwnd, hdc );
    }
}


/***********************************************************************
 *           update_now
 *
 * Implementation of RDW_UPDATENOW behavior.
 */
static void update_now( HWND hwnd, UINT rdw_flags )
{
    HWND child = 0;

    /* desktop window never gets WM_PAINT, only WM_ERASEBKGND */
    if (hwnd == GetDesktopWindow_k()) erase_now( hwnd, rdw_flags | RDW_NOCHILDREN );

    /* loop while we find a child to repaint */
    for (;;)
    {
        UINT flags = UPDATE_PAINT | UPDATE_INTERNALPAINT;

        if (rdw_flags & RDW_NOCHILDREN) flags |= UPDATE_NOCHILDREN;
        else if (rdw_flags & RDW_ALLCHILDREN) flags |= UPDATE_ALLCHILDREN;

        if (!get_update_flags( hwnd, &child, &flags )) break;
        if (!flags) break;  /* nothing more to do */

        SendMessageW_k( child, WM_PAINT, 0, 0 );
        if (rdw_flags & RDW_NOCHILDREN) break;
    }
}


/*************************************************************************
 *             fix_caret
 *
 * Helper for ScrollWindowEx:
 * If the return value is 0, no special caret handling is necessary.
 * Otherwise the return value is the handle of the window that owns the
 * caret. Its caret needs to be hidden during the scroll operation and
 * moved to new_caret_pos if move_caret is TRUE.
 */
static HWND fix_caret(HWND hWnd, const RECT *scroll_rect, INT dx, INT dy,
                     UINT flags, LPBOOL move_caret, LPPOINT new_caret_pos)
{
    GUITHREADINFO info;
    RECT rect, mapped_rcCaret;
    BOOL hide_caret = FALSE;

    info.cbSize = sizeof(info);
    if (!GetGUIThreadInfo_k( GetCurrentThreadId_k(), &info )) return 0;
    if (!info.hwndCaret) return 0;
    
    if (info.hwndCaret == hWnd)
    {
        /* Move the caret if it's (partially) in the source rectangle */
        if (IntersectRect_k(&rect, scroll_rect, &info.rcCaret))
        {
            *move_caret = TRUE;
            hide_caret = TRUE;
            new_caret_pos->x = info.rcCaret.left + dx;
            new_caret_pos->y = info.rcCaret.top + dy;
        }
        else
        {
            *move_caret = FALSE;
            
            /* Hide the caret if it's in the destination rectangle */
            rect = *scroll_rect;
            OffsetRect_k(&rect, dx, dy);
            hide_caret = IntersectRect_k(&rect, &rect, &info.rcCaret);
        }
    }
    else
    {
        if ((flags & SW_SCROLLCHILDREN) && IsChild_k(hWnd, info.hwndCaret))
        {
            *move_caret = FALSE;
            
            /* Hide the caret if it's in the source or in the destination
               rectangle */
            mapped_rcCaret = info.rcCaret;
            MapWindowPoints_k(info.hwndCaret, hWnd, (LPPOINT)&mapped_rcCaret, 2);
            
            if (IntersectRect_k(&rect, scroll_rect, &mapped_rcCaret))
            {
                hide_caret = TRUE;
            }
            else
            {
                rect = *scroll_rect;
                OffsetRect_k(&rect, dx, dy);
                hide_caret = IntersectRect_k(&rect, &rect, &mapped_rcCaret);
            }
        }
        else
            return 0;
    }

    if (hide_caret)
    {    
        HideCaret_k(info.hwndCaret);
        return info.hwndCaret;
    }
    else
        return 0;
}


/***********************************************************************
 *		BeginPaint (USER32.@)
 */
HDC WINAPI BeginPaint_k( HWND hwnd, PAINTSTRUCT *lps )
{
    HRGN hrgn;
    HDC hdc;
    BOOL erase;
    RECT rect;
    UINT flags = UPDATE_NONCLIENT | UPDATE_ERASE | UPDATE_PAINT | UPDATE_INTERNALPAINT | UPDATE_NOCHILDREN;

    HideCaret_k( hwnd );

    if (!(hrgn = send_ncpaint( hwnd, NULL, &flags ))) return 0;

    erase = send_erase( hwnd, flags, hrgn, &rect, &hdc );

    //TRACE("hdc = %p box = (%s), fErase = %d\n", hdc, wine_dbgstr_rect(&rect), erase);

    if (!lps)
    {
        release_dc( hwnd, hdc, TRUE );
        return 0;
    }
    lps->fErase = erase;
    lps->rcPaint = rect;
    lps->hdc = hdc;
    return hdc;
}


/***********************************************************************
 *		EndPaint (USER32.@)
 */
BOOL WINAPI EndPaint_k( HWND hwnd, const PAINTSTRUCT *lps )
{
    ShowCaret_k( hwnd );
    flush_window_surfaces( FALSE );
    if (!lps) return FALSE;
    release_dc( hwnd, lps->hdc, TRUE );
    return TRUE;
}


/***********************************************************************
 *		GetDCEx (USER32.@)
 */
HDC WINAPI GetDCEx_k( HWND hwnd, HRGN hrgnClip, DWORD flags )
{
    const DWORD clip_flags = DCX_PARENTCLIP | DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN | DCX_WINDOW;
    const DWORD user_flags = clip_flags | DCX_NORESETATTRS; /* flags that can be set by user */
    struct dce *dce;
    BOOL bUpdateVisRgn = TRUE;
    HWND parent;
    LONG window_style = GetWindowLongW_k( hwnd, GWL_STYLE );

    if (!hwnd) hwnd = GetDesktopWindow_k();
    else hwnd = WIN_GetFullHandle( hwnd );

    TRACE("hwnd %p, hrgnClip %p, flags %08x\n", hwnd, hrgnClip, flags);

    if (!IsWindow_k(hwnd)) return 0;

    /* fixup flags */

    if (flags & (DCX_WINDOW | DCX_PARENTCLIP)) flags |= DCX_CACHE;

    if (flags & DCX_USESTYLE)
    {
        flags &= ~(DCX_CLIPCHILDREN | DCX_CLIPSIBLINGS | DCX_PARENTCLIP);

        if (window_style & WS_CLIPSIBLINGS) flags |= DCX_CLIPSIBLINGS;

        if (!(flags & DCX_WINDOW))
        {
            if (GetClassLongW_k( hwnd, GCL_STYLE ) & CS_PARENTDC) flags |= DCX_PARENTCLIP;

            if (window_style & WS_CLIPCHILDREN && !(window_style & WS_MINIMIZE))
                flags |= DCX_CLIPCHILDREN;
        }
    }

    if (flags & DCX_WINDOW) flags &= ~DCX_CLIPCHILDREN;

    parent = GetAncestor_k( hwnd, GA_PARENT );
    if (!parent || (parent == GetDesktopWindow_k()))
        flags = (flags & ~DCX_PARENTCLIP) | DCX_CLIPSIBLINGS;

    /* it seems parent clip is ignored when clipping siblings or children */
    if (flags & (DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN)) flags &= ~DCX_PARENTCLIP;

    if( flags & DCX_PARENTCLIP )
    {
        LONG parent_style = GetWindowLongW_k( parent, GWL_STYLE );
        if( (window_style & WS_VISIBLE) && (parent_style & WS_VISIBLE) )
        {
            flags &= ~DCX_CLIPCHILDREN;
            if (parent_style & WS_CLIPSIBLINGS) flags |= DCX_CLIPSIBLINGS;
        }
    }

    /* find a suitable DCE */

    if ((flags & DCX_CACHE) || !(dce = get_window_dce( hwnd )))
    {
        struct dce *dceEmpty = NULL, *dceUnused = NULL;

        /* Strategy: First, we attempt to find a non-empty but unused DCE with
         * compatible flags. Next, we look for an empty entry. If the cache is
         * full we have to purge one of the unused entries.
         */
        USER_Lock();
        LIST_FOR_EACH_ENTRY( dce, &dce_list, struct dce, entry )
        {
            if ((dce->flags & DCX_CACHE) && !dce->count)
            {
                dceUnused = dce;

                if (!dce->hwnd) dceEmpty = dce;
                else if ((dce->hwnd == hwnd) && !((dce->flags ^ flags) & clip_flags))
                {
                    TRACE("\tfound valid %p dce [%p], flags %08x\n",
                          dce, hwnd, dce->flags );
                    bUpdateVisRgn = FALSE;
                    break;
                }
            }
        }

        if (&dce->entry == &dce_list)  /* nothing found */
            dce = dceEmpty ? dceEmpty : dceUnused;

        if (dce) dce->count = 1;

        USER_Unlock();

        /* if there's no dce empty or unused, allocate a new one */
        if (!dce)
        {
            if (!(dce = alloc_dce())) return 0;
            dce->flags = DCX_CACHE;
            USER_Lock();
            list_add_head( &dce_list, &dce->entry );
            USER_Unlock();
        }
    }
    else
    {
        flags |= DCX_NORESETATTRS;
        if (dce->hwnd == hwnd)
        {
            TRACE("\tskipping hVisRgn update\n");
            bUpdateVisRgn = FALSE; /* updated automatically, via DCHook() */
        }
        else
        {
            /* we should free dce->clip_rgn here, but Windows apparently doesn't */
            dce->flags &= ~(DCX_EXCLUDERGN | DCX_INTERSECTRGN);
            dce->clip_rgn = 0;
        }
    }

    if (flags & (DCX_INTERSECTRGN | DCX_EXCLUDERGN))
    {
        /* if the extra clip region has changed, get rid of the old one */
        if (dce->clip_rgn != hrgnClip || ((flags ^ dce->flags) & (DCX_INTERSECTRGN | DCX_EXCLUDERGN)))
            delete_clip_rgn( dce );
        dce->clip_rgn = hrgnClip;
        if (!dce->clip_rgn) dce->clip_rgn = CreateRectRgn_k( 0, 0, 0, 0 );
        dce->flags |= flags & (DCX_INTERSECTRGN | DCX_EXCLUDERGN);
        bUpdateVisRgn = TRUE;
    }

    if (GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_LAYOUTRTL) SetLayout_k( dce->hdc, LAYOUT_RTL );

    dce->hwnd = hwnd;
    dce->flags = (dce->flags & ~user_flags) | (flags & user_flags);

    if (SetHookFlags_k( dce->hdc, DCHF_VALIDATEVISRGN )) bUpdateVisRgn = TRUE;  /* DC was dirty */

    if (bUpdateVisRgn) update_visible_region( dce );

    TRACE("(%p,%p,0x%x): returning %p\n", hwnd, hrgnClip, flags, dce->hdc);
    return dce->hdc;
}


/***********************************************************************
 *		GetDC (USER32.@)
 *
 * Get a device context.
 *
 * RETURNS
 *	Success: Handle to the device context
 *	Failure: NULL.
 */
HDC WINAPI GetDC_k( HWND hwnd )
{
    if (!hwnd) return GetDCEx_k( 0, 0, DCX_CACHE | DCX_WINDOW );
    return GetDCEx_k( hwnd, 0, DCX_USESTYLE );
}


/***********************************************************************
 *		GetWindowDC (USER32.@)
 */
HDC WINAPI GetWindowDC_k( HWND hwnd )
{
    return GetDCEx_k( hwnd, 0, DCX_USESTYLE | DCX_WINDOW );
}


/***********************************************************************
 *		ReleaseDC (USER32.@)
 *
 * Release a device context.
 *
 * RETURNS
 *	Success: Non-zero. Resources used by hdc are released.
 *	Failure: 0.
 */
INT WINAPI ReleaseDC_k( HWND hwnd, HDC hdc )
{
    return release_dc( hwnd, hdc, FALSE );
}


/**********************************************************************
 *		WindowFromDC (USER32.@)
 */
HWND WINAPI WindowFromDC_k( HDC hdc )
{
    struct dce *dce;
    HWND hwnd = 0;

    USER_Lock();
    dce = (struct dce *)GetDCHook_k( hdc, NULL );
    if (dce) hwnd = dce->hwnd;
    USER_Unlock();
    return hwnd;
}


/***********************************************************************
 *		LockWindowUpdate (USER32.@)
 *
 * Enables or disables painting in the chosen window.
 *
 * PARAMS
 *  hwnd [I] handle to a window.
 *
 * RETURNS
 *  If successful, returns nonzero value. Otherwise,
 *  returns 0.
 *
 * NOTES
 *  You can lock only one window at a time.
 */
BOOL WINAPI LockWindowUpdate_k( HWND hwnd )
{
    static HWND lockedWnd;

    FIXME("(%p), partial stub!\n",hwnd);

    USER_Lock();
    if (lockedWnd)
    {
        if (!hwnd)
        {
            /* Unlock lockedWnd */
            /* FIXME: Do something */
        }
        else
        {
            /* Attempted to lock a second window */
            /* Return FALSE and do nothing */
            USER_Unlock();
            return FALSE;
        }
    }
    lockedWnd = hwnd;
    USER_Unlock();
    return TRUE;
}


/***********************************************************************
 *		RedrawWindow (USER32.@)
 */
BOOL WINAPI RedrawWindow_k( HWND hwnd, const RECT *rect, HRGN hrgn, UINT flags )
{
    static const RECT empty;
    BOOL ret;

    if (!hwnd) hwnd = GetDesktopWindow_k();

    /* process pending expose events before painting */
    if (flags & RDW_UPDATENOW) USER_Driver->pMsgWaitForMultipleObjectsEx( 0, NULL, 0, QS_PAINT, 0 );

    if (rect && !hrgn)
    {
        if (IsRectEmpty_k( rect )) rect = &empty;
        ret = redraw_window_rect_or_region( hwnd, flags, rect, 0 );
    }
    else if (!hrgn)
    {
        ret = redraw_window_rect_or_region( hwnd, flags, NULL, 0 );
    }
    else  /* need to build a list of the region rectangles */
    {
        ret = redraw_window_rect_or_region( hwnd, flags, NULL, hrgn );
    }

    if (flags & RDW_UPDATENOW) update_now( hwnd, flags );
    else if (flags & RDW_ERASENOW) erase_now( hwnd, flags );

    return ret;
}


/***********************************************************************
 *		UpdateWindow (USER32.@)
 */
BOOL WINAPI UpdateWindow_k( HWND hwnd )
{
    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }

    return RedrawWindow_k( hwnd, NULL, 0, RDW_UPDATENOW | RDW_ALLCHILDREN );
}


/***********************************************************************
 *		InvalidateRgn (USER32.@)
 */
BOOL WINAPI InvalidateRgn_k( HWND hwnd, HRGN hrgn, BOOL erase )
{
    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }

    return RedrawWindow_k(hwnd, NULL, hrgn, RDW_INVALIDATE | (erase ? RDW_ERASE : 0) );
}


/***********************************************************************
 *		InvalidateRect (USER32.@)
 *
 * MSDN: if hwnd parameter is NULL, InvalidateRect invalidates and redraws
 * all windows and sends WM_ERASEBKGND and WM_NCPAINT.
 */
BOOL WINAPI InvalidateRect_k( HWND hwnd, const RECT *rect, BOOL erase )
{
    UINT flags = RDW_INVALIDATE | (erase ? RDW_ERASE : 0);

    if (!hwnd)
    {
        flags = RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_ERASENOW;
        rect = NULL;
    }

    return RedrawWindow_k( hwnd, rect, 0, flags );
}


/***********************************************************************
 *		ValidateRgn (USER32.@)
 */
BOOL WINAPI ValidateRgn_k( HWND hwnd, HRGN hrgn )
{
    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }

    return RedrawWindow_k( hwnd, NULL, hrgn, RDW_VALIDATE );
}


/***********************************************************************
 *		ValidateRect (USER32.@)
 *
 * MSDN: if hwnd parameter is NULL, ValidateRect invalidates and redraws
 * all windows and sends WM_ERASEBKGND and WM_NCPAINT.
 */
BOOL WINAPI ValidateRect_k( HWND hwnd, const RECT *rect )
{
    UINT flags = RDW_VALIDATE;

    if (!hwnd)
    {
        flags = RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_FRAME | RDW_ERASE | RDW_ERASENOW;
        rect = NULL;
    }

    return RedrawWindow_k( hwnd, rect, 0, flags );
}


/***********************************************************************
 *		GetUpdateRgn (USER32.@)
 */
INT WINAPI GetUpdateRgn_k( HWND hwnd, HRGN hrgn, BOOL erase )
{
    INT retval = ERROR;
    UINT flags = UPDATE_NOCHILDREN;
    HRGN update_rgn;

    if (erase) flags |= UPDATE_NONCLIENT | UPDATE_ERASE;

    if ((update_rgn = send_ncpaint( hwnd, NULL, &flags )))
    {
        retval = CombineRgn_k( hrgn, update_rgn, 0, RGN_COPY );
        if (send_erase( hwnd, flags, update_rgn, NULL, NULL ))
        {
            flags = UPDATE_DELAYED_ERASE;
            get_update_flags( hwnd, NULL, &flags );
        }
        /* map region to client coordinates */
        map_window_region( 0, hwnd, hrgn );
    }
    return retval;
}


/***********************************************************************
 *		GetUpdateRect (USER32.@)
 */
BOOL WINAPI GetUpdateRect_k( HWND hwnd, LPRECT rect, BOOL erase )
{
    UINT flags = UPDATE_NOCHILDREN;
    HRGN update_rgn;
    BOOL need_erase;

    if (erase) flags |= UPDATE_NONCLIENT | UPDATE_ERASE;

    if (!(update_rgn = send_ncpaint( hwnd, NULL, &flags ))) return FALSE;

    if (rect)
    {
        if (GetRgnBox_k( update_rgn, rect ) != NULLREGION)
        {
            HDC hdc = GetDCEx_k( hwnd, 0, DCX_USESTYLE );
            DWORD layout = SetLayout_k( hdc, 0 );  /* MapWindowPoints mirrors already */
            MapWindowPoints_k( 0, hwnd, (LPPOINT)rect, 2 );
            DPtoLP_k( hdc, (LPPOINT)rect, 2 );
            SetLayout_k( hdc, layout );
            ReleaseDC_k( hwnd, hdc );
        }
    }
    need_erase = send_erase( hwnd, flags, update_rgn, NULL, NULL );

    /* check if we still have an update region */
    flags = UPDATE_PAINT | UPDATE_NOCHILDREN;
    if (need_erase) flags |= UPDATE_DELAYED_ERASE;
    return (get_update_flags( hwnd, NULL, &flags ) && (flags & UPDATE_PAINT));
}


/***********************************************************************
 *		ExcludeUpdateRgn (USER32.@)
 */
INT WINAPI ExcludeUpdateRgn_k( HDC hdc, HWND hwnd )
{
    HRGN update_rgn = CreateRectRgn_k( 0, 0, 0, 0 );
    INT ret = GetUpdateRgn_k( hwnd, update_rgn, FALSE );

    if (ret != ERROR)
    {
        POINT pt;

        GetDCOrgEx_k( hdc, &pt );
        MapWindowPoints_k( 0, hwnd, &pt, 1 );
        OffsetRgn_k( update_rgn, -pt.x, -pt.y );
        ret = ExtSelectClipRgn_k( hdc, update_rgn, RGN_DIFF );
        DeleteObject_k( update_rgn );
    }
    return ret;
}


static INT scroll_window( HWND hwnd, INT dx, INT dy, const RECT *rect, const RECT *clipRect,
                          HRGN hrgnUpdate, LPRECT rcUpdate, UINT flags, BOOL is_ex )
{
    INT   retVal = NULLREGION;
    BOOL  bOwnRgn = TRUE;
    BOOL  bUpdate = (rcUpdate || hrgnUpdate || flags & (SW_INVALIDATE | SW_ERASE));
    int rdw_flags;
    HRGN  hrgnTemp;
    HRGN  hrgnWinupd = 0;
    HDC   hDC;
    RECT  rc, cliprc;
    HWND hwndCaret = NULL;
    BOOL moveCaret = FALSE;
    POINT newCaretPos;

    //TRACE( "%p, %d,%d hrgnUpdate=%p rcUpdate = %p %s %04x\n", hwnd, dx, dy, hrgnUpdate, rcUpdate, wine_dbgstr_rect(rect), flags );
    //TRACE( "clipRect = %s\n", wine_dbgstr_rect(clipRect));
    if( flags & ~( SW_SCROLLCHILDREN | SW_INVALIDATE | SW_ERASE))
        FIXME("some flags (%04x) are unhandled\n", flags);

    rdw_flags = (flags & SW_ERASE) && (flags & SW_INVALIDATE) ? RDW_INVALIDATE | RDW_ERASE  : RDW_INVALIDATE ;

    if (!WIN_IsWindowDrawable( hwnd, TRUE )) return ERROR;
    hwnd = WIN_GetFullHandle( hwnd );

    GetClientRect_k(hwnd, &rc);

    if (clipRect) IntersectRect_k(&cliprc,&rc,clipRect);
    else cliprc = rc;

    if (rect) IntersectRect_k(&rc, &rc, rect);

    if( hrgnUpdate ) bOwnRgn = FALSE;
    else if( bUpdate ) hrgnUpdate = CreateRectRgn_k( 0, 0, 0, 0 );

    newCaretPos.x = newCaretPos.y = 0;

    if( !IsRectEmpty_k(&cliprc) && (dx || dy)) {
        DWORD dcxflags = 0;
        DWORD style = GetWindowLongW_k( hwnd, GWL_STYLE );

        hwndCaret = fix_caret(hwnd, &rc, dx, dy, flags, &moveCaret, &newCaretPos);

        if (is_ex) dcxflags |= DCX_CACHE;
        if( style & WS_CLIPSIBLINGS) dcxflags |= DCX_CLIPSIBLINGS;
        if( GetClassLongW_k( hwnd, GCL_STYLE ) & CS_PARENTDC)
            dcxflags |= DCX_PARENTCLIP;
        if( !(flags & SW_SCROLLCHILDREN) && (style & WS_CLIPCHILDREN))
            dcxflags |= DCX_CLIPCHILDREN;
        hDC = GetDCEx_k( hwnd, 0, dcxflags);
        if (hDC)
        {
            ScrollDC_k( hDC, dx, dy, &rc, &cliprc, hrgnUpdate, rcUpdate );

            ReleaseDC_k( hwnd, hDC );

            if (!bUpdate)
                RedrawWindow_k( hwnd, NULL, hrgnUpdate, rdw_flags);
        }

        /* If the windows has an update region, this must be
         * scrolled as well. Keep a copy in hrgnWinupd
         * to be added to hrngUpdate at the end. */
        hrgnTemp = CreateRectRgn_k( 0, 0, 0, 0 );
        retVal = GetUpdateRgn_k( hwnd, hrgnTemp, FALSE );
        if (retVal != NULLREGION)
        {
            HRGN hrgnClip = CreateRectRgnIndirect_k(&cliprc);
            if( !bOwnRgn) {
                hrgnWinupd = CreateRectRgn_k( 0, 0, 0, 0);
                CombineRgn_k( hrgnWinupd, hrgnTemp, 0, RGN_COPY);
            }
            OffsetRgn_k( hrgnTemp, dx, dy );
            CombineRgn_k( hrgnTemp, hrgnTemp, hrgnClip, RGN_AND );
            if( !bOwnRgn)
                CombineRgn_k( hrgnWinupd, hrgnWinupd, hrgnTemp, RGN_OR );
            RedrawWindow_k( hwnd, NULL, hrgnTemp, rdw_flags);

           /* Catch the case where the scrolling amount exceeds the size of the
            * original window. This generated a second update area that is the
            * location where the original scrolled content would end up.
            * This second region is not returned by the ScrollDC and sets
            * ScrollWindowEx apart from just a ScrollDC.
            *
            * This has been verified with testing on windows.
            */
            if (abs(dx) > abs(rc.right - rc.left) ||
                abs(dy) > abs(rc.bottom - rc.top))
            {
                SetRectRgn_k( hrgnTemp, rc.left + dx, rc.top + dy, rc.right+dx, rc.bottom + dy);
                CombineRgn_k( hrgnTemp, hrgnTemp, hrgnClip, RGN_AND );
                CombineRgn_k( hrgnUpdate, hrgnUpdate, hrgnTemp, RGN_OR );

                if( !bOwnRgn)
                    CombineRgn_k( hrgnWinupd, hrgnWinupd, hrgnTemp, RGN_OR );
            }
            DeleteObject_k( hrgnClip );
        }
        DeleteObject_k( hrgnTemp );
    } else {
        /* nothing was scrolled */
        if( !bOwnRgn)
            SetRectRgn_k( hrgnUpdate, 0, 0, 0, 0 );
        SetRectEmpty_k( rcUpdate);
    }

    if( flags & SW_SCROLLCHILDREN )
    {
        HWND *list = WIN_ListChildren( hwnd );
        if (list)
        {
            int i;
            RECT r, dummy;
            for (i = 0; list[i]; i++)
            {
                WIN_GetRectangles( list[i], COORDS_PARENT, &r, NULL );
                if (!rect || IntersectRect_k(&dummy, &r, rect))
                    SetWindowPos_k( list[i], 0, r.left + dx, r.top  + dy, 0, 0,
                                  SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE |
                                  SWP_NOREDRAW | SWP_DEFERERASE );
            }
            HeapFree_k( GetProcessHeap_k(), 0, list );
        }
    }

    if( flags & (SW_INVALIDATE | SW_ERASE) )
        RedrawWindow_k( hwnd, NULL, hrgnUpdate, rdw_flags |
                      ((flags & SW_SCROLLCHILDREN) ? RDW_ALLCHILDREN : 0 ) );

    if( hrgnWinupd) {
        CombineRgn_k( hrgnUpdate, hrgnUpdate, hrgnWinupd, RGN_OR);
        DeleteObject_k( hrgnWinupd);
    }

    if( hwndCaret ) {
        if ( moveCaret ) SetCaretPos_k( newCaretPos.x, newCaretPos.y );
        ShowCaret_k(hwndCaret);
    }

    if( bOwnRgn && hrgnUpdate ) DeleteObject_k( hrgnUpdate );

    return retVal;
}


/*************************************************************************
 *		ScrollWindowEx (USER32.@)
 *
 * Note: contrary to what the doc says, pixels that are scrolled from the
 *      outside of clipRect to the inside are NOT painted.
 *
 */
INT WINAPI ScrollWindowEx_k( HWND hwnd, INT dx, INT dy,
                           const RECT *rect, const RECT *clipRect,
                           HRGN hrgnUpdate, LPRECT rcUpdate,
                           UINT flags )
{
    return scroll_window( hwnd, dx, dy, rect, clipRect, hrgnUpdate, rcUpdate, flags, TRUE );
}

/*************************************************************************
 *		ScrollWindow (USER32.@)
 *
 */
BOOL WINAPI ScrollWindow_k( HWND hwnd, INT dx, INT dy,
                          const RECT *rect, const RECT *clipRect )
{
    return scroll_window( hwnd, dx, dy, rect, clipRect, 0, NULL,
                          SW_INVALIDATE | SW_ERASE | (rect ? 0 : SW_SCROLLCHILDREN), FALSE ) != ERROR;
}


/*************************************************************************
 *		ScrollDC (USER32.@)
 *
 * dx, dy, lprcScroll and lprcClip are all in logical coordinates (msdn is
 * wrong) hrgnUpdate is returned in device coordinates with rcUpdate in
 * logical coordinates.
 */
BOOL WINAPI ScrollDC_k( HDC hdc, INT dx, INT dy, const RECT *scroll, const RECT *clip,
                      HRGN ret_update_rgn, LPRECT update_rect )

{
    HRGN update_rgn = ret_update_rgn;
    RECT src_rect, clip_rect, offset;
    INT dxdev, dydev;
    HRGN dstrgn, cliprgn, visrgn;
    BOOL ret;

    //TRACE( "dx,dy %d,%d scroll %s clip %s update %p rect %p\n", dx, dy, wine_dbgstr_rect(scroll), wine_dbgstr_rect(clip), ret_update_rgn, update_rect );

    /* get the visible region */
    visrgn = CreateRectRgn_k( 0, 0, 0, 0 );
    GetRandomRgn_k( hdc, visrgn, SYSRGN );
    if (!(GetVersion_k() & 0x80000000))
    {
        POINT org;
        GetDCOrgEx_k( hdc, &org );
        OffsetRgn_k( visrgn, -org.x, -org.y );
    }

    /* intersect with the clipping region if the DC has one */
    cliprgn = CreateRectRgn_k( 0, 0, 0, 0);
    if (GetClipRgn_k( hdc, cliprgn ) != 1)
    {
        DeleteObject_k( cliprgn );
        cliprgn = 0;
    }
    else CombineRgn_k( visrgn, visrgn, cliprgn, RGN_AND );

    /* only those pixels in the scroll rectangle that remain in the clipping
     * rect are scrolled. */
    if (clip)
        clip_rect = *clip;
    else
        GetClipBox_k( hdc, &clip_rect );
    src_rect = clip_rect;
    OffsetRect_k( &clip_rect, -dx, -dy );
    IntersectRect_k( &src_rect, &src_rect, &clip_rect );

    /* if an scroll rectangle is specified, only the pixels within that
     * rectangle are scrolled */
    if (scroll) IntersectRect_k( &src_rect, &src_rect, scroll );

    /* now convert to device coordinates */
    LPtoDP_k( hdc, (LPPOINT)&src_rect, 2 );
    //TRACE( "source rect: %s\n", wine_dbgstr_rect(&src_rect) );
    /* also dx and dy */
    SetRect_k( &offset, 0, 0, dx, dy );
    LPtoDP_k( hdc, (LPPOINT)&offset, 2 );
    dxdev = offset.right - offset.left;
    dydev = offset.bottom - offset.top;

    /* now intersect with the visible region to get the pixels that will actually scroll */
    dstrgn = CreateRectRgnIndirect_k( &src_rect );
    CombineRgn_k( dstrgn, dstrgn, visrgn, RGN_AND );
    OffsetRgn_k( dstrgn, dxdev, dydev );
    ExtSelectClipRgn_k( hdc, dstrgn, RGN_AND );

    /* compute the update areas.  This is the combined clip rectangle
     * minus the scrolled region, and intersected with the visible region. */
    if (ret_update_rgn || update_rect)
    {
        /* intersect clip and scroll rectangles, allowing NULL values */
        if (scroll)
        {
            if (clip)
                IntersectRect_k( &clip_rect, clip, scroll );
            else
                clip_rect = *scroll;
        }
        else if (clip)
            clip_rect = *clip;
        else
            GetClipBox_k( hdc, &clip_rect );

        /* Convert the combined clip rectangle to device coordinates */
        LPtoDP_k( hdc, (LPPOINT)&clip_rect, 2 );
        if (update_rgn)
            SetRectRgn_k( update_rgn, clip_rect.left, clip_rect.top, clip_rect.right, clip_rect.bottom );
        else
            update_rgn = CreateRectRgnIndirect_k( &clip_rect );

        CombineRgn_k( update_rgn, update_rgn, visrgn, RGN_AND );
        CombineRgn_k( update_rgn, update_rgn, dstrgn, RGN_DIFF );
    }

    ret = USER_Driver->pScrollDC( hdc, dx, dy, update_rgn );

    if (ret && update_rect)
    {
        GetRgnBox_k( update_rgn, update_rect );
        DPtoLP_k( hdc, (LPPOINT)update_rect, 2 );
        //TRACE( "returning update_rect %s\n", wine_dbgstr_rect(update_rect) );
    }
    if (!ret_update_rgn) DeleteObject_k( update_rgn );
    SelectClipRgn_k( hdc, cliprgn );
    if (cliprgn) DeleteObject_k( cliprgn );
    DeleteObject_k( visrgn );
    DeleteObject_k( dstrgn );
    return ret;
}

/************************************************************************
 *		PrintWindow (USER32.@)
 *
 */
BOOL WINAPI PrintWindow_k(HWND hwnd, HDC hdcBlt, UINT nFlags)
{
    UINT flags = PRF_CHILDREN | PRF_ERASEBKGND | PRF_OWNED | PRF_CLIENT;
    if(!(nFlags & PW_CLIENTONLY))
    {
        flags |= PRF_NONCLIENT;
    }
    SendMessageW_k(hwnd, WM_PRINT, (WPARAM)hdcBlt, flags);
    return TRUE;
}
