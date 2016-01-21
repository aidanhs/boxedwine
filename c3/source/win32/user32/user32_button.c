/* File: button.c -- Button type widgets
 *
 * Copyright (C) 1993 Johannes Ruscheinski
 * Copyright (C) 1993 David Metcalfe
 * Copyright (C) 1994 Alexandre Julliard
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
 * NOTES
 *
 * This code was audited for completeness against the documented features
 * of Comctl32.dll version 6.0 on Oct. 3, 2004, by Dimitrie O. Paun.
 * 
 * Unless otherwise noted, we believe this code to be complete, as per
 * the specification mentioned above.
 * If you discover missing features, or bugs, please note them below.
 * 
 * TODO
 *  Styles
 *  - BS_NOTIFY: is it complete?
 *  - BS_RIGHTBUTTON: same as BS_LEFTTEXT
 *
 *  Messages
 *  - WM_CHAR: Checks a (manual or automatic) check box on '+' or '=', clears it on '-' key.
 *  - WM_SETFOCUS: For (manual or automatic) radio buttons, send the parent window BN_CLICKED
 *  - WM_NCCREATE: Turns any BS_OWNERDRAW button into a BS_PUSHBUTTON button.
 *  - WM_SYSKEYUP
 *  - BCM_GETIDEALSIZE
 *  - BCM_GETIMAGELIST
 *  - BCM_GETTEXTMARGIN
 *  - BCM_SETIMAGELIST
 *  - BCM_SETTEXTMARGIN
 *  
 *  Notifications
 *  - BCN_HOTITEMCHANGE
 *  - BN_DISABLE
 *  - BN_PUSHED/BN_HILITE
 *  + BN_KILLFOCUS: is it OK?
 *  - BN_PAINT
 *  + BN_SETFOCUS: is it OK?
 *  - BN_UNPUSHED/BN_UNHILITE
 *  - NM_CUSTOMDRAW
 *
 *  Structures/Macros/Definitions
 *  - BUTTON_IMAGELIST
 *  - NMBCHOTITEM
 *  - Button_GetIdealSize
 *  - Button_GetImageList
 *  - Button_GetTextMargin
 *  - Button_SetImageList
 *  - Button_SetTextMargin
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define OEMRESOURCE

#include "../include/winbase.h"
#include "user_private.h"
#include "controls.h"
#include "win.h"

/* GetWindowLong offsets for window extra information */
#define STATE_GWL_OFFSET  0
#define HFONT_GWL_OFFSET  (sizeof(LONG))
#define HIMAGE_GWL_OFFSET (HFONT_GWL_OFFSET+sizeof(HFONT))
#define NB_EXTRA_BYTES    (HIMAGE_GWL_OFFSET+sizeof(HANDLE))

/* undocumented flags */
#define BUTTON_NSTATES         0x0F
#define BUTTON_BTNPRESSED      0x40
#define BUTTON_UNKNOWN2        0x20
#define BUTTON_UNKNOWN3        0x10

#define BUTTON_NOTIFY_PARENT(hWnd, code) \
    do { /* Notify parent which has created this button control */ \
        TRACE("notification " #code " sent to hwnd=%p\n", GetParent_k(hWnd)); \
        SendMessageW_k(GetParent_k(hWnd), WM_COMMAND, \
                     MAKEWPARAM(GetWindowLongPtrW_k((hWnd),GWLP_ID), (code)), \
                     (LPARAM)(hWnd)); \
    } while(0)

static UINT BUTTON_CalcLabelRect( HWND hwnd, HDC hdc, RECT *rc );
static void PB_Paint( HWND hwnd, HDC hDC, UINT action );
static void CB_Paint( HWND hwnd, HDC hDC, UINT action );
static void GB_Paint( HWND hwnd, HDC hDC, UINT action );
static void UB_Paint( HWND hwnd, HDC hDC, UINT action );
static void OB_Paint( HWND hwnd, HDC hDC, UINT action );
static void BUTTON_CheckAutoRadioButton( HWND hwnd );

#define MAX_BTN_TYPE  16

static const WORD maxCheckState[MAX_BTN_TYPE] =
{
    BST_UNCHECKED,      /* BS_PUSHBUTTON */
    BST_UNCHECKED,      /* BS_DEFPUSHBUTTON */
    BST_CHECKED,        /* BS_CHECKBOX */
    BST_CHECKED,        /* BS_AUTOCHECKBOX */
    BST_CHECKED,        /* BS_RADIOBUTTON */
    BST_INDETERMINATE,  /* BS_3STATE */
    BST_INDETERMINATE,  /* BS_AUTO3STATE */
    BST_UNCHECKED,      /* BS_GROUPBOX */
    BST_UNCHECKED,      /* BS_USERBUTTON */
    BST_CHECKED,        /* BS_AUTORADIOBUTTON */
    BST_UNCHECKED,      /* BS_PUSHBOX */
    BST_UNCHECKED       /* BS_OWNERDRAW */
};

typedef void (*pfPaint)( HWND hwnd, HDC hdc, UINT action );

static const pfPaint btnPaintFunc[MAX_BTN_TYPE] =
{
    PB_Paint,    /* BS_PUSHBUTTON */
    PB_Paint,    /* BS_DEFPUSHBUTTON */
    CB_Paint,    /* BS_CHECKBOX */
    CB_Paint,    /* BS_AUTOCHECKBOX */
    CB_Paint,    /* BS_RADIOBUTTON */
    CB_Paint,    /* BS_3STATE */
    CB_Paint,    /* BS_AUTO3STATE */
    GB_Paint,    /* BS_GROUPBOX */
    UB_Paint,    /* BS_USERBUTTON */
    CB_Paint,    /* BS_AUTORADIOBUTTON */
    NULL,        /* BS_PUSHBOX */
    OB_Paint     /* BS_OWNERDRAW */
};

static HBITMAP hbitmapCheckBoxes = 0;
static WORD checkBoxWidth = 0, checkBoxHeight = 0;


/*********************************************************************
 * button class descriptor
 */
static const WCHAR buttonW[] = {'B','u','t','t','o','n',0};
const struct builtin_class_descr BUTTON_builtin_class =
{
    buttonW,             /* name */
    CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW | CS_PARENTDC, /* style  */
    WINPROC_BUTTON,      /* proc */
    NB_EXTRA_BYTES,      /* extra */
    IDC_ARROW,           /* cursor */
    0                    /* brush */
};


static INLINE LONG get_button_state( HWND hwnd )
{
    return GetWindowLongW_k( hwnd, STATE_GWL_OFFSET );
}

static INLINE void set_button_state( HWND hwnd, LONG state )
{
    SetWindowLongW_k( hwnd, STATE_GWL_OFFSET, state );
}

static INLINE HFONT get_button_font( HWND hwnd )
{
    return (HFONT)GetWindowLongPtrW_k( hwnd, HFONT_GWL_OFFSET );
}

static INLINE void set_button_font( HWND hwnd, HFONT font )
{
    SetWindowLongPtrW_k( hwnd, HFONT_GWL_OFFSET, (LONG_PTR)font );
}

static INLINE UINT get_button_type( LONG window_style )
{
    return (window_style & BS_TYPEMASK);
}

/* paint a button of any type */
static INLINE void paint_button( HWND hwnd, LONG style, UINT action )
{
    if (btnPaintFunc[style] && IsWindowVisible_k(hwnd))
    {
        HDC hdc = GetDC_k( hwnd );
        btnPaintFunc[style]( hwnd, hdc, action );
        ReleaseDC_k( hwnd, hdc );
    }
}

/* retrieve the button text; returned buffer must be freed by caller */
static INLINE WCHAR *get_button_text( HWND hwnd )
{
    INT len = 512;
    WCHAR *buffer = HeapAlloc_k( GetProcessHeap_k(), 0, (len + 1) * sizeof(WCHAR) );
    if (buffer) InternalGetWindowText_k( hwnd, buffer, len + 1 );
    return buffer;
}

/***********************************************************************
 *           ButtonWndProc_common
 */
LRESULT ButtonWndProc_common(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL unicode )
{
    RECT rect;
    POINT pt;
    LONG style = GetWindowLongW_k( hWnd, GWL_STYLE );
    UINT btn_type = get_button_type( style );
    LONG state;
    HANDLE oldHbitmap;

    if (!IsWindow_k( hWnd )) return 0;

    pt.x = (short)LOWORD(lParam);
    pt.y = (short)HIWORD(lParam);

    switch (uMsg)
    {
    case WM_GETDLGCODE:
        switch(btn_type)
        {
        case BS_USERBUTTON:
        case BS_PUSHBUTTON:      return DLGC_BUTTON | DLGC_UNDEFPUSHBUTTON;
        case BS_DEFPUSHBUTTON:   return DLGC_BUTTON | DLGC_DEFPUSHBUTTON;
        case BS_RADIOBUTTON:
        case BS_AUTORADIOBUTTON: return DLGC_BUTTON | DLGC_RADIOBUTTON;
        case BS_GROUPBOX:        return DLGC_STATIC;
        default:                 return DLGC_BUTTON;
        }

    case WM_ENABLE:
        paint_button( hWnd, btn_type, ODA_DRAWENTIRE );
        break;

    case WM_CREATE:
        if (!hbitmapCheckBoxes)
        {
            BITMAP bmp;
            hbitmapCheckBoxes = LoadBitmapW_k(0, MAKEINTRESOURCEW(OBM_CHECKBOXES));
            GetObjectW_k( hbitmapCheckBoxes, sizeof(bmp), &bmp );
            checkBoxWidth  = bmp.bmWidth / 4;
            checkBoxHeight = bmp.bmHeight / 3;
        }
        if (btn_type >= MAX_BTN_TYPE)
            return -1; /* abort */

        /* XP turns a BS_USERBUTTON into BS_PUSHBUTTON */
        if (btn_type == BS_USERBUTTON )
        {
            style = (style & ~BS_TYPEMASK) | BS_PUSHBUTTON;
            WIN_SetStyle( hWnd, style, BS_TYPEMASK & ~style );
        }
        set_button_state( hWnd, BST_UNCHECKED );
        return 0;

    case WM_ERASEBKGND:
        if (btn_type == BS_OWNERDRAW)
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            HBRUSH hBrush;
            HWND parent = GetParent_k(hWnd);
            if (!parent) parent = hWnd;
            hBrush = (HBRUSH)SendMessageW_k(parent, WM_CTLCOLORBTN, (WPARAM)hdc, (LPARAM)hWnd);
            if (!hBrush) /* did the app forget to call defwindowproc ? */
                hBrush = (HBRUSH)DefWindowProcW_k(parent, WM_CTLCOLORBTN,
                                                (WPARAM)hdc, (LPARAM)hWnd);
            GetClientRect_k(hWnd, &rc);
            FillRect_k(hdc, &rc, hBrush);
        }
        return 1;

    case WM_PRINTCLIENT:
    case WM_PAINT:
        if (btnPaintFunc[btn_type])
        {
            PAINTSTRUCT ps;
            HDC hdc = wParam ? (HDC)wParam : BeginPaint_k( hWnd, &ps );
            int nOldMode = SetBkMode_k( hdc, OPAQUE );
            (btnPaintFunc[btn_type])( hWnd, hdc, ODA_DRAWENTIRE );
            SetBkMode_k(hdc, nOldMode); /*  reset painting mode */
            if( !wParam ) EndPaint_k( hWnd, &ps );
        }
        break;

    case WM_KEYDOWN:
	if (wParam == VK_SPACE)
	{
	    SendMessageW_k( hWnd, BM_SETSTATE, TRUE, 0 );
            set_button_state( hWnd, get_button_state( hWnd ) | BUTTON_BTNPRESSED );
            SetCapture_k( hWnd );
	}
	break;

    case WM_LBUTTONDBLCLK:
        if(style & BS_NOTIFY ||
           btn_type == BS_RADIOBUTTON ||
           btn_type == BS_USERBUTTON ||
           btn_type == BS_OWNERDRAW)
        {
            BUTTON_NOTIFY_PARENT(hWnd, BN_DOUBLECLICKED);
            break;
        }
        /* fall through */
    case WM_LBUTTONDOWN:
        SetCapture_k( hWnd );
        SetFocus_k( hWnd );
        set_button_state( hWnd, get_button_state( hWnd ) | BUTTON_BTNPRESSED );
        SendMessageW_k( hWnd, BM_SETSTATE, TRUE, 0 );
        break;

    case WM_KEYUP:
	if (wParam != VK_SPACE)
	    break;
	/* fall through */
    case WM_LBUTTONUP:
        state = get_button_state( hWnd );
        if (!(state & BUTTON_BTNPRESSED)) break;
        state &= BUTTON_NSTATES;
        set_button_state( hWnd, state );
        if (!(state & BST_PUSHED))
        {
            ReleaseCapture_k();
            break;
        }
        SendMessageW_k( hWnd, BM_SETSTATE, FALSE, 0 );
        ReleaseCapture_k();
        GetClientRect_k( hWnd, &rect );
	if (uMsg == WM_KEYUP || PtInRect_k( &rect, pt ))
        {
            state = get_button_state( hWnd );
            switch(btn_type)
            {
            case BS_AUTOCHECKBOX:
                SendMessageW_k( hWnd, BM_SETCHECK, !(state & BST_CHECKED), 0 );
                break;
            case BS_AUTORADIOBUTTON:
                SendMessageW_k( hWnd, BM_SETCHECK, TRUE, 0 );
                break;
            case BS_AUTO3STATE:
                SendMessageW_k( hWnd, BM_SETCHECK,
                                (state & BST_INDETERMINATE) ? 0 : ((state & 3) + 1), 0 );
                break;
            }
            BUTTON_NOTIFY_PARENT(hWnd, BN_CLICKED);
        }
        break;

    case WM_CAPTURECHANGED:
        TRACE("WM_CAPTURECHANGED %p\n", hWnd);
        state = get_button_state( hWnd );
        if (state & BUTTON_BTNPRESSED)
        {
            state &= BUTTON_NSTATES;
            set_button_state( hWnd, state );
            if (state & BST_PUSHED) SendMessageW_k( hWnd, BM_SETSTATE, FALSE, 0 );
        }
        break;

    case WM_MOUSEMOVE:
        if ((wParam & MK_LBUTTON) && GetCapture_k() == hWnd)
        {
            GetClientRect_k( hWnd, &rect );
            SendMessageW_k( hWnd, BM_SETSTATE, PtInRect_k(&rect, pt), 0 );
        }
        break;

    case WM_SETTEXT:
    {
        /* Clear an old text here as Windows does */
        HDC hdc = GetDC_k(hWnd);
        HBRUSH hbrush;
        RECT client, rc;
        HWND parent = GetParent_k(hWnd);

        if (!parent) parent = hWnd;
        hbrush = (HBRUSH)SendMessageW_k(parent, WM_CTLCOLORSTATIC,
				      (WPARAM)hdc, (LPARAM)hWnd);
        if (!hbrush) /* did the app forget to call DefWindowProc ? */
            hbrush = (HBRUSH)DefWindowProcW_k(parent, WM_CTLCOLORSTATIC,
					    (WPARAM)hdc, (LPARAM)hWnd);

        GetClientRect_k(hWnd, &client);
        rc = client;
        BUTTON_CalcLabelRect(hWnd, hdc, &rc);
        /* Clip by client rect bounds */
        if (rc.right > client.right) rc.right = client.right;
        if (rc.bottom > client.bottom) rc.bottom = client.bottom;
        FillRect_k(hdc, &rc, hbrush);
        ReleaseDC_k(hWnd, hdc);

        if (unicode) DefWindowProcW_k( hWnd, WM_SETTEXT, wParam, lParam );
        else DefWindowProcA_k( hWnd, WM_SETTEXT, wParam, lParam );
        if (btn_type == BS_GROUPBOX) /* Yes, only for BS_GROUPBOX */
            InvalidateRect_k( hWnd, NULL, TRUE );
        else
            paint_button( hWnd, btn_type, ODA_DRAWENTIRE );
        return 1; /* success. FIXME: check text length */
    }

    case WM_SETFONT:
        set_button_font( hWnd, (HFONT)wParam );
        if (lParam) InvalidateRect_k(hWnd, NULL, TRUE);
        break;

    case WM_GETFONT:
        return (LRESULT)get_button_font( hWnd );

    case WM_SETFOCUS:
        TRACE("WM_SETFOCUS %p\n",hWnd);
        set_button_state( hWnd, get_button_state(hWnd) | BST_FOCUS );
        paint_button( hWnd, btn_type, ODA_FOCUS );
        if (style & BS_NOTIFY)
            BUTTON_NOTIFY_PARENT(hWnd, BN_SETFOCUS);
        break;

    case WM_KILLFOCUS:
        TRACE("WM_KILLFOCUS %p\n",hWnd);
        state = get_button_state( hWnd );
        set_button_state( hWnd, state & ~BST_FOCUS );
	paint_button( hWnd, btn_type, ODA_FOCUS );

        if ((state & BUTTON_BTNPRESSED) && GetCapture_k() == hWnd)
            ReleaseCapture_k();
        if (style & BS_NOTIFY)
            BUTTON_NOTIFY_PARENT(hWnd, BN_KILLFOCUS);

        InvalidateRect_k( hWnd, NULL, FALSE );
        break;

    case WM_SYSCOLORCHANGE:
        InvalidateRect_k( hWnd, NULL, FALSE );
        break;

    case BM_SETSTYLE:
        if ((wParam & BS_TYPEMASK) >= MAX_BTN_TYPE) break;
        btn_type = wParam & BS_TYPEMASK;
        style = (style & ~BS_TYPEMASK) | btn_type;
        WIN_SetStyle( hWnd, style, BS_TYPEMASK & ~style );

        /* Only redraw if lParam flag is set.*/
        if (lParam)
            InvalidateRect_k( hWnd, NULL, TRUE );

        break;

    case BM_CLICK:
	SendMessageW_k( hWnd, WM_LBUTTONDOWN, 0, 0 );
	SendMessageW_k( hWnd, WM_LBUTTONUP, 0, 0 );
	break;

    case BM_SETIMAGE:
        /* Check that image format matches button style */
        switch (style & (BS_BITMAP|BS_ICON))
        {
        case BS_BITMAP:
            if (wParam != IMAGE_BITMAP) return 0;
            break;
        case BS_ICON:
            if (wParam != IMAGE_ICON) return 0;
            break;
        default:
            return 0;
        }
        oldHbitmap = (HBITMAP)SetWindowLongPtrW_k( hWnd, HIMAGE_GWL_OFFSET, lParam );
	InvalidateRect_k( hWnd, NULL, FALSE );
	return (LRESULT)oldHbitmap;

    case BM_GETIMAGE:
        return GetWindowLongPtrW_k( hWnd, HIMAGE_GWL_OFFSET );

    case BM_GETCHECK:
        return get_button_state( hWnd ) & 3;

    case BM_SETCHECK:
        if (wParam > maxCheckState[btn_type]) wParam = maxCheckState[btn_type];
        state = get_button_state( hWnd );
        if ((btn_type == BS_RADIOBUTTON) || (btn_type == BS_AUTORADIOBUTTON))
        {
            if (wParam) WIN_SetStyle( hWnd, WS_TABSTOP, 0 );
            else WIN_SetStyle( hWnd, 0, WS_TABSTOP );
        }
        if ((state & 3) != wParam)
        {
            set_button_state( hWnd, (state & ~3) | wParam );
            paint_button( hWnd, btn_type, ODA_SELECT );
        }
        if ((btn_type == BS_AUTORADIOBUTTON) && (wParam == BST_CHECKED) && (style & WS_CHILD))
            BUTTON_CheckAutoRadioButton( hWnd );
        break;

    case BM_GETSTATE:
        return get_button_state( hWnd );

    case BM_SETSTATE:
        state = get_button_state( hWnd );
        if (wParam)
            set_button_state( hWnd, state | BST_PUSHED );
        else
            set_button_state( hWnd, state & ~BST_PUSHED );

        paint_button( hWnd, btn_type, ODA_SELECT );
        break;

    case WM_NCHITTEST:
        if(btn_type == BS_GROUPBOX) return HTTRANSPARENT;
        /* fall through */
    default:
        return unicode ? DefWindowProcW_k(hWnd, uMsg, wParam, lParam) :
                         DefWindowProcA_k(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

/**********************************************************************
 * Convert button styles to flags used by DrawText.
 */
static UINT BUTTON_BStoDT( DWORD style, DWORD ex_style )
{
   UINT dtStyle = DT_NOCLIP;  /* We use SelectClipRgn_k to limit output */

   /* "Convert" pushlike buttons to pushbuttons */
   if (style & BS_PUSHLIKE)
      style &= ~BS_TYPEMASK;

   if (!(style & BS_MULTILINE))
      dtStyle |= DT_SINGLELINE;
   else
      dtStyle |= DT_WORDBREAK;

   switch (style & BS_CENTER)
   {
      case BS_LEFT:   /* DT_LEFT is 0 */    break;
      case BS_RIGHT:  dtStyle |= DT_RIGHT;  break;
      case BS_CENTER: dtStyle |= DT_CENTER; break;
      default:
         /* Pushbutton's text is centered by default */
         if (get_button_type(style) <= BS_DEFPUSHBUTTON) dtStyle |= DT_CENTER;
         /* all other flavours have left aligned text */
   }

   if (ex_style & WS_EX_RIGHT) dtStyle = DT_RIGHT | (dtStyle & ~(DT_LEFT | DT_CENTER));

   /* DrawText ignores vertical alignment for multiline text,
    * but we use these flags to align label manually.
    */
   if (get_button_type(style) != BS_GROUPBOX)
   {
      switch (style & BS_VCENTER)
      {
         case BS_TOP:     /* DT_TOP is 0 */      break;
         case BS_BOTTOM:  dtStyle |= DT_BOTTOM;  break;
         case BS_VCENTER: /* fall through */
         default:         dtStyle |= DT_VCENTER; break;
      }
   }
   else
      /* GroupBox's text is always single line and is top aligned. */
      dtStyle |= DT_SINGLELINE;

   return dtStyle;
}

/**********************************************************************
 *       BUTTON_CalcLabelRect
 *
 *   Calculates label's Rectangle_k depending on button style.
 *
 * Returns flags to be passed to DrawText.
 * Calculated Rectangle_k doesn't take into account button state
 * (pushed, etc.). If there is nothing to draw (no text/image) output
 * Rectangle_k is empty, and return value is (UINT)-1.
 */
static UINT BUTTON_CalcLabelRect(HWND hwnd, HDC hdc, RECT *rc)
{
   LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
   LONG ex_style = GetWindowLongW_k( hwnd, GWL_EXSTYLE );
   WCHAR *text;
   ICONINFO    iconInfo;
   BITMAP      bm;
   UINT        dtStyle = BUTTON_BStoDT( style, ex_style );
   RECT        r = *rc;
   INT         n;

   /* Calculate label Rectangle_k according to label type */
   switch (style & (BS_ICON|BS_BITMAP))
   {
      case BS_TEXT:
          if (!(text = get_button_text( hwnd ))) goto empty_rect;
          if (!text[0])
          {
              HeapFree_k( GetProcessHeap_k(), 0, text );
              goto empty_rect;
          }
          DrawTextW_k(hdc, text, -1, &r, dtStyle | DT_CALCRECT);
          HeapFree_k( GetProcessHeap_k(), 0, text );
          break;

      case BS_ICON:
         if (!GetIconInfo_k((HICON)GetWindowLongPtrW_k( hwnd, HIMAGE_GWL_OFFSET ), &iconInfo))
            goto empty_rect;

         GetObjectW_k (iconInfo.hbmColor, sizeof(BITMAP), &bm);

         r.right  = r.left + bm.bmWidth;
         r.bottom = r.top  + bm.bmHeight;

         DeleteObject_k(iconInfo.hbmColor);
         DeleteObject_k(iconInfo.hbmMask);
         break;

      case BS_BITMAP:
         if (!GetObjectW_k( (HANDLE)GetWindowLongPtrW_k( hwnd, HIMAGE_GWL_OFFSET ), sizeof(BITMAP), &bm))
            goto empty_rect;

         r.right  = r.left + bm.bmWidth;
         r.bottom = r.top  + bm.bmHeight;
         break;

      default:
      empty_rect:
         rc->right = r.left;
         rc->bottom = r.top;
         return (UINT)-1;
   }

   /* Position label inside bounding Rectangle_k according to
    * alignment flags. (calculated rect is always left-top aligned).
    * If label is aligned to any side - shift label in opposite
    * direction to leave extra space for focus Rectangle_k.
    */
   switch (dtStyle & (DT_CENTER|DT_RIGHT))
   {
      case DT_LEFT:    r.left++;  r.right++;  break;
      case DT_CENTER:  n = r.right - r.left;
                       r.left   = rc->left + ((rc->right - rc->left) - n) / 2;
                       r.right  = r.left + n; break;
      case DT_RIGHT:   n = r.right - r.left;
                       r.right  = rc->right - 1;
                       r.left   = r.right - n;
                       break;
   }

   switch (dtStyle & (DT_VCENTER|DT_BOTTOM))
   {
      case DT_TOP:     r.top++;  r.bottom++;  break;
      case DT_VCENTER: n = r.bottom - r.top;
                       r.top    = rc->top + ((rc->bottom - rc->top) - n) / 2;
                       r.bottom = r.top + n;  break;
      case DT_BOTTOM:  n = r.bottom - r.top;
                       r.bottom = rc->bottom - 1;
                       r.top    = r.bottom - n;
                       break;
   }

   *rc = r;
   return dtStyle;
}


/**********************************************************************
 *       BUTTON_DrawTextCallback
 *
 *   Callback function used by DrawStateW function.
 */
static BOOL CALLBACK BUTTON_DrawTextCallback(HDC hdc, LPARAM lp, WPARAM wp, int cx, int cy)
{
   RECT rc;
   rc.left = 0;
   rc.top = 0;
   rc.right = cx;
   rc.bottom = cy;

   DrawTextW_k(hdc, (LPCWSTR)lp, -1, &rc, (UINT)wp);
   return TRUE;
}


/**********************************************************************
 *       BUTTON_DrawLabel
 *
 *   Common function for drawing button label.
 */
static void BUTTON_DrawLabel(HWND hwnd, HDC hdc, UINT dtFlags, const RECT *rc)
{
   DRAWSTATEPROC lpOutputProc = NULL;
   LPARAM lp;
   WPARAM wp = 0;
   HBRUSH hbr = 0;
   UINT flags = IsWindowEnabled_k(hwnd) ? DSS_NORMAL : DSS_DISABLED;
   LONG state = get_button_state( hwnd );
   LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
   WCHAR *text = NULL;

   /* FIXME: To draw disabled label in Win31 look-and-feel, we probably
    * must use DSS_MONO flag and COLOR_GRAYTEXT brush (or maybe DSS_UNION).
    * I don't have Win31 on hand to verify that, so I leave it as is.
    */

   if ((style & BS_PUSHLIKE) && (state & BST_INDETERMINATE))
   {
      hbr = GetSysColorBrush_k(COLOR_GRAYTEXT);
      flags |= DSS_MONO;
   }

   switch (style & (BS_ICON|BS_BITMAP))
   {
      case BS_TEXT:
         /* DST_COMPLEX -- is 0 */
         lpOutputProc = BUTTON_DrawTextCallback;
         if (!(text = get_button_text( hwnd ))) return;
         lp = (LPARAM)text;
         wp = dtFlags;
         break;

      case BS_ICON:
         flags |= DST_ICON;
         lp = GetWindowLongPtrW_k( hwnd, HIMAGE_GWL_OFFSET );
         break;

      case BS_BITMAP:
         flags |= DST_BITMAP;
         lp = GetWindowLongPtrW_k( hwnd, HIMAGE_GWL_OFFSET );
         break;

      default:
         return;
   }

   DrawStateW_k(hdc, hbr, lpOutputProc, lp, wp, rc->left, rc->top,
              rc->right - rc->left, rc->bottom - rc->top, flags);
   HeapFree_k( GetProcessHeap_k(), 0, text );
}

/**********************************************************************
 *       Push Button Functions
 */
static void PB_Paint( HWND hwnd, HDC hDC, UINT action )
{
    RECT     rc, r;
    UINT     dtFlags, uState;
    HPEN     hOldPen;
    HBRUSH   hOldBrush;
    INT      oldBkMode;
    COLORREF oldTxtColor;
    HFONT hFont;
    LONG state = get_button_state( hwnd );
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    BOOL pushedState = (state & BST_PUSHED);
    HWND parent;
    HRGN hrgn;

    GetClientRect_k( hwnd, &rc );

    /* Send WM_CTLCOLOR to allow changing the font (the colors are fixed) */
    if ((hFont = get_button_font( hwnd ))) SelectObject_k( hDC, hFont );
    parent = GetParent_k(hwnd);
    if (!parent) parent = hwnd;
    SendMessageW_k( parent, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hwnd );

    hrgn = set_control_clipping( hDC, &rc );

    hOldPen = SelectObject_k(hDC, SYSCOLOR_GetPen(COLOR_WINDOWFRAME));
    hOldBrush = SelectObject_k(hDC,GetSysColorBrush_k(COLOR_BTNFACE));
    oldBkMode = SetBkMode_k(hDC, TRANSPARENT);

    if (get_button_type(style) == BS_DEFPUSHBUTTON)
    {
        if (action != ODA_FOCUS)
            Rectangle_k(hDC, rc.left, rc.top, rc.right, rc.bottom);
	InflateRect_k( &rc, -1, -1 );
    }

    /* completely skip the drawing if only focus has changed */
    if (action == ODA_FOCUS) goto draw_focus;

    uState = DFCS_BUTTONPUSH;

    if (style & BS_FLAT)
        uState |= DFCS_MONO;
    else if (pushedState)
    {
	if (get_button_type(style) == BS_DEFPUSHBUTTON )
	    uState |= DFCS_FLAT;
	else
	    uState |= DFCS_PUSHED;
    }

    if (state & (BST_CHECKED | BST_INDETERMINATE))
        uState |= DFCS_CHECKED;

    DrawFrameControl_k( hDC, &rc, DFC_BUTTON, uState );

    /* draw button label */
    r = rc;
    dtFlags = BUTTON_CalcLabelRect(hwnd, hDC, &r);

    if (dtFlags == (UINT)-1L)
       goto cleanup;

    if (pushedState)
       OffsetRect_k(&r, 1, 1);

    oldTxtColor = SetTextColor_k( hDC, GetSysColor_k(COLOR_BTNTEXT) );

    BUTTON_DrawLabel(hwnd, hDC, dtFlags, &r);

    SetTextColor_k( hDC, oldTxtColor );

draw_focus:
    if (action == ODA_FOCUS || (state & BST_FOCUS))
    {
        InflateRect_k( &rc, -2, -2 );
        DrawFocusRect_k( hDC, &rc );
    }

 cleanup:
    SelectObject_k( hDC, hOldPen );
    SelectObject_k( hDC, hOldBrush );
    SetBkMode_k(hDC, oldBkMode);
    SelectClipRgn_k( hDC, hrgn );
    if (hrgn) DeleteObject_k( hrgn );
}

/**********************************************************************
 *       Check Box & Radio Button Functions
 */

static void CB_Paint( HWND hwnd, HDC hDC, UINT action )
{
    RECT rbox, rtext, client;
    HBRUSH hBrush;
    int delta;
    UINT dtFlags;
    HFONT hFont;
    LONG state = get_button_state( hwnd );
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    HWND parent;
    HRGN hrgn;

    if (style & BS_PUSHLIKE)
    {
        PB_Paint( hwnd, hDC, action );
	return;
    }

    GetClientRect_k(hwnd, &client);
    rbox = rtext = client;

    if ((hFont = get_button_font( hwnd ))) SelectObject_k( hDC, hFont );

    parent = GetParent_k(hwnd);
    if (!parent) parent = hwnd;
    hBrush = (HBRUSH)SendMessageW_k(parent, WM_CTLCOLORSTATIC,
				  (WPARAM)hDC, (LPARAM)hwnd);
    if (!hBrush) /* did the app forget to call defwindowproc ? */
        hBrush = (HBRUSH)DefWindowProcW_k(parent, WM_CTLCOLORSTATIC,
					(WPARAM)hDC, (LPARAM)hwnd );
    hrgn = set_control_clipping( hDC, &client );

    if (style & BS_LEFTTEXT)
    {
	/* magic +4 is what CTL3D expects */

        rtext.right -= checkBoxWidth + 4;
        rbox.left = rbox.right - checkBoxWidth;
    }
    else
    {
        rtext.left += checkBoxWidth + 4;
        rbox.right = checkBoxWidth;
    }

    /* Since WM_ERASEBKGND does nothing, first prepare background */
    if (action == ODA_SELECT) FillRect_k( hDC, &rbox, hBrush );
    if (action == ODA_DRAWENTIRE) FillRect_k( hDC, &client, hBrush );

    /* Draw label */
    client = rtext;
    dtFlags = BUTTON_CalcLabelRect(hwnd, hDC, &rtext);
    
    /* Only adjust rbox when rtext is valid */
    if (dtFlags != (UINT)-1L)
    {
	rbox.top = rtext.top;
	rbox.bottom = rtext.bottom;
    }

    /* Draw the check-box bitmap */
    if (action == ODA_DRAWENTIRE || action == ODA_SELECT)
    {
	UINT flags;

	if ((get_button_type(style) == BS_RADIOBUTTON) ||
	    (get_button_type(style) == BS_AUTORADIOBUTTON)) flags = DFCS_BUTTONRADIO;
	else if (state & BST_INDETERMINATE) flags = DFCS_BUTTON3STATE;
	else flags = DFCS_BUTTONCHECK;

	if (state & (BST_CHECKED | BST_INDETERMINATE)) flags |= DFCS_CHECKED;
	if (state & BST_PUSHED) flags |= DFCS_PUSHED;

	if (style & WS_DISABLED) flags |= DFCS_INACTIVE;

	/* rbox must have the correct height */
	delta = rbox.bottom - rbox.top - checkBoxHeight;
	
	if (style & BS_TOP) {
	    if (delta > 0) {
		rbox.bottom = rbox.top + checkBoxHeight;
	    } else { 
		rbox.top -= -delta/2 + 1;
		rbox.bottom = rbox.top + checkBoxHeight;
	    }
	} else if (style & BS_BOTTOM) {
	    if (delta > 0) {
		rbox.top = rbox.bottom - checkBoxHeight;
	    } else {
		rbox.bottom += -delta/2 + 1;
		rbox.top = rbox.bottom - checkBoxHeight;
	    }
	} else { /* Default */
	    if (delta > 0) {
		int ofs = (delta / 2);
		rbox.bottom -= ofs + 1;
		rbox.top = rbox.bottom - checkBoxHeight;
	    } else if (delta < 0) {
		int ofs = (-delta / 2);
		rbox.top -= ofs + 1;
		rbox.bottom = rbox.top + checkBoxHeight;
	    }
	}

	DrawFrameControl_k( hDC, &rbox, DFC_BUTTON, flags );
    }

    if (dtFlags == (UINT)-1L) /* Noting to draw */
	return;

    if (action == ODA_DRAWENTIRE)
	BUTTON_DrawLabel(hwnd, hDC, dtFlags, &rtext);

    /* ... and focus */
    if (action == ODA_FOCUS || (state & BST_FOCUS))
    {
	rtext.left--;
	rtext.right++;
	IntersectRect_k(&rtext, &rtext, &client);
	DrawFocusRect_k( hDC, &rtext );
    }
    SelectClipRgn_k( hDC, hrgn );
    if (hrgn) DeleteObject_k( hrgn );
}


/**********************************************************************
 *       BUTTON_CheckAutoRadioButton
 *
 * hwnd is checked, uncheck every other auto radio button in group
 */
static void BUTTON_CheckAutoRadioButton( HWND hwnd )
{
    HWND parent, sibling, start;

    parent = GetParent_k(hwnd);
    /* make sure that starting control is not disabled or invisible */
    start = sibling = GetNextDlgGroupItem_k( parent, hwnd, TRUE );
    do
    {
        if (!sibling) break;
        if ((hwnd != sibling) &&
            ((GetWindowLongW_k( sibling, GWL_STYLE) & BS_TYPEMASK) == BS_AUTORADIOBUTTON))
            SendMessageW_k( sibling, BM_SETCHECK, BST_UNCHECKED, 0 );
        sibling = GetNextDlgGroupItem_k( parent, sibling, FALSE );
    } while (sibling != start);
}


/**********************************************************************
 *       Group Box Functions
 */

static void GB_Paint( HWND hwnd, HDC hDC, UINT action )
{
    RECT rc, rcFrame;
    HBRUSH hbr;
    HFONT hFont;
    UINT dtFlags;
    TEXTMETRICW tm;
    LONG style = GetWindowLongW_k( hwnd, GWL_STYLE );
    HWND parent;
    HRGN hrgn;

    if ((hFont = get_button_font( hwnd ))) SelectObject_k( hDC, hFont );
    /* GroupBox acts like static control, so it sends CTLCOLORSTATIC */
    parent = GetParent_k(hwnd);
    if (!parent) parent = hwnd;
    hbr = (HBRUSH)SendMessageW_k(parent, WM_CTLCOLORSTATIC, (WPARAM)hDC, (LPARAM)hwnd);
    if (!hbr) /* did the app forget to call defwindowproc ? */
        hbr = (HBRUSH)DefWindowProcW_k(parent, WM_CTLCOLORSTATIC,
				     (WPARAM)hDC, (LPARAM)hwnd);
    GetClientRect_k( hwnd, &rc);
    rcFrame = rc;
    hrgn = set_control_clipping( hDC, &rc );

    GetTextMetricsW_k(hDC, &tm);
    rcFrame.top += (tm.tmHeight / 2) - 1;
    DrawEdge_k(hDC, &rcFrame, EDGE_ETCHED, BF_RECT | ((style & BS_FLAT) ? BF_FLAT : 0));

    InflateRect_k(&rc, -7, 1);
    dtFlags = BUTTON_CalcLabelRect(hwnd, hDC, &rc);

    if (dtFlags != (UINT)-1)
    {
        /* Because buttons have CS_PARENTDC class style, there is a chance
         * that label will be drawn out of client rect.
         * But Windows doesn't clip label's rect, so do I.
         */

        /* There is 1-pixel margin at the left, right, and bottom */
        rc.left--; rc.right++; rc.bottom++;
        FillRect_k(hDC, &rc, hbr);
        rc.left++; rc.right--; rc.bottom--;

        BUTTON_DrawLabel(hwnd, hDC, dtFlags, &rc);
    }
    SelectClipRgn_k( hDC, hrgn );
    if (hrgn) DeleteObject_k( hrgn );
}


/**********************************************************************
 *       User Button Functions
 */

static void UB_Paint( HWND hwnd, HDC hDC, UINT action )
{
    RECT rc;
    HBRUSH hBrush;
    HFONT hFont;
    LONG state = get_button_state( hwnd );
    HWND parent;

    GetClientRect_k( hwnd, &rc);

    if ((hFont = get_button_font( hwnd ))) SelectObject_k( hDC, hFont );

    parent = GetParent_k(hwnd);
    if (!parent) parent = hwnd;
    hBrush = (HBRUSH)SendMessageW_k(parent, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hwnd);
    if (!hBrush) /* did the app forget to call defwindowproc ? */
        hBrush = (HBRUSH)DefWindowProcW_k(parent, WM_CTLCOLORBTN,
					(WPARAM)hDC, (LPARAM)hwnd);

    FillRect_k( hDC, &rc, hBrush );
    if (action == ODA_FOCUS || (state & BST_FOCUS))
        DrawFocusRect_k( hDC, &rc );

    switch (action)
    {
    case ODA_FOCUS:
        BUTTON_NOTIFY_PARENT( hwnd, (state & BST_FOCUS) ? BN_SETFOCUS : BN_KILLFOCUS );
        break;

    case ODA_SELECT:
        BUTTON_NOTIFY_PARENT( hwnd, (state & BST_PUSHED) ? BN_HILITE : BN_UNHILITE );
        break;

    default:
        BUTTON_NOTIFY_PARENT( hwnd, BN_PAINT );
        break;
    }
}


/**********************************************************************
 *       Ownerdrawn Button Functions
 */

static void OB_Paint( HWND hwnd, HDC hDC, UINT action )
{
    LONG state = get_button_state( hwnd );
    DRAWITEMSTRUCT dis;
    LONG_PTR id = GetWindowLongPtrW_k( hwnd, GWLP_ID );
    HWND parent;
    HFONT hFont, hPrevFont = 0;
    HRGN hrgn;

    dis.CtlType    = ODT_BUTTON;
    dis.CtlID      = id;
    dis.itemID     = 0;
    dis.itemAction = action;
    dis.itemState  = ((state & BST_FOCUS) ? ODS_FOCUS : 0) |
                     ((state & BST_PUSHED) ? ODS_SELECTED : 0) |
                     (IsWindowEnabled_k(hwnd) ? 0: ODS_DISABLED);
    dis.hwndItem   = hwnd;
    dis.hDC        = hDC;
    dis.itemData   = 0;
    GetClientRect_k( hwnd, &dis.rcItem );

    if ((hFont = get_button_font( hwnd ))) hPrevFont = SelectObject_k( hDC, hFont );
    parent = GetParent_k(hwnd);
    if (!parent) parent = hwnd;
    SendMessageW_k( parent, WM_CTLCOLORBTN, (WPARAM)hDC, (LPARAM)hwnd );

    hrgn = set_control_clipping( hDC, &dis.rcItem );

    SendMessageW_k( GetParent_k(hwnd), WM_DRAWITEM, id, (LPARAM)&dis );
    if (hPrevFont) SelectObject_k(hDC, hPrevFont);
    SelectClipRgn_k( hDC, hrgn );
    if (hrgn) DeleteObject_k( hrgn );
}
