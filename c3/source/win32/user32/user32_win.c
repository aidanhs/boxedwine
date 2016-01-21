/*
 * Window related functions
 *
 * Copyright 1993, 1994 Alexandre Julliard
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
#include <stdlib.h>
#include <string.h>
#include "../include/unicode.h"
#include "../include/winbase.h"
#include "win.h"
#include "../include/list.h"
#include "../include/gdi_driver.h"
#include "kprocess.h"
#include "controls.h"

static struct list window_surfaces = LIST_INIT( window_surfaces );

static CRITICAL_SECTION surfaces_section;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &surfaces_section,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": surfaces_section") }
};
static CRITICAL_SECTION surfaces_section = { &critsect_debug, -1, 0, 0, 0, 0 };

static INLINE struct user_thread_info *get_user_thread_info(void)
{
    return (struct user_thread_info *)(currentThread()->Win32ClientInfo);
}

/**********************************************************************/

/* helper for Get/SetWindowLong */
static INLINE LONG_PTR get_win_data( const void *ptr, UINT size )
{
    if (size == sizeof(WORD))
    {
        WORD ret;
        memcpy( &ret, ptr, sizeof(ret) );
        return ret;
    }
    else if (size == sizeof(DWORD))
    {
        DWORD ret;
        memcpy( &ret, ptr, sizeof(ret) );
        return ret;
    }
    else
    {
        LONG_PTR ret;
        memcpy( &ret, ptr, sizeof(ret) );
        return ret;
    }
}

/* helper for Get/SetWindowLong */
static INLINE void set_win_data( void *ptr, LONG_PTR val, UINT size )
{
    if (size == sizeof(WORD))
    {
        WORD newval = (WORD)val;
        memcpy( ptr, &newval, sizeof(newval) );
    }
    else if (size == sizeof(DWORD))
    {
        DWORD newval = val;
        memcpy( ptr, &newval, sizeof(newval) );
    }
    else
    {
        memcpy( ptr, &val, sizeof(val) );
    }
}

/***********************************************************************
 *           get_user_handle_ptr
 */
BOOL isValidUserHandle( HANDLE handle, enum user_obj_type type )
{
    if ((int)handle < MAX_USER_HANDLES && userHandles[(int)handle].type == type) {
        return TRUE;
    }
    return FALSE;
}


/***********************************************************************
 *           create_window_handle
 *
 * Create a window handle with the server.
 */
struct tagCLASS *CLASS_FindClass( LPCWSTR name, HINSTANCE hinstance );
static WND *create_window_handle( HWND parent, HWND owner, LPCWSTR name,
                                  HINSTANCE instance, BOOL unicode )
{
    WND *win;
    HWND handle;
    WNDCLASSW classInfo;
    struct KProcess* process = currentProcess();
    struct tagCLASS *class = CLASS_FindClass(name, instance);

    if (!class)
        return NULL;
    handle = (HWND)allocUserHandle(process, USER_HANDLE_WND);
    GetClassInfoW_k(instance, name, &classInfo);

    if (!handle)
    {
        return NULL;
    }

    if (!(win = HeapAlloc_k( GetProcessHeap_k(), HEAP_ZERO_MEMORY,
                           sizeof(WND) + classInfo.cbWndExtra - sizeof(win->wExtra) )))
    {
        freeUserHandle((int)handle);
        SetLastError_k( ERROR_NOT_ENOUGH_MEMORY );
        return NULL;
    }

    if (!parent)
    {
        struct user_thread_info *thread_info = get_user_thread_info();

        if (!thread_info->msg_window) thread_info->msg_window = handle;
    }
    win->obj.handle = handle;
    win->obj.type   = USER_WINDOW;
    win->parent     = parent;
    win->owner      = owner;
    win->class      = class;
    win->winproc    = classInfo.lpfnWndProc;
    win->cbWndExtra = classInfo.cbWndExtra;
    win->unlinked   = pblListNewLinkedList();
    win->children   = pblListNewLinkedList();
    userHandles[(int)handle].data = win;

    if (WINPROC_IsUnicode( win->winproc, unicode )) win->flags |= WIN_ISUNICODE;
    return win;
}


/***********************************************************************
 *           free_window_handle
 *
 * Free a window handle.
 */
static void free_window_handle( HWND hwnd )
{
    if (isValidUserHandle(hwnd, USER_HANDLE_WND))
    {
        freeUserHandle((int)hwnd);        
    }
}


/*******************************************************************
 *           list_window_children
 *
 * Build an array of the children of a given window. The array must be
 * freed with HeapFree. Returns NULL when no windows are found.
 */

static int listChildren(WND *win, struct tagCLASS *class, DWORD tid, HWND* list) {
    PblIterator* it = pblListIterator(win->children);
    int count = 0;
    
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        WND* child = pblMapEntryValue(entry);
        if (tid && child->tid!=tid) continue;
        if (class && class!=child->class) continue;
        if (list) {
            list[count]=child->obj.handle;
        }
        count++;
    }
    pblIteratorFree(it);
    return count;
}

static HWND *list_window_children( HDESK desktop, HWND hwnd, LPCWSTR name, DWORD tid )
{
    HWND *list;
    struct tagCLASS *class = NULL;
    WND *win;
    
    
    if (name) {
        class = CLASS_FindClass(name, NULL);
        if (!class)
            return NULL;
    }
    if (hwnd) {
        win = WIN_GetPtr(hwnd);
        if (!win)
            return NULL;
    } else {
        win = WIN_GetPtr(((struct desktop*)desktop)->wnd);
    }
    
    list = HeapAlloc_k( GetProcessHeap_k(), 0, (listChildren(win, class, tid, NULL)+1) * sizeof(HWND));
    listChildren(win, class, tid, list);
    return NULL;
}

/* check if any area of a window needs repainting */
static INLINE int win_needs_repaint( WND *win )
{
    return win->update_region || (win->paint_flags & PAINT_INTERNAL);
}

static INLINE WND *get_next_window( WND *win )
{
    WND* parent = WIN_GetPtr(win->parent);
    WND* result;

    int index = pblListIndexOf(parent->children, win);
    result = pblListGet(parent->children, index+1);
    if (result == (WND*)-1)
        return NULL;
    return result;
}


/* find a child of the specified window that needs repainting */
static WND *find_child_to_repaint( WND *parent, DWORD tid )
{
    WND *ptr, *ret = NULL;
    PblIterator* it = pblListIterator(parent->children);
    
    while (it && pblIteratorHasNext(it)) 
    {
        PblMapEntry* entry = pblIteratorNext(it);
        WND* ptr = pblMapEntryValue(entry);

        if (!(ptr->dwStyle & WS_VISIBLE)) continue;
        if (ptr->tid == tid && win_needs_repaint( ptr ))
            ret = ptr;
        else if (!(ptr->dwStyle & WS_MINIMIZE)) /* explore its children */
            ret = find_child_to_repaint( ptr, tid );
        if (ret) break;
    }
    pblIteratorFree(it);

    if (ret && (ret->dwExStyle & WS_EX_TRANSPARENT))
    {
        /* transparent window, check for non-transparent sibling to paint first */
        for (ptr = get_next_window(ret); ptr; ptr = get_next_window(ptr))
        {
            if (!(ptr->dwStyle & WS_VISIBLE)) continue;
            if (ptr->dwExStyle & WS_EX_TRANSPARENT) continue;
            if (ptr->tid != tid) continue;
            if (win_needs_repaint( ptr )) return ptr;
        }
    }
    return ret;
}

/* find a window that needs to receive a WM_PAINT; also clear its internal paint flag */
HWND find_window_to_repaint( HWND parent, DWORD tid )
{
    WND *ptr, *win, *top_window = (WND*)getThread(tid)->input->desktop->wnd;

    if (!top_window) return 0;

    if (top_window->tid == tid && win_needs_repaint( top_window )) win = top_window;
    else win = find_child_to_repaint( top_window, tid );

    if (win && parent)
    {
        /* check that it is a child of the specified parent */
        for (ptr = win; ptr; ptr = WIN_GetPtr(ptr->parent))
            if (ptr->handle == parent) break;
        /* otherwise don't return any window, we don't repaint a child before its parent */
        if (!ptr) win = NULL;
    }
    if (!win) return 0;
    win->paint_flags &= ~PAINT_INTERNAL;
    return win->handle;
}


/*******************************************************************
 *           list_window_parents
 *
 * Build an array of all parents of a given window, starting with
 * the immediate parent. The array must be freed with HeapFree.
 */
static int listParents(WND *win, HWND* list) {
    int count = 0;
    win = WIN_GetPtr(win->parent);
    // don't add desktop
    while (win->parent) {
        if (list) {
            list[count]=win->obj.handle;
        }
        count++;
        win = WIN_GetPtr(win->parent);
    }
    return count;
}
static HWND *list_window_parents( HWND hwnd )
{
    WND *win = WIN_GetPtr(hwnd);
    HWND *list = 0;

    if (win) {
        list = HeapAlloc_k( GetProcessHeap_k(), 0, (listParents(win, NULL)+1) * sizeof(HWND));
        listParents(win, list);
    }
    return list;
}


/*******************************************************************
 *           send_parent_notify
 */
static void send_parent_notify( HWND hwnd, UINT msg )
{
    if ((GetWindowLongW_k( hwnd, GWL_STYLE ) & (WS_CHILD | WS_POPUP)) == WS_CHILD &&
        !(GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_NOPARENTNOTIFY))
    {
        HWND parent = GetParent_k(hwnd);
        if (parent && parent != GetDesktopWindow_k())
            SendMessageW_k( parent, WM_PARENTNOTIFY,
                          MAKEWPARAM( msg, GetWindowLongPtrW_k( hwnd, GWLP_ID )), (LPARAM)hwnd );
    }
}


/*******************************************************************
 *		update_window_state
 *
 * Trigger an update of the window's driver state and surface.
 */
static void update_window_state( HWND hwnd )
{
    RECT window_rect, client_rect;

    WIN_GetRectangles( hwnd, COORDS_PARENT, &window_rect, &client_rect );
    set_window_pos( hwnd, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE |
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW, &window_rect, &client_rect, NULL );
}


/*******************************************************************
 *           get_hwnd_message_parent
 *
 * Return the parent for HWND_MESSAGE windows.
 */
HWND get_hwnd_message_parent(void)
{
    struct user_thread_info *thread_info = get_user_thread_info();

    if (!thread_info->msg_window) GetDesktopWindow_k();  /* trigger creation */
    return thread_info->msg_window;
}


/*******************************************************************
 *           is_desktop_window
 *
 * Check if window is the desktop or the HWND_MESSAGE top parent.
 */
BOOL is_desktop_window( HWND hwnd )
{
    struct user_thread_info *thread_info = get_user_thread_info();

    if (!hwnd) return FALSE;
    if (hwnd == thread_info->top_window) return TRUE;
    if (hwnd == thread_info->msg_window) return TRUE;

    if (!HIWORD(hwnd) || HIWORD(hwnd) == 0xffff)
    {
        if (LOWORD(thread_info->top_window) == LOWORD(hwnd)) return TRUE;
        if (LOWORD(thread_info->msg_window) == LOWORD(hwnd)) return TRUE;
    }
    return FALSE;
}


/*******************************************************************
 * Dummy window surface for windows that shouldn't get painted.
 */

static void dummy_surface_lock( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void dummy_surface_unlock( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void *dummy_surface_get_bitmap_info( struct window_surface *window_surface, BITMAPINFO *info )
{
    static DWORD dummy_data;

    info->bmiHeader.biSize          = sizeof( info->bmiHeader );
    info->bmiHeader.biWidth         = dummy_surface.rect.right;
    info->bmiHeader.biHeight        = dummy_surface.rect.bottom;
    info->bmiHeader.biPlanes        = 1;
    info->bmiHeader.biBitCount      = 32;
    info->bmiHeader.biCompression   = BI_RGB;
    info->bmiHeader.biSizeImage     = 0;
    info->bmiHeader.biXPelsPerMeter = 0;
    info->bmiHeader.biYPelsPerMeter = 0;
    info->bmiHeader.biClrUsed       = 0;
    info->bmiHeader.biClrImportant  = 0;
    return &dummy_data;
}

static RECT *dummy_surface_get_bounds( struct window_surface *window_surface )
{
    static RECT dummy_bounds;
    return &dummy_bounds;
}

static void dummy_surface_set_region( struct window_surface *window_surface, HRGN region )
{
    /* nothing to do */
}

static void dummy_surface_flush( struct window_surface *window_surface )
{
    /* nothing to do */
}

static void dummy_surface_destroy( struct window_surface *window_surface )
{
    /* nothing to do */
}

static const struct window_surface_funcs dummy_surface_funcs =
{
    dummy_surface_lock,
    dummy_surface_unlock,
    dummy_surface_get_bitmap_info,
    dummy_surface_get_bounds,
    dummy_surface_set_region,
    dummy_surface_flush,
    dummy_surface_destroy
};

struct window_surface dummy_surface = { &dummy_surface_funcs, { NULL, NULL }, 1, { 0, 0, 1, 1 } };


/*******************************************************************
 *           register_window_surface
 *
 * Register a window surface in the global list, possibly replacing another one.
 */
void register_window_surface( struct window_surface *old, struct window_surface *new )
{
    if (old == new) return;
    EnterCriticalSection_k( &surfaces_section );
    if (old && old != &dummy_surface) list_remove( &old->entry );
    if (new && new != &dummy_surface) list_add_tail( &window_surfaces, &new->entry );
    LeaveCriticalSection_k( &surfaces_section );
}


/*******************************************************************
 *           flush_window_surfaces
 *
 * Flush pending output from all window surfaces.
 */
void flush_window_surfaces( BOOL idle )
{
    static DWORD last_idle;
    DWORD now;
    struct window_surface *surface;

    EnterCriticalSection_k( &surfaces_section );
    now = GetTickCount_k();
    if (idle) last_idle = now;
    /* if not idle, we only flush if there's evidence that the app never goes idle */
    else if ((int)(now - last_idle) < 1000) goto done;

    LIST_FOR_EACH_ENTRY( surface, &window_surfaces, struct window_surface, entry )
        surface->funcs->flush( surface );
done:
    LeaveCriticalSection_k( &surfaces_section );
}


/***********************************************************************
 *           WIN_GetPtr
 *
 * Return a pointer to the WND structure if local to the process,
 * or WND_OTHER_PROCESS if handle may be valid in other process.
 * If ret value is a valid pointer, it must be released with WIN_ReleasePtr.
 */
WND *WIN_GetPtr( HWND hwnd )
{
    struct KProcess* process = currentProcess();
    if ((int)hwnd < MAX_USER_HANDLES && userHandles[(int)hwnd].type == USER_HANDLE_WND)
        return userHandles[(int)hwnd].data;
    return NULL;
}


/***********************************************************************
 *           WIN_IsCurrentProcess
 *
 * Check whether a given window belongs to the current process (and return the full handle).
 */
HWND WIN_IsCurrentProcess( HWND hwnd )
{
    WND *ptr;
    HWND ret;

    if (!(ptr = WIN_GetPtr( hwnd )) || ptr == WND_DESKTOP) return 0;
    ret = ptr->obj.handle;
    return ret;
}


/***********************************************************************
 *           WIN_IsCurrentThread
 *
 * Check whether a given window belongs to the current thread (and return the full handle).
 */
HWND WIN_IsCurrentThread( HWND hwnd )
{
    WND *ptr;
    HWND ret = 0;

    if (!(ptr = WIN_GetPtr( hwnd )) || ptr == WND_DESKTOP) return 0;
    if (ptr->tid == GetCurrentThreadId_k()) ret = ptr->obj.handle;
    return ret;
}


/***********************************************************************
 *           WIN_GetFullHandle
 *
 * Convert a possibly truncated window handle to a full 32-bit handle.
 */
HWND WIN_GetFullHandle( HWND hwnd )
{
    return hwnd;
}


/***********************************************************************
 *           WIN_SetOwner
 *
 * Change the owner of a window.
 */
HWND WIN_SetOwner( HWND hwnd, HWND owner )
{
    WND *win = WIN_GetPtr( hwnd );
    HWND ret = 0;

    if (!win || is_desktop_window(hwnd)) return 0;
    win->owner = owner;
    return ret;
}


/***********************************************************************
 *           WIN_SetStyle
 *
 * Change the style of a window.
 */
ULONG WIN_SetStyle( HWND hwnd, ULONG set_bits, ULONG clear_bits )
{
    BOOL made_visible = FALSE;
    STYLESTRUCT style;
    WND *win = WIN_GetPtr( hwnd );

    if (!win || is_desktop_window(hwnd)) return 0;
    style.styleOld = win->dwStyle;
    style.styleNew = (win->dwStyle | set_bits) & ~clear_bits;
    if (style.styleNew == style.styleOld)
    {
        return style.styleNew;
    }
    win->dwStyle = style.styleNew;

    if ((style.styleOld ^ style.styleNew) & WS_VISIBLE)
    {
        /* Some apps try to make their window visible through WM_SETREDRAW.
         * Only do that if the window was never explicitly hidden,
         * because Steam messes with WM_SETREDRAW after hiding its windows. */
        made_visible = !(win->flags & WIN_HIDDEN) && (style.styleNew & WS_VISIBLE);
        invalidate_dce( win, NULL );
    }

    USER_Driver->pSetWindowStyle( hwnd, GWL_STYLE, &style );
    if (made_visible) update_window_state( hwnd );

    return style.styleOld;
}


/***********************************************************************
 *           WIN_GetRectangles
 *
 * Get the window and client rectangles.
 */
BOOL WIN_GetRectangles( HWND hwnd, enum coords_relative relative, RECT *rectWindow, RECT *rectClient )
{
    WND *win = WIN_GetPtr( hwnd );
    BOOL ret = TRUE;

    if (!win)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    if (is_desktop_window(hwnd))
    {
        RECT rect;
        rect.left = rect.top = 0;
        if (hwnd == get_hwnd_message_parent())
        {
            rect.right  = 100;
            rect.bottom = 100;
        }
        else
        {
            rect.right  = GetSystemMetrics_k(SM_CXSCREEN);
            rect.bottom = GetSystemMetrics_k(SM_CYSCREEN);
        }
        if (rectWindow) *rectWindow = rect;
        if (rectClient) *rectClient = rect;
        return TRUE;
    }
    RECT window_rect = win->rectWindow, client_rect = win->rectClient;

    switch (relative)
    {
    case COORDS_CLIENT:
        OffsetRect_k( &window_rect, -win->rectClient.left, -win->rectClient.top );
        OffsetRect_k( &client_rect, -win->rectClient.left, -win->rectClient.top );
        if (win->dwExStyle & WS_EX_LAYOUTRTL)
            mirror_rect( &win->rectClient, &window_rect );
        break;
    case COORDS_WINDOW:
        OffsetRect_k( &window_rect, -win->rectWindow.left, -win->rectWindow.top );
        OffsetRect_k( &client_rect, -win->rectWindow.left, -win->rectWindow.top );
        if (win->dwExStyle & WS_EX_LAYOUTRTL)
            mirror_rect( &win->rectWindow, &client_rect );
        break;
    case COORDS_PARENT:
        if (win->parent)
        {
            WND *parent = WIN_GetPtr( win->parent );
            if (is_desktop_window(win->parent)) break;
            if (parent->dwExStyle & WS_EX_LAYOUTRTL)
            {
                mirror_rect( &parent->rectClient, &window_rect );
                mirror_rect( &parent->rectClient, &client_rect );
            }
        }
        break;
    case COORDS_SCREEN:
        while (win->parent)
        {
            WND *parent = WIN_GetPtr( win->parent );
            if (is_desktop_window(win->parent)) break;
            win = parent;
            if (win->parent)
            {
                OffsetRect_k( &window_rect, win->rectClient.left, win->rectClient.top );
                OffsetRect_k( &client_rect, win->rectClient.left, win->rectClient.top );
            }
        }
        break;
    }
    if (rectWindow) *rectWindow = window_rect;
    if (rectClient) *rectClient = client_rect;
    return TRUE;
}


/***********************************************************************
 *           WIN_DestroyWindow
 *
 * Destroy storage associated to a window. "Internals" p.358
 */
LRESULT WIN_DestroyWindow( HWND hwnd )
{
    WND *wndPtr = WIN_GetPtr( hwnd );
    HWND *list;
    HMENU menu = 0, sys_menu;
    HWND icon_title;
    struct window_surface *surface;

    TRACE("%p\n", hwnd );

    /* free child windows */
    if ((list = WIN_ListChildren( hwnd )))
    {
        int i;
        for (i = 0; list[i]; i++)
        {
            if (WIN_IsCurrentThread( list[i] )) WIN_DestroyWindow( list[i] );
            else SendMessageW_k( list[i], WM_WINE_DESTROYWINDOW, 0, 0 );
        }
        HeapFree_k( GetProcessHeap_k(), 0, list );
    }

    if (!wndPtr)
        return 0;

    /* Unlink now so we won't bother with the children later on */
    wndPtr->parent = 0;

    /*
     * Send the WM_NCDESTROY to the window being destroyed.
     */
    SendMessageW_k( hwnd, WM_NCDESTROY, 0, 0 );

    /* FIXME: do we need to fake QS_MOUSEMOVE wakebit? */

    /* free resources associated with the window */

    if ((wndPtr->dwStyle & (WS_CHILD | WS_POPUP)) != WS_CHILD)
        menu = (HMENU)wndPtr->wIDmenu;
    sys_menu = wndPtr->hSysMenu;
    free_dce( wndPtr->dce, hwnd );
    wndPtr->dce = NULL;
    icon_title = wndPtr->icon_title;
    HeapFree_k( GetProcessHeap_k(), 0, wndPtr->text );
    wndPtr->text = NULL;
    HeapFree_k( GetProcessHeap_k(), 0, wndPtr->pScroll );
    wndPtr->pScroll = NULL;
    surface = wndPtr->surface;
    wndPtr->surface = NULL;

    if (icon_title) DestroyWindow_k( icon_title );
    if (menu) DestroyMenu_k( menu );
    if (sys_menu) DestroyMenu_k( sys_menu );
    if (surface)
    {
        register_window_surface( surface, NULL );
        window_surface_release( surface );
    }

    USER_Driver->pDestroyWindow( hwnd );

    free_window_handle( hwnd );
    return 0;
}


/***********************************************************************
 *		destroy_thread_window
 *
 * Destroy a window upon exit of its thread.
 */
static void destroy_thread_window( HWND hwnd )
{
    WND *wndPtr;
    HWND *list;
    HMENU menu = 0, sys_menu = 0;
    struct window_surface *surface = NULL;

    /* free child windows */

    if ((list = WIN_ListChildren( hwnd )))
    {
        int i;
        for (i = 0; list[i]; i++)
        {
            if (WIN_IsCurrentThread( list[i] )) destroy_thread_window( list[i] );
            else SendMessageW_k( list[i], WM_WINE_DESTROYWINDOW, 0, 0 );
        }
        HeapFree_k( GetProcessHeap_k(), 0, list );
    }

    /* destroy the client-side storage */
    if (wndPtr = WIN_GetPtr(hwnd))
    {
        if ((wndPtr->dwStyle & (WS_CHILD | WS_POPUP)) != WS_CHILD) menu = (HMENU)wndPtr->wIDmenu;
        sys_menu = wndPtr->hSysMenu;
        free_dce( wndPtr->dce, hwnd );
        surface = wndPtr->surface;
        wndPtr->surface = NULL;
        freeUserHandle((int)hwnd);
    }

    HeapFree_k( GetProcessHeap_k(), 0, wndPtr );
    if (menu) DestroyMenu_k( menu );
    if (sys_menu) DestroyMenu_k( sys_menu );
    if (surface)
    {
        register_window_surface( surface, NULL );
        window_surface_release( surface );
    }
}


/***********************************************************************
 *		destroy_thread_child_windows
 *
 * Destroy child windows upon exit of its thread.
 */
static void destroy_thread_child_windows( HWND hwnd )
{
    HWND *list;
    int i;

    if (WIN_IsCurrentThread( hwnd ))
    {
        destroy_thread_window( hwnd );
    }
    else if ((list = WIN_ListChildren( hwnd )))
    {
        for (i = 0; list[i]; i++) destroy_thread_child_windows( list[i] );
        HeapFree_k( GetProcessHeap_k(), 0, list );
    }
}


/***********************************************************************
 *           WIN_DestroyThreadWindows
 *
 * Destroy all children of 'wnd' owned by the current thread.
 */
void WIN_DestroyThreadWindows( HWND hwnd )
{
    HWND *list;
    int i;

    if (!(list = WIN_ListChildren( hwnd ))) return;

    /* reset owners of top-level windows */
    for (i = 0; list[i]; i++)
    {
        if (!WIN_IsCurrentThread( list[i] ))
        {
            HWND owner = GetWindow_k( list[i], GW_OWNER );
            if (owner && WIN_IsCurrentThread( owner )) WIN_SetOwner( list[i], 0 );
        }
    }

    for (i = 0; list[i]; i++) destroy_thread_child_windows( list[i] );
    HeapFree_k( GetProcessHeap_k(), 0, list );
}


/***********************************************************************
 *           WIN_FixCoordinates
 *
 * Fix the coordinates - Helper for WIN_CreateWindowEx.
 * returns default show mode in sw.
 */
static void WIN_FixCoordinates( CREATESTRUCTW *cs, INT *sw)
{
#define IS_DEFAULT(x)  ((x) == CW_USEDEFAULT || (x) == (SHORT)0x8000)
    POINT pos[2];

    if (cs->dwExStyle & WS_EX_MDICHILD)
    {
        UINT id = 0;

        MDI_CalcDefaultChildPos(cs->hwndParent, -1, pos, 0, &id);
        if (!(cs->style & WS_POPUP)) cs->hMenu = ULongToHandle(id);

        TRACE("MDI child id %04x\n", id);
    }

    if (cs->style & (WS_CHILD | WS_POPUP))
    {
        if (cs->dwExStyle & WS_EX_MDICHILD)
        {
            if (IS_DEFAULT(cs->x))
            {
                cs->x = pos[0].x;
                cs->y = pos[0].y;
            }
            if (IS_DEFAULT(cs->cx) || !cs->cx) cs->cx = pos[1].x;
            if (IS_DEFAULT(cs->cy) || !cs->cy) cs->cy = pos[1].y;
        }
        else
        {
            if (IS_DEFAULT(cs->x)) cs->x = cs->y = 0;
            if (IS_DEFAULT(cs->cx)) cs->cx = cs->cy = 0;
        }
    }
    else  /* overlapped window */
    {
        HMONITOR monitor;
        MONITORINFO mon_info;
        STARTUPINFOW info;

        if (!IS_DEFAULT(cs->x) && !IS_DEFAULT(cs->cx) && !IS_DEFAULT(cs->cy)) return;

        monitor = MonitorFromWindow_k( cs->hwndParent, MONITOR_DEFAULTTOPRIMARY );
        mon_info.cbSize = sizeof(mon_info);
        GetMonitorInfoW_k( monitor, &mon_info );
        GetStartupInfoW_k( &info );

        if (IS_DEFAULT(cs->x))
        {
            if (!IS_DEFAULT(cs->y)) *sw = cs->y;
            cs->x = (info.dwFlags & STARTF_USEPOSITION) ? info.dwX : mon_info.rcWork.left;
            cs->y = (info.dwFlags & STARTF_USEPOSITION) ? info.dwY : mon_info.rcWork.top;
        }

        if (IS_DEFAULT(cs->cx))
        {
            if (info.dwFlags & STARTF_USESIZE)
            {
                cs->cx = info.dwXSize;
                cs->cy = info.dwYSize;
            }
            else
            {
                cs->cx = (mon_info.rcWork.right - mon_info.rcWork.left) * 3 / 4 - cs->x;
                cs->cy = (mon_info.rcWork.bottom - mon_info.rcWork.top) * 3 / 4 - cs->y;
            }
        }
        /* neither x nor cx are default. Check the y values .
         * In the trace we see Outlook and Outlook Express using
         * cy set to CW_USEDEFAULT when opening the address book.
         */
        else if (IS_DEFAULT(cs->cy))
        {
            FIXME("Strange use of CW_USEDEFAULT in nHeight\n");
            cs->cy = (mon_info.rcWork.bottom - mon_info.rcWork.top) * 3 / 4 - cs->y;
        }
    }
#undef IS_DEFAULT
}

/***********************************************************************
 *           dump_window_styles
 */
static void dump_window_styles( DWORD style, DWORD exstyle )
{
    TRACE( "style:" );
    if(style & WS_POPUP) TRACE(" WS_POPUP");
    if(style & WS_CHILD) TRACE(" WS_CHILD");
    if(style & WS_MINIMIZE) TRACE(" WS_MINIMIZE");
    if(style & WS_VISIBLE) TRACE(" WS_VISIBLE");
    if(style & WS_DISABLED) TRACE(" WS_DISABLED");
    if(style & WS_CLIPSIBLINGS) TRACE(" WS_CLIPSIBLINGS");
    if(style & WS_CLIPCHILDREN) TRACE(" WS_CLIPCHILDREN");
    if(style & WS_MAXIMIZE) TRACE(" WS_MAXIMIZE");
    if((style & WS_CAPTION) == WS_CAPTION) TRACE(" WS_CAPTION");
    else
    {
        if(style & WS_BORDER) TRACE(" WS_BORDER");
        if(style & WS_DLGFRAME) TRACE(" WS_DLGFRAME");
    }
    if(style & WS_VSCROLL) TRACE(" WS_VSCROLL");
    if(style & WS_HSCROLL) TRACE(" WS_HSCROLL");
    if(style & WS_SYSMENU) TRACE(" WS_SYSMENU");
    if(style & WS_THICKFRAME) TRACE(" WS_THICKFRAME");
    if (style & WS_CHILD)
    {
        if(style & WS_GROUP) TRACE(" WS_GROUP");
        if(style & WS_TABSTOP) TRACE(" WS_TABSTOP");
    }
    else
    {
        if(style & WS_MINIMIZEBOX) TRACE(" WS_MINIMIZEBOX");
        if(style & WS_MAXIMIZEBOX) TRACE(" WS_MAXIMIZEBOX");
    }

    /* FIXME: Add dumping of BS_/ES_/SBS_/LBS_/CBS_/DS_/etc. styles */
#define DUMPED_STYLES \
    ((DWORD)(WS_POPUP | \
     WS_CHILD | \
     WS_MINIMIZE | \
     WS_VISIBLE | \
     WS_DISABLED | \
     WS_CLIPSIBLINGS | \
     WS_CLIPCHILDREN | \
     WS_MAXIMIZE | \
     WS_BORDER | \
     WS_DLGFRAME | \
     WS_VSCROLL | \
     WS_HSCROLL | \
     WS_SYSMENU | \
     WS_THICKFRAME | \
     WS_GROUP | \
     WS_TABSTOP | \
     WS_MINIMIZEBOX | \
     WS_MAXIMIZEBOX))

    if(style & ~DUMPED_STYLES) TRACE(" %08x", style & ~DUMPED_STYLES);
    TRACE("\n");
#undef DUMPED_STYLES

    TRACE( "exstyle:" );
    if(exstyle & WS_EX_DLGMODALFRAME) TRACE(" WS_EX_DLGMODALFRAME");
    if(exstyle & WS_EX_DRAGDETECT) TRACE(" WS_EX_DRAGDETECT");
    if(exstyle & WS_EX_NOPARENTNOTIFY) TRACE(" WS_EX_NOPARENTNOTIFY");
    if(exstyle & WS_EX_TOPMOST) TRACE(" WS_EX_TOPMOST");
    if(exstyle & WS_EX_ACCEPTFILES) TRACE(" WS_EX_ACCEPTFILES");
    if(exstyle & WS_EX_TRANSPARENT) TRACE(" WS_EX_TRANSPARENT");
    if(exstyle & WS_EX_MDICHILD) TRACE(" WS_EX_MDICHILD");
    if(exstyle & WS_EX_TOOLWINDOW) TRACE(" WS_EX_TOOLWINDOW");
    if(exstyle & WS_EX_WINDOWEDGE) TRACE(" WS_EX_WINDOWEDGE");
    if(exstyle & WS_EX_CLIENTEDGE) TRACE(" WS_EX_CLIENTEDGE");
    if(exstyle & WS_EX_CONTEXTHELP) TRACE(" WS_EX_CONTEXTHELP");
    if(exstyle & WS_EX_RIGHT) TRACE(" WS_EX_RIGHT");
    if(exstyle & WS_EX_RTLREADING) TRACE(" WS_EX_RTLREADING");
    if(exstyle & WS_EX_LEFTSCROLLBAR) TRACE(" WS_EX_LEFTSCROLLBAR");
    if(exstyle & WS_EX_CONTROLPARENT) TRACE(" WS_EX_CONTROLPARENT");
    if(exstyle & WS_EX_STATICEDGE) TRACE(" WS_EX_STATICEDGE");
    if(exstyle & WS_EX_APPWINDOW) TRACE(" WS_EX_APPWINDOW");
    if(exstyle & WS_EX_LAYERED) TRACE(" WS_EX_LAYERED");
    if(exstyle & WS_EX_LAYOUTRTL) TRACE(" WS_EX_LAYOUTRTL");

#define DUMPED_EX_STYLES \
    ((DWORD)(WS_EX_DLGMODALFRAME | \
     WS_EX_DRAGDETECT | \
     WS_EX_NOPARENTNOTIFY | \
     WS_EX_TOPMOST | \
     WS_EX_ACCEPTFILES | \
     WS_EX_TRANSPARENT | \
     WS_EX_MDICHILD | \
     WS_EX_TOOLWINDOW | \
     WS_EX_WINDOWEDGE | \
     WS_EX_CLIENTEDGE | \
     WS_EX_CONTEXTHELP | \
     WS_EX_RIGHT | \
     WS_EX_RTLREADING | \
     WS_EX_LEFTSCROLLBAR | \
     WS_EX_CONTROLPARENT | \
     WS_EX_STATICEDGE | \
     WS_EX_APPWINDOW | \
     WS_EX_LAYERED | \
     WS_EX_LAYOUTRTL))

    if(exstyle & ~DUMPED_EX_STYLES) TRACE(" %08x", exstyle & ~DUMPED_EX_STYLES);
    TRACE("\n");
#undef DUMPED_EX_STYLES
}


/***********************************************************************
 *           WIN_CreateWindowEx
 *
 * Implementation of CreateWindowEx().
 */
HWND WIN_CreateWindowEx( CREATESTRUCTW *cs, LPCWSTR className, HINSTANCE module, BOOL unicode )
{
    INT cx, cy, style, sw = SW_SHOW;
    LRESULT result;
    RECT rect;
    WND *wndPtr;
    HWND hwnd, parent, owner, top_child = 0;
    MDICREATESTRUCTW mdi_cs;
    CBT_CREATEWNDW cbtc;
    CREATESTRUCTW cbcs;

    TRACE("%s %s ex=%08x style=%08x %d,%d %dx%d parent=%p menu=%p inst=%p params=%p\n",
          unicode ? debugstr_w(cs->lpszName) : debugstr_a((LPCSTR)cs->lpszName),
          debugstr_w(className),
          cs->dwExStyle, cs->style, cs->x, cs->y, cs->cx, cs->cy,
          cs->hwndParent, cs->hMenu, cs->hInstance, cs->lpCreateParams );

    /* Fix the styles for MDI children */
    if (cs->dwExStyle & WS_EX_MDICHILD)
    {
        UINT flags = 0;

        wndPtr = WIN_GetPtr(cs->hwndParent);
        if (wndPtr && wndPtr != WND_DESKTOP)
        {
            flags = wndPtr->flags;
        }

        if (!(flags & WIN_ISMDICLIENT))
        {
            WARN("WS_EX_MDICHILD, but parent %p is not MDIClient\n", cs->hwndParent);
            return 0;
        }

        /* cs->lpCreateParams of WM_[NC]CREATE is different for MDI children.
         * MDICREATESTRUCT members have the originally passed values.
         *
         * Note: we rely on the fact that MDICREATESTRUCTA and MDICREATESTRUCTW
         * have the same layout.
         */
        mdi_cs.szClass = cs->lpszClass;
        mdi_cs.szTitle = cs->lpszName;
        mdi_cs.hOwner = cs->hInstance;
        mdi_cs.x = cs->x;
        mdi_cs.y = cs->y;
        mdi_cs.cx = cs->cx;
        mdi_cs.cy = cs->cy;
        mdi_cs.style = cs->style;
        mdi_cs.lParam = (LPARAM)cs->lpCreateParams;

        cs->lpCreateParams = &mdi_cs;

        if (GetWindowLongW_k(cs->hwndParent, GWL_STYLE) & MDIS_ALLCHILDSTYLES)
        {
            if (cs->style & WS_POPUP)
            {
                TRACE("WS_POPUP with MDIS_ALLCHILDSTYLES is not allowed\n");
                return 0;
            }
            cs->style |= WS_CHILD | WS_CLIPSIBLINGS;
        }
        else
        {
            cs->style &= ~WS_POPUP;
            cs->style |= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION |
                WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        }

        top_child = GetWindow_k(cs->hwndParent, GW_CHILD);

        if (top_child)
        {
            /* Restore current maximized child */
            if((cs->style & WS_VISIBLE) && IsZoomed_k(top_child))
            {
                TRACE("Restoring current maximized child %p\n", top_child);
                if (cs->style & WS_MAXIMIZE)
                {
                    /* if the new window is maximized don't bother repainting */
                    SendMessageW_k( top_child, WM_SETREDRAW, FALSE, 0 );
                    ShowWindow_k( top_child, SW_SHOWNORMAL );
                    SendMessageW_k( top_child, WM_SETREDRAW, TRUE, 0 );
                }
                else ShowWindow_k( top_child, SW_SHOWNORMAL );
            }
        }
    }

    /* Find the parent window */

    parent = cs->hwndParent;
    owner = 0;

    if (cs->hwndParent == HWND_MESSAGE)
    {
        cs->hwndParent = parent = get_hwnd_message_parent();
    }
    else if (cs->hwndParent)
    {
        if ((cs->style & (WS_CHILD|WS_POPUP)) != WS_CHILD)
        {
            parent = GetDesktopWindow_k();
            owner = cs->hwndParent;
        }
        else
        {
            DWORD parent_style = GetWindowLongW_k( parent, GWL_EXSTYLE );
            if ((parent_style & WS_EX_LAYOUTRTL) && !(parent_style & WS_EX_NOINHERITLAYOUT))
                cs->dwExStyle |= WS_EX_LAYOUTRTL;
        }
    }
    else
    {
        static const WCHAR messageW[] = {'M','e','s','s','a','g','e',0};

        if ((cs->style & (WS_CHILD|WS_POPUP)) == WS_CHILD)
        {
            WARN("No parent for child window\n" );
            SetLastError_k(ERROR_TLW_WITH_WSCHILD);
            return 0;  /* WS_CHILD needs a parent, but WS_POPUP doesn't */
        }
        /* are we creating the desktop or HWND_MESSAGE parent itself? */
        if (className != (LPCWSTR)DESKTOP_CLASS_ATOM &&
            (IS_INTRESOURCE(className) || strcmpiW( className, messageW )))
        {
            DWORD layout;
            GetProcessDefaultLayout_k( &layout );
            if (layout & LAYOUT_RTL) cs->dwExStyle |= WS_EX_LAYOUTRTL;
            parent = GetDesktopWindow_k();
        }
    }

    WIN_FixCoordinates(cs, &sw); /* fix default coordinates */

    if ((cs->dwExStyle & WS_EX_DLGMODALFRAME) ||
        ((!(cs->dwExStyle & WS_EX_STATICEDGE)) &&
          (cs->style & (WS_DLGFRAME | WS_THICKFRAME))))
        cs->dwExStyle |= WS_EX_WINDOWEDGE;
    else
        cs->dwExStyle &= ~WS_EX_WINDOWEDGE;

    /* Create the window structure */

    if (!(wndPtr = create_window_handle( parent, owner, className, module, unicode )))
        return 0;
    hwnd = wndPtr->obj.handle;

    /* Fill the window structure */

    wndPtr->tid            = GetCurrentThreadId_k();
    wndPtr->hInstance      = cs->hInstance;
    wndPtr->text           = NULL;
    wndPtr->dwStyle        = cs->style & ~WS_VISIBLE;
    wndPtr->dwExStyle      = cs->dwExStyle;
    wndPtr->wIDmenu        = 0;
    wndPtr->helpContext    = 0;
    wndPtr->pScroll        = NULL;
    wndPtr->userdata       = 0;
    wndPtr->hIcon          = 0;
    wndPtr->hIconSmall     = 0;
    wndPtr->hSysMenu       = 0;

    wndPtr->min_pos.x = wndPtr->min_pos.y = -1;
    wndPtr->max_pos.x = wndPtr->max_pos.y = -1;
    SetRect_k( &wndPtr->normal_rect, cs->x, cs->y, cs->x + cs->cx, cs->y + cs->cy );

    if (wndPtr->dwStyle & WS_SYSMENU) SetSystemMenu_k( hwnd, 0 );

    /*
     * Correct the window styles.
     *
     * It affects only the style loaded into the WIN structure.
     */

    if ((wndPtr->dwStyle & (WS_CHILD | WS_POPUP)) != WS_CHILD)
    {
        wndPtr->dwStyle |= WS_CLIPSIBLINGS;
        if (!(wndPtr->dwStyle & WS_POPUP))
            wndPtr->dwStyle |= WS_CAPTION;
    }

    /* WS_EX_WINDOWEDGE depends on some other styles */
    if (wndPtr->dwExStyle & WS_EX_DLGMODALFRAME)
        wndPtr->dwExStyle |= WS_EX_WINDOWEDGE;
    else if (wndPtr->dwStyle & (WS_DLGFRAME | WS_THICKFRAME))
    {
        if (!((wndPtr->dwExStyle & WS_EX_STATICEDGE) &&
            (wndPtr->dwStyle & (WS_CHILD | WS_POPUP))))
            wndPtr->dwExStyle |= WS_EX_WINDOWEDGE;
    }
    else
        wndPtr->dwExStyle &= ~WS_EX_WINDOWEDGE;

    if (!(wndPtr->dwStyle & (WS_CHILD | WS_POPUP)))
        wndPtr->flags |= WIN_NEED_SIZE;

    /* Set the window menu */

    if ((wndPtr->dwStyle & (WS_CHILD | WS_POPUP)) != WS_CHILD)
    {
        if (cs->hMenu)
        {
            if (!MENU_SetMenu(hwnd, cs->hMenu))
            {
                free_window_handle( hwnd );
                return 0;
            }
        }
        else
        {
            LPCWSTR menuName = (LPCWSTR)GetClassLongPtrW_k( hwnd, GCLP_MENUNAME );
            if (menuName)
            {
                cs->hMenu = LoadMenuW_k( cs->hInstance, menuName );
                if (cs->hMenu) MENU_SetMenu( hwnd, cs->hMenu );
            }
        }
    }
    else SetWindowLongPtrW_k( hwnd, GWLP_ID, (ULONG_PTR)cs->hMenu );

    /* call the WH_CBT hook */

    /* the window style passed to the hook must be the real window style,
     * rather than just the window style that the caller to CreateWindowEx
     * passed in, so we have to copy the original CREATESTRUCT and get the
     * the real style. */
    cbcs = *cs;
    cbcs.style = wndPtr->dwStyle;
    cbtc.lpcs = &cbcs;
    cbtc.hwndInsertAfter = HWND_TOP;
    if (HOOK_CallHooks( WH_CBT, HCBT_CREATEWND, (WPARAM)hwnd, (LPARAM)&cbtc, unicode )) goto failed;

    /* send the WM_GETMINMAXINFO message and fix the size if needed */

    cx = cs->cx;
    cy = cs->cy;
    if ((cs->style & WS_THICKFRAME) || !(cs->style & (WS_POPUP | WS_CHILD)))
    {
        POINT maxSize, maxPos, minTrack, maxTrack;
        WINPOS_GetMinMaxInfo( hwnd, &maxSize, &maxPos, &minTrack, &maxTrack);
        if (maxTrack.x < cx) cx = maxTrack.x;
        if (maxTrack.y < cy) cy = maxTrack.y;
        if (minTrack.x > cx) cx = minTrack.x;
        if (minTrack.y > cy) cy = minTrack.y;
    }

    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    SetRect_k( &rect, cs->x, cs->y, cs->x + cx, cs->y + cy );
    /* check for wraparound */
    if (cs->x + cx < cs->x) rect.right = 0x7fffffff;
    if (cs->y + cy < cs->y) rect.bottom = 0x7fffffff;
    if (!set_window_pos( hwnd, 0, SWP_NOZORDER | SWP_NOACTIVATE, &rect, &rect, NULL )) goto failed;

    /* send WM_NCCREATE */

    TRACE( "hwnd %p cs %d,%d %dx%d\n", hwnd, cs->x, cs->y, cx, cy );
    if (unicode)
        result = SendMessageW_k( hwnd, WM_NCCREATE, 0, (LPARAM)cs );
    else
        result = SendMessageA_k( hwnd, WM_NCCREATE, 0, (LPARAM)cs );
    if (!result)
    {
        WARN( "%p: aborted by WM_NCCREATE\n", hwnd );
        goto failed;
    }

    /* send WM_NCCALCSIZE */

    if (WIN_GetRectangles( hwnd, COORDS_PARENT, &rect, NULL ))
    {
        /* yes, even if the CBT hook was called with HWND_TOP */
        HWND insert_after = (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD) ? HWND_BOTTOM : HWND_TOP;
        RECT client_rect = rect;

        /* the rectangle is in screen coords for WM_NCCALCSIZE when wparam is FALSE */
        MapWindowPoints_k( parent, 0, (POINT *)&client_rect, 2 );
        SendMessageW_k( hwnd, WM_NCCALCSIZE, FALSE, (LPARAM)&client_rect );
        MapWindowPoints_k( 0, parent, (POINT *)&client_rect, 2 );
        set_window_pos( hwnd, insert_after, SWP_NOACTIVATE, &rect, &client_rect, NULL );
    }
    else return 0;

    /* send WM_CREATE */

    if (unicode)
        result = SendMessageW_k( hwnd, WM_CREATE, 0, (LPARAM)cs );
    else
        result = SendMessageA_k( hwnd, WM_CREATE, 0, (LPARAM)cs );
    if (result == -1) goto failed;

    /* call the driver */

    if (!USER_Driver->pCreateWindow( hwnd )) goto failed;

    NotifyWinEvent_k(EVENT_OBJECT_CREATE, hwnd, OBJID_WINDOW, 0);

    /* send the size messages */

    if (!(wndPtr = WIN_GetPtr( hwnd )) || wndPtr == WND_DESKTOP) return 0;
    if (!(wndPtr->flags & WIN_NEED_SIZE))
    {
        WIN_GetRectangles( hwnd, COORDS_PARENT, NULL, &rect );
        SendMessageW_k( hwnd, WM_SIZE, SIZE_RESTORED,
                      MAKELONG(rect.right-rect.left, rect.bottom-rect.top));
        SendMessageW_k( hwnd, WM_MOVE, 0, MAKELONG( rect.left, rect.top ) );
    }

    /* Show the window, maximizing or minimizing if needed */

    style = WIN_SetStyle( hwnd, 0, WS_MAXIMIZE | WS_MINIMIZE );
    if (style & (WS_MINIMIZE | WS_MAXIMIZE))
    {
        RECT newPos;
        UINT swFlag = (style & WS_MINIMIZE) ? SW_MINIMIZE : SW_MAXIMIZE;

        swFlag = WINPOS_MinMaximize( hwnd, swFlag, &newPos );
        swFlag |= SWP_FRAMECHANGED; /* Frame always gets changed */
        if (!(style & WS_VISIBLE) || (style & WS_CHILD) || GetActiveWindow_k()) swFlag |= SWP_NOACTIVATE;
        SetWindowPos_k( hwnd, 0, newPos.left, newPos.top, newPos.right - newPos.left,
                      newPos.bottom - newPos.top, swFlag );
    }

    /* Notify the parent window only */

    send_parent_notify( hwnd, WM_CREATE );
    if (!IsWindow_k( hwnd )) return 0;

    if (cs->style & WS_VISIBLE)
    {
        if (cs->style & WS_MAXIMIZE)
            sw = SW_SHOW;
        else if (cs->style & WS_MINIMIZE)
            sw = SW_SHOWMINIMIZED;

        ShowWindow_k( hwnd, sw );
        if (cs->dwExStyle & WS_EX_MDICHILD)
        {
            SendMessageW_k(cs->hwndParent, WM_MDIREFRESHMENU, 0, 0);
            /* ShowWindow won't activate child windows */
            SetWindowPos_k( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );
        }
    }

    /* Call WH_SHELL hook */

    if (!(GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD) && !GetWindow_k( hwnd, GW_OWNER ))
        HOOK_CallHooks( WH_SHELL, HSHELL_WINDOWCREATED, (WPARAM)hwnd, 0, TRUE );

    TRACE("created window %p\n", hwnd);
    return hwnd;

failed:
    WIN_DestroyWindow( hwnd );
    return 0;
}


/***********************************************************************
 *		CreateWindowExA (USER32.@)
 */
HWND WINAPI CreateWindowExA_k( DWORD exStyle, LPCSTR className,
                                 LPCSTR windowName, DWORD style, INT x,
                                 INT y, INT width, INT height,
                                 HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID data )
{
    CREATESTRUCTA cs;

    cs.lpCreateParams = data;
    cs.hInstance      = instance;
    cs.hMenu          = menu;
    cs.hwndParent     = parent;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = width;
    cs.cy             = height;
    cs.style          = style;
    cs.lpszName       = windowName;
    cs.lpszClass      = className;
    cs.dwExStyle      = exStyle;

    if (!IS_INTRESOURCE(className))
    {
        WCHAR bufferW[256];
        if (!MultiByteToWideChar_k( CP_ACP, 0, className, -1, bufferW, sizeof(bufferW)/sizeof(WCHAR) ))
            return 0;
        return wow_handlers.create_window( (CREATESTRUCTW *)&cs, bufferW, instance, FALSE );
    }
    /* Note: we rely on the fact that CREATESTRUCTA and */
    /* CREATESTRUCTW have the same layout. */
    return wow_handlers.create_window( (CREATESTRUCTW *)&cs, (LPCWSTR)className, instance, FALSE );
}


/***********************************************************************
 *		CreateWindowExW (USER32.@)
 */
HWND WINAPI CreateWindowExW_k( DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, INT x,
                                 INT y, INT width, INT height,
                                 HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID data )
{
    CREATESTRUCTW cs;

    cs.lpCreateParams = data;
    cs.hInstance      = instance;
    cs.hMenu          = menu;
    cs.hwndParent     = parent;
    cs.x              = x;
    cs.y              = y;
    cs.cx             = width;
    cs.cy             = height;
    cs.style          = style;
    cs.lpszName       = windowName;
    cs.lpszClass      = className;
    cs.dwExStyle      = exStyle;

    return wow_handlers.create_window( &cs, className, instance, TRUE );
}


/***********************************************************************
 *           WIN_SendDestroyMsg
 */
static void WIN_SendDestroyMsg( HWND hwnd )
{
    GUITHREADINFO info;

    info.cbSize = sizeof(info);
    if (GetGUIThreadInfo_k( GetCurrentThreadId_k(), &info ))
    {
        if (hwnd == info.hwndCaret) DestroyCaret_k();
        if (hwnd == info.hwndActive) WINPOS_ActivateOtherWindow( hwnd );
    }

    /*
     * Send the WM_DESTROY to the window.
     */
    SendMessageW_k( hwnd, WM_DESTROY, 0, 0);

    /*
     * This WM_DESTROY message can trigger re-entrant calls to DestroyWindow
     * make sure that the window still exists when we come back.
     */
    if (IsWindow_k(hwnd))
    {
        HWND* pWndArray;
        int i;

        if (!(pWndArray = WIN_ListChildren( hwnd ))) return;

        for (i = 0; pWndArray[i]; i++)
        {
            if (IsWindow_k( pWndArray[i] )) WIN_SendDestroyMsg( pWndArray[i] );
        }
        HeapFree_k( GetProcessHeap_k(), 0, pWndArray );
    }
    else
      WARN("\tdestroyed itself while in WM_DESTROY!\n");
}


/***********************************************************************
 *		DestroyWindow (USER32.@)
 */
BOOL WINAPI DestroyWindow_k( HWND hwnd )
{
    BOOL is_child;

    if (!(hwnd = WIN_IsCurrentThread( hwnd )) || is_desktop_window( hwnd ))
    {
        SetLastError_k( ERROR_ACCESS_DENIED );
        return FALSE;
    }

    TRACE("(%p)\n", hwnd);

      /* Call hooks */

    if (HOOK_CallHooks( WH_CBT, HCBT_DESTROYWND, (WPARAM)hwnd, 0, TRUE )) return FALSE;

    if (MENU_IsMenuActive() == hwnd)
        EndMenu_k();

    is_child = (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD) != 0;

    if (is_child)
    {
        if (!USER_IsExitingThread( GetCurrentThreadId_k() ))
            send_parent_notify( hwnd, WM_DESTROY );
    }
    else if (!GetWindow_k( hwnd, GW_OWNER ))
    {
        HOOK_CallHooks( WH_SHELL, HSHELL_WINDOWDESTROYED, (WPARAM)hwnd, 0L, TRUE );
        /* FIXME: clean up palette - see "Internals" p.352 */
    }

    if (!IsWindow_k(hwnd)) return TRUE;

      /* Hide the window */
    if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_VISIBLE)
    {
        /* Only child windows receive WM_SHOWWINDOW in DestroyWindow() */
        if (is_child)
            ShowWindow_k( hwnd, SW_HIDE );
        else
            SetWindowPos_k( hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE |
                          SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW );
    }

    if (!IsWindow_k(hwnd)) return TRUE;

      /* Recursively destroy owned windows */

    if (!is_child)
    {
        for (;;)
        {
            int i, got_one = 0;
            HWND *list = WIN_ListChildren( GetDesktopWindow_k() );
            if (list)
            {
                for (i = 0; list[i]; i++)
                {
                    if (GetWindow_k( list[i], GW_OWNER ) != hwnd) continue;
                    if (WIN_IsCurrentThread( list[i] ))
                    {
                        DestroyWindow_k( list[i] );
                        got_one = 1;
                        continue;
                    }
                    WIN_SetOwner( list[i], 0 );
                }
                HeapFree_k( GetProcessHeap_k(), 0, list );
            }
            if (!got_one) break;
        }
    }

      /* Send destroy messages */

    WIN_SendDestroyMsg( hwnd );
    if (!IsWindow_k( hwnd )) return TRUE;

    if (GetClipboardOwner_k() == hwnd)
        CLIPBOARD_ReleaseOwner();

      /* Destroy the window storage */

    WIN_DestroyWindow( hwnd );
    return TRUE;
}


/***********************************************************************
 *		CloseWindow (USER32.@)
 */
BOOL WINAPI CloseWindow_k( HWND hwnd )
{
    if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD) return FALSE;
    ShowWindow_k( hwnd, SW_MINIMIZE );
    return TRUE;
}


/***********************************************************************
 *		OpenIcon (USER32.@)
 */
BOOL WINAPI OpenIcon_k( HWND hwnd )
{
    if (!IsIconic_k( hwnd )) return FALSE;
    ShowWindow_k( hwnd, SW_SHOWNORMAL );
    return TRUE;
}


/***********************************************************************
 *		FindWindowExW (USER32.@)
 */
HWND WINAPI FindWindowExW_k( HWND parent, HWND child, LPCWSTR className, LPCWSTR title )
{
    HWND *list = NULL;
    HWND retvalue = 0;
    int i = 0, len = 0;
    WCHAR *buffer = NULL;

    if (!parent && child) parent = GetDesktopWindow_k();
    else if (parent == HWND_MESSAGE) parent = get_hwnd_message_parent();

    if (title)
    {
        len = strlenW(title) + 1;  /* one extra char to check for chars beyond the end */
        if (!(buffer = HeapAlloc_k( GetProcessHeap_k(), 0, (len + 1) * sizeof(WCHAR) ))) return 0;
    }

    if (!(list = list_window_children( 0, parent, className, 0 ))) goto done;

    if (child)
    {
        child = WIN_GetFullHandle( child );
        while (list[i] && list[i] != child) i++;
        if (!list[i]) goto done;
        i++;  /* start from next window */
    }

    if (title)
    {
        while (list[i])
        {
            if (GetWindowTextW_k( list[i], buffer, len + 1 ))
            {
                if (!strcmpiW( buffer, title )) break;
            }
            else
            {
                if (!title[0]) break;
            }
            i++;
        }
    }
    retvalue = list[i];

 done:
    HeapFree_k( GetProcessHeap_k(), 0, list );
    HeapFree_k( GetProcessHeap_k(), 0, buffer );
    return retvalue;
}



/***********************************************************************
 *		FindWindowA (USER32.@)
 */
HWND WINAPI FindWindowA_k( LPCSTR className, LPCSTR title )
{
    HWND ret = FindWindowExA_k( 0, 0, className, title );
    if (!ret) SetLastError_k(ERROR_CANNOT_FIND_WND_CLASS);
    return ret;
}


/***********************************************************************
 *		FindWindowExA (USER32.@)
 */
HWND WINAPI FindWindowExA_k( HWND parent, HWND child, LPCSTR className, LPCSTR title )
{
    LPWSTR titleW = NULL;
    HWND hwnd = 0;

    if (title)
    {
        DWORD len = MultiByteToWideChar_k( CP_ACP, 0, title, -1, NULL, 0 );
        if (!(titleW = HeapAlloc_k( GetProcessHeap_k(), 0, len * sizeof(WCHAR) ))) return 0;
        MultiByteToWideChar_k( CP_ACP, 0, title, -1, titleW, len );
    }

    if (!IS_INTRESOURCE(className))
    {
        WCHAR classW[256];
        if (MultiByteToWideChar_k( CP_ACP, 0, className, -1, classW, sizeof(classW)/sizeof(WCHAR) ))
            hwnd = FindWindowExW_k( parent, child, classW, titleW );
    }
    else
    {
        hwnd = FindWindowExW_k( parent, child, (LPCWSTR)className, titleW );
    }

    HeapFree_k( GetProcessHeap_k(), 0, titleW );
    return hwnd;
}


/***********************************************************************
 *		FindWindowW (USER32.@)
 */
HWND WINAPI FindWindowW_k( LPCWSTR className, LPCWSTR title )
{
    return FindWindowExW_k( 0, 0, className, title );
}


/**********************************************************************
 *		GetDesktopWindow (USER32.@)
 */
HWND WINAPI GetDesktopWindow_k(void)
{
    return get_user_thread_info()->top_window;
}


/*******************************************************************
 *		EnableWindow (USER32.@)
 */
BOOL WINAPI EnableWindow_k( HWND hwnd, BOOL enable )
{
    BOOL retvalue;
    HWND full_handle;

    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (!(full_handle = WIN_IsCurrentThread( hwnd )))
        return SendMessageW_k( hwnd, WM_WINE_ENABLEWINDOW, enable, 0 );

    hwnd = full_handle;

    TRACE("( %p, %d )\n", hwnd, enable);

    retvalue = !IsWindowEnabled_k( hwnd );

    if (enable && retvalue)
    {
        WIN_SetStyle( hwnd, 0, WS_DISABLED );
        SendMessageW_k( hwnd, WM_ENABLE, TRUE, 0 );
    }
    else if (!enable && !retvalue)
    {
        HWND capture_wnd;

        SendMessageW_k( hwnd, WM_CANCELMODE, 0, 0);

        WIN_SetStyle( hwnd, WS_DISABLED, 0 );

        if (hwnd == GetFocus_k())
            SetFocus_k( 0 );  /* A disabled window can't have the focus */

        capture_wnd = GetCapture_k();
        if (hwnd == capture_wnd || IsChild_k(hwnd, capture_wnd))
            ReleaseCapture_k();  /* A disabled window can't capture the mouse */

        SendMessageW_k( hwnd, WM_ENABLE, FALSE, 0 );
    }
    return retvalue;
}


/***********************************************************************
 *		IsWindowEnabled (USER32.@)
 */
BOOL WINAPI IsWindowEnabled_k(HWND hWnd)
{
    return !(GetWindowLongW_k( hWnd, GWL_STYLE ) & WS_DISABLED);
}


/***********************************************************************
 *		IsWindowUnicode (USER32.@)
 */
BOOL WINAPI IsWindowUnicode_k( HWND hwnd )
{
    WND * wndPtr;

    if (!(wndPtr = WIN_GetPtr(hwnd))) return FALSE;

    if (wndPtr == WND_DESKTOP) return TRUE;

    return (wndPtr->flags & WIN_ISUNICODE) != 0;
}


/**********************************************************************
 *	     WIN_GetWindowLong
 *
 * Helper function for GetWindowLong().
 */
static LONG_PTR WIN_GetWindowLong( HWND hwnd, INT offset, UINT size, BOOL unicode )
{
    LONG_PTR retvalue = 0;
    WND *wndPtr;

    if (offset == GWLP_HWNDPARENT)
    {
        HWND parent = GetAncestor_k( hwnd, GA_PARENT );
        if (parent == GetDesktopWindow_k()) parent = GetWindow_k( hwnd, GW_OWNER );
        return (ULONG_PTR)parent;
    }

    if (!(wndPtr = WIN_GetPtr( hwnd )))
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }
    
    if (offset >= 0)
    {
        if (offset > (int)(wndPtr->cbWndExtra - size))
        {
            WARN("Invalid offset %d\n", offset );
            SetLastError_k( ERROR_INVALID_INDEX );
            return 0;
        }
        retvalue = get_win_data( (char *)wndPtr->wExtra + offset, size );

        /* Special case for dialog window procedure */
        if ((offset == DWLP_DLGPROC) && (size == sizeof(LONG_PTR)) && wndPtr->dlgInfo)
            retvalue = (LONG_PTR)WINPROC_GetProc( (WNDPROC)retvalue, unicode );
        return retvalue;
    }

    switch(offset)
    {
    case GWLP_USERDATA:  retvalue = wndPtr->userdata; break;
    case GWL_STYLE:      retvalue = wndPtr->dwStyle; break;
    case GWL_EXSTYLE:    retvalue = wndPtr->dwExStyle; break;
    case GWLP_ID:        retvalue = wndPtr->wIDmenu; break;
    case GWLP_HINSTANCE: retvalue = (ULONG_PTR)wndPtr->hInstance; break;
    case GWLP_WNDPROC:
        /* This looks like a hack only for the edit control (see tests). This makes these controls
         * more tolerant to A/W mismatches. The lack of W->A->W conversion for such a mismatch suggests
         * that the hack is in GetWindowLongPtr[AW], not in winprocs.
         */
        if (wndPtr->winproc == BUILTIN_WINPROC(WINPROC_EDIT) && (!unicode != !(wndPtr->flags & WIN_ISUNICODE)))
            retvalue = (ULONG_PTR)wndPtr->winproc;
        else
            retvalue = (ULONG_PTR)WINPROC_GetProc( wndPtr->winproc, unicode );
        break;
    default:
        WARN("Unknown offset %d\n", offset );
        SetLastError_k( ERROR_INVALID_INDEX );
        break;
    }
    return retvalue;
}


/**********************************************************************
 *	     WIN_SetWindowLong
 *
 * Helper function for SetWindowLong().
 *
 * 0 is the failure code. However, in the case of failure SetLastError
 * must be set to distinguish between a 0 return value and a failure.
 */
LONG_PTR WIN_SetWindowLong( HWND hwnd, INT offset, UINT size, LONG_PTR newval, BOOL unicode )
{
    STYLESTRUCT style;
    BOOL made_visible = FALSE;
    LONG_PTR retval = 0;
    WND *wndPtr;

    TRACE( "%p %d %lx %c\n", hwnd, offset, newval, unicode ? 'W' : 'A' );

    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    if (!(wndPtr = WIN_GetPtr( hwnd )))
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }
    if (wndPtr == WND_DESKTOP)
    {
        /* can't change anything on the desktop window */
        SetLastError_k( ERROR_ACCESS_DENIED );
        return 0;
    }

    /* first some special cases */
    switch( offset )
    {
    case GWL_STYLE:
        style.styleOld = wndPtr->dwStyle;
        style.styleNew = newval;
        SendMessageW_k( hwnd, WM_STYLECHANGING, GWL_STYLE, (LPARAM)&style );
        newval = style.styleNew;
        /* WS_CLIPSIBLINGS can't be reset on top-level windows */
        if (wndPtr->parent == GetDesktopWindow_k()) newval |= WS_CLIPSIBLINGS;
        /* FIXME: changing WS_DLGFRAME | WS_THICKFRAME is supposed to change
           WS_EX_WINDOWEDGE too */
        break;
    case GWL_EXSTYLE:
        style.styleOld = wndPtr->dwExStyle;
        style.styleNew = newval;
        SendMessageW_k( hwnd, WM_STYLECHANGING, GWL_EXSTYLE, (LPARAM)&style );
        /* WS_EX_TOPMOST can only be changed through SetWindowPos */
        newval = (style.styleNew & ~WS_EX_TOPMOST) | (wndPtr->dwExStyle & WS_EX_TOPMOST);
        /* WS_EX_WINDOWEDGE depends on some other styles */
        if (newval & WS_EX_DLGMODALFRAME)
            newval |= WS_EX_WINDOWEDGE;
        else if (!(newval & WS_EX_STATICEDGE) && (wndPtr->dwStyle & (WS_DLGFRAME | WS_THICKFRAME)))
            newval |= WS_EX_WINDOWEDGE;
        else
            newval &= ~WS_EX_WINDOWEDGE;
        break;
    case GWLP_HWNDPARENT:
        if (wndPtr->parent == GetDesktopWindow_k())
        {
            return (ULONG_PTR)WIN_SetOwner( hwnd, (HWND)newval );
        }
        else
        {
            return (ULONG_PTR)SetParent_k( hwnd, (HWND)newval );
        }
    case GWLP_WNDPROC:
    {
        WNDPROC proc;
        UINT old_flags = wndPtr->flags;
        retval = WIN_GetWindowLong( hwnd, offset, size, unicode );
        proc = WINPROC_AllocProc( (WNDPROC)newval, unicode );
        if (proc) wndPtr->winproc = proc;
        if (WINPROC_IsUnicode( proc, unicode )) wndPtr->flags |= WIN_ISUNICODE;
        else wndPtr->flags &= ~WIN_ISUNICODE;
        if (!((old_flags ^ wndPtr->flags) & WIN_ISUNICODE))
        {
            return retval;
        }
        /* update is_unicode flag on the server side */
        break;
    }
    case GWLP_ID:
    case GWLP_HINSTANCE:
    case GWLP_USERDATA:
        break;
    case DWLP_DLGPROC:
        if ((wndPtr->cbWndExtra - sizeof(LONG_PTR) >= DWLP_DLGPROC) &&
            (size == sizeof(LONG_PTR)) && wndPtr->dlgInfo)
        {
            WNDPROC *ptr = (WNDPROC *)((char *)wndPtr->wExtra + DWLP_DLGPROC);
            retval = (ULONG_PTR)WINPROC_GetProc( *ptr, unicode );
            *ptr = WINPROC_AllocProc( (WNDPROC)newval, unicode );
            return retval;
        }
        /* fall through */
    default:
        if (offset < 0 || offset > (int)(wndPtr->cbWndExtra - size))
        {
            WARN("Invalid offset %d\n", offset );
            SetLastError_k( ERROR_INVALID_INDEX );
            return 0;
        }
        else if (get_win_data( (char *)wndPtr->wExtra + offset, size ) == newval)
        {
            /* already set to the same value */
            return newval;
        }
        break;
    }

    if ((offset == GWL_STYLE && ((style.styleOld ^ style.styleNew) & WS_VISIBLE)) ||
        (offset == GWL_EXSTYLE && ((style.styleOld ^ style.styleNew) & WS_EX_LAYERED)))
    {
        made_visible = !(wndPtr->flags & WIN_HIDDEN) && (wndPtr->dwStyle & WS_VISIBLE);
        invalidate_dce( wndPtr, NULL );
    }

    if (offset == GWL_STYLE || offset == GWL_EXSTYLE)
    {
        style.styleOld = retval;
        style.styleNew = newval;
        USER_Driver->pSetWindowStyle( hwnd, offset, &style );
        if (made_visible) update_window_state( hwnd );
        SendMessageW_k( hwnd, WM_STYLECHANGED, offset, (LPARAM)&style );
    }

    return retval;
}


/**********************************************************************
 *		GetWindowWord (USER32.@)
 */
WORD WINAPI GetWindowWord_k( HWND hwnd, INT offset )
{
    switch(offset)
    {
    case GWLP_ID:
    case GWLP_HINSTANCE:
    case GWLP_HWNDPARENT:
        break;
    default:
        if (offset < 0)
        {
            WARN("Invalid offset %d\n", offset );
            SetLastError_k( ERROR_INVALID_INDEX );
            return 0;
        }
        break;
    }
    return (WORD)WIN_GetWindowLong( hwnd, offset, sizeof(WORD), FALSE );
}


/**********************************************************************
 *		GetWindowLongA (USER32.@)
 */
LONG WINAPI GetWindowLongA_k( HWND hwnd, INT offset )
{
    return WIN_GetWindowLong( hwnd, offset, sizeof(LONG), FALSE );
}


/**********************************************************************
 *		GetWindowLongW (USER32.@)
 */
LONG WINAPI GetWindowLongW_k( HWND hwnd, INT offset )
{
    return WIN_GetWindowLong( hwnd, offset, sizeof(LONG), TRUE );
}


/**********************************************************************
 *		SetWindowWord (USER32.@)
 */
WORD WINAPI SetWindowWord_k( HWND hwnd, INT offset, WORD newval )
{
    switch(offset)
    {
    case GWLP_ID:
    case GWLP_HINSTANCE:
    case GWLP_HWNDPARENT:
        break;
    default:
        if (offset < 0)
        {
            WARN("Invalid offset %d\n", offset );
            SetLastError_k( ERROR_INVALID_INDEX );
            return 0;
        }
        break;
    }
    return (WORD)WIN_SetWindowLong( hwnd, offset, sizeof(WORD), newval, FALSE );
}


/**********************************************************************
 *		SetWindowLongA (USER32.@)
 *
 * See SetWindowLongW.
 */
LONG WINAPI SetWindowLongA_k( HWND hwnd, INT offset, LONG newval )
{
    return WIN_SetWindowLong( hwnd, offset, sizeof(LONG), newval, FALSE );
}


/**********************************************************************
 *		SetWindowLongW (USER32.@) Set window attribute
 *
 * SetWindowLong() alters one of a window's attributes or sets a 32-bit (long)
 * value in a window's extra memory.
 *
 * The _hwnd_ parameter specifies the window.  is the handle to a
 * window that has extra memory. The _newval_ parameter contains the
 * new attribute or extra memory value.  If positive, the _offset_
 * parameter is the byte-addressed location in the window's extra
 * memory to set.  If negative, _offset_ specifies the window
 * attribute to set, and should be one of the following values:
 *
 * GWL_EXSTYLE      The window's extended window style
 *
 * GWL_STYLE        The window's window style.
 *
 * GWLP_WNDPROC     Pointer to the window's window procedure.
 *
 * GWLP_HINSTANCE   The window's pplication instance handle.
 *
 * GWLP_ID          The window's identifier.
 *
 * GWLP_USERDATA    The window's user-specified data.
 *
 * If the window is a dialog box, the _offset_ parameter can be one of
 * the following values:
 *
 * DWLP_DLGPROC     The address of the window's dialog box procedure.
 *
 * DWLP_MSGRESULT   The return value of a message
 *                  that the dialog box procedure processed.
 *
 * DWLP_USER        Application specific information.
 *
 * RETURNS
 *
 * If successful, returns the previous value located at _offset_. Otherwise,
 * returns 0.
 *
 * NOTES
 *
 * Extra memory for a window class is specified by a nonzero cbWndExtra
 * parameter of the WNDCLASS structure passed to RegisterClass() at the
 * time of class creation.
 *
 * Using GWL_WNDPROC to set a new window procedure effectively creates
 * a window subclass. Use CallWindowProc() in the new windows procedure
 * to pass messages to the superclass's window procedure.
 *
 * The user data is reserved for use by the application which created
 * the window.
 *
 * Do not use GWL_STYLE to change the window's WS_DISABLED style;
 * instead, call the EnableWindow() function to change the window's
 * disabled state.
 *
 * Do not use GWL_HWNDPARENT to reset the window's parent, use
 * SetParent() instead.
 *
 * Win95:
 * When offset is GWL_STYLE and the calling app's ver is 4.0,
 * it sends WM_STYLECHANGING before changing the settings
 * and WM_STYLECHANGED afterwards.
 * App ver 4.0 can't use SetWindowLong to change WS_EX_TOPMOST.
 */
LONG WINAPI SetWindowLongW_k(
    HWND hwnd,  /* [in] window to alter */
    INT offset, /* [in] offset, in bytes, of location to alter */
    LONG newval /* [in] new value of location */
) {
    return WIN_SetWindowLong( hwnd, offset, sizeof(LONG), newval, TRUE );
}


/*******************************************************************
 *		GetWindowTextA (USER32.@)
 */
INT WINAPI GetWindowTextA_k( HWND hwnd, LPSTR lpString, INT nMaxCount )
{
    WCHAR *buffer;
    WND* win;

    if (!lpString) return 0;

    if (WIN_IsCurrentProcess( hwnd ))
        return (INT)SendMessageA_k( hwnd, WM_GETTEXT, nMaxCount, (LPARAM)lpString );

    /* when window belongs to other process, don't send a message */
    if (nMaxCount <= 0) return 0;
    if (!(buffer = HeapAlloc_k( GetProcessHeap_k(), 0, nMaxCount * sizeof(WCHAR) ))) return 0;
    if (!(win = WIN_GetPtr( hwnd ))) return 0;
    if (win->text) lstrcpynW( buffer, win->text, nMaxCount );
    if (!WideCharToMultiByte_k( CP_ACP, 0, buffer, -1, lpString, nMaxCount, NULL, NULL ))
        lpString[nMaxCount-1] = 0;
    HeapFree_k( GetProcessHeap_k(), 0, buffer );
    return strlen(lpString);
}


/*******************************************************************
 *		InternalGetWindowText (USER32.@)
 */
INT WINAPI InternalGetWindowText_k(HWND hwnd,LPWSTR lpString,INT nMaxCount )
{
    WND *win;

    if (nMaxCount <= 0) return 0;
    if (!(win = WIN_GetPtr( hwnd ))) return 0;
    if (win == WND_DESKTOP) lpString[0] = 0;
    if (win->text) lstrcpynW( lpString, win->text, nMaxCount );
    else lpString[0] = 0;
    return strlenW(lpString);
}


/*******************************************************************
 *		GetWindowTextW (USER32.@)
 */
INT WINAPI GetWindowTextW_k( HWND hwnd, LPWSTR lpString, INT nMaxCount )
{
    WND *win;

    if (!lpString) return 0;

    if (WIN_IsCurrentProcess( hwnd ))
        return (INT)SendMessageW_k( hwnd, WM_GETTEXT, nMaxCount, (LPARAM)lpString );

    /* when window belongs to other process, don't send a message */
    if (nMaxCount <= 0) return 0;
    if (!(win = WIN_GetPtr( hwnd ))) return 0;
    if (win->text) lstrcpynW( lpString, win->text, nMaxCount );
    else lpString[0] = 0;
    return strlenW(lpString);
}


/*******************************************************************
 *		SetWindowTextA (USER32.@)
 *		SetWindowText  (USER32.@)
 */
BOOL WINAPI SetWindowTextA_k( HWND hwnd, LPCSTR lpString )
{
    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    if (!WIN_IsCurrentProcess( hwnd ))
        WARN( "setting text %s of other process window %p should not use SendMessage\n",
               debugstr_a(lpString), hwnd );
    return (BOOL)SendMessageA_k( hwnd, WM_SETTEXT, 0, (LPARAM)lpString );
}


/*******************************************************************
 *		SetWindowTextW (USER32.@)
 */
BOOL WINAPI SetWindowTextW_k( HWND hwnd, LPCWSTR lpString )
{
    if (is_broadcast(hwnd))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    if (!WIN_IsCurrentProcess( hwnd ))
        WARN( "setting text %s of other process window %p should not use SendMessage\n",
               debugstr_w(lpString), hwnd );
    return (BOOL)SendMessageW_k( hwnd, WM_SETTEXT, 0, (LPARAM)lpString );
}


/*******************************************************************
 *		GetWindowTextLengthA (USER32.@)
 */
INT WINAPI GetWindowTextLengthA_k( HWND hwnd )
{
    return SendMessageA_k( hwnd, WM_GETTEXTLENGTH, 0, 0 );
}

/*******************************************************************
 *		GetWindowTextLengthW (USER32.@)
 */
INT WINAPI GetWindowTextLengthW_k( HWND hwnd )
{
    return SendMessageW_k( hwnd, WM_GETTEXTLENGTH, 0, 0 );
}


/*******************************************************************
 *		IsWindow (USER32.@)
 */
BOOL WINAPI IsWindow_k( HWND hwnd )
{
    WND *ptr;

    if (!(ptr = WIN_GetPtr( hwnd ))) return FALSE;
    return TRUE;
}


/***********************************************************************
 *		GetWindowThreadProcessId (USER32.@)
 */
DWORD WINAPI GetWindowThreadProcessId_k( HWND hwnd, LPDWORD process )
{
    WND *ptr;
    DWORD tid = 0;

    if (!(ptr = WIN_GetPtr( hwnd )))
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }

    /* got a valid window */
    tid = ptr->tid;
    if (process) *process = ptr->pid;
    return tid;
}


/*****************************************************************
 *		GetParent (USER32.@)
 */
HWND WINAPI GetParent_k( HWND hwnd )
{
    WND *wndPtr;
    HWND retvalue = 0;

    if (!(wndPtr = WIN_GetPtr( hwnd )))
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }
    if (wndPtr == WND_DESKTOP) return 0;
    if (wndPtr->dwStyle & WS_POPUP) retvalue = wndPtr->owner;
    else if (wndPtr->dwStyle & WS_CHILD) retvalue = wndPtr->parent;
    return retvalue;
}


/*****************************************************************
 *		GetAncestor (USER32.@)
 */
HWND WINAPI GetAncestor_k( HWND hwnd, UINT type )
{
    WND *win;
    HWND *list, ret = 0;

    switch(type)
    {
    case GA_PARENT:
        if (!(win = WIN_GetPtr( hwnd )))
        {
            SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
            return 0;
        }
        if (win == WND_DESKTOP) return 0;
        ret = win->parent;
        break;

    case GA_ROOT:
        if (!(list = list_window_parents( hwnd ))) return 0;

        if (!list[0] || !list[1]) ret = WIN_GetFullHandle( hwnd );  /* top-level window */
        else
        {
            int count = 2;
            while (list[count]) count++;
            ret = list[count - 2];  /* get the one before the desktop */
        }
        HeapFree_k( GetProcessHeap_k(), 0, list );
        break;

    case GA_ROOTOWNER:
        if (is_desktop_window( hwnd )) return 0;
        ret = WIN_GetFullHandle( hwnd );
        for (;;)
        {
            HWND parent = GetParent_k( ret );
            if (!parent) break;
            ret = parent;
        }
        break;
    }
    return ret;
}


/*****************************************************************
 *		SetParent (USER32.@)
 */
HWND WINAPI SetParent_k( HWND hwnd, HWND parent )
{
    HWND full_handle;
    HWND old_parent = 0;
    BOOL was_visible;
    WND *wndPtr;
    POINT pt;

    if (is_broadcast(hwnd) || is_broadcast(parent))
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!parent) parent = GetDesktopWindow_k();
    else if (parent == HWND_MESSAGE) parent = get_hwnd_message_parent();
    else parent = WIN_GetFullHandle( parent );

    if (!IsWindow_k( parent ))
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }

    /* Some applications try to set a child as a parent */
    if (IsChild_k(hwnd, parent))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }

    if (!(full_handle = WIN_IsCurrentThread( hwnd )))
        return (HWND)SendMessageW_k( hwnd, WM_WINE_SETPARENT, (WPARAM)parent, 0 );

    if (full_handle == parent)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }

    /* Windows hides the window first, then shows it again
     * including the WM_SHOWWINDOW messages and all */
    was_visible = ShowWindow_k( hwnd, SW_HIDE );

    wndPtr = WIN_GetPtr( hwnd );
    if (!wndPtr || wndPtr == WND_DESKTOP) return 0;

    pt.x = wndPtr->rectWindow.left;
    pt.y = wndPtr->rectWindow.top;

    old_parent = wndPtr->parent;
    wndPtr->parent = parent;

    USER_Driver->pSetParent( full_handle, parent, old_parent );

    /* SetParent additionally needs to make hwnd the topmost window
       in the x-order and send the expected WM_WINDOWPOSCHANGING and
       WM_WINDOWPOSCHANGED notification messages.
    */
    SetWindowPos_k( hwnd, HWND_TOP, pt.x, pt.y, 0, 0, SWP_NOSIZE );

    if (was_visible) ShowWindow_k( hwnd, SW_SHOW );

    return old_parent;
}


/*******************************************************************
 *		IsChild (USER32.@)
 */
BOOL WINAPI IsChild_k( HWND parent, HWND child )
{
    HWND *list = list_window_parents( child );
    int i;
    BOOL ret;

    if (!list) return FALSE;
    parent = WIN_GetFullHandle( parent );
    for (i = 0; list[i]; i++) if (list[i] == parent) break;
    ret = list[i] && list[i+1];
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}


/***********************************************************************
 *		IsWindowVisible (USER32.@)
 */
BOOL WINAPI IsWindowVisible_k( HWND hwnd )
{
    HWND *list;
    BOOL retval = TRUE;
    int i;

    if (!(GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_VISIBLE)) return FALSE;
    if (!(list = list_window_parents( hwnd ))) return TRUE;
    if (list[0])
    {
        for (i = 0; list[i+1]; i++)
            if (!(GetWindowLongW_k( list[i], GWL_STYLE ) & WS_VISIBLE)) break;
        retval = !list[i+1] && (list[i] == GetDesktopWindow_k());  /* top message window isn't visible */
    }
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return retval;
}


/***********************************************************************
 *           WIN_IsWindowDrawable
 *
 * hwnd is drawable when it is visible, all parents are not
 * minimized, and it is itself not minimized unless we are
 * trying to draw its default class icon.
 */
BOOL WIN_IsWindowDrawable( HWND hwnd, BOOL icon )
{
    HWND *list;
    BOOL retval = TRUE;
    int i;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );

    if (!(style & WS_VISIBLE)) return FALSE;
    if ((style & WS_MINIMIZE) && icon && GetClassLongPtrW_k( hwnd, GCLP_HICON ))  return FALSE;

    if (!(list = list_window_parents( hwnd ))) return TRUE;
    if (list[0])
    {
        for (i = 0; list[i+1]; i++)
            if ((GetWindowLongW_k( list[i], GWL_STYLE ) & (WS_VISIBLE|WS_MINIMIZE)) != WS_VISIBLE)
                break;
        retval = !list[i+1] && (list[i] == GetDesktopWindow_k());  /* top message window isn't visible */
    }
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return retval;
}


/*******************************************************************
 *		GetTopWindow (USER32.@)
 */
HWND WINAPI GetTopWindow_k( HWND hwnd )
{
    if (!hwnd) hwnd = GetDesktopWindow_k();
    return GetWindow_k( hwnd, GW_CHILD );
}


/*******************************************************************
 *		GetWindow (USER32.@)
 */
HWND WINAPI GetWindow_k( HWND hwnd, UINT rel )
{
    WND *wndPtr = WIN_GetPtr( hwnd );
    WND *parentPtr;

    if (!wndPtr) {
        SetLastError_k( ERROR_INVALID_HANDLE );
        return 0;
    }
    parentPtr = WIN_GetPtr(wndPtr->parent);

    switch(rel)
    {
    case GW_HWNDFIRST:
        if (pblListSize(parentPtr->children))
            return (*(WND**)pblListGetFirst(parentPtr->children))->handle;
        return 0;
    case GW_HWNDLAST:
        if (pblListSize(parentPtr->children))
            return (*(WND**)pblListGetLast(parentPtr->children))->handle;
        return 0;
    case GW_HWNDNEXT:
        return pblListGet(parentPtr->children, pblListIndexOf(parentPtr->children, wndPtr)+1);
    case GW_HWNDPREV:
        return pblListGet(parentPtr->children, pblListIndexOf(parentPtr->children, wndPtr)-1);
    case GW_OWNER:
        return wndPtr->owner;
    case GW_CHILD:
        if (!pblListSize(wndPtr->children))
            return 0;
        return pblListGetFirst(wndPtr->children);
    }
    return 0;
}


/*******************************************************************
 *		ShowOwnedPopups (USER32.@)
 */
BOOL WINAPI ShowOwnedPopups_k( HWND owner, BOOL fShow )
{
    int count = 0;
    WND *pWnd;
    HWND *win_array = WIN_ListChildren( GetDesktopWindow_k() );

    if (!win_array) return TRUE;

    while (win_array[count]) count++;
    while (--count >= 0)
    {
        if (GetWindow_k( win_array[count], GW_OWNER ) != owner) continue;
        if (!(pWnd = WIN_GetPtr( win_array[count] ))) continue;
        if (fShow)
        {
            if (pWnd->flags & WIN_NEEDS_SHOW_OWNEDPOPUP)
            {
                /* In Windows, ShowOwnedPopups(TRUE) generates
                 * WM_SHOWWINDOW messages with SW_PARENTOPENING,
                 * regardless of the state of the owner
                 */
                SendMessageW_k(win_array[count], WM_SHOWWINDOW, SW_SHOWNORMAL, SW_PARENTOPENING);
                continue;
            }
        }
        else
        {
            if (pWnd->dwStyle & WS_VISIBLE)
            {
                /* In Windows, ShowOwnedPopups(FALSE) generates
                 * WM_SHOWWINDOW messages with SW_PARENTCLOSING,
                 * regardless of the state of the owner
                 */
                SendMessageW_k(win_array[count], WM_SHOWWINDOW, SW_HIDE, SW_PARENTCLOSING);
                continue;
            }
        }
    }
    HeapFree_k( GetProcessHeap_k(), 0, win_array );
    return TRUE;
}


/*******************************************************************
 *		GetLastActivePopup (USER32.@)
 */
HWND WINAPI GetLastActivePopup_k( HWND hwnd )
{
    WND *pWnd = WIN_GetPtr(hwnd);

    if (!pWnd) {
        SetLastError_k( ERROR_INVALID_HANDLE );
        return 0;
    }
    return pWnd->lastActive;
}


/*******************************************************************
 *           WIN_ListChildren
 *
 * Build an array of the children of a given window. The array must be
 * freed with HeapFree. Returns NULL when no windows are found.
 */
HWND *WIN_ListChildren( HWND hwnd )
{
    if (!hwnd)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return NULL;
    }
    return list_window_children( 0, hwnd, NULL, 0 );
}


/*******************************************************************
 *		EnumWindows (USER32.@)
 */
BOOL WINAPI EnumWindows_k( WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
    HWND *list;
    BOOL ret = TRUE;
    int i;

    USER_CheckNotLock();

    /* We have to build a list of all windows first, to avoid */
    /* unpleasant side-effects, for instance if the callback */
    /* function changes the Z-order of the windows.          */

    if (!(list = WIN_ListChildren( GetDesktopWindow_k() ))) return TRUE;

    /* Now call the callback function for every window */

    for (i = 0; list[i]; i++)
    {
        /* Make sure that the window still exists */
        if (!IsWindow_k( list[i] )) continue;
        if (!(ret = lpEnumFunc( list[i], lParam ))) break;
    }
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}


/**********************************************************************
 *		EnumThreadWindows (USER32.@)
 */
BOOL WINAPI EnumThreadWindows_k( DWORD id, WNDENUMPROC func, LPARAM lParam )
{
    HWND *list;
    int i;
    BOOL ret = TRUE;

    USER_CheckNotLock();

    if (!(list = list_window_children( 0, GetDesktopWindow_k(), NULL, id ))) return TRUE;

    /* Now call the callback function for every window */

    for (i = 0; list[i]; i++)
        if (!(ret = func( list[i], lParam ))) break;
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}


/***********************************************************************
 *              EnumDesktopWindows   (USER32.@)
 */
BOOL WINAPI EnumDesktopWindows_k( HDESK desktop, WNDENUMPROC func, LPARAM lparam )
{
    HWND *list;
    int i;

    USER_CheckNotLock();

    if (!(list = list_window_children( desktop, 0, NULL, 0 ))) return TRUE;

    for (i = 0; list[i]; i++)
        if (!func( list[i], lparam )) break;
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return TRUE;
}


/**********************************************************************
 *           WIN_EnumChildWindows
 *
 * Helper function for EnumChildWindows().
 */
static BOOL WIN_EnumChildWindows( HWND *list, WNDENUMPROC func, LPARAM lParam )
{
    HWND *childList;
    BOOL ret = FALSE;

    for ( ; *list; list++)
    {
        /* Make sure that the window still exists */
        if (!IsWindow_k( *list )) continue;
        /* Build children list first */
        childList = WIN_ListChildren( *list );

        ret = func( *list, lParam );

        if (childList)
        {
            if (ret) ret = WIN_EnumChildWindows( childList, func, lParam );
            HeapFree_k( GetProcessHeap_k(), 0, childList );
        }
        if (!ret) return FALSE;
    }
    return TRUE;
}


/**********************************************************************
 *		EnumChildWindows (USER32.@)
 */
BOOL WINAPI EnumChildWindows_k( HWND parent, WNDENUMPROC func, LPARAM lParam )
{
    HWND *list;
    BOOL ret;

    USER_CheckNotLock();

    if (!(list = WIN_ListChildren( parent ))) return FALSE;
    ret = WIN_EnumChildWindows( list, func, lParam );
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}


/*******************************************************************
 *		AnyPopup (USER32.@)
 */
BOOL WINAPI AnyPopup_k(void)
{
    int i;
    BOOL retvalue;
    HWND *list = WIN_ListChildren( GetDesktopWindow_k() );

    if (!list) return FALSE;
    for (i = 0; list[i]; i++)
    {
        if (IsWindowVisible_k( list[i] ) && GetWindow_k( list[i], GW_OWNER )) break;
    }
    retvalue = (list[i] != 0);
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return retvalue;
}


/*******************************************************************
 *		FlashWindow (USER32.@)
 */
BOOL WINAPI FlashWindow_k( HWND hWnd, BOOL bInvert )
{
    WND *wndPtr;

    TRACE("%p\n", hWnd);

    if (IsIconic_k( hWnd ))
    {
        RedrawWindow_k( hWnd, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_FRAME );

        wndPtr = WIN_GetPtr(hWnd);
        if (!wndPtr || wndPtr == WND_DESKTOP) return FALSE;
        if (bInvert && !(wndPtr->flags & WIN_NCACTIVATED))
        {
            wndPtr->flags |= WIN_NCACTIVATED;
        }
        else
        {
            wndPtr->flags &= ~WIN_NCACTIVATED;
        }
        return TRUE;
    }
    else
    {
        WPARAM wparam;

        wndPtr = WIN_GetPtr(hWnd);
        if (!wndPtr || wndPtr == WND_DESKTOP) return FALSE;
        hWnd = wndPtr->obj.handle;  /* make it a full handle */

        if (bInvert) wparam = !(wndPtr->flags & WIN_NCACTIVATED);
        else wparam = (hWnd == GetForegroundWindow_k());

        SendMessageW_k( hWnd, WM_NCACTIVATE, wparam, 0 );
        return wparam;
    }
}

/*******************************************************************
 *		FlashWindowEx (USER32.@)
 */
BOOL WINAPI FlashWindowEx_k( PFLASHWINFO pfwi )
{
    FIXME("%p\n", pfwi);
    return TRUE;
}

/*******************************************************************
 *		GetWindowContextHelpId (USER32.@)
 */
DWORD WINAPI GetWindowContextHelpId_k( HWND hwnd )
{
    DWORD retval;
    WND *wnd = WIN_GetPtr( hwnd );
    if (!wnd || wnd == WND_DESKTOP) return 0;
    retval = wnd->helpContext;
    return retval;
}


/*******************************************************************
 *		SetWindowContextHelpId (USER32.@)
 */
BOOL WINAPI SetWindowContextHelpId_k( HWND hwnd, DWORD id )
{
    WND *wnd = WIN_GetPtr( hwnd );
    if (!wnd || wnd == WND_DESKTOP) return FALSE;
    wnd->helpContext = id;
    return TRUE;
}


/*******************************************************************
 *		DragDetect (USER32.@)
 */
BOOL WINAPI DragDetect_k( HWND hWnd, POINT pt )
{
    MSG msg;
    RECT rect;
    WORD wDragWidth = GetSystemMetrics_k(SM_CXDRAG);
    WORD wDragHeight= GetSystemMetrics_k(SM_CYDRAG);

    rect.left = pt.x - wDragWidth;
    rect.right = pt.x + wDragWidth;

    rect.top = pt.y - wDragHeight;
    rect.bottom = pt.y + wDragHeight;

    SetCapture_k(hWnd);

    while(1)
    {
        while (PeekMessageW_k( &msg, 0, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ))
        {
            if( msg.message == WM_LBUTTONUP )
            {
                ReleaseCapture_k();
                return 0;
            }
            if( msg.message == WM_MOUSEMOVE )
            {
                POINT tmp;
                tmp.x = (short)LOWORD(msg.lParam);
                tmp.y = (short)HIWORD(msg.lParam);
                if( !PtInRect_k( &rect, tmp ))
                {
                    ReleaseCapture_k();
                    return 1;
                }
            }
        }
        WaitMessage_k();
    }
    return 0;
}

/******************************************************************************
 *		GetWindowModuleFileNameA (USER32.@)
 */
UINT WINAPI GetWindowModuleFileNameA_k( HWND hwnd, LPSTR module, UINT size )
{
    WND *win;
    HINSTANCE hinst;

    TRACE( "%p, %p, %u\n", hwnd, module, size );

    win = WIN_GetPtr( hwnd );
    if (!win || win == WND_DESKTOP)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }
    hinst = win->hInstance;

    return GetModuleFileNameA_k( hinst, module, size );
}

/******************************************************************************
 *		GetWindowModuleFileNameW (USER32.@)
 */
UINT WINAPI GetWindowModuleFileNameW_k( HWND hwnd, LPWSTR module, UINT size )
{
    WND *win;
    HINSTANCE hinst;

    TRACE( "%p, %p, %u\n", hwnd, module, size );

    win = WIN_GetPtr( hwnd );
    if (!win || win == WND_DESKTOP)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return 0;
    }
    hinst = win->hInstance;
    return GetModuleFileNameW_k( hinst, module, size );
}

/******************************************************************************
 *              GetWindowInfo (USER32.@)
 *
 * Note: tests show that Windows doesn't check cbSize of the structure.
 */
BOOL WINAPI GetWindowInfo_k( HWND hwnd, PWINDOWINFO pwi)
{
    if (!pwi) return FALSE;
    if (!WIN_GetRectangles( hwnd, COORDS_SCREEN, &pwi->rcWindow, &pwi->rcClient )) return FALSE;

    pwi->dwStyle = GetWindowLongW_k(hwnd, GWL_STYLE);
    pwi->dwExStyle = GetWindowLongW_k(hwnd, GWL_EXSTYLE);
    pwi->dwWindowStatus = ((GetActiveWindow_k() == hwnd) ? WS_ACTIVECAPTION : 0);

    pwi->cxWindowBorders = pwi->rcClient.left - pwi->rcWindow.left;
    pwi->cyWindowBorders = pwi->rcWindow.bottom - pwi->rcClient.bottom;

    pwi->atomWindowType = (ATOM)GetClassLongW_k( hwnd, GCW_ATOM );
    pwi->wCreatorVersion = 0x0400;

    return TRUE;
}

/******************************************************************************
 *              SwitchDesktop (USER32.@)
 *
 * NOTES: Sets the current input or interactive desktop.
 */
BOOL WINAPI SwitchDesktop_k( HDESK hDesktop)
{
    FIXME("(hwnd %p) stub!\n", hDesktop);
    return TRUE;
}


/***********************************************************************
 *           __wine_set_pixel_format
 */
BOOL CDECL __wine_set_pixel_format( HWND hwnd, int format )
{
    WND *win = WIN_GetPtr( hwnd );

    if (!win || win == WND_DESKTOP)
    {
        WARN( "setting format %d on win %p not supported\n", format, hwnd );
        return FALSE;
    }
    win->pixel_format = format;

    update_window_state( hwnd );
    return TRUE;
}


/*****************************************************************************
 *              SetLayeredWindowAttributes (USER32.@)
 */
BOOL WINAPI SetLayeredWindowAttributes_k( HWND hwnd, COLORREF key, BYTE alpha, DWORD flags )
{
    WND *win;

    win = WIN_GetPtr( hwnd );
    if (!win || win == WND_DESKTOP)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    win->color_key = key;
    win->alpha = alpha;
    win->layered_flags = flags; 

    USER_Driver->pSetLayeredWindowAttributes( hwnd, key, alpha, flags );

    return TRUE;
}


/*****************************************************************************
 *              GetLayeredWindowAttributes (USER32.@)
 */
BOOL WINAPI GetLayeredWindowAttributes_k( HWND hwnd, COLORREF *key, BYTE *alpha, DWORD *flags )
{
    WND *win;

    win = WIN_GetPtr( hwnd );
    if (!win || win == WND_DESKTOP)
    {
        SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        return FALSE;
    }
    if (key) *key = win->color_key;
    if (alpha) *alpha = win->alpha;
    if (flags) *flags = win->layered_flags;

    return TRUE;
}


/*****************************************************************************
 *              UpdateLayeredWindowIndirect  (USER32.@)
 */
BOOL WINAPI UpdateLayeredWindowIndirect_k( HWND hwnd, const UPDATELAYEREDWINDOWINFO *info )
{
    DWORD flags = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW;
    RECT window_rect, client_rect;
    SIZE offset;

    if (!info ||
        info->cbSize != sizeof(*info) ||
        info->dwFlags & ~(ULW_COLORKEY | ULW_ALPHA | ULW_OPAQUE | ULW_EX_NORESIZE) ||
        !(GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_LAYERED) ||
        GetLayeredWindowAttributes_k( hwnd, NULL, NULL, NULL ))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    WIN_GetRectangles( hwnd, COORDS_PARENT, &window_rect, &client_rect );

    if (info->pptDst)
    {
        offset.cx = info->pptDst->x - window_rect.left;
        offset.cy = info->pptDst->y - window_rect.top;
        OffsetRect_k( &client_rect, offset.cx, offset.cy );
        OffsetRect_k( &window_rect, offset.cx, offset.cy );
        flags &= ~SWP_NOMOVE;
    }
    if (info->psize)
    {
        offset.cx = info->psize->cx - (window_rect.right - window_rect.left);
        offset.cy = info->psize->cy - (window_rect.bottom - window_rect.top);
        if (info->psize->cx <= 0 || info->psize->cy <= 0)
        {
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return FALSE;
        }
        if ((info->dwFlags & ULW_EX_NORESIZE) && (offset.cx || offset.cy))
        {
            SetLastError_k( ERROR_INCORRECT_SIZE );
            return FALSE;
        }
        client_rect.right  += offset.cx;
        client_rect.bottom += offset.cy;
        window_rect.right  += offset.cx;
        window_rect.bottom += offset.cy;
        flags &= ~SWP_NOSIZE;
    }

    if (!USER_Driver->pUpdateLayeredWindow( hwnd, info, &window_rect )) return FALSE;

    set_window_pos( hwnd, 0, flags, &window_rect, &client_rect, NULL );
    return TRUE;
}


/*****************************************************************************
 *              UpdateLayeredWindow (USER32.@)
 */
BOOL WINAPI UpdateLayeredWindow_k( HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize,
                                 HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend,
                                 DWORD flags)
{
    UPDATELAYEREDWINDOWINFO info;

    if (flags & ULW_EX_NORESIZE)  /* only valid for UpdateLayeredWindowIndirect */
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }
    info.cbSize   = sizeof(info);
    info.hdcDst   = hdcDst;
    info.pptDst   = pptDst;
    info.psize    = psize;
    info.hdcSrc   = hdcSrc;
    info.pptSrc   = pptSrc;
    info.crKey    = crKey;
    info.pblend   = pblend;
    info.dwFlags  = flags;
    info.prcDirty = NULL;
    return UpdateLayeredWindowIndirect_k( hwnd, &info );
}


/******************************************************************************
 *                    GetProcessDefaultLayout [USER32.@]
 *
 * Gets the default layout for parentless windows.
 */
BOOL WINAPI GetProcessDefaultLayout_k( DWORD *layout )
{
    struct KProcess* process = currentProcess();

    if (!layout)
    {
        SetLastError_k( ERROR_NOACCESS );
        return FALSE;
    }
    /*
    if (process->process_layout == ~0u)
    {
        static const WCHAR translationW[] = { '\\','V','a','r','F','i','l','e','I','n','f','o',
                                              '\\','T','r','a','n','s','l','a','t','i','o','n', 0 };
        static const WCHAR filedescW[] = { '\\','S','t','r','i','n','g','F','i','l','e','I','n','f','o',
                                           '\\','%','0','4','x','%','0','4','x',
                                           '\\','F','i','l','e','D','e','s','c','r','i','p','t','i','o','n',0 };
        WCHAR *str, buffer[MAX_PATH];
        DWORD i, len, version_layout = 0;
        DWORD user_lang = GetUserDefaultLangID();
        DWORD *languages;
        void *data = NULL;

        GetModuleFileNameW_k( 0, buffer, MAX_PATH );
        if (!(len = GetFileVersionInfoSizeW_k( buffer, NULL ))) goto done;
        if (!(data = HeapAlloc_k( GetProcessHeap_k(), 0, len ))) goto done;
        if (!GetFileVersionInfoW_k( buffer, 0, len, data )) goto done;
        if (!VerQueryValueW_k( data, translationW, (void **)&languages, &len ) || !len) goto done;

        len /= sizeof(DWORD);
        for (i = 0; i < len; i++) if (LOWORD(languages[i]) == user_lang) break;
        if (i == len)  // try neutral language 
            for (i = 0; i < len; i++)
                if (LOWORD(languages[i]) == MAKELANGID( PRIMARYLANGID(user_lang), SUBLANG_NEUTRAL )) break;
        if (i == len) i = 0;  // default to the first one 

        sprintfW( buffer, filedescW, LOWORD(languages[i]), HIWORD(languages[i]) );
        if (!VerQueryValueW_k( data, buffer, (void **)&str, &len )) goto done;
        TRACE( "found description %s\n", debugstr_w( str ));
        if (str[0] == 0x200e && str[1] == 0x200e) version_layout = LAYOUT_RTL;

    done:
        HeapFree_k( GetProcessHeap_k(), 0, data );
        process->process_layout = version_layout;
    }
    */
    *layout = process->process_layout;
    return TRUE;
}


/******************************************************************************
 *                    SetProcessDefaultLayout [USER32.@]
 *
 * Sets the default layout for parentless windows.
 */
BOOL WINAPI SetProcessDefaultLayout_k( DWORD layout )
{
    currentProcess()->process_layout = layout;
    return TRUE;
}


/* 64bit versions */

#ifdef GetWindowLongPtrW
#undef GetWindowLongPtrW
#endif

#ifdef GetWindowLongPtrA
#undef GetWindowLongPtrA
#endif

#ifdef SetWindowLongPtrW
#undef SetWindowLongPtrW
#endif

#ifdef SetWindowLongPtrA
#undef SetWindowLongPtrA
#endif

/*****************************************************************************
 *              GetWindowLongPtrW (USER32.@)
 */
LONG_PTR WINAPI GetWindowLongPtrW_k( HWND hwnd, INT offset )
{
    return WIN_GetWindowLong( hwnd, offset, sizeof(LONG_PTR), TRUE );
}

/*****************************************************************************
 *              GetWindowLongPtrA (USER32.@)
 */
LONG_PTR WINAPI GetWindowLongPtrA_k( HWND hwnd, INT offset )
{
    return WIN_GetWindowLong( hwnd, offset, sizeof(LONG_PTR), FALSE );
}

/*****************************************************************************
 *              SetWindowLongPtrW (USER32.@)
 */
LONG_PTR WINAPI SetWindowLongPtrW_k( HWND hwnd, INT offset, LONG_PTR newval )
{
    return WIN_SetWindowLong( hwnd, offset, sizeof(LONG_PTR), newval, TRUE );
}

/*****************************************************************************
 *              SetWindowLongPtrA (USER32.@)
 */
LONG_PTR WINAPI SetWindowLongPtrA_k( HWND hwnd, INT offset, LONG_PTR newval )
{
    return WIN_SetWindowLong( hwnd, offset, sizeof(LONG_PTR), newval, FALSE );
}
