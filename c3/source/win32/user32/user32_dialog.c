/*
 * Dialog functions
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
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

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../include/winbase.h"
#include "../include/unicode.h"
#include "controls.h"
#include "win.h"
#include "user_private.h"

  /* Dialog control information */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    INT16      x;
    INT16      y;
    INT16      cx;
    INT16      cy;
    UINT_PTR   id;
    LPCWSTR    className;
    LPCWSTR    windowName;
    LPCVOID    data;
} DLG_CONTROL_INFO;

  /* Dialog template */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    UINT16     nbItems;
    INT16      x;
    INT16      y;
    INT16      cx;
    INT16      cy;
    LPCWSTR    menuName;
    LPCWSTR    className;
    LPCWSTR    caption;
    INT16      pointSize;
    WORD       weight;
    BOOL       italic;
    LPCWSTR    faceName;
    BOOL       dialogEx;
} DLG_TEMPLATE;

  /* Radio button group */
typedef struct
{
    UINT firstID;
    UINT lastID;
    UINT checkID;
} RADIOGROUP;


/*********************************************************************
 * dialog class descriptor
 */
const struct builtin_class_descr DIALOG_builtin_class =
{
    (LPCWSTR)DIALOG_CLASS_ATOM, /* name */
    CS_SAVEBITS | CS_DBLCLKS, /* style  */
    WINPROC_DIALOG,     /* proc */
    DLGWINDOWEXTRA,     /* extra */
    IDC_ARROW,          /* cursor */
    0                   /* brush */
};


/***********************************************************************
 *           DIALOG_EnableOwner
 *
 * Helper function for modal dialogs to enable again the
 * owner of the dialog box.
 */
static void DIALOG_EnableOwner( HWND hOwner )
{
    /* Owner must be a top-level window */
    if (hOwner)
        hOwner = GetAncestor_k( hOwner, GA_ROOT );
    if (!hOwner) return;
    EnableWindow_k( hOwner, TRUE );
}


/***********************************************************************
 *           DIALOG_DisableOwner
 *
 * Helper function for modal dialogs to disable the
 * owner of the dialog box. Returns TRUE if owner was enabled.
 */
static BOOL DIALOG_DisableOwner( HWND hOwner )
{
    /* Owner must be a top-level window */
    if (hOwner)
        hOwner = GetAncestor_k( hOwner, GA_ROOT );
    if (!hOwner) return FALSE;
    if (IsWindowEnabled_k( hOwner ))
    {
        EnableWindow_k( hOwner, FALSE );
        return TRUE;
    }
    else
        return FALSE;
}

/***********************************************************************
 *           DIALOG_GetControl32
 *
 * Return the class and text of the control pointed to by ptr,
 * fill the header structure and return a pointer to the next control.
 */
static const WORD *DIALOG_GetControl32( const WORD *p, DLG_CONTROL_INFO *info,
                                        BOOL dialogEx )
{
    if (dialogEx)
    {
        info->helpId  = GET_DWORD(p); p += 2;
        info->exStyle = GET_DWORD(p); p += 2;
        info->style   = GET_DWORD(p); p += 2;
    }
    else
    {
        info->helpId  = 0;
        info->style   = GET_DWORD(p); p += 2;
        info->exStyle = GET_DWORD(p); p += 2;
    }
    info->x       = GET_WORD(p); p++;
    info->y       = GET_WORD(p); p++;
    info->cx      = GET_WORD(p); p++;
    info->cy      = GET_WORD(p); p++;

    if (dialogEx)
    {
        /* id is a DWORD for DIALOGEX */
        info->id = GET_DWORD(p);
        p += 2;
    }
    else
    {
        info->id = GET_WORD(p);
        p++;
    }

    if (GET_WORD(p) == 0xffff)
    {
        static const WCHAR class_names[6][10] =
        {
            { 'B','u','t','t','o','n', },             /* 0x80 */
            { 'E','d','i','t', },                     /* 0x81 */
            { 'S','t','a','t','i','c', },             /* 0x82 */
            { 'L','i','s','t','B','o','x', },         /* 0x83 */
            { 'S','c','r','o','l','l','B','a','r', }, /* 0x84 */
            { 'C','o','m','b','o','B','o','x', }      /* 0x85 */
        };
        WORD id = GET_WORD(p+1);
        /* Windows treats dialog control class ids 0-5 same way as 0x80-0x85 */
        if ((id >= 0x80) && (id <= 0x85)) id -= 0x80;
        if (id <= 5)
            info->className = class_names[id];
        else
        {
            info->className = NULL;
            ERR("Unknown built-in class id %04x\n", id );
        }
        p += 2;
    }
    else
    {
        info->className = p;
        p += strlenW( info->className ) + 1;
    }

    if (GET_WORD(p) == 0xffff)  /* Is it an integer id? */
    {
        info->windowName = MAKEINTRESOURCEW(GET_WORD(p + 1));
        p += 2;
    }
    else
    {
        info->windowName = p;
        p += strlenW( info->windowName ) + 1;
    }

    TRACE("    %s %s %ld, %d, %d, %d, %d, %08x, %08x, %08x\n",
          debugstr_w( info->className ), debugstr_w( info->windowName ),
          info->id, info->x, info->y, info->cx, info->cy,
          info->style, info->exStyle, info->helpId );

    if (GET_WORD(p))
    {
        info->data = p + 1;
        p += GET_WORD(p) / sizeof(WORD);
    }
    else info->data = NULL;
    p++;

    /* Next control is on dword boundary */
    return (const WORD *)(((UINT_PTR)p + 3) & ~3);
}


/***********************************************************************
 *           DIALOG_CreateControls32
 *
 * Create the control windows for a dialog.
 */
static BOOL DIALOG_CreateControls32( HWND hwnd, LPCSTR template, const DLG_TEMPLATE *dlgTemplate,
                                     HINSTANCE hInst, BOOL unicode )
{
    DIALOGINFO *dlgInfo = DIALOG_get_info( hwnd, TRUE );
    DLG_CONTROL_INFO info;
    HWND hwndCtrl, hwndDefButton = 0;
    INT items = dlgTemplate->nbItems;

    TRACE(" BEGIN\n" );
    while (items--)
    {
        template = (LPCSTR)DIALOG_GetControl32( (const WORD *)template, &info,
                                                dlgTemplate->dialogEx );
        info.style &= ~WS_POPUP;
        info.style |= WS_CHILD;

        if (info.style & WS_BORDER)
        {
            info.style &= ~WS_BORDER;
            info.exStyle |= WS_EX_CLIENTEDGE;
        }
        if (unicode)
        {
            hwndCtrl = CreateWindowExW_k( info.exStyle | WS_EX_NOPARENTNOTIFY,
                                        info.className, info.windowName,
                                        info.style | WS_CHILD,
                                        MulDiv_k(info.x, dlgInfo->xBaseUnit, 4),
                                        MulDiv_k(info.y, dlgInfo->yBaseUnit, 8),
                                        MulDiv_k(info.cx, dlgInfo->xBaseUnit, 4),
                                        MulDiv_k(info.cy, dlgInfo->yBaseUnit, 8),
                                        hwnd, (HMENU)info.id,
                                        hInst, (LPVOID)info.data );
        }
        else
        {
            LPCSTR class = (LPCSTR)info.className;
            LPCSTR caption = (LPCSTR)info.windowName;
            LPSTR class_tmp = NULL;
            LPSTR caption_tmp = NULL;

            if (!IS_INTRESOURCE(class))
            {
                DWORD len = WideCharToMultiByte_k( CP_ACP, 0, info.className, -1, NULL, 0, NULL, NULL );
                class_tmp = HeapAlloc_k(GetProcessHeap_k(), 0, len );
                WideCharToMultiByte_k( CP_ACP, 0, info.className, -1, class_tmp, len, NULL, NULL );
                class = class_tmp;
            }
            if (!IS_INTRESOURCE(caption))
            {
                DWORD len = WideCharToMultiByte_k( CP_ACP, 0, info.windowName, -1, NULL, 0, NULL, NULL );
                caption_tmp = HeapAlloc_k(GetProcessHeap_k(), 0, len );
                WideCharToMultiByte_k( CP_ACP, 0, info.windowName, -1, caption_tmp, len, NULL, NULL );
                caption = caption_tmp;
            }
            hwndCtrl = CreateWindowExA_k( info.exStyle | WS_EX_NOPARENTNOTIFY,
                                        class, caption, info.style | WS_CHILD,
                                        MulDiv_k(info.x, dlgInfo->xBaseUnit, 4),
                                        MulDiv_k(info.y, dlgInfo->yBaseUnit, 8),
                                        MulDiv_k(info.cx, dlgInfo->xBaseUnit, 4),
                                        MulDiv_k(info.cy, dlgInfo->yBaseUnit, 8),
                                        hwnd, (HMENU)info.id,
                                        hInst, (LPVOID)info.data );
            HeapFree_k( GetProcessHeap_k(), 0, class_tmp );
            HeapFree_k( GetProcessHeap_k(), 0, caption_tmp );
        }
        if (!hwndCtrl)
        {
            WARN("control %s %s creation failed\n", debugstr_w(info.className),
                 debugstr_w(info.windowName));
            if (dlgTemplate->style & DS_NOFAILCREATE) continue;
            return FALSE;
        }

            /* Send initialisation messages to the control */
        if (dlgInfo->hUserFont) SendMessageW_k( hwndCtrl, WM_SETFONT,
                                             (WPARAM)dlgInfo->hUserFont, 0 );
        if (SendMessageW_k(hwndCtrl, WM_GETDLGCODE, 0, 0) & DLGC_DEFPUSHBUTTON)
        {
              /* If there's already a default push-button, set it back */
              /* to normal and use this one instead. */
            if (hwndDefButton)
                SendMessageW_k( hwndDefButton, BM_SETSTYLE, BS_PUSHBUTTON, FALSE );
            hwndDefButton = hwndCtrl;
            dlgInfo->idResult = GetWindowLongPtrA_k( hwndCtrl, GWLP_ID );
        }
    }
    TRACE(" END\n" );
    return TRUE;
}


/***********************************************************************
 *           DIALOG_ParseTemplate32
 *
 * Fill a DLG_TEMPLATE structure from the dialog template, and return
 * a pointer to the first control.
 */
static LPCSTR DIALOG_ParseTemplate32( LPCSTR template, DLG_TEMPLATE * result )
{
    const WORD *p = (const WORD *)template;
    WORD signature;
    WORD dlgver;

    dlgver = GET_WORD(p); p++;
    signature = GET_WORD(p); p++;

    if (dlgver == 1 && signature == 0xffff)  /* DIALOGEX resource */
    {
        result->dialogEx = TRUE;
        result->helpId   = GET_DWORD(p); p += 2;
        result->exStyle  = GET_DWORD(p); p += 2;
        result->style    = GET_DWORD(p); p += 2;
    }
    else
    {
        result->style = GET_DWORD(p - 2);
        result->dialogEx = FALSE;
        result->helpId   = 0;
        result->exStyle  = GET_DWORD(p); p += 2;
    }
    result->nbItems = GET_WORD(p); p++;
    result->x       = GET_WORD(p); p++;
    result->y       = GET_WORD(p); p++;
    result->cx      = GET_WORD(p); p++;
    result->cy      = GET_WORD(p); p++;
    TRACE("DIALOG%s %d, %d, %d, %d, %d\n",
           result->dialogEx ? "EX" : "", result->x, result->y,
           result->cx, result->cy, result->helpId );
    TRACE(" STYLE 0x%08x\n", result->style );
    TRACE(" EXSTYLE 0x%08x\n", result->exStyle );

    /* Get the menu name */

    switch(GET_WORD(p))
    {
    case 0x0000:
        result->menuName = NULL;
        p++;
        break;
    case 0xffff:
        result->menuName = MAKEINTRESOURCEW(GET_WORD( p + 1 ));
        p += 2;
        TRACE(" MENU %04x\n", LOWORD(result->menuName) );
        break;
    default:
        result->menuName = p;
        TRACE(" MENU %s\n", debugstr_w(result->menuName) );
        p += strlenW( result->menuName ) + 1;
        break;
    }

    /* Get the class name */

    switch(GET_WORD(p))
    {
    case 0x0000:
        result->className = (LPCWSTR)DIALOG_CLASS_ATOM;
        p++;
        break;
    case 0xffff:
        result->className = MAKEINTRESOURCEW(GET_WORD( p + 1 ));
        p += 2;
        TRACE(" CLASS %04x\n", LOWORD(result->className) );
        break;
    default:
        result->className = p;
        TRACE(" CLASS %s\n", debugstr_w( result->className ));
        p += strlenW( result->className ) + 1;
        break;
    }

    /* Get the window caption */

    result->caption = p;
    p += strlenW( result->caption ) + 1;
    TRACE(" CAPTION %s\n", debugstr_w( result->caption ) );

    /* Get the font name */

    result->pointSize = 0;
    result->faceName = NULL;
    result->weight = FW_DONTCARE;
    result->italic = FALSE;

    if (result->style & DS_SETFONT)
    {
        result->pointSize = GET_WORD(p);
        p++;

        /* If pointSize is 0x7fff, it means that we need to use the font
         * in NONCLIENTMETRICSW.lfMessageFont, and NOT read the weight,
         * italic, and facename from the dialog template.
         */
        if (result->pointSize == 0x7fff)
        {
            /* We could call SystemParametersInfo here, but then we'd have
             * to convert from pixel size to point size (which can be
             * imprecise).
             */
            TRACE(" FONT: Using message box font\n");
        }
        else
        {
            if (result->dialogEx)
            {
                result->weight = GET_WORD(p); p++;
                result->italic = LOBYTE(GET_WORD(p)); p++;
            }
            result->faceName = p;
            p += strlenW( result->faceName ) + 1;

            TRACE(" FONT %d, %s, %d, %s\n",
                  result->pointSize, debugstr_w( result->faceName ),
                  result->weight, result->italic ? "TRUE" : "FALSE" );
        }
    }

    /* First control is on dword boundary */
    return (LPCSTR)(((UINT_PTR)p + 3) & ~3);
}


/***********************************************************************
 *           DIALOG_CreateIndirect
 *       Creates a dialog box window
 *
 *       modal = TRUE if we are called from a modal dialog box.
 *       (it's more compatible to do it here, as under Windows the owner
 *       is never disabled if the dialog fails because of an invalid template)
 */
static HWND DIALOG_CreateIndirect( HINSTANCE hInst, LPCVOID dlgTemplate,
                                   HWND owner, DLGPROC dlgProc, LPARAM param,
                                   BOOL unicode, BOOL modal )
{
    HWND hwnd;
    RECT rect;
    POINT pos;
    SIZE size;
    DLG_TEMPLATE template;
    DIALOGINFO * dlgInfo = NULL;
    DWORD units = GetDialogBaseUnits_k();
    BOOL ownerEnabled = TRUE;
    HMENU hMenu = 0;
    HFONT hUserFont = 0;
    UINT flags = 0;
    UINT xBaseUnit = LOWORD(units);
    UINT yBaseUnit = HIWORD(units);

      /* Parse dialog template */

    if (!dlgTemplate) return 0;
    dlgTemplate = DIALOG_ParseTemplate32( dlgTemplate, &template );

      /* Load menu */

    if (template.menuName) hMenu = LoadMenuW_k( hInst, template.menuName );

      /* Create custom font if needed */

    if (template.style & DS_SETFONT)
    {
        HDC dc = GetDC_k(0);

        if (template.pointSize == 0x7fff)
        {
            /* We get the message font from the non-client metrics */
            NONCLIENTMETRICSW ncMetrics;

            ncMetrics.cbSize = sizeof(NONCLIENTMETRICSW);
            if (SystemParametersInfoW_k(SPI_GETNONCLIENTMETRICS,
                                      sizeof(NONCLIENTMETRICSW), &ncMetrics, 0))
            {
                hUserFont = CreateFontIndirectW_k( &ncMetrics.lfMessageFont );
            }
        }
        else
        {
            /* We convert the size to pixels and then make it -ve.  This works
             * for both +ve and -ve template.pointSize */
            int pixels = MulDiv_k(template.pointSize, GetDeviceCaps_k(dc , LOGPIXELSY), 72);
            hUserFont = CreateFontW_k( -pixels, 0, 0, 0, template.weight,
                                              template.italic, FALSE, FALSE, DEFAULT_CHARSET, 0, 0,
                                              PROOF_QUALITY, FF_DONTCARE,
                                              template.faceName );
        }

        if (hUserFont)
        {
            SIZE charSize;
            HFONT hOldFont = SelectObject_k( dc, hUserFont );
            charSize.cx = GdiGetCharDimensions_k( dc, NULL, &charSize.cy );
            if (charSize.cx)
            {
                xBaseUnit = charSize.cx;
                yBaseUnit = charSize.cy;
            }
            SelectObject_k( dc, hOldFont );
        }
        ReleaseDC_k(0, dc);
        TRACE("units = %d,%d\n", xBaseUnit, yBaseUnit );
    }

    /* Create dialog main window */

    rect.left = rect.top = 0;
    rect.right = MulDiv_k(template.cx, xBaseUnit, 4);
    rect.bottom =  MulDiv_k(template.cy, yBaseUnit, 8);

    if (template.style & DS_CONTROL)
        template.style &= ~(WS_CAPTION|WS_SYSMENU);
    template.style |= DS_3DLOOK;
    if (template.style & DS_MODALFRAME)
        template.exStyle |= WS_EX_DLGMODALFRAME;
    if ((template.style & DS_CONTROL) || !(template.style & WS_CHILD))
        template.exStyle |= WS_EX_CONTROLPARENT;
    AdjustWindowRectEx_k( &rect, template.style, (hMenu != 0), template.exStyle );
    pos.x = rect.left;
    pos.y = rect.top;
    size.cx = rect.right - rect.left;
    size.cy = rect.bottom - rect.top;

    if (template.x == (SHORT)0x8000 /*CW_USEDEFAULT16*/)
    {
        pos.x = pos.y = CW_USEDEFAULT;
    }
    else
    {
        HMONITOR monitor = 0;
        MONITORINFO mon_info;

        mon_info.cbSize = sizeof(mon_info);
        if (template.style & DS_CENTER)
        {
            monitor = MonitorFromWindow_k( owner ? owner : GetActiveWindow_k(), MONITOR_DEFAULTTOPRIMARY );
            GetMonitorInfoW_k( monitor, &mon_info );
            pos.x = (mon_info.rcWork.left + mon_info.rcWork.right - size.cx) / 2;
            pos.y = (mon_info.rcWork.top + mon_info.rcWork.bottom - size.cy) / 2;
        }
        else if (template.style & DS_CENTERMOUSE)
        {
            GetCursorPos_k( &pos );
            monitor = MonitorFromPoint_k( pos, MONITOR_DEFAULTTOPRIMARY );
            GetMonitorInfoW_k( monitor, &mon_info );
        }
        else
        {
            pos.x += MulDiv_k(template.x, xBaseUnit, 4);
            pos.y += MulDiv_k(template.y, yBaseUnit, 8);
            if (!(template.style & (WS_CHILD|DS_ABSALIGN))) ClientToScreen_k( owner, &pos );
        }
        if ( !(template.style & WS_CHILD) )
        {
            INT dX, dY;

            /* try to fit it into the desktop */

            if (!monitor)
            {
                SetRect_k( &rect, pos.x, pos.y, pos.x + size.cx, pos.y + size.cy );
                monitor = MonitorFromRect_k( &rect, MONITOR_DEFAULTTOPRIMARY );
                GetMonitorInfoW_k( monitor, &mon_info );
            }
            if ((dX = pos.x + size.cx + GetSystemMetrics_k(SM_CXDLGFRAME) - mon_info.rcWork.right) > 0)
                pos.x -= dX;
            if ((dY = pos.y + size.cy + GetSystemMetrics_k(SM_CYDLGFRAME) - mon_info.rcWork.bottom) > 0)
                pos.y -= dY;
            if( pos.x < mon_info.rcWork.left ) pos.x = mon_info.rcWork.left;
            if( pos.y < mon_info.rcWork.top ) pos.y = mon_info.rcWork.top;
        }
    }

    if (modal)
    {
        ownerEnabled = DIALOG_DisableOwner( owner );
        if (ownerEnabled) flags |= DF_OWNERENABLED;
    }

    if (unicode)
    {
        hwnd = CreateWindowExW_k(template.exStyle, template.className, template.caption,
                               template.style & ~WS_VISIBLE, pos.x, pos.y, size.cx, size.cy,
                               owner, hMenu, hInst, NULL );
    }
    else
    {
        LPCSTR class = (LPCSTR)template.className;
        LPCSTR caption = (LPCSTR)template.caption;
        LPSTR class_tmp = NULL;
        LPSTR caption_tmp = NULL;

        if (!IS_INTRESOURCE(class))
        {
            DWORD len = WideCharToMultiByte_k( CP_ACP, 0, template.className, -1, NULL, 0, NULL, NULL );
            class_tmp = HeapAlloc_k(GetProcessHeap_k(), 0, len );
            WideCharToMultiByte_k( CP_ACP, 0, template.className, -1, class_tmp, len, NULL, NULL );
            class = class_tmp;
        }
        if (!IS_INTRESOURCE(caption))
        {
            DWORD len = WideCharToMultiByte_k( CP_ACP, 0, template.caption, -1, NULL, 0, NULL, NULL );
            caption_tmp = HeapAlloc_k(GetProcessHeap_k(), 0, len );
            WideCharToMultiByte_k( CP_ACP, 0, template.caption, -1, caption_tmp, len, NULL, NULL );
            caption = caption_tmp;
        }
        hwnd = CreateWindowExA_k(template.exStyle, class, caption,
                               template.style & ~WS_VISIBLE, pos.x, pos.y, size.cx, size.cy,
                               owner, hMenu, hInst, NULL );
        HeapFree_k( GetProcessHeap_k(), 0, class_tmp );
        HeapFree_k( GetProcessHeap_k(), 0, caption_tmp );
    }

    if (!hwnd)
    {
        if (hUserFont) DeleteObject_k( hUserFont );
        if (hMenu) DestroyMenu_k( hMenu );
        if (modal && (flags & DF_OWNERENABLED)) DIALOG_EnableOwner(owner);
        return 0;
    }

    /* moved this from the top of the method to here as DIALOGINFO structure
    will be valid only after WM_CREATE message has been handled in DefDlgProc
    All the members of the structure get filled here using temp variables */
    dlgInfo = DIALOG_get_info( hwnd, TRUE );
    dlgInfo->hwndFocus   = 0;
    dlgInfo->hUserFont   = hUserFont;
    dlgInfo->hMenu       = hMenu;
    dlgInfo->xBaseUnit   = xBaseUnit;
    dlgInfo->yBaseUnit   = yBaseUnit;
    dlgInfo->flags       = flags;

    if (template.helpId) SetWindowContextHelpId_k( hwnd, template.helpId );

    if (unicode) SetWindowLongPtrW_k( hwnd, DWLP_DLGPROC, (ULONG_PTR)dlgProc );
    else SetWindowLongPtrA_k( hwnd, DWLP_DLGPROC, (ULONG_PTR)dlgProc );

    if (dlgProc && dlgInfo->hUserFont)
        SendMessageW_k( hwnd, WM_SETFONT, (WPARAM)dlgInfo->hUserFont, 0 );

    /* Create controls */

    if (DIALOG_CreateControls32( hwnd, dlgTemplate, &template, hInst, unicode ))
    {
        /* Send initialisation messages and set focus */

        if (dlgProc)
        {
            HWND focus = GetNextDlgTabItem_k( hwnd, 0, FALSE );
            if (!focus) focus = GetNextDlgGroupItem_k( hwnd, 0, FALSE );
            if (SendMessageW_k( hwnd, WM_INITDIALOG, (WPARAM)focus, param ) && IsWindow_k( hwnd ) &&
                ((~template.style & DS_CONTROL) || (template.style & WS_VISIBLE)))
            {
                /* By returning TRUE, app has requested a default focus assignment.
                 * WM_INITDIALOG may have changed the tab order, so find the first
                 * tabstop control again. */
                dlgInfo->hwndFocus = GetNextDlgTabItem_k( hwnd, 0, FALSE );
                if (!dlgInfo->hwndFocus) dlgInfo->hwndFocus = GetNextDlgGroupItem_k( hwnd, 0, FALSE );
                if( dlgInfo->hwndFocus )
                    SetFocus_k( dlgInfo->hwndFocus );
            }
        }

        if (template.style & WS_VISIBLE && !(GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_VISIBLE))
        {
           ShowWindow_k( hwnd, SW_SHOWNORMAL );   /* SW_SHOW doesn't always work */
        }
        return hwnd;
    }
    if (modal && ownerEnabled) DIALOG_EnableOwner(owner);
    if( IsWindow_k(hwnd) ) DestroyWindow_k( hwnd );
    return 0;
}


/***********************************************************************
 *		CreateDialogParamA (USER32.@)
 */
HWND WINAPI CreateDialogParamA_k( HINSTANCE hInst, LPCSTR name, HWND owner,
                                DLGPROC dlgProc, LPARAM param )
{
    HRSRC hrsrc;
    LPCDLGTEMPLATEA ptr;

    if (!(hrsrc = FindResourceA_k( hInst, name, (LPSTR)RT_DIALOG ))) return 0;
    if (!(ptr = LoadResource_k(hInst, hrsrc))) return 0;
    return CreateDialogIndirectParamA_k( hInst, ptr, owner, dlgProc, param );
}


/***********************************************************************
 *		CreateDialogParamW (USER32.@)
 */
HWND WINAPI CreateDialogParamW_k( HINSTANCE hInst, LPCWSTR name, HWND owner,
                                DLGPROC dlgProc, LPARAM param )
{
    HRSRC hrsrc;
    LPCDLGTEMPLATEA ptr;

    if (!(hrsrc = FindResourceW_k( hInst, name, (LPWSTR)RT_DIALOG ))) return 0;
    if (!(ptr = LoadResource_k(hInst, hrsrc))) return 0;
    return CreateDialogIndirectParamW_k( hInst, ptr, owner, dlgProc, param );
}


/***********************************************************************
 *		CreateDialogIndirectParamAorW (USER32.@)
 */
HWND WINAPI CreateDialogIndirectParamAorW_k( HINSTANCE hInst, LPCVOID dlgTemplate,
                                           HWND owner, DLGPROC dlgProc, LPARAM param,
                                           DWORD flags )
{
    return DIALOG_CreateIndirect( hInst, dlgTemplate, owner, dlgProc, param, !flags, FALSE );
}

/***********************************************************************
 *		CreateDialogIndirectParamA (USER32.@)
 */
HWND WINAPI CreateDialogIndirectParamA_k( HINSTANCE hInst, LPCDLGTEMPLATEA dlgTemplate,
                                        HWND owner, DLGPROC dlgProc, LPARAM param )
{
    return CreateDialogIndirectParamAorW_k( hInst, dlgTemplate, owner, dlgProc, param, 2 );
}

/***********************************************************************
 *		CreateDialogIndirectParamW (USER32.@)
 */
HWND WINAPI CreateDialogIndirectParamW_k( HINSTANCE hInst, LPCDLGTEMPLATEW dlgTemplate,
                                        HWND owner, DLGPROC dlgProc, LPARAM param )
{
    return CreateDialogIndirectParamAorW_k( hInst, dlgTemplate, owner, dlgProc, param, 0 );
}


/***********************************************************************
 *           DIALOG_DoDialogBox
 */
INT DIALOG_DoDialogBox( HWND hwnd, HWND owner )
{
    DIALOGINFO * dlgInfo;
    MSG msg;
    INT retval;
    HWND ownerMsg = GetAncestor_k( owner, GA_ROOT );
    BOOL bFirstEmpty;

    if (!(dlgInfo = DIALOG_get_info( hwnd, FALSE ))) return -1;

    bFirstEmpty = TRUE;
    if (!(dlgInfo->flags & DF_END)) /* was EndDialog called in WM_INITDIALOG ? */
    {
        for (;;)
        {
            if (!PeekMessageW_k( &msg, 0, 0, 0, PM_REMOVE ))
            {
                if (bFirstEmpty)
                {
                    /* ShowWindow the first time the queue goes empty */
                    ShowWindow_k( hwnd, SW_SHOWNORMAL );
                    bFirstEmpty = FALSE;
                }
                if (!(GetWindowLongW_k( hwnd, GWL_STYLE ) & DS_NOIDLEMSG))
               {
                    /* No message present -> send ENTERIDLE and wait */
                    SendMessageW_k( ownerMsg, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)hwnd );
                }
                GetMessageW_k( &msg, 0, 0, 0 );
            }

            if (msg.message == WM_QUIT)
            {
                PostQuitMessage_k( msg.wParam );
                if (!IsWindow_k( hwnd )) return 0;
                break;
            }
            if (!IsWindow_k( hwnd )) return 0;
            if (!(dlgInfo->flags & DF_END) && !IsDialogMessageW_k( hwnd, &msg))
            {
                TranslateMessage_k( &msg );
                DispatchMessageW_k( &msg );
            }
            if (!IsWindow_k( hwnd )) return 0;
            if (dlgInfo->flags & DF_END) break;

            if (bFirstEmpty && msg.message == WM_TIMER)
            {
                ShowWindow_k( hwnd, SW_SHOWNORMAL );
                bFirstEmpty = FALSE;
            }
        }
    }
    if (dlgInfo->flags & DF_OWNERENABLED) DIALOG_EnableOwner( owner );
    retval = dlgInfo->idResult;
    DestroyWindow_k( hwnd );
    return retval;
}


/***********************************************************************
 *		DialogBoxParamA (USER32.@)
 */
INT_PTR WINAPI DialogBoxParamA_k( HINSTANCE hInst, LPCSTR name,
                                HWND owner, DLGPROC dlgProc, LPARAM param )
{
    HWND hwnd;
    HRSRC hrsrc;
    LPCDLGTEMPLATEA ptr;

    if (!(hrsrc = FindResourceA_k( hInst, name, (LPSTR)RT_DIALOG ))) return -1;
    if (!(ptr = LoadResource_k(hInst, hrsrc))) return -1;
    hwnd = DIALOG_CreateIndirect( hInst, ptr, owner, dlgProc, param, FALSE, TRUE );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return 0;
}


/***********************************************************************
 *		DialogBoxParamW (USER32.@)
 */
INT_PTR WINAPI DialogBoxParamW_k( HINSTANCE hInst, LPCWSTR name,
                                HWND owner, DLGPROC dlgProc, LPARAM param )
{
    HWND hwnd;
    HRSRC hrsrc;
    LPCDLGTEMPLATEW ptr;

    if (!(hrsrc = FindResourceW_k( hInst, name, (LPWSTR)RT_DIALOG ))) return -1;
    if (!(ptr = LoadResource_k(hInst, hrsrc))) return -1;
    hwnd = DIALOG_CreateIndirect( hInst, ptr, owner, dlgProc, param, TRUE, TRUE );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return 0;
}


/***********************************************************************
 *		DialogBoxIndirectParamAorW (USER32.@)
 */
INT_PTR WINAPI DialogBoxIndirectParamAorW_k( HINSTANCE hInstance, LPCVOID template,
                                           HWND owner, DLGPROC dlgProc,
                                           LPARAM param, DWORD flags )
{
    HWND hwnd = DIALOG_CreateIndirect( hInstance, template, owner, dlgProc, param, !flags, TRUE );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}

/***********************************************************************
 *		DialogBoxIndirectParamA (USER32.@)
 */
INT_PTR WINAPI DialogBoxIndirectParamA_k(HINSTANCE hInstance, LPCDLGTEMPLATEA template,
                                       HWND owner, DLGPROC dlgProc, LPARAM param )
{
    return DialogBoxIndirectParamAorW_k( hInstance, template, owner, dlgProc, param, 2 );
}


/***********************************************************************
 *		DialogBoxIndirectParamW (USER32.@)
 */
INT_PTR WINAPI DialogBoxIndirectParamW_k(HINSTANCE hInstance, LPCDLGTEMPLATEW template,
                                       HWND owner, DLGPROC dlgProc, LPARAM param )
{
    return DialogBoxIndirectParamAorW_k( hInstance, template, owner, dlgProc, param, 0 );
}

/***********************************************************************
 *		EndDialog (USER32.@)
 */
BOOL WINAPI EndDialog_k( HWND hwnd, INT_PTR retval )
{
    BOOL wasEnabled = TRUE;
    DIALOGINFO * dlgInfo;
    HWND owner;

    TRACE("%p %ld\n", hwnd, retval );

    if (!(dlgInfo = DIALOG_get_info( hwnd, FALSE )))
    {
        ERR("got invalid window handle (%p); buggy app !?\n", hwnd);
        return FALSE;
    }
    dlgInfo->idResult = retval;
    dlgInfo->flags |= DF_END;
    wasEnabled = (dlgInfo->flags & DF_OWNERENABLED);

    owner = GetWindow_k( hwnd, GW_OWNER );
    if (wasEnabled && owner)
        DIALOG_EnableOwner( owner );

    /* Windows sets the focus to the dialog itself in EndDialog */

    if (IsChild_k(hwnd, GetFocus_k()))
       SetFocus_k( hwnd );

    /* Don't have to send a ShowWindow(SW_HIDE), just do
       SetWindowPos with SWP_HIDEWINDOW as done in Windows */

    SetWindowPos_k(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE
                 | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

    if (hwnd == GetActiveWindow_k())
    {
        /* If this dialog was given an owner then set the focus to that owner
           even when the owner is disabled (normally when a window closes any
           disabled windows cannot receive the focus). */
        if (owner)
            SetForegroundWindow_k( owner );
        else
            WINPOS_ActivateOtherWindow( hwnd );
    }

    /* unblock dialog loop */
    PostMessageA_k(hwnd, WM_NULL, 0, 0);
    return TRUE;
}


/***********************************************************************
 *           DIALOG_IsAccelerator
 */
static BOOL DIALOG_IsAccelerator( HWND hwnd, HWND hwndDlg, WPARAM wParam )
{
    HWND hwndControl = hwnd;
    HWND hwndNext;
    INT dlgCode;
    WCHAR buffer[128];

    do
    {
        DWORD style = GetWindowLongW_k( hwndControl, GWL_STYLE );
        if ((style & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE)
        {
            dlgCode = SendMessageW_k( hwndControl, WM_GETDLGCODE, 0, 0 );
            if ( (dlgCode & (DLGC_BUTTON | DLGC_STATIC)) &&
                 GetWindowTextW_k( hwndControl, buffer, sizeof(buffer)/sizeof(WCHAR) ))
            {
                /* find the accelerator key */
                LPWSTR p = buffer - 2;

                do
                {
                    p = strchrW( p + 2, '&' );
                }
                while (p != NULL && p[1] == '&');

                /* and check if it's the one we're looking for */
                if (p != NULL && toupperW( p[1] ) == toupperW( (WCHAR)wParam ) )
                {
                    if ((dlgCode & DLGC_STATIC) || (style & 0x0f) == BS_GROUPBOX )
                    {
                        /* set focus to the control */
                        SendMessageW_k( hwndDlg, WM_NEXTDLGCTL, (WPARAM)hwndControl, 1);
                        /* and bump it on to next */
                        SendMessageW_k( hwndDlg, WM_NEXTDLGCTL, 0, 0);
                    }
                    else if (dlgCode & DLGC_BUTTON)
                    {
                        /* send BM_CLICK message to the control */
                        SendMessageW_k( hwndControl, BM_CLICK, 0, 0 );
                    }
                    return TRUE;
                }
            }
            hwndNext = GetWindow_k( hwndControl, GW_CHILD );
        }
        else hwndNext = 0;

        if (!hwndNext) hwndNext = GetWindow_k( hwndControl, GW_HWNDNEXT );

        while (!hwndNext && hwndControl)
        {
            hwndControl = GetParent_k( hwndControl );
            if (hwndControl == hwndDlg)
            {
                if(hwnd==hwndDlg)   /* prevent endless loop */
                {
                    hwndNext=hwnd;
                    break;
                }
                hwndNext = GetWindow_k( hwndDlg, GW_CHILD );
            }
            else
                hwndNext = GetWindow_k( hwndControl, GW_HWNDNEXT );
        }
        hwndControl = hwndNext;
    }
    while (hwndControl && (hwndControl != hwnd));

    return FALSE;
}

/***********************************************************************
 *           DIALOG_FindMsgDestination
 *
 * The messages that IsDialogMessage sends may not go to the dialog
 * calling IsDialogMessage if that dialog is a child, and it has the
 * DS_CONTROL style set.
 * We propagate up until we hit one that does not have DS_CONTROL, or
 * whose parent is not a dialog.
 *
 * This is undocumented behaviour.
 */
static HWND DIALOG_FindMsgDestination( HWND hwndDlg )
{
    while (GetWindowLongA_k(hwndDlg, GWL_STYLE) & DS_CONTROL)
    {
        WND *pParent;
        HWND hParent = GetParent_k(hwndDlg);
        if (!hParent) break;

        pParent = WIN_GetPtr(hParent);
        if (!pParent || pParent == WND_DESKTOP) break;

        if (!pParent->dlgInfo)
        {
            break;
        }

        hwndDlg = hParent;
    }

    return hwndDlg;
}

/***********************************************************************
 *              DIALOG_FixOneChildOnChangeFocus
 *
 * Callback helper for DIALOG_FixChildrenOnChangeFocus
 */

static BOOL CALLBACK DIALOG_FixOneChildOnChangeFocus (HWND hwndChild,
        LPARAM lParam)
{
    /* If a default pushbutton then no longer default */
    if (DLGC_DEFPUSHBUTTON & SendMessageW_k(hwndChild, WM_GETDLGCODE, 0, 0))
        SendMessageW_k(hwndChild, BM_SETSTYLE, BS_PUSHBUTTON, TRUE);
    return TRUE;
}

/***********************************************************************
 *              DIALOG_FixChildrenOnChangeFocus
 *
 * Following the change of focus that occurs for example after handling
 * a WM_KEYDOWN VK_TAB in IsDialogMessage, some tidying of the dialog's
 * children may be required.
 */
static void DIALOG_FixChildrenOnChangeFocus (HWND hwndDlg, HWND hwndNext)
{
    INT dlgcode_next = SendMessageW_k(hwndNext, WM_GETDLGCODE, 0, 0);
    /* INT dlgcode_dlg  = SendMessageW_k(hwndDlg, WM_GETDLGCODE, 0, 0); */
    /* Windows does ask for this.  I don't know why yet */

    EnumChildWindows_k(hwndDlg, DIALOG_FixOneChildOnChangeFocus, 0);

    /* If the button that is getting the focus WAS flagged as the default
     * pushbutton then ask the dialog what it thinks the default is and
     * set that in the default style.
     */
    if (dlgcode_next & DLGC_DEFPUSHBUTTON)
    {
        DWORD def_id = SendMessageW_k(hwndDlg, DM_GETDEFID, 0, 0);
        if (HIWORD(def_id) == DC_HASDEFID)
        {
            HWND hwndDef;
            def_id = LOWORD(def_id);
            hwndDef = GetDlgItem_k(hwndDlg, def_id);
            if (hwndDef)
            {
                INT dlgcode_def = SendMessageW_k(hwndDef, WM_GETDLGCODE, 0, 0);
                /* I know that if it is a button then it should already be a
                 * UNDEFPUSHBUTTON, since we have just told the buttons to 
                 * change style.  But maybe they ignored our request
                 */
                if ((dlgcode_def & DLGC_BUTTON) &&
                        (dlgcode_def &  DLGC_UNDEFPUSHBUTTON))
                {
                    SendMessageW_k(hwndDef, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
                }
            }
        }
    }
    else if ((dlgcode_next & DLGC_BUTTON) && (dlgcode_next & DLGC_UNDEFPUSHBUTTON))
    {
        SendMessageW_k(hwndNext, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE);
        /* I wonder why it doesn't send a DM_SETDEFID */
    }
}

/***********************************************************************
 *           DIALOG_IdToHwnd
 *
 * A recursive version of GetDlgItem
 *
 * RETURNS
 *  The HWND for a Child ID.
 */
static HWND DIALOG_IdToHwnd( HWND hwndDlg, INT id )
{
    int i;
    HWND *list = WIN_ListChildren( hwndDlg );
    HWND ret = 0;

    if (!list) return 0;

    for (i = 0; list[i]; i++)
    {
        if (GetWindowLongPtrW_k( list[i], GWLP_ID ) == id)
        {
            ret = list[i];
            break;
        }

        /* Recurse into every child */
        if ((ret = DIALOG_IdToHwnd( list[i], id ))) break;
    }

    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}

/***********************************************************************
 *		IsDialogMessageW (USER32.@)
 */
BOOL WINAPI IsDialogMessageW_k( HWND hwndDlg, LPMSG msg )
{
    INT dlgCode = 0;

    if (CallMsgFilterW_k( msg, MSGF_DIALOGBOX )) return TRUE;

    hwndDlg = WIN_GetFullHandle( hwndDlg );
    if (is_desktop_window(hwndDlg)) return FALSE;
    if ((hwndDlg != msg->hwnd) && !IsChild_k( hwndDlg, msg->hwnd )) return FALSE;

    hwndDlg = DIALOG_FindMsgDestination(hwndDlg);

    switch(msg->message)
    {
    case WM_KEYDOWN:
        dlgCode = SendMessageW_k( msg->hwnd, WM_GETDLGCODE, msg->wParam, (LPARAM)msg );
        if (dlgCode & (DLGC_WANTMESSAGE)) break;

        switch(msg->wParam)
        {
        case VK_TAB:
            if (!(dlgCode & DLGC_WANTTAB))
            {
                BOOL fIsDialog = TRUE;
                WND *pWnd = WIN_GetPtr( hwndDlg );

                if (pWnd )
                {
                    fIsDialog = (pWnd->dlgInfo != NULL);
                }

                /* I am not sure under which circumstances the TAB is handled
                 * each way.  All I do know is that it does not always simply
                 * send WM_NEXTDLGCTL.  (Personally I have never yet seen it
                 * do so but I presume someone has)
                 */
                if (fIsDialog)
                    SendMessageW_k( hwndDlg, WM_NEXTDLGCTL, (GetKeyState_k(VK_SHIFT) & 0x8000), 0 );
                else
                {
                    /* It would appear that GetNextDlgTabItem can handle being
                     * passed hwndDlg rather than NULL but that is undocumented
                     * so let's do it properly
                     */
                    HWND hwndFocus = GetFocus_k();
                    HWND hwndNext = GetNextDlgTabItem_k(hwndDlg,
                            hwndFocus == hwndDlg ? NULL : hwndFocus,
                            GetKeyState_k(VK_SHIFT) & 0x8000);
                    if (hwndNext)
                    {
                        dlgCode = SendMessageW_k(hwndNext, WM_GETDLGCODE,
                                msg->wParam, (LPARAM)msg);
                        if (dlgCode & DLGC_HASSETSEL)
                        {
                            INT maxlen = 1 + SendMessageW_k(hwndNext, WM_GETTEXTLENGTH, 0, 0);
                            WCHAR *buffer = HeapAlloc_k(GetProcessHeap_k(), 0, maxlen * sizeof(WCHAR));
                            if (buffer)
                            {
                                INT length;
                                SendMessageW_k(hwndNext, WM_GETTEXT, maxlen, (LPARAM) buffer);
                                length = strlenW (buffer);
                                HeapFree_k(GetProcessHeap_k(), 0, buffer);
                                SendMessageW_k(hwndNext, EM_SETSEL, 0, length);
                            }
                        }
                        SetFocus_k(hwndNext);
                        DIALOG_FixChildrenOnChangeFocus (hwndDlg, hwndNext);
                    }
                    else
                        return FALSE;
                }
                return TRUE;
            }
            break;

        case VK_RIGHT:
        case VK_DOWN:
        case VK_LEFT:
        case VK_UP:
            if (!(dlgCode & DLGC_WANTARROWS))
            {
                BOOL fPrevious = (msg->wParam == VK_LEFT || msg->wParam == VK_UP);
                HWND hwndNext = GetNextDlgGroupItem_k (hwndDlg, GetFocus_k(), fPrevious );
                SendMessageW_k( hwndDlg, WM_NEXTDLGCTL, (WPARAM)hwndNext, 1 );
                return TRUE;
            }
            break;

        case VK_CANCEL:
        case VK_ESCAPE:
            SendMessageW_k( hwndDlg, WM_COMMAND, IDCANCEL, (LPARAM)GetDlgItem_k( hwndDlg, IDCANCEL ) );
            return TRUE;

        case VK_EXECUTE:
        case VK_RETURN:
            {
                DWORD dw;
                if ((GetFocus_k() == msg->hwnd) &&
                    (SendMessageW_k(msg->hwnd, WM_GETDLGCODE, 0, 0) & DLGC_DEFPUSHBUTTON))
                {
                    SendMessageW_k(hwndDlg, WM_COMMAND, MAKEWPARAM (GetDlgCtrlID_k(msg->hwnd),BN_CLICKED), (LPARAM)msg->hwnd); 
                }
                else if (DC_HASDEFID == HIWORD(dw = SendMessageW_k(hwndDlg, DM_GETDEFID, 0, 0)))
                {
                    HWND hwndDef = DIALOG_IdToHwnd(hwndDlg, LOWORD(dw));
                    if (!hwndDef || IsWindowEnabled_k(hwndDef))
                        SendMessageW_k( hwndDlg, WM_COMMAND, MAKEWPARAM( LOWORD(dw), BN_CLICKED ), (LPARAM)hwndDef);
                }
                else
                {
                    SendMessageW_k( hwndDlg, WM_COMMAND, IDOK, (LPARAM)GetDlgItem_k( hwndDlg, IDOK ) );

                }
            }
            return TRUE;
        }
        break;

    case WM_CHAR:
        /* FIXME Under what circumstances does WM_GETDLGCODE get sent?
         * It does NOT get sent in the test program I have
         */
        dlgCode = SendMessageW_k( msg->hwnd, WM_GETDLGCODE, msg->wParam, (LPARAM)msg );
        if (dlgCode & (DLGC_WANTCHARS|DLGC_WANTMESSAGE)) break;
        if (msg->wParam == '\t' && (dlgCode & DLGC_WANTTAB)) break;
        /* drop through */

    case WM_SYSCHAR:
        if (DIALOG_IsAccelerator( WIN_GetFullHandle(msg->hwnd), hwndDlg, msg->wParam ))
        {
            /* don't translate or dispatch */
            return TRUE;
        }
        break;
    }

    TranslateMessage_k( msg );
    DispatchMessageW_k( msg );
    return TRUE;
}


/***********************************************************************
 *		GetDlgCtrlID (USER32.@)
 */
INT WINAPI GetDlgCtrlID_k( HWND hwnd )
{
    return GetWindowLongPtrW_k( hwnd, GWLP_ID );
}


/***********************************************************************
 *		GetDlgItem_k(USER32.@)
 */
HWND WINAPI GetDlgItem_k( HWND hwndDlg, INT id )
{
    int i;
    HWND *list = WIN_ListChildren( hwndDlg );
    HWND ret = 0;

    if (!list) return 0;

    for (i = 0; list[i]; i++) if (GetWindowLongPtrW_k( list[i], GWLP_ID ) == id) break;
    ret = list[i];
    HeapFree_k( GetProcessHeap_k(), 0, list );
    return ret;
}


/*******************************************************************
 *		SendDlgItemMessageA (USER32.@)
 */
LRESULT WINAPI SendDlgItemMessageA_k( HWND hwnd, INT id, UINT msg,
                                      WPARAM wParam, LPARAM lParam )
{
    HWND hwndCtrl = GetDlgItem_k( hwnd, id );
    if (hwndCtrl) return SendMessageA_k( hwndCtrl, msg, wParam, lParam );
    else return 0;
}


/*******************************************************************
 *		SendDlgItemMessageW (USER32.@)
 */
LRESULT WINAPI SendDlgItemMessageW_k( HWND hwnd, INT id, UINT msg,
                                      WPARAM wParam, LPARAM lParam )
{
    HWND hwndCtrl = GetDlgItem_k( hwnd, id );
    if (hwndCtrl) return SendMessageW_k( hwndCtrl, msg, wParam, lParam );
    else return 0;
}


/*******************************************************************
 *		SetDlgItemTextA (USER32.@)
 */
BOOL WINAPI SetDlgItemTextA_k( HWND hwnd, INT id, LPCSTR lpString )
{
    return SendDlgItemMessageA_k( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpString );
}


/*******************************************************************
 *		SetDlgItemTextW (USER32.@)
 */
BOOL WINAPI SetDlgItemTextW_k( HWND hwnd, INT id, LPCWSTR lpString )
{
    return SendDlgItemMessageW_k( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpString );
}


/***********************************************************************
 *		GetDlgItemTextA (USER32.@)
 */
UINT WINAPI GetDlgItemTextA_k( HWND hwnd, INT id, LPSTR str, INT len )
{
    if (str && (len > 0)) str[0] = '\0';
    return (UINT)SendDlgItemMessageA_k( hwnd, id, WM_GETTEXT,
                                         len, (LPARAM)str );
}


/***********************************************************************
 *		GetDlgItemTextW (USER32.@)
 */
UINT WINAPI GetDlgItemTextW_k( HWND hwnd, INT id, LPWSTR str, INT len )
{
    if (str && (len > 0)) str[0] = '\0';
    return (UINT)SendDlgItemMessageW_k( hwnd, id, WM_GETTEXT,
                                         len, (LPARAM)str );
}


/*******************************************************************
 *		SetDlgItemInt (USER32.@)
 */
BOOL WINAPI SetDlgItemInt_k( HWND hwnd, INT id, UINT value,
                             BOOL fSigned )
{
    char str[20];

    if (fSigned) sprintf( str, "%d", (INT)value );
    else sprintf( str, "%u", value );
    SendDlgItemMessageA_k( hwnd, id, WM_SETTEXT, 0, (LPARAM)str );
    return TRUE;
}


/***********************************************************************
 *		GetDlgItemInt (USER32.@)
 */
UINT WINAPI GetDlgItemInt_k( HWND hwnd, INT id, BOOL *translated,
                               BOOL fSigned )
{
    char str[30];
    char * endptr;
    LONG_PTR result = 0;

    if (translated) *translated = FALSE;
    if (!SendDlgItemMessageA_k(hwnd, id, WM_GETTEXT, sizeof(str), (LPARAM)str))
        return 0;
    if (fSigned)
    {
        result = strtol( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if (((result == LONG_MIN) || (result == LONG_MAX)) && (errno==ERANGE))
            return 0;
    }
    else
    {
        result = strtoul( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if ((result == ULONG_MAX) && (errno == ERANGE)) return 0;
    }
    if (translated) *translated = TRUE;
    return (UINT)result;
}


/***********************************************************************
 *		CheckDlgButton (USER32.@)
 */
BOOL WINAPI CheckDlgButton_k( HWND hwnd, INT id, UINT check )
{
    SendDlgItemMessageW_k( hwnd, id, BM_SETCHECK, check, 0 );
    return TRUE;
}


/***********************************************************************
 *		IsDlgButtonChecked (USER32.@)
 */
UINT WINAPI IsDlgButtonChecked_k( HWND hwnd, int id )
{
    return (UINT)SendDlgItemMessageW_k( hwnd, id, BM_GETCHECK, 0, 0 );
}


/***********************************************************************
 *           CheckRB
 *
 * Callback function used to check/uncheck radio buttons that fall
 * within a specific range of IDs.
 */
static BOOL CALLBACK CheckRB(HWND hwndChild, LPARAM lParam)
{
    LONG lChildID = GetWindowLongPtrW_k(hwndChild, GWLP_ID);
    RADIOGROUP *lpRadioGroup = (RADIOGROUP *) lParam;

    if (((UINT)lChildID >= lpRadioGroup->firstID) &&
        ((UINT)lChildID <= lpRadioGroup->lastID))
    {
        if (lChildID == lpRadioGroup->checkID)
        {
            SendMessageW_k(hwndChild, BM_SETCHECK, BST_CHECKED, 0);
        }
        else
        {
            SendMessageW_k(hwndChild, BM_SETCHECK, BST_UNCHECKED, 0);
        }
    }

    return TRUE;
}


/***********************************************************************
 *		CheckRadioButton (USER32.@)
 */
BOOL WINAPI CheckRadioButton_k( HWND hwndDlg, int firstID,
                              int lastID, int checkID )
{
    RADIOGROUP radioGroup;

    radioGroup.firstID = firstID;
    radioGroup.lastID = lastID;
    radioGroup.checkID = checkID;

    return EnumChildWindows_k(hwndDlg, CheckRB, (LPARAM)&radioGroup);
}


/***********************************************************************
 *		GetDialogBaseUnits (USER.243)
 *		GetDialogBaseUnits (USER32.@)
 */
DWORD WINAPI GetDialogBaseUnits_k(void)
{
    static DWORD units;

    if (!units)
    {
        HDC hdc;
        SIZE size;

        if ((hdc = GetDC_k(0)))
        {
            size.cx = GdiGetCharDimensions_k( hdc, NULL, &size.cy );
            if (size.cx) units = MAKELONG( size.cx, size.cy );
            ReleaseDC_k( 0, hdc );
        }
        TRACE("base units = %d,%d\n", LOWORD(units), HIWORD(units) );
    }
    return units;
}


/***********************************************************************
 *		MapDialogRect (USER32.@)
 */
BOOL WINAPI MapDialogRect_k( HWND hwnd, LPRECT rect )
{
    DIALOGINFO * dlgInfo;
    if (!(dlgInfo = DIALOG_get_info( hwnd, FALSE ))) return FALSE;
    rect->left   = MulDiv_k(rect->left, dlgInfo->xBaseUnit, 4);
    rect->right  = MulDiv_k(rect->right, dlgInfo->xBaseUnit, 4);
    rect->top    = MulDiv_k(rect->top, dlgInfo->yBaseUnit, 8);
    rect->bottom = MulDiv_k(rect->bottom, dlgInfo->yBaseUnit, 8);
    return TRUE;
}


/***********************************************************************
 *		GetNextDlgGroupItem (USER32.@)
 *
 * Corrections to MSDN documentation
 *
 * (Under Windows 2000 at least, where hwndDlg is not actually a dialog)
 * 1. hwndCtrl can be hwndDlg in which case it behaves as for NULL
 * 2. Prev of NULL or hwndDlg fails
 */
HWND WINAPI GetNextDlgGroupItem_k( HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious )
{
    HWND hwnd, hwndNext, retvalue, hwndLastGroup = 0;
    BOOL fLooped=FALSE;
    BOOL fSkipping=FALSE;

    hwndDlg = WIN_GetFullHandle( hwndDlg );
    hwndCtrl = WIN_GetFullHandle( hwndCtrl );

    if (hwndDlg == hwndCtrl) hwndCtrl = NULL;
    if (!hwndCtrl && fPrevious) return 0;

    if (hwndCtrl)
    {
        if (!IsChild_k(hwndDlg, hwndCtrl)) return 0;
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        if (!(hwndCtrl = GetWindow_k( hwndDlg, GW_CHILD ))) return 0;
        /* MSDN is wrong. fPrevious does not result in the last child */

        /* Maybe that first one is valid.  If so then we don't want to skip it*/
        if ((GetWindowLongW_k( hwndCtrl, GWL_STYLE ) & (WS_VISIBLE|WS_DISABLED)) == WS_VISIBLE)
        {
            return hwndCtrl;
        }
    }

    /* Always go forward around the group and list of controls; for the 
     * previous control keep track; for the next break when you find one
     */
    retvalue = hwndCtrl;
    hwnd = hwndCtrl;
    while (hwndNext = GetWindow_k(hwnd, GW_HWNDNEXT),
           1)
    {
        while (!hwndNext)
        {
            /* Climb out until there is a next sibling of the ancestor or we
             * reach the top (in which case we loop back to the start)
             */
            if (hwndDlg == GetParent_k(hwnd))
            {
                /* Wrap around to the beginning of the list, within the same
                 * group. (Once only)
                 */
                if (fLooped) goto end;
                fLooped = TRUE;
                hwndNext = GetWindow_k(hwndDlg, GW_CHILD);
            }
            else
            {
                hwnd = GetParent_k(hwnd);
                hwndNext = GetWindow_k(hwnd, GW_HWNDNEXT);
            }
        }
        hwnd = hwndNext;

        /* Wander down the leading edge of controlparents */
        while ( (GetWindowLongW_k(hwnd, GWL_EXSTYLE) & WS_EX_CONTROLPARENT) &&
                ((GetWindowLongW_k(hwnd, GWL_STYLE) & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE) &&
                (hwndNext = GetWindow_k(hwnd, GW_CHILD)))
            hwnd = hwndNext;
        /* Question.  If the control is a control parent but either has no
         * children or is not visible/enabled then if it has a WS_GROUP does
         * it count?  For that matter does it count anyway?
         * I believe it doesn't count.
         */

        if ((GetWindowLongW_k(hwnd, GWL_STYLE) & WS_GROUP))
        {
            hwndLastGroup = hwnd;
            if (!fSkipping)
            {
                /* Look for the beginning of the group */
                fSkipping = TRUE;
            }
        }

        if (hwnd == hwndCtrl)
        {
            if (!fSkipping) break;
            if (hwndLastGroup == hwnd) break;
            hwnd = hwndLastGroup;
            fSkipping = FALSE;
            fLooped = FALSE;
        }

        if (!fSkipping &&
            (GetWindowLongW_k(hwnd, GWL_STYLE) & (WS_VISIBLE|WS_DISABLED)) ==
             WS_VISIBLE)
        {
            retvalue = hwnd;
            if (!fPrevious) break;
        }
    }
end:
    return retvalue;
}


/***********************************************************************
 *           DIALOG_GetNextTabItem
 *
 * Recursive helper for GetNextDlgTabItem
 */
static HWND DIALOG_GetNextTabItem( HWND hwndMain, HWND hwndDlg, HWND hwndCtrl, BOOL fPrevious )
{
    LONG dsStyle;
    LONG exStyle;
    UINT wndSearch = fPrevious ? GW_HWNDPREV : GW_HWNDNEXT;
    HWND retWnd = 0;
    HWND hChildFirst = 0;

    if(!hwndCtrl)
    {
        hChildFirst = GetWindow_k(hwndDlg,GW_CHILD);
        if(fPrevious) hChildFirst = GetWindow_k(hChildFirst,GW_HWNDLAST);
    }
    else if (IsChild_k( hwndMain, hwndCtrl ))
    {
        hChildFirst = GetWindow_k(hwndCtrl,wndSearch);
        if(!hChildFirst)
        {
            if(GetParent_k(hwndCtrl) != hwndMain)
                /* i.e. if we are not at the top level of the recursion */
                hChildFirst = GetWindow_k(GetParent_k(hwndCtrl),wndSearch);
            else
                hChildFirst = GetWindow_k(hwndCtrl, fPrevious ? GW_HWNDLAST : GW_HWNDFIRST);
        }
    }

    while(hChildFirst)
    {
        dsStyle = GetWindowLongA_k(hChildFirst,GWL_STYLE);
        exStyle = GetWindowLongA_k(hChildFirst,GWL_EXSTYLE);
        if( (exStyle & WS_EX_CONTROLPARENT) && (dsStyle & WS_VISIBLE) && !(dsStyle & WS_DISABLED))
        {
            HWND retWnd;
            retWnd = DIALOG_GetNextTabItem(hwndMain,hChildFirst,NULL,fPrevious );
            if (retWnd) return (retWnd);
        }
        else if( (dsStyle & WS_TABSTOP) && (dsStyle & WS_VISIBLE) && !(dsStyle & WS_DISABLED))
        {
            return (hChildFirst);
        }
        hChildFirst = GetWindow_k(hChildFirst,wndSearch);
    }
    if(hwndCtrl)
    {
        HWND hParent = GetParent_k(hwndCtrl);
        while(hParent)
        {
            if(hParent == hwndMain) break;
            retWnd = DIALOG_GetNextTabItem(hwndMain,GetParent_k(hParent),hParent,fPrevious );
            if(retWnd) break;
            hParent = GetParent_k(hParent);
        }
        if(!retWnd)
            retWnd = DIALOG_GetNextTabItem(hwndMain,hwndMain,NULL,fPrevious );
    }
    return retWnd ? retWnd : hwndCtrl;
}

/***********************************************************************
 *		GetNextDlgTabItem (USER32.@)
 */
HWND WINAPI GetNextDlgTabItem_k( HWND hwndDlg, HWND hwndCtrl,
                                   BOOL fPrevious )
{
    hwndDlg = WIN_GetFullHandle( hwndDlg );
    hwndCtrl = WIN_GetFullHandle( hwndCtrl );

    /* Undocumented but tested under Win2000 and WinME */
    if (hwndDlg == hwndCtrl) hwndCtrl = NULL;

    /* Contrary to MSDN documentation, tested under Win2000 and WinME
     * NB GetLastError returns whatever was set before the function was
     * called.
     */
    if (!hwndCtrl && fPrevious) return 0;

    return DIALOG_GetNextTabItem(hwndDlg,hwndDlg,hwndCtrl,fPrevious);
}

/**********************************************************************
 *           DIALOG_DlgDirSelect
 *
 * Helper function for DlgDirSelect*
 */
static BOOL DIALOG_DlgDirSelect( HWND hwnd, LPWSTR str, INT len,
                                 INT id, BOOL unicode, BOOL combo )
{
    WCHAR *buffer, *ptr;
    INT item, size;
    BOOL ret;
    HWND listbox = GetDlgItem_k( hwnd, id );

    TRACE("%p %s %d\n", hwnd, unicode ? debugstr_w(str) : debugstr_a((LPSTR)str), id );
    if (!listbox) return FALSE;

    item = SendMessageW_k(listbox, combo ? CB_GETCURSEL : LB_GETCURSEL, 0, 0 );
    if (item == LB_ERR) return FALSE;

    size = SendMessageW_k(listbox, combo ? CB_GETLBTEXTLEN : LB_GETTEXTLEN, item, 0 );
    if (size == LB_ERR) return FALSE;

    if (!(buffer = HeapAlloc_k(GetProcessHeap_k(), 0, (size+2) * sizeof(WCHAR) ))) return FALSE;

    SendMessageW_k( listbox, combo ? CB_GETLBTEXT : LB_GETTEXT, item, (LPARAM)buffer );

    if ((ret = (buffer[0] == '[')))  /* drive or directory */
    {
        if (buffer[1] == '-')  /* drive */
        {
            buffer[3] = ':';
            buffer[4] = 0;
            ptr = buffer + 2;
        }
        else
        {
            buffer[strlenW(buffer)-1] = '\\';
            ptr = buffer + 1;
        }
    }
    else
    {
        /* Filenames without a dot extension must have one tacked at the end */
        if (strchrW(buffer, '.') == NULL)
        {
            buffer[strlenW(buffer)+1] = '\0';
            buffer[strlenW(buffer)] = '.';
        }
        ptr = buffer;
    }

    if (!unicode)
    {
        if (len > 0 && !WideCharToMultiByte_k( CP_ACP, 0, ptr, -1, (LPSTR)str, len, 0, 0 ))
            ((LPSTR)str)[len-1] = 0;
    }
    else lstrcpynW( str, ptr, len );
    HeapFree_k( GetProcessHeap_k(), 0, buffer );
    TRACE("Returning %d %s\n", ret, unicode ? debugstr_w(str) : debugstr_a((LPSTR)str) );
    return ret;
}


/**********************************************************************
 *	    DIALOG_DlgDirListW
 *
 * Helper function for DlgDirList*W
 */
static INT DIALOG_DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                               INT idStatic, UINT attrib, BOOL combo )
{
    HWND hwnd;
    LPWSTR orig_spec = spec;
    WCHAR any[] = {'*','.','*',0};

#define SENDMSG(msg,wparam,lparam) \
    ((attrib & DDL_POSTMSGS) ? PostMessageW_k( hwnd, msg, wparam, lparam ) \
                             : SendMessageW_k( hwnd, msg, wparam, lparam ))

    TRACE("%p %s %d %d %04x\n", hDlg, debugstr_w(spec), idLBox, idStatic, attrib );

    /* If the path exists and is a directory, chdir to it */
    if (!spec || !spec[0] || SetCurrentDirectoryW_k( spec )) spec = any;
    else
    {
        WCHAR *p, *p2;
        p = spec;
        if ((p2 = strchrW( p, ':' ))) p = p2 + 1;
        if ((p2 = strrchrW( p, '\\' ))) p = p2;
        if ((p2 = strrchrW( p, '/' ))) p = p2;
        if (p != spec)
        {
            WCHAR sep = *p;
            *p = 0;
            if (!SetCurrentDirectoryW_k( spec ))
            {
                *p = sep;  /* Restore the original spec */
                return FALSE;
            }
            spec = p + 1;
        }
    }

    TRACE( "mask=%s\n", debugstr_w(spec) );

    if (idLBox && ((hwnd = GetDlgItem_k( hDlg, idLBox )) != 0))
    {
        if (attrib == DDL_DRIVES) attrib |= DDL_EXCLUSIVE;

        SENDMSG( combo ? CB_RESETCONTENT : LB_RESETCONTENT, 0, 0 );
        if (attrib & DDL_DIRECTORY)
        {
            if (!(attrib & DDL_EXCLUSIVE))
            {
                SENDMSG( combo ? CB_DIR : LB_DIR,
                         attrib & ~(DDL_DIRECTORY | DDL_DRIVES),
                         (LPARAM)spec );
            }
            SENDMSG( combo ? CB_DIR : LB_DIR,
                   (attrib & (DDL_DIRECTORY | DDL_DRIVES)) | DDL_EXCLUSIVE,
                   (LPARAM)any );
        }
        else
        {
            SENDMSG( combo ? CB_DIR : LB_DIR, attrib, (LPARAM)spec );
        }
    }

    /* Convert path specification to uppercase */
    if (spec) CharUpperW_k(spec);

    if (idStatic && ((hwnd = GetDlgItem_k( hDlg, idStatic )) != 0))
    {
        WCHAR temp[MAX_PATH];
        GetCurrentDirectoryW_k( sizeof(temp)/sizeof(WCHAR), temp );
        CharLowerW_k( temp );
        /* Can't use PostMessage() here, because the string is on the stack */
        SetDlgItemTextW_k( hDlg, idStatic, temp );
    }

    if (orig_spec && (spec != orig_spec))
    {
        /* Update the original file spec */
        WCHAR *p = spec;
        while ((*orig_spec++ = *p++));
    }

    return TRUE;
#undef SENDMSG
}


/**********************************************************************
 *	    DIALOG_DlgDirListA
 *
 * Helper function for DlgDirList*A
 */
static INT DIALOG_DlgDirListA( HWND hDlg, LPSTR spec, INT idLBox,
                               INT idStatic, UINT attrib, BOOL combo )
{
    if (spec)
    {
        INT ret, len = MultiByteToWideChar_k( CP_ACP, 0, spec, -1, NULL, 0 );
        LPWSTR specW = HeapAlloc_k(GetProcessHeap_k(), 0, len * sizeof(WCHAR) );
        MultiByteToWideChar_k( CP_ACP, 0, spec, -1, specW, len );
        ret = DIALOG_DlgDirListW( hDlg, specW, idLBox, idStatic, attrib, combo );
        WideCharToMultiByte_k( CP_ACP, 0, specW, -1, spec, 0x7fffffff, NULL, NULL );
        HeapFree_k( GetProcessHeap_k(), 0, specW );
        return ret;
    }
    return DIALOG_DlgDirListW( hDlg, NULL, idLBox, idStatic, attrib, combo );
}


/**********************************************************************
 *		DlgDirSelectExA (USER32.@)
 */
BOOL WINAPI DlgDirSelectExA_k( HWND hwnd, LPSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, (LPWSTR)str, len, id, FALSE, FALSE );
}


/**********************************************************************
 *		DlgDirSelectExW (USER32.@)
 */
BOOL WINAPI DlgDirSelectExW_k( HWND hwnd, LPWSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, TRUE, FALSE );
}


/**********************************************************************
 *		DlgDirSelectComboBoxExA (USER32.@)
 */
BOOL WINAPI DlgDirSelectComboBoxExA_k( HWND hwnd, LPSTR str, INT len,
                                         INT id )
{
    return DIALOG_DlgDirSelect( hwnd, (LPWSTR)str, len, id, FALSE, TRUE );
}


/**********************************************************************
 *		DlgDirSelectComboBoxExW (USER32.@)
 */
BOOL WINAPI DlgDirSelectComboBoxExW_k( HWND hwnd, LPWSTR str, INT len,
                                         INT id)
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, TRUE, TRUE );
}


/**********************************************************************
 *		DlgDirListA (USER32.@)
 */
INT WINAPI DlgDirListA_k( HWND hDlg, LPSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListA( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *		DlgDirListW (USER32.@)
 */
INT WINAPI DlgDirListW_k( HWND hDlg, LPWSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *		DlgDirListComboBoxA (USER32.@)
 */
INT WINAPI DlgDirListComboBoxA_k( HWND hDlg, LPSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListA( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}


/**********************************************************************
 *		DlgDirListComboBoxW (USER32.@)
 */
INT WINAPI DlgDirListComboBoxW_k( HWND hDlg, LPWSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}
