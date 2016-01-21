/*
 * Message boxes
 *
 * Copyright 1995 Bernd Schmidt
 * Copyright 2004 Ivan Leo Puoti, Juan Lang
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
#include "../include/unicode.h"
#include "user_private.h"

#define MSGBOX_IDICON stc1
#define MSGBOX_IDTEXT 100
#define IDS_ERROR     2

struct ThreadWindows
{
    UINT numHandles;
    UINT numAllocs;
    HWND *handles;
};

static BOOL CALLBACK MSGBOX_EnumProc(HWND hwnd, LPARAM lParam)
{
    struct ThreadWindows *threadWindows = (struct ThreadWindows *)lParam;

    if (!EnableWindow_k(hwnd, FALSE))
    {
        if(threadWindows->numHandles >= threadWindows->numAllocs)
        {
            threadWindows->handles = HeapReAlloc_k(GetProcessHeap_k(), 0, threadWindows->handles,
                                                 (threadWindows->numAllocs*2)*sizeof(HWND));
            threadWindows->numAllocs *= 2;
        }
        threadWindows->handles[threadWindows->numHandles++]=hwnd;
    }
   return TRUE;
}

static HFONT MSGBOX_OnInit(HWND hwnd, LPMSGBOXPARAMSW lpmb)
{
    HFONT hFont = 0, hPrevFont = 0;
    RECT rect;
    HWND hItem;
    HDC hdc;
    int i, buttons;
    int bspace, bw, bh, theight, tleft, wwidth, wheight, wleft, wtop, bpos;
    int borheight, borwidth, iheight, ileft, iwidth, twidth, tiheight;
    NONCLIENTMETRICSW nclm;
    HMONITOR monitor = 0;
    MONITORINFO mon_info;
    LPCWSTR lpszText;
    WCHAR *buffer = NULL;
    const WCHAR *ptr;

    /* Index the order the buttons need to appear to an ID* constant */
    static const int buttonOrder[10] = { IDYES, IDNO, IDOK, IDABORT, IDRETRY,
                                         IDCANCEL, IDIGNORE, IDTRYAGAIN,
                                         IDCONTINUE, IDHELP };

    nclm.cbSize = sizeof(nclm);
    SystemParametersInfoW_k(SPI_GETNONCLIENTMETRICS, 0, &nclm, 0);
    hFont = CreateFontIndirectW_k(&nclm.lfMessageFont);
    /* set button font */
    for (i = IDOK; i <= IDCONTINUE; i++)
        /* no close button */
        if (i != IDCLOSE)
            SendDlgItemMessageW_k (hwnd, i, WM_SETFONT, (WPARAM)hFont, 0);

    /* set text font */
    SendDlgItemMessageW_k (hwnd, MSGBOX_IDTEXT, WM_SETFONT, (WPARAM)hFont, 0);

    if (!IS_INTRESOURCE(lpmb->lpszCaption)) {
       SetWindowTextW_k(hwnd, lpmb->lpszCaption);
    } else {
        UINT len = LoadStringW_k( lpmb->hInstance, LOWORD(lpmb->lpszCaption), (LPWSTR)&ptr, 0 );
        if (!len) len = LoadStringW_k( user32_module, IDS_ERROR, (LPWSTR)&ptr, 0 );
        buffer = HeapAlloc_k( GetProcessHeap_k(), 0, (len + 1) * sizeof(WCHAR) );
        if (buffer)
        {
            memcpy( buffer, ptr, len * sizeof(WCHAR) );
            buffer[len] = 0;
            SetWindowTextW_k( hwnd, buffer );
            HeapFree_k( GetProcessHeap_k(), 0, buffer );
            buffer = NULL;
        }
    }
    if (IS_INTRESOURCE(lpmb->lpszText)) {
        UINT len = LoadStringW_k( lpmb->hInstance, LOWORD(lpmb->lpszText), (LPWSTR)&ptr, 0 );
        lpszText = buffer = HeapAlloc_k( GetProcessHeap_k(), 0, (len + 1) * sizeof(WCHAR) );
        if (buffer)
        {
            memcpy( buffer, ptr, len * sizeof(WCHAR) );
            buffer[len] = 0;
        }
    } else {
       lpszText = lpmb->lpszText;
    }

    TRACE_(msgbox)("%s\n", debugstr_w(lpszText));
    SetWindowTextW_k(GetDlgItem_k(hwnd, MSGBOX_IDTEXT), lpszText);

    /* Remove not selected buttons */
    switch(lpmb->dwStyle & MB_TYPEMASK) {
    case MB_OK:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCANCEL));
	/* fall through */
    case MB_OKCANCEL:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDABORT));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDRETRY));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDIGNORE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDYES));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDNO));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDTRYAGAIN));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCONTINUE));
	break;
    case MB_ABORTRETRYIGNORE:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDOK));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCANCEL));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDYES));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDNO));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCONTINUE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDTRYAGAIN));
	break;
    case MB_YESNO:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCANCEL));
	/* fall through */
    case MB_YESNOCANCEL:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDOK));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDABORT));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDRETRY));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDIGNORE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCONTINUE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDTRYAGAIN));
	break;
    case MB_RETRYCANCEL:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDOK));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDABORT));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDIGNORE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDYES));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDNO));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDCONTINUE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDTRYAGAIN));
	break;
    case MB_CANCELTRYCONTINUE:
	DestroyWindow_k(GetDlgItem_k(hwnd, IDOK));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDABORT));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDIGNORE));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDYES));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDNO));
	DestroyWindow_k(GetDlgItem_k(hwnd, IDRETRY));
    }
    /* Set the icon */
    switch(lpmb->dwStyle & MB_ICONMASK) {
    case MB_ICONEXCLAMATION:
	SendDlgItemMessageW_k(hwnd, MSGBOX_IDICON, STM_SETICON,
			    (WPARAM)LoadIconW_k(0, (LPWSTR)IDI_EXCLAMATION), 0);
	break;
    case MB_ICONQUESTION:
	SendDlgItemMessageW_k(hwnd, MSGBOX_IDICON, STM_SETICON,
			    (WPARAM)LoadIconW_k(0, (LPWSTR)IDI_QUESTION), 0);
	break;
    case MB_ICONASTERISK:
	SendDlgItemMessageW_k(hwnd, MSGBOX_IDICON, STM_SETICON,
			    (WPARAM)LoadIconW_k(0, (LPWSTR)IDI_ASTERISK), 0);
	break;
    case MB_ICONHAND:
      SendDlgItemMessageW_k(hwnd, MSGBOX_IDICON, STM_SETICON,
			    (WPARAM)LoadIconW_k(0, (LPWSTR)IDI_HAND), 0);
      break;
    case MB_USERICON:
      SendDlgItemMessageW_k(hwnd, MSGBOX_IDICON, STM_SETICON,
			  (WPARAM)LoadIconW_k(lpmb->hInstance, lpmb->lpszIcon), 0);
      break;
    default:
	/* By default, Windows 95/98/NT do not associate an icon to message boxes.
         * So wine should do the same.
         */
	break;
    }

    /* Remove Help button unless MB_HELP supplied */
    if (!(lpmb->dwStyle & MB_HELP)) {
        DestroyWindow_k(GetDlgItem_k(hwnd, IDHELP));
    }

    /* Position everything */
    GetWindowRect_k(hwnd, &rect);
    borheight = rect.bottom - rect.top;
    borwidth  = rect.right - rect.left;
    GetClientRect_k(hwnd, &rect);
    borheight -= rect.bottom - rect.top;
    borwidth  -= rect.right - rect.left;

    /* Get the icon height */
    GetWindowRect_k(GetDlgItem_k(hwnd, MSGBOX_IDICON), &rect);
    MapWindowPoints_k(0, hwnd, (LPPOINT)&rect, 2);
    if (!(lpmb->dwStyle & MB_ICONMASK))
    {
        rect.bottom = rect.top;
        rect.right = rect.left;
    }
    iheight = rect.bottom - rect.top;
    ileft = rect.left;
    iwidth = rect.right - ileft;

    hdc = GetDC_k(hwnd);
    if (hFont)
	hPrevFont = SelectObject_k(hdc, hFont);

    /* Get the number of visible buttons and their size */
    bh = bw = 1; /* Minimum button sizes */
    for (buttons = 0, i = IDOK; i <= IDCONTINUE; i++)
    {
        if (i == IDCLOSE) continue; /* No CLOSE button */
	hItem = GetDlgItem_k(hwnd, i);
	if (GetWindowLongW_k(hItem, GWL_STYLE) & WS_VISIBLE)
	{
	    WCHAR buttonText[1024];
	    int w, h;
	    buttons++;
	    if (GetWindowTextW_k(hItem, buttonText, 1024))
	    {
		DrawTextW_k( hdc, buttonText, -1, &rect, DT_LEFT | DT_EXPANDTABS | DT_CALCRECT);
		h = rect.bottom - rect.top;
		w = rect.right - rect.left;
		if (h > bh) bh = h;
		if (w > bw)  bw = w ;
	    }
	}
    }
    bw = max(bw, bh * 2);
    /* Button white space */
    bh = bh * 2;
    bw = bw * 2;
    bspace = bw/3; /* Space between buttons */

    /* Get the text size */
    GetClientRect_k(GetDlgItem_k(hwnd, MSGBOX_IDTEXT), &rect);
    rect.top = rect.left = rect.bottom = 0;
    DrawTextW_k( hdc, lpszText, -1, &rect,
	       DT_LEFT | DT_EXPANDTABS | DT_WORDBREAK | DT_CALCRECT);
    /* Min text width corresponds to space for the buttons */
    tleft = ileft;
    if (iwidth) tleft += ileft + iwidth;
    twidth = max((bw + bspace) * buttons + bspace - tleft, rect.right);
    theight = rect.bottom;

    if (hFont)
	SelectObject_k(hdc, hPrevFont);
    ReleaseDC_k(hwnd, hdc);

    tiheight = 16 + max(iheight, theight);
    wwidth  = tleft + twidth + ileft + borwidth;
    wheight = 8 + tiheight + bh + borheight;

    /* Message boxes are always desktop centered, so query desktop size and center window */
    monitor = MonitorFromWindow_k(lpmb->hwndOwner ? lpmb->hwndOwner : GetActiveWindow_k(), MONITOR_DEFAULTTOPRIMARY);
    mon_info.cbSize = sizeof(mon_info);
    GetMonitorInfoW_k(monitor, &mon_info);
    wleft = (mon_info.rcWork.left + mon_info.rcWork.right - wwidth) / 2;
    wtop = (mon_info.rcWork.top + mon_info.rcWork.bottom - wheight) / 2;

    /* Resize and center the window */
    SetWindowPos_k(hwnd, 0, wleft, wtop, wwidth, wheight,
		 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);

    /* Position the icon */
    SetWindowPos_k(GetDlgItem_k(hwnd, MSGBOX_IDICON), 0, ileft, (tiheight - iheight) / 2, 0, 0,
		 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);

    /* Position the text */
    SetWindowPos_k(GetDlgItem_k(hwnd, MSGBOX_IDTEXT), 0, tleft, (tiheight - theight) / 2, twidth, theight,
		 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);

    /* Position the buttons */
    bpos = (wwidth - (bw + bspace) * buttons + bspace) / 2;
    for (buttons = i = 0; i < (sizeof(buttonOrder) / sizeof(buttonOrder[0])); i++) {

	/* Convert the button order to ID* value to order for the buttons */
	hItem = GetDlgItem_k(hwnd, buttonOrder[i]);
	if (GetWindowLongW_k(hItem, GWL_STYLE) & WS_VISIBLE) {
	    if (buttons++ == ((lpmb->dwStyle & MB_DEFMASK) >> 8)) {
		SetFocus_k(hItem);
		SendMessageW_k( hItem, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE );
	    }
	    SetWindowPos_k(hItem, 0, bpos, tiheight, bw, bh,
			 SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW);
	    bpos += bw + bspace;
	}
    }

    /*handle modal message boxes*/
    if (((lpmb->dwStyle & MB_TASKMODAL) && (lpmb->hwndOwner==NULL)) || (lpmb->dwStyle & MB_SYSTEMMODAL))
        SetWindowPos_k(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    HeapFree_k( GetProcessHeap_k(), 0, buffer );
    return hFont;
}


/**************************************************************************
 *           MSGBOX_DlgProc
 *
 * Dialog procedure for message boxes.
 */
static INT_PTR CALLBACK MSGBOX_DlgProc( HWND hwnd, UINT message,
                                        WPARAM wParam, LPARAM lParam )
{
  HFONT hFont;

  switch(message) {
   case WM_INITDIALOG:
   {
       LPMSGBOXPARAMSW mbp = (LPMSGBOXPARAMSW)lParam;
       SetWindowContextHelpId_k(hwnd, mbp->dwContextHelpId);
       hFont = MSGBOX_OnInit(hwnd, mbp);
       SetPropA_k(hwnd, "WINE_MSGBOX_HFONT", hFont);
       SetPropA_k(hwnd, "WINE_MSGBOX_HELPCALLBACK", mbp->lpfnMsgBoxCallback);
       break;
   }

   case WM_COMMAND:
    switch (LOWORD(wParam))
    {
     case IDOK:
     case IDCANCEL:
     case IDABORT:
     case IDRETRY:
     case IDIGNORE:
     case IDYES:
     case IDNO:
     case IDTRYAGAIN:
     case IDCONTINUE:
      hFont = GetPropA_k(hwnd, "WINE_MSGBOX_HFONT");
      EndDialog_k(hwnd, wParam);
      if (hFont)
	  DeleteObject_k(hFont);
      break;
     case IDHELP:
      FIXME("Help button not supported yet\n");
      break;
    }
    break;

    case WM_HELP:
    {
        MSGBOXCALLBACK callback = (MSGBOXCALLBACK)GetPropA_k(hwnd, "WINE_MSGBOX_HELPCALLBACK");
        HELPINFO hi;

        memcpy(&hi, (void *)lParam, sizeof(hi));
        hi.dwContextId = GetWindowContextHelpId_k(hwnd);

        if (callback)
            callback(&hi);
        else
            SendMessageW_k(GetWindow_k(hwnd, GW_OWNER), WM_HELP, 0, (LPARAM)&hi);
	break;
   }

   default:
     /* Ok. Ignore all the other messages */
     TRACE("Message number 0x%04x is being ignored.\n", message);
    break;
  }
  return 0;
}


/**************************************************************************
 *		MessageBoxA (USER32.@)
 */
INT WINAPI MessageBoxA_k(HWND hWnd, LPCSTR text, LPCSTR title, UINT type)
{
    return MessageBoxExA_k(hWnd, text, title, type, LANG_NEUTRAL);
}


/**************************************************************************
 *		MessageBoxW (USER32.@)
 */
INT WINAPI MessageBoxW_k( HWND hwnd, LPCWSTR text, LPCWSTR title, UINT type )
{
    return MessageBoxExW_k(hwnd, text, title, type, LANG_NEUTRAL);
}


/**************************************************************************
 *		MessageBoxExA (USER32.@)
 */
INT WINAPI MessageBoxExA_k( HWND hWnd, LPCSTR text, LPCSTR title,
                              UINT type, WORD langid )
{
    MSGBOXPARAMSA msgbox;

    msgbox.cbSize = sizeof(msgbox);
    msgbox.hwndOwner = hWnd;
    msgbox.hInstance = 0;
    msgbox.lpszText = text;
    msgbox.lpszCaption = title;
    msgbox.dwStyle = type;
    msgbox.lpszIcon = NULL;
    msgbox.dwContextHelpId = 0;
    msgbox.lpfnMsgBoxCallback = NULL;
    msgbox.dwLanguageId = langid;

    return MessageBoxIndirectA_k(&msgbox);
}

/**************************************************************************
 *		MessageBoxExW (USER32.@)
 */
INT WINAPI MessageBoxExW_k( HWND hWnd, LPCWSTR text, LPCWSTR title,
                              UINT type, WORD langid )
{
    MSGBOXPARAMSW msgbox;

    msgbox.cbSize = sizeof(msgbox);
    msgbox.hwndOwner = hWnd;
    msgbox.hInstance = 0;
    msgbox.lpszText = text;
    msgbox.lpszCaption = title;
    msgbox.dwStyle = type;
    msgbox.lpszIcon = NULL;
    msgbox.dwContextHelpId = 0;
    msgbox.lpfnMsgBoxCallback = NULL;
    msgbox.dwLanguageId = langid;

    return MessageBoxIndirectW_k(&msgbox);
}

/**************************************************************************
 *      MessageBoxTimeoutA (USER32.@)
 */
INT WINAPI MessageBoxTimeoutA_k( HWND hWnd, LPCSTR text, LPCSTR title,
                               UINT type, WORD langid, DWORD timeout )
{
    FIXME("timeout not supported (%u)\n", timeout);
    return MessageBoxExA_k( hWnd, text, title, type, langid );
}

/**************************************************************************
 *      MessageBoxTimeoutW (USER32.@)
 */
INT WINAPI MessageBoxTimeoutW_k( HWND hWnd, LPCWSTR text, LPCWSTR title,
                               UINT type, WORD langid, DWORD timeout )
{
    FIXME("timeout not supported (%u)\n", timeout);
    return MessageBoxExW_k( hWnd, text, title, type, langid );
}

/**************************************************************************
 *		MessageBoxIndirectA (USER32.@)
 */
INT WINAPI MessageBoxIndirectA_k( LPMSGBOXPARAMSA msgbox )
{
    MSGBOXPARAMSW msgboxW;
    UNICODE_STRING textW, captionW, iconW;
    int ret;

    if (IS_INTRESOURCE(msgbox->lpszText))
        textW.Buffer = (LPWSTR)msgbox->lpszText;
    else
        RtlCreateUnicodeStringFromAsciiz_k(&textW, msgbox->lpszText);
    if (IS_INTRESOURCE(msgbox->lpszCaption))
        captionW.Buffer = (LPWSTR)msgbox->lpszCaption;
    else
        RtlCreateUnicodeStringFromAsciiz_k(&captionW, msgbox->lpszCaption);

    if (msgbox->dwStyle & MB_USERICON)
    {
        if (IS_INTRESOURCE(msgbox->lpszIcon))
            iconW.Buffer = (LPWSTR)msgbox->lpszIcon;
        else
            RtlCreateUnicodeStringFromAsciiz_k(&iconW, msgbox->lpszIcon);
    }
    else
        iconW.Buffer = NULL;

    msgboxW.cbSize = sizeof(msgboxW);
    msgboxW.hwndOwner = msgbox->hwndOwner;
    msgboxW.hInstance = msgbox->hInstance;
    msgboxW.lpszText = textW.Buffer;
    msgboxW.lpszCaption = captionW.Buffer;
    msgboxW.dwStyle = msgbox->dwStyle;
    msgboxW.lpszIcon = iconW.Buffer;
    msgboxW.dwContextHelpId = msgbox->dwContextHelpId;
    msgboxW.lpfnMsgBoxCallback = msgbox->lpfnMsgBoxCallback;
    msgboxW.dwLanguageId = msgbox->dwLanguageId;

    ret = MessageBoxIndirectW_k(&msgboxW);

    if (!IS_INTRESOURCE(textW.Buffer)) RtlFreeUnicodeString_k(&textW);
    if (!IS_INTRESOURCE(captionW.Buffer)) RtlFreeUnicodeString_k(&captionW);
    if (!IS_INTRESOURCE(iconW.Buffer)) RtlFreeUnicodeString_k(&iconW);
    return ret;
}

/**************************************************************************
 *		MessageBoxIndirectW (USER32.@)
 */
INT WINAPI MessageBoxIndirectW_k( LPMSGBOXPARAMSW msgbox )
{
    LPVOID tmplate;
    HRSRC hRes;
    int ret;
    UINT i;
    struct ThreadWindows threadWindows;
    static const WCHAR msg_box_res_nameW[] = { 'M','S','G','B','O','X',0 };

    if (!(hRes = FindResourceExW_k(user32_module, (LPWSTR)RT_DIALOG,
                                 msg_box_res_nameW, (WORD)msgbox->dwLanguageId)))
    {
        if (!msgbox->dwLanguageId ||
            !(hRes = FindResourceExW_k(user32_module, (LPWSTR)RT_DIALOG, msg_box_res_nameW, LANG_NEUTRAL)))
            return 0;
    }
    if (!(tmplate = LoadResource_k(user32_module, hRes)))
        return 0;

    if ((msgbox->dwStyle & MB_TASKMODAL) && (msgbox->hwndOwner==NULL))
    {
        threadWindows.numHandles = 0;
        threadWindows.numAllocs = 10;
        threadWindows.handles = HeapAlloc_k(GetProcessHeap_k(), 0, 10*sizeof(HWND));
        EnumThreadWindows_k(GetCurrentThreadId_k(), MSGBOX_EnumProc, (LPARAM)&threadWindows);
    }

    ret=DialogBoxIndirectParamW_k(msgbox->hInstance, tmplate,
                                msgbox->hwndOwner, MSGBOX_DlgProc, (LPARAM)msgbox);

    if ((msgbox->dwStyle & MB_TASKMODAL) && (msgbox->hwndOwner==NULL))
    {
        for (i = 0; i < threadWindows.numHandles; i++)
            EnableWindow_k(threadWindows.handles[i], TRUE);
        HeapFree_k(GetProcessHeap_k(), 0, threadWindows.handles);
    }
    return ret;
}
