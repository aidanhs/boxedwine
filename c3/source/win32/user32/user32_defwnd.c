/*
 * Default window procedure
 *
 * Copyright 1993, 1996 Alexandre Julliard
 *	     1995 Alex Korobka
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

#include <string.h>
#include <stdarg.h>

#include "../include/winbase.h"
#include "win.h"
#include "user_private.h"
#include "controls.h"
#include "../include/unicode.h"

  /* bits in the dwKeyData */
#define KEYDATA_ALT             0x2000
#define KEYDATA_PREVSTATE       0x4000

#define DRAG_FILE  0x454C4946

static short iF10Key = 0;
static short iMenuSysKey = 0;
static const WCHAR imm32W[] = { 'i','m','m','3','2','\0' };

/***********************************************************************
 *           DEFWND_HandleWindowPosChanged
 *
 * Handle the WM_WINDOWPOSCHANGED message.
 */
static void DEFWND_HandleWindowPosChanged( HWND hwnd, const WINDOWPOS *winpos )
{
    RECT rect;

    WIN_GetRectangles( hwnd, COORDS_PARENT, NULL, &rect );
    if (!(winpos->flags & SWP_NOCLIENTMOVE))
        SendMessageW_k( hwnd, WM_MOVE, 0, MAKELONG(rect.left, rect.top));

    if (!(winpos->flags & SWP_NOCLIENTSIZE) || (winpos->flags & SWP_STATECHANGED))
    {
        if (IsIconic_k( hwnd ))
        {
            SendMessageW_k( hwnd, WM_SIZE, SIZE_MINIMIZED, 0 );
        }
        else
        {
            WPARAM wp = IsZoomed_k( hwnd ) ? SIZE_MAXIMIZED : SIZE_RESTORED;

            SendMessageW_k( hwnd, WM_SIZE, wp, MAKELONG(rect.right-rect.left, rect.bottom-rect.top) );
        }
    }
}


/***********************************************************************
 *           DEFWND_SetTextA
 *
 * Set the window text.
 */
static void DEFWND_SetTextA( HWND hwnd, LPCSTR text )
{
    int count;
    WCHAR *textW;
    WND *wndPtr;

    if (!text) text = "";
    count = MultiByteToWideChar_k( CP_ACP, 0, text, -1, NULL, 0 );

    if (!(wndPtr = WIN_GetPtr( hwnd ))) return;
    if ((textW = HeapAlloc_k(GetProcessHeap_k(), 0, count * sizeof(WCHAR))))
    {
        HeapFree_k(GetProcessHeap_k(), 0, wndPtr->text);
        wndPtr->text = textW;
        MultiByteToWideChar_k( CP_ACP, 0, text, -1, textW, count );
    }
    else
        ERR("Not enough memory for window text\n");

    USER_Driver->pSetWindowText( hwnd, textW );
}

/***********************************************************************
 *           DEFWND_SetTextW
 *
 * Set the window text.
 */
static void DEFWND_SetTextW( HWND hwnd, LPCWSTR text )
{
    static const WCHAR empty_string[] = {0};
    WND *wndPtr;
    int count;

    if (!text) text = empty_string;
    count = strlenW(text) + 1;

    if (!(wndPtr = WIN_GetPtr( hwnd ))) return;
    HeapFree_k(GetProcessHeap_k(), 0, wndPtr->text);
    if ((wndPtr->text = HeapAlloc_k(GetProcessHeap_k(), 0, count * sizeof(WCHAR))))
    {
        strcpyW( wndPtr->text, text );
    }
    else
        ERR("Not enough memory for window text\n");
    text = wndPtr->text;

    USER_Driver->pSetWindowText( hwnd, text );
}

/***********************************************************************
 *           DEFWND_ControlColor
 *
 * Default colors for control painting.
 */
HBRUSH DEFWND_ControlColor( HDC hDC, UINT ctlType )
{
    if( ctlType == CTLCOLOR_SCROLLBAR)
    {
        HBRUSH hb = GetSysColorBrush_k(COLOR_SCROLLBAR);
        COLORREF bk = GetSysColor_k(COLOR_3DHILIGHT);
        SetTextColor_k( hDC, GetSysColor_k(COLOR_3DFACE));
        SetBkColor_k( hDC, bk);

        /* if COLOR_WINDOW happens to be the same as COLOR_3DHILIGHT
         * we better use 0x55aa bitmap brush to make scrollbar's background
         * look different from the window background.
         */
        if (bk == GetSysColor_k(COLOR_WINDOW))
            return getSYSCOLOR_55AABrush();

        UnrealizeObject_k( hb );
        return hb;
    }

    SetTextColor_k( hDC, GetSysColor_k(COLOR_WINDOWTEXT));

    if ((ctlType == CTLCOLOR_EDIT) || (ctlType == CTLCOLOR_LISTBOX))
        SetBkColor_k( hDC, GetSysColor_k(COLOR_WINDOW) );
    else {
        SetBkColor_k( hDC, GetSysColor_k(COLOR_3DFACE) );
        return GetSysColorBrush_k(COLOR_3DFACE);
    }
    return GetSysColorBrush_k(COLOR_WINDOW);
}


/***********************************************************************
 *           DEFWND_Print
 *
 * This method handles the default behavior for the WM_PRINT message.
 */
static void DEFWND_Print( HWND hwnd, HDC hdc, ULONG uFlags)
{
  /*
   * Visibility flag.
   */
  if ( (uFlags & PRF_CHECKVISIBLE) &&
       !IsWindowVisible_k(hwnd) )
      return;

  /*
   * Unimplemented flags.
   */
  if ( (uFlags & PRF_CHILDREN) ||
       (uFlags & PRF_OWNED)    ||
       (uFlags & PRF_NONCLIENT) )
  {
    WARN("WM_PRINT message with unsupported flags\n");
  }

  /*
   * Background
   */
  if ( uFlags & PRF_ERASEBKGND)
    SendMessageW_k(hwnd, WM_ERASEBKGND, (WPARAM)hdc, 0);

  /*
   * Client area
   */
  if ( uFlags & PRF_CLIENT)
    SendMessageW_k(hwnd, WM_PRINTCLIENT, (WPARAM)hdc, uFlags);
}


/*
 * helpers for calling IMM32
 *
 * WM_IME_* messages are generated only by IMM32,
 * so I assume imm32 is already LoadLibrary-ed.
 */
static HWND DEFWND_ImmGetDefaultIMEWnd( HWND hwnd )
{
    HINSTANCE hInstIMM = GetModuleHandleW_k( imm32W );
    HWND (WINAPI *pFunc)(HWND);
    HWND hwndRet = 0;

    if (!hInstIMM)
    {
        ERR( "cannot get IMM32 handle\n" );
        return 0;
    }

    pFunc = (void*)GetProcAddress_k(hInstIMM,"ImmGetDefaultIMEWnd");
    if ( pFunc != NULL )
        hwndRet = (*pFunc)( hwnd );

    return hwndRet;
}

static BOOL DEFWND_ImmIsUIMessageA( HWND hwndIME, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HINSTANCE hInstIMM = GetModuleHandleW_k( imm32W );
    BOOL (WINAPI *pFunc)(HWND,UINT,WPARAM,LPARAM);
    BOOL fRet = FALSE;

    if (!hInstIMM)
    {
        ERR( "cannot get IMM32 handle\n" );
        return FALSE;
    }

    pFunc = (void*)GetProcAddress_k(hInstIMM,"ImmIsUIMessageA");
    if ( pFunc != NULL )
        fRet = (*pFunc)( hwndIME, msg, wParam, lParam );

    return fRet;
}

static BOOL DEFWND_ImmIsUIMessageW( HWND hwndIME, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HINSTANCE hInstIMM = GetModuleHandleW_k( imm32W );
    BOOL (WINAPI *pFunc)(HWND,UINT,WPARAM,LPARAM);
    BOOL fRet = FALSE;

    if (!hInstIMM)
    {
        ERR( "cannot get IMM32 handle\n" );
        return FALSE;
    }

    pFunc = (void*)GetProcAddress_k(hInstIMM,"ImmIsUIMessageW");
    if ( pFunc != NULL )
        fRet = (*pFunc)( hwndIME, msg, wParam, lParam );

    return fRet;
}



/***********************************************************************
 *           DEFWND_DefWinProc
 *
 * Default window procedure for messages that are the same in Ansi and Unicode.
 */
static LRESULT DEFWND_DefWinProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch(msg)
    {
    case WM_NCPAINT:
        return NC_HandleNCPaint( hwnd, (HRGN)wParam );

    case WM_NCHITTEST:
        {
            POINT pt;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            return NC_HandleNCHitTest( hwnd, pt );
        }

    case WM_NCCALCSIZE:
        return NC_HandleNCCalcSize( hwnd, wParam, (RECT *)lParam );

    case WM_WINDOWPOSCHANGING:
        return WINPOS_HandleWindowPosChanging( hwnd, (WINDOWPOS *)lParam );

    case WM_WINDOWPOSCHANGED:
        DEFWND_HandleWindowPosChanged( hwnd, (const WINDOWPOS *)lParam );
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        iF10Key = iMenuSysKey = 0;
        break;

    case WM_NCLBUTTONDOWN:
        return NC_HandleNCLButtonDown( hwnd, wParam, lParam );

    case WM_LBUTTONDBLCLK:
        return NC_HandleNCLButtonDblClk( hwnd, HTCLIENT, lParam );

    case WM_NCLBUTTONDBLCLK:
        return NC_HandleNCLButtonDblClk( hwnd, wParam, lParam );

    case WM_NCRBUTTONDOWN:
        return NC_HandleNCRButtonDown( hwnd, wParam, lParam );

    case WM_RBUTTONUP:
        {
            POINT pt;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            ClientToScreen_k(hwnd, &pt);
            SendMessageW_k( hwnd, WM_CONTEXTMENU, (WPARAM)hwnd, MAKELPARAM(pt.x, pt.y) );
        }
        break;

    case WM_NCRBUTTONUP:
        /*
         * FIXME : we must NOT send WM_CONTEXTMENU on a WM_NCRBUTTONUP (checked
         * in Windows), but what _should_ we do? According to MSDN :
         * "If it is appropriate to do so, the system sends the WM_SYSCOMMAND
         * message to the window". When is it appropriate?
         */
        break;

    case WM_CONTEXTMENU:
        if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD)
            SendMessageW_k( GetParent_k(hwnd), msg, wParam, lParam );
        else
        {
            LONG hitcode;
            POINT pt;
            WND *wndPtr = WIN_GetPtr( hwnd );
            HMENU hMenu = wndPtr->hSysMenu;

            if (!hMenu) return 0;
            pt.x = (short)LOWORD(lParam);
            pt.y = (short)HIWORD(lParam);
            hitcode = NC_HandleNCHitTest(hwnd, pt);

            /* Track system popup if click was in the caption area. */
            if (hitcode==HTCAPTION || hitcode==HTSYSMENU)
               TrackPopupMenu_k(GetSystemMenu_k(hwnd, FALSE),
                               TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
                               pt.x, pt.y, 0, hwnd, NULL);
        }
        break;

    case WM_POPUPSYSTEMMENU:
        {
            /* This is an undocumented message used by the windows taskbar to
               display the system menu of windows that belong to other processes. */
            HMENU menu = GetSystemMenu_k(hwnd, FALSE);

            if (menu)
                TrackPopupMenu_k(menu, TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
                               LOWORD(lParam), HIWORD(lParam), 0, hwnd, NULL);
            return 0;
        }

    case WM_NCACTIVATE:
        return NC_HandleNCActivate( hwnd, wParam, lParam );

    case WM_NCDESTROY:
        {
            WND *wndPtr = WIN_GetPtr( hwnd );
            if (!wndPtr) return 0;
            HeapFree_k( GetProcessHeap_k(), 0, wndPtr->text );
            wndPtr->text = NULL;
            HeapFree_k( GetProcessHeap_k(), 0, wndPtr->pScroll );
            wndPtr->pScroll = NULL;
            return 0;
        }

    case WM_PRINT:
        DEFWND_Print(hwnd, (HDC)wParam, lParam);
        return 0;

    case WM_PAINTICON:
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint_k( hwnd, &ps );
            if( hdc )
            {
              HICON hIcon;
              if (IsIconic_k(hwnd) && ((hIcon = (HICON)GetClassLongPtrW_k( hwnd, GCLP_HICON))) )
              {
                  RECT rc;
                  int x, y;

                  GetClientRect_k( hwnd, &rc );
                  x = (rc.right - rc.left - GetSystemMetrics_k(SM_CXICON))/2;
                  y = (rc.bottom - rc.top - GetSystemMetrics_k(SM_CYICON))/2;
                  //TRACE("Painting class icon: vis rect=(%s)\n", wine_dbgstr_rect(&ps.rcPaint));
                  DrawIcon_k( hdc, x, y, hIcon );
              }
              EndPaint_k( hwnd, &ps );
            }
            return 0;
        }

    case WM_SYNCPAINT:
        RedrawWindow_k ( hwnd, NULL, 0, RDW_ERASENOW | RDW_ERASE | RDW_ALLCHILDREN );
        return 0;

    case WM_SETREDRAW:
        if (wParam) WIN_SetStyle( hwnd, WS_VISIBLE, 0 );
        else
        {
            RedrawWindow_k( hwnd, NULL, 0, RDW_ALLCHILDREN | RDW_VALIDATE );
            WIN_SetStyle( hwnd, 0, WS_VISIBLE );
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow_k( hwnd );
        return 0;

    case WM_MOUSEACTIVATE:
        if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD)
        {
            LONG ret = SendMessageW_k( GetParent_k(hwnd), WM_MOUSEACTIVATE, wParam, lParam );
            if (ret) return ret;
        }

        /* Caption clicks are handled by NC_HandleNCLButtonDown() */
        return MA_ACTIVATE;

    case WM_ACTIVATE:
        /* The default action in Windows is to set the keyboard focus to
         * the window, if it's being activated and not minimized */
        if (LOWORD(wParam) != WA_INACTIVE) {
            if (!IsIconic_k(hwnd)) SetFocus_k(hwnd);
        }
        break;

    case WM_MOUSEWHEEL:
        if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD)
            return SendMessageW_k( GetParent_k(hwnd), WM_MOUSEWHEEL, wParam, lParam );
        break;

    case WM_ERASEBKGND:
    case WM_ICONERASEBKGND:
        {
            RECT rect;
            HDC hdc = (HDC)wParam;
            HBRUSH hbr = (HBRUSH)GetClassLongPtrW_k( hwnd, GCLP_HBRBACKGROUND );
            if (!hbr) return 0;

            if (GetClassLongW_k( hwnd, GCL_STYLE ) & CS_PARENTDC)
            {
                /* can't use GetClipBox_k with a parent DC or we fill the whole parent */
                GetClientRect_k( hwnd, &rect );
                DPtoLP_k( hdc, (LPPOINT)&rect, 2 );
            }
            else GetClipBox_k( hdc, &rect );
            FillRect_k( hdc, &rect, hbr );
            return 1;
        }

    case WM_GETDLGCODE:
        return 0;

    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORSCROLLBAR:
        return (LRESULT)DEFWND_ControlColor( (HDC)wParam, msg - WM_CTLCOLORMSGBOX );

    case WM_CTLCOLOR:
        return (LRESULT)DEFWND_ControlColor( (HDC)wParam, HIWORD(lParam) );

    case WM_SETCURSOR:
        if (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD)
        {
            /* with the exception of the border around a resizable wnd,
             * give the parent first chance to set the cursor */
            if ((LOWORD(lParam) < HTSIZEFIRST) || (LOWORD(lParam) > HTSIZELAST))
            {
                HWND parent = GetParent_k( hwnd );
                if (parent != GetDesktopWindow_k() &&
                    SendMessageW_k( parent, WM_SETCURSOR, wParam, lParam )) return TRUE;
            }
        }
        NC_HandleSetCursor( hwnd, wParam, lParam );
        break;

    case WM_SYSCOMMAND:
        return NC_HandleSysCommand( hwnd, wParam, lParam );

    case WM_KEYDOWN:
        if(wParam == VK_F10) iF10Key = VK_F10;
        break;

    case WM_SYSKEYDOWN:
        if( HIWORD(lParam) & KEYDATA_ALT )
        {
            /* if( HIWORD(lParam) & ~KEYDATA_PREVSTATE ) */
              if ( (wParam == VK_MENU || wParam == VK_LMENU
                    || wParam == VK_RMENU) && !iMenuSysKey )
                iMenuSysKey = 1;
              else
                iMenuSysKey = 0;

            iF10Key = 0;

            if( wParam == VK_F4 )       /* try to close the window */
            {
                HWND top = GetAncestor_k( hwnd, GA_ROOT );
                if (!(GetClassLongW_k( top, GCL_STYLE ) & CS_NOCLOSE))
                    PostMessageW_k( top, WM_SYSCOMMAND, SC_CLOSE, 0 );
            }
        }
        else if( wParam == VK_F10 )
        {
            if (GetKeyState_k(VK_SHIFT) & 0x8000)
                SendMessageW_k( hwnd, WM_CONTEXTMENU, (WPARAM)hwnd, -1 );
            iF10Key = 1;
        }
        else if( wParam == VK_ESCAPE && (GetKeyState_k(VK_SHIFT) & 0x8000))
            SendMessageW_k( hwnd, WM_SYSCOMMAND, SC_KEYMENU, ' ' );
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        /* Press and release F10 or ALT */
        if (((wParam == VK_MENU || wParam == VK_LMENU || wParam == VK_RMENU)
             && iMenuSysKey) || ((wParam == VK_F10) && iF10Key))
              SendMessageW_k( GetAncestor_k( hwnd, GA_ROOT ), WM_SYSCOMMAND, SC_KEYMENU, 0L );
        iMenuSysKey = iF10Key = 0;
        break;

    case WM_SYSCHAR:
    {
        iMenuSysKey = 0;
        if (wParam == '\r' && IsIconic_k(hwnd))
        {
            PostMessageW_k( hwnd, WM_SYSCOMMAND, SC_RESTORE, 0L );
            break;
        }
        if ((HIWORD(lParam) & KEYDATA_ALT) && wParam)
        {
            if (wParam == '\t' || wParam == '\x1b') break;
            if (wParam == ' ' && (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD))
                SendMessageW_k( GetParent_k(hwnd), msg, wParam, lParam );
            else
                SendMessageW_k( hwnd, WM_SYSCOMMAND, SC_KEYMENU, wParam );
        }
        else /* check for Ctrl-Esc */
            if (wParam != '\x1b') MessageBeep_k(0);
        break;
    }

    case WM_SHOWWINDOW:
        {
            LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
            WND *pWnd;
            if (!lParam) return 0; /* sent from ShowWindow_k */
            if ((style & WS_VISIBLE) && wParam) return 0;
            if (!(style & WS_VISIBLE) && !wParam) return 0;
            if (!GetWindow_k( hwnd, GW_OWNER )) return 0;
            if (!(pWnd = WIN_GetPtr( hwnd ))) return 0;
            if (wParam)
            {
                if (!(pWnd->flags & WIN_NEEDS_SHOW_OWNEDPOPUP))
                {
                    return 0;
                }
                pWnd->flags &= ~WIN_NEEDS_SHOW_OWNEDPOPUP;
            }
            else pWnd->flags |= WIN_NEEDS_SHOW_OWNEDPOPUP;
            ShowWindow_k( hwnd, wParam ? SW_SHOWNOACTIVATE : SW_HIDE );
            break;
        }

    case WM_CANCELMODE:
        iMenuSysKey = 0;
        MENU_EndMenu( hwnd );
        if (GetCapture_k() == hwnd) ReleaseCapture_k();
        break;

    case WM_VKEYTOITEM:
    case WM_CHARTOITEM:
        return -1;

    case WM_DROPOBJECT:
        return DRAG_FILE;

    case WM_QUERYDROPOBJECT:
        return (GetWindowLongA_k( hwnd, GWL_EXSTYLE ) & WS_EX_ACCEPTFILES) != 0;

    case WM_QUERYDRAGICON:
        {
            UINT len;

            HICON hIcon = (HICON)GetClassLongPtrW_k( hwnd, GCLP_HICON );
            HINSTANCE instance = (HINSTANCE)GetWindowLongPtrW_k( hwnd, GWLP_HINSTANCE );
            if (hIcon) return (LRESULT)hIcon;
            for(len=1; len<64; len++)
                if((hIcon = LoadIconW_k(instance, MAKEINTRESOURCEW(len))))
                    return (LRESULT)hIcon;
            return (LRESULT)LoadIconW_k(0, (LPWSTR)IDI_APPLICATION);
        }
        break;

    case WM_ISACTIVEICON:
        {
            WND *wndPtr = WIN_GetPtr( hwnd );
            BOOL ret = (wndPtr->flags & WIN_NCACTIVATED) != 0;
            return ret;
        }

    case WM_NOTIFYFORMAT:
      if (IsWindowUnicode_k(hwnd)) return NFR_UNICODE;
      else return NFR_ANSI;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
        return 1;

    case WM_SETICON:
        {
            HICON ret;
            WND *wndPtr = WIN_GetPtr( hwnd );

            switch(wParam)
            {
            case ICON_SMALL:
                ret = wndPtr->hIconSmall;
                wndPtr->hIconSmall = (HICON)lParam;
                break;
            case ICON_BIG:
                ret = wndPtr->hIcon;
                wndPtr->hIcon = (HICON)lParam;
                break;
            default:
                ret = 0;
                break;
            }

            USER_Driver->pSetWindowIcon( hwnd, wParam, (HICON)lParam );

            if( (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CAPTION) == WS_CAPTION )
                NC_HandleNCPaint( hwnd , (HRGN)1 );  /* Repaint caption */

            return (LRESULT)ret;
        }

    case WM_GETICON:
        {
            HICON ret;
            WND *wndPtr = WIN_GetPtr( hwnd );

            switch(wParam)
            {
            case ICON_SMALL:
                ret = wndPtr->hIconSmall;
                break;
            case ICON_BIG:
                ret = wndPtr->hIcon;
                break;
            case ICON_SMALL2:
                ret = wndPtr->hIconSmall;
                if (!ret) ret = (HICON)GetClassLongPtrW_k( hwnd, GCLP_HICONSM );
                /* FIXME: should have a default here if class icon is null */
                break;
            default:
                ret = 0;
                break;
            }
            return (LRESULT)ret;
        }

    case WM_HELP:
        SendMessageW_k( GetParent_k(hwnd), msg, wParam, lParam );
        break;

    case WM_STYLECHANGED:
        if (wParam == GWL_STYLE && (GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_LAYERED))
        {
            STYLESTRUCT *style = (STYLESTRUCT *)lParam;
            if ((style->styleOld ^ style->styleNew) & (WS_CAPTION|WS_THICKFRAME|WS_VSCROLL|WS_HSCROLL))
                SetWindowPos_k( hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER |
                              SWP_NOSIZE | SWP_NOMOVE | SWP_NOCLIENTSIZE | SWP_NOCLIENTMOVE );
        }
        break;

    case WM_APPCOMMAND:
        {
            HWND parent = GetParent_k(hwnd);
            if(!parent)
                HOOK_CallHooks(WH_SHELL, HSHELL_APPCOMMAND, wParam, lParam, TRUE);
            else
                SendMessageW_k( parent, msg, wParam, lParam );
            break;
        }
    case WM_KEYF1:
        {
            HELPINFO hi;

            hi.cbSize = sizeof(HELPINFO);
            GetCursorPos_k( &hi.MousePos );
            if (MENU_IsMenuActive())
            {
                hi.iContextType = HELPINFO_MENUITEM;
                hi.hItemHandle = MENU_IsMenuActive();
                hi.iCtrlId = MenuItemFromPoint_k( hwnd, hi.hItemHandle, hi.MousePos );
                hi.dwContextId = GetMenuContextHelpId_k( hi.hItemHandle );
            }
            else
            {
                hi.iContextType = HELPINFO_WINDOW;
                hi.hItemHandle = hwnd;
                hi.iCtrlId = GetWindowLongPtrA_k( hwnd, GWLP_ID );
                hi.dwContextId = GetWindowContextHelpId_k( hwnd );
            }
            SendMessageW_k( hwnd, WM_HELP, 0, (LPARAM)&hi );
            break;
        }

    case WM_INPUTLANGCHANGEREQUEST:
        ActivateKeyboardLayout_k( (HKL)lParam, 0 );
        break;

    case WM_INPUTLANGCHANGE:
        {
            int count = 0;
            HWND *win_array = WIN_ListChildren( hwnd );

            if (!win_array)
                break;
            while (win_array[count])
                SendMessageW_k( win_array[count++], WM_INPUTLANGCHANGE, wParam, lParam);
            HeapFree_k(GetProcessHeap_k(),0,win_array);
            break;
        }

    }

    return 0;
}

static LPARAM DEFWND_GetTextA( WND *wndPtr, LPSTR dest, WPARAM wParam )
{
    LPARAM result = 0;

    //__TRY
    {
        if (wndPtr->text)
        {
            if (!WideCharToMultiByte_k( CP_ACP, 0, wndPtr->text, -1,
                                      dest, wParam, NULL, NULL )) dest[wParam-1] = 0;
            result = strlen( dest );
        }
        else dest[0] = '\0';
    }
    //__EXCEPT_PAGE_FAULT
    //{
    //    return 0;
    //}
    //__ENDTRY
    return result;
}

/***********************************************************************
 *              DefWindowProcA (USER32.@)
 *
 * See DefWindowProcW.
 */
LRESULT WINAPI DefWindowProcA_k( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    LRESULT result = 0;
    HWND full_handle;

    if (!(full_handle = WIN_IsCurrentProcess( hwnd )))
    {
        if (!IsWindow_k( hwnd )) return 0;
        ERR( "called for other process window %p\n", hwnd );
        return 0;
    }
    hwnd = full_handle;

    SPY_EnterMessage( SPY_DEFWNDPROC, hwnd, msg, wParam, lParam );

    switch(msg)
    {
    case WM_NCCREATE:
        if (lParam)
        {
            CREATESTRUCTA *cs = (CREATESTRUCTA *)lParam;
            /* check for string, as static icons, bitmaps (SS_ICON, SS_BITMAP)
             * may have child window IDs instead of window name */
            if (!IS_INTRESOURCE(cs->lpszName))
                DEFWND_SetTextA( hwnd, cs->lpszName );
            result = 1;

            if(cs->style & (WS_HSCROLL | WS_VSCROLL))
            {
                SCROLLINFO si = {sizeof si, SIF_ALL, 0, 100, 0, 0, 0};
                SetScrollInfo_k( hwnd, SB_HORZ, &si, FALSE );
                SetScrollInfo_k( hwnd, SB_VERT, &si, FALSE );
            }
        }
        break;

    case WM_GETTEXTLENGTH:
        {
            WND *wndPtr = WIN_GetPtr( hwnd );
            if (wndPtr && wndPtr->text)
                result = WideCharToMultiByte_k( CP_ACP, 0, wndPtr->text, strlenW(wndPtr->text),
                                              NULL, 0, NULL, NULL );
        }
        break;

    case WM_GETTEXT:
        if (wParam)
        {
            LPSTR dest = (LPSTR)lParam;
            WND *wndPtr = WIN_GetPtr( hwnd );

            if (!wndPtr) break;
            result = DEFWND_GetTextA( wndPtr, dest, wParam );

        }
        break;

    case WM_SETTEXT:
        DEFWND_SetTextA( hwnd, (LPCSTR)lParam );
        if( (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CAPTION) == WS_CAPTION )
            NC_HandleNCPaint( hwnd , (HRGN)1 );  /* Repaint caption */
        result = 1; /* success. FIXME: check text length */
        break;

    case WM_IME_CHAR:
        if (HIBYTE(wParam)) PostMessageA_k( hwnd, WM_CHAR, HIBYTE(wParam), lParam );
        PostMessageA_k( hwnd, WM_CHAR, LOBYTE(wParam), lParam );
        break;

    case WM_IME_KEYDOWN:
        result = PostMessageA_k( hwnd, WM_KEYDOWN, wParam, lParam );
        break;

    case WM_IME_KEYUP:
        result = PostMessageA_k( hwnd, WM_KEYUP, wParam, lParam );
        break;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_SELECT:
    case WM_IME_NOTIFY:
        {
            HWND hwndIME;

            hwndIME = DEFWND_ImmGetDefaultIMEWnd( hwnd );
            if (hwndIME)
                result = SendMessageA_k( hwndIME, msg, wParam, lParam );
        }
        break;
    case WM_IME_SETCONTEXT:
        {
            HWND hwndIME;

            hwndIME = DEFWND_ImmGetDefaultIMEWnd( hwnd );
            if (hwndIME)
                result = DEFWND_ImmIsUIMessageA( hwndIME, msg, wParam, lParam );
        }
        break;

    case WM_SYSCHAR:
    {
        CHAR ch = (CHAR)(LOWORD(wParam));
        WCHAR wch;
        MultiByteToWideChar_k(CP_ACP, 0, &ch, 1, &wch, 1);
        wParam = MAKEWPARAM( wch, HIWORD(wParam) );
    }
    /* fall through */
    default:
        result = DEFWND_DefWinProc( hwnd, msg, wParam, lParam );
        break;
    }

    SPY_ExitMessage( SPY_RESULT_DEFWND, hwnd, msg, result, wParam, lParam );
    return result;
}


static LPARAM DEFWND_GetTextW( WND *wndPtr, LPWSTR dest, WPARAM wParam )
{
    LPARAM result = 0;

    //__TRY
    {
        if (wndPtr->text)
        {
            lstrcpynW( dest, wndPtr->text, wParam );
            result = strlenW( dest );
        }
        else dest[0] = '\0';
    }
    //__EXCEPT_PAGE_FAULT
    //{
    //    return 0;
    //}
    //__ENDTRY

    return result;
}

/***********************************************************************
 *              DefWindowProcW (USER32.@) Calls default window message handler
 *
 * Calls default window procedure for messages not processed
 *  by application.
 *
 *  RETURNS
 *     Return value is dependent upon the message.
*/
LRESULT WINAPI DefWindowProcW_k(
    HWND hwnd,      /* [in] window procedure receiving message */
    UINT msg,       /* [in] message identifier */
    WPARAM wParam,  /* [in] first message parameter */
    LPARAM lParam )   /* [in] second message parameter */
{
    LRESULT result = 0;
    HWND full_handle;

    if (!(full_handle = WIN_IsCurrentProcess( hwnd )))
    {
        if (!IsWindow_k( hwnd )) return 0;
        ERR( "called for other process window %p\n", hwnd );
        return 0;
    }
    hwnd = full_handle;
    SPY_EnterMessage( SPY_DEFWNDPROC, hwnd, msg, wParam, lParam );

    switch(msg)
    {
    case WM_NCCREATE:
        if (lParam)
        {
            CREATESTRUCTW *cs = (CREATESTRUCTW *)lParam;
            /* check for string, as static icons, bitmaps (SS_ICON, SS_BITMAP)
             * may have child window IDs instead of window name */
            if (!IS_INTRESOURCE(cs->lpszName))
                DEFWND_SetTextW( hwnd, cs->lpszName );
            result = 1;

            if(cs->style & (WS_HSCROLL | WS_VSCROLL))
            {
                SCROLLINFO si = {sizeof si, SIF_ALL, 0, 100, 0, 0, 0};
                SetScrollInfo_k( hwnd, SB_HORZ, &si, FALSE );
                SetScrollInfo_k( hwnd, SB_VERT, &si, FALSE );
            }
        }
        break;

    case WM_GETTEXTLENGTH:
        {
            WND *wndPtr = WIN_GetPtr( hwnd );
            if (wndPtr && wndPtr->text) result = (LRESULT)strlenW(wndPtr->text);
        }
        break;

    case WM_GETTEXT:
        if (wParam)
        {
            LPWSTR dest = (LPWSTR)lParam;
            WND *wndPtr = WIN_GetPtr( hwnd );

            if (!wndPtr) break;
            result = DEFWND_GetTextW( wndPtr, dest, wParam );
        }
        break;

    case WM_SETTEXT:
        DEFWND_SetTextW( hwnd, (LPCWSTR)lParam );
        if( (GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CAPTION) == WS_CAPTION )
            NC_HandleNCPaint( hwnd , (HRGN)1 );  /* Repaint caption */
        result = 1; /* success. FIXME: check text length */
        break;

    case WM_IME_CHAR:
        PostMessageW_k( hwnd, WM_CHAR, wParam, lParam );
        break;

    case WM_IME_KEYDOWN:
        result = PostMessageW_k( hwnd, WM_KEYDOWN, wParam, lParam );
        break;

    case WM_IME_KEYUP:
        result = PostMessageW_k( hwnd, WM_KEYUP, wParam, lParam );
        break;

    case WM_IME_SETCONTEXT:
        {
            HWND hwndIME;

            hwndIME = DEFWND_ImmGetDefaultIMEWnd( hwnd );
            if (hwndIME)
                result = DEFWND_ImmIsUIMessageW( hwndIME, msg, wParam, lParam );
        }
        break;

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_SELECT:
    case WM_IME_NOTIFY:
        {
            HWND hwndIME;

            hwndIME = DEFWND_ImmGetDefaultIMEWnd( hwnd );
            if (hwndIME)
                result = SendMessageW_k( hwndIME, msg, wParam, lParam );
        }
        break;

    default:
        result = DEFWND_DefWinProc( hwnd, msg, wParam, lParam );
        break;
    }
    SPY_ExitMessage( SPY_RESULT_DEFWND, hwnd, msg, result, wParam, lParam );
    return result;
}
