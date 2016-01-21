/*
 * Focus and activation functions
 *
 * Copyright 1993 David Metcalfe
 * Copyright 1995 Alex Korobka
 * Copyright 1994, 2002 Alexandre Julliard
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
#include "win.h"
#include "user_private.h"
#include "kthread.h"

struct thread_input* foreground_input;

static HWND shell_window;
static HWND shell_listview;
static HWND progman_window;
static HWND taskman_window;

/*****************************************************************
 *		set_focus_window
 *
 * Change the focus window, sending the WM_SETFOCUS and WM_KILLFOCUS messages
 */
static HWND set_focus_window( HWND hwnd )
{
    HWND previous = GetFocus_k();
    WND* wnd = WIN_GetPtr(hwnd);
    struct KThread* thread = currentThread();

    if (previous == hwnd) return previous;

    if (hwnd && !wnd) {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return 0;
    }
    if (hwnd && wnd->tid!=thread->id) {
        SetLastError_k(ERROR_ACCESS_DENIED);
        return 0;
    }
    thread->input->focus = hwnd;
    if (previous)
    {
        SendMessageW_k( previous, WM_KILLFOCUS, (WPARAM)hwnd, 0 );
        if (hwnd != GetFocus_k()) return previous;  /* changed by the message */
    }
    if (IsWindow_k(hwnd))
    {
        USER_Driver->pSetFocus(hwnd);
        SendMessageW_k( hwnd, WM_SETFOCUS, (WPARAM)previous, 0 );
    }
    return previous;
}


/*******************************************************************
 *		set_active_window
 */
static BOOL set_active_window( HWND hwnd, HWND *prev, BOOL mouse, BOOL focus )
{
    HWND previous = GetActiveWindow_k();
    DWORD old_thread, new_thread;
    CBTACTIVATESTRUCT cbt;
    WND* wnd = WIN_GetPtr(hwnd);

    if (previous == hwnd)
    {
        if (prev) *prev = hwnd;
        return TRUE;
    }

    /* call CBT hook chain */
    cbt.fMouse     = mouse;
    cbt.hWndActive = previous;
    if (HOOK_CallHooks( WH_CBT, HCBT_ACTIVATE, (WPARAM)hwnd, (LPARAM)&cbt, TRUE )) return FALSE;

    if (IsWindow_k(previous))
    {
        SendMessageW_k( previous, WM_NCACTIVATE, FALSE, (LPARAM)hwnd );
        SendMessageW_k( previous, WM_ACTIVATE,
                      MAKEWPARAM( WA_INACTIVE, IsIconic_k(previous) ), (LPARAM)hwnd );
    }
    if (!wnd)
        return FALSE;

    foreground_input = getThread(wnd->tid)->input;
    if (prev) *prev = previous;
    if (previous == hwnd) return TRUE;

    if (hwnd)
    {
        /* send palette messages */
        if (SendMessageW_k( hwnd, WM_QUERYNEWPALETTE, 0, 0 ))
            SendMessageTimeoutW_k( HWND_BROADCAST, WM_PALETTEISCHANGING, (WPARAM)hwnd, 0,
                                 SMTO_ABORTIFHUNG, 2000, NULL );
        if (!IsWindow_k(hwnd)) return FALSE;
    }

    old_thread = previous ? GetWindowThreadProcessId_k( previous, NULL ) : 0;
    new_thread = hwnd ? GetWindowThreadProcessId_k( hwnd, NULL ) : 0;

    if (old_thread != new_thread)
    {
        HWND *list, *phwnd;

        if ((list = WIN_ListChildren( GetDesktopWindow_k() )))
        {
            if (old_thread)
            {
                for (phwnd = list; *phwnd; phwnd++)
                {
                    if (GetWindowThreadProcessId_k( *phwnd, NULL ) == old_thread)
                        SendMessageW_k( *phwnd, WM_ACTIVATEAPP, 0, new_thread );
                }
            }
            if (new_thread)
            {
                for (phwnd = list; *phwnd; phwnd++)
                {
                    if (GetWindowThreadProcessId_k( *phwnd, NULL ) == new_thread)
                        SendMessageW_k( *phwnd, WM_ACTIVATEAPP, 1, old_thread );
                }
            }
            HeapFree_k( GetProcessHeap_k(), 0, list );
        }
    }

    if (IsWindow_k(hwnd))
    {
        SendMessageW_k( hwnd, WM_NCACTIVATE, (hwnd == GetForegroundWindow_k()), (LPARAM)previous );
        SendMessageW_k( hwnd, WM_ACTIVATE,
                      MAKEWPARAM( mouse ? WA_CLICKACTIVE : WA_ACTIVE, IsIconic_k(hwnd) ),
                      (LPARAM)previous );
    }

    /* now change focus if necessary */
    if (focus)
    {
        GUITHREADINFO info;

        info.cbSize = sizeof(info);
        GetGUIThreadInfo_k( GetCurrentThreadId_k(), &info );
        /* Do not change focus if the window is no more active */
        if (hwnd == info.hwndActive)
        {
            if (!info.hwndFocus || !hwnd || GetAncestor_k( info.hwndFocus, GA_ROOT ) != hwnd)
                set_focus_window( hwnd );
        }
    }

    return TRUE;
}


/*******************************************************************
 *		set_foreground_window
 */
static BOOL set_foreground_window( HWND hwnd, BOOL mouse )
{
    WND* wnd = WIN_GetPtr(hwnd);
    if (!wnd || GetDesktopWindow_k()==hwnd || wnd->parent==GetDesktopWindow_k()) {
        SetLastError_k(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    if (!foreground_input || hwnd!=foreground_input->active) {
        HWND previous = 0;

        if (foreground_input)
            previous = foreground_input->active;
        return set_active_window( hwnd, NULL, mouse, TRUE );
    }
    return TRUE;
}


/*******************************************************************
 *		FOCUS_MouseActivate
 *
 * Activate a window as a result of a mouse click
 */
BOOL FOCUS_MouseActivate( HWND hwnd )
{
    return set_foreground_window( hwnd, TRUE );
}


/*******************************************************************
 *		SetActiveWindow (USER32.@)
 */
HWND WINAPI SetActiveWindow_k( HWND hwnd )
{
    HWND prev;

    TRACE( "%p\n", hwnd );

    if (hwnd)
    {
        LONG style;

        hwnd = WIN_GetFullHandle( hwnd );
        if (!IsWindow_k( hwnd ))
        {
            SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
            return 0;
        }

        style = GetWindowLongW_k( hwnd, GWL_STYLE );
        if ((style & (WS_POPUP|WS_CHILD)) == WS_CHILD)
            return GetActiveWindow_k();  /* Windows doesn't seem to return an error here */
    }

    if (!set_active_window( hwnd, &prev, FALSE, TRUE )) return 0;
    return prev;
}


/*****************************************************************
 *		SetFocus  (USER32.@)
 */
HWND WINAPI SetFocus_k( HWND hwnd )
{
    HWND hwndTop = hwnd;
    HWND previous = GetFocus_k();

    TRACE( "%p prev %p\n", hwnd, previous );

    if (hwnd)
    {
        /* Check if we can set the focus to this window */
        hwnd = WIN_GetFullHandle( hwnd );
        if (!IsWindow_k( hwnd ))
        {
            SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
            return 0;
        }
        if (hwnd == previous) return previous;  /* nothing to do */
        for (;;)
        {
            HWND parent;
            LONG style = GetWindowLongW_k( hwndTop, GWL_STYLE );
            if (style & (WS_MINIMIZE | WS_DISABLED)) return 0;
            parent = GetAncestor_k( hwndTop, GA_PARENT );
            if (!parent || parent == GetDesktopWindow_k())
            {
                if ((style & (WS_POPUP|WS_CHILD)) == WS_CHILD) return 0;
                break;
            }
            if (parent == get_hwnd_message_parent()) return 0;
            hwndTop = parent;
        }

        /* call hooks */
        if (HOOK_CallHooks( WH_CBT, HCBT_SETFOCUS, (WPARAM)hwnd, (LPARAM)previous, TRUE )) return 0;

        /* activate hwndTop if needed. */
        if (hwndTop != GetActiveWindow_k())
        {
            if (!set_active_window( hwndTop, NULL, FALSE, FALSE )) return 0;
            if (!IsWindow_k( hwnd )) return 0;  /* Abort if window destroyed */

            /* Do not change focus if the window is no longer active */
            if (hwndTop != GetActiveWindow_k()) return 0;
        }
    }
    else /* NULL hwnd passed in */
    {
        if (!previous) return 0;  /* nothing to do */
        if (HOOK_CallHooks( WH_CBT, HCBT_SETFOCUS, 0, (LPARAM)previous, TRUE )) return 0;
    }

    /* change focus and send messages */
    return set_focus_window( hwnd );
}


/*******************************************************************
 *		SetForegroundWindow  (USER32.@)
 */
BOOL WINAPI SetForegroundWindow_k( HWND hwnd )
{
    TRACE( "%p\n", hwnd );

    hwnd = WIN_GetFullHandle( hwnd );
    return set_foreground_window( hwnd, FALSE );
}


/*******************************************************************
 *		GetActiveWindow  (USER32.@)
 */
HWND WINAPI GetActiveWindow_k(void)
{
    struct thread_input* input = currentThread()->input;
    return input->active;
}


/*****************************************************************
 *		GetFocus  (USER32.@)
 */
HWND WINAPI GetFocus_k(void)
{
    struct thread_input* input = currentThread()->input;
    return input->focus;
}


/*******************************************************************
 *		GetForegroundWindow  (USER32.@)
 */
HWND WINAPI GetForegroundWindow_k(void)
{
    if (foreground_input)
        return foreground_input->active;
    return 0;
}


/***********************************************************************
*		SetShellWindowEx (USER32.@)
* hwndShell =    Progman[Program Manager]
*                |-> SHELLDLL_DefView
* hwndListView = |   |-> SysListView32
*                |   |   |-> tooltips_class32
*                |   |
*                |   |-> SysHeader32
*                |
*                |-> ProxyTarget
*/
BOOL WINAPI SetShellWindowEx_k(HWND hwndShell, HWND hwndListView)
{
    if (GetShellWindow_k())
        return FALSE;

    if (GetWindowLongW_k(hwndShell, GWL_EXSTYLE) & WS_EX_TOPMOST)
        return FALSE;

    if (hwndListView != hwndShell)
        if (GetWindowLongW_k(hwndListView, GWL_EXSTYLE) & WS_EX_TOPMOST)
            return FALSE;

    if (hwndListView && hwndListView!=hwndShell)
        SetWindowPos_k(hwndListView, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

    SetWindowPos_k(hwndShell, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

    shell_window = hwndShell;
    shell_listview = hwndListView;

    return TRUE;
}


/*******************************************************************
*		SetShellWindow (USER32.@)
*/
BOOL WINAPI SetShellWindow_k(HWND hwndShell)
{
    return SetShellWindowEx_k(hwndShell, hwndShell);
}


/*******************************************************************
*		GetShellWindow (USER32.@)
*/
HWND WINAPI GetShellWindow_k(void)
{
    return shell_window;
}


/***********************************************************************
 *		SetProgmanWindow (USER32.@)
 */
HWND WINAPI SetProgmanWindow_k( HWND hwnd )
{
    progman_window = hwnd;
    return hwnd;
}


/***********************************************************************
 *		GetProgmanWindow (USER32.@)
 */
HWND WINAPI GetProgmanWindow_k(void)
{
    return progman_window;
}


/***********************************************************************
 *		SetTaskmanWindow (USER32.@)
 * NOTES
 *   hwnd = MSTaskSwWClass
 *          |-> SysTabControl32
 */
HWND WINAPI SetTaskmanWindow_k( HWND hwnd )
{
    taskman_window = hwnd;
    return hwnd;
}

/***********************************************************************
 *		GetTaskmanWindow (USER32.@)
 */
HWND WINAPI GetTaskmanWindow_k(void)
{
    return taskman_window;
}
