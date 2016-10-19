/*
 * Boxedwine graphics driver initialisation functions
 *
 * Copyright 1996 Alexandre Julliard
 * Copyright 2011012013 Ken Thomases for CodeWeavers, Inc.
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
 * Foundation, Inc.1 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#pragma GCC diagnostic ignored "-Wreturn-type"
#include "config.h"

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "wine/debug.h"
#include "wine/gdi_driver.h"
#include "winreg.h"

#include "winuser.h"
#include "winternl.h"
#include "winnt.h"
#include "shellapi.h"
#include "imm.h"
#include "ddk/imm.h"
#include "wine/library.h"
#include "wine/wgl.h"
#include "wine/wgl_driver.h"
#include "wine/wglext.h"
#include "wine/unicode.h"
#include "wine/server.h"

#include <dlfcn.h>

WINE_DEFAULT_DEBUG_CHANNEL(boxeddrv);

#define USE_GL_FUNC(name) #name,
static const char *opengl_func_names[] = { ALL_WGL_FUNCS };
#undef USE_GL_FUNC

typedef struct
{
	struct gdi_physdev  dev;
} BOXEDDRV_PDEVICE;

#define BOXED_BASE 0

#define BOXED_ACQUIRE_CLIPBOARD						(BOXED_BASE)
#define BOXED_ACTIVATE_KEYBOARD_LAYOUT				(BOXED_BASE+1)
#define BOXED_BEEP									(BOXED_BASE+2)
#define BOXED_CHANGE_DISPLAY_SETTINGS_EX			(BOXED_BASE+3)
#define BOXED_CLIP_CURSOR							(BOXED_BASE+4)
#define BOXED_COUNT_CLIPBOARD_FORMATS				(BOXED_BASE+5)
#define BOXED_CREATE_DESKTOP_WINDOW					(BOXED_BASE+6)
#define BOXED_CREATE_WINDOW							(BOXED_BASE+7)
#define BOXED_DESTROY_CURSOR_ICON					(BOXED_BASE+8)
#define BOXED_DESTROY_WINDOW						(BOXED_BASE+9)
#define BOXED_EMPTY_CLIPBOARD						(BOXED_BASE+10)
#define BOXED_END_CLIPBOARD_UPDATE					(BOXED_BASE+11)
#define BOXED_ENUM_CLIPBOARD_FORMATS				(BOXED_BASE+12)
#define BOXED_ENUM_DISPLAY_MONITORS					(BOXED_BASE+13)
#define BOXED_ENUM_DISPLAY_SETTINGS_EX				(BOXED_BASE+14)
#define BOXED_GET_CLIPBOARD_DATA					(BOXED_BASE+15)
#define BOXED_GET_CURSOR_POS						(BOXED_BASE+16)
#define BOXED_GET_KEYBOARD_LAYOUT					(BOXED_BASE+17)
#define BOXED_GET_KEYBOARD_LAYOUT_NAME				(BOXED_BASE+18)
#define BOXED_GET_KEY_NAME							(BOXED_BASE+19)
#define BOXED_GET_MONITOR_INFO						(BOXED_BASE+20)
#define BOXED_IS_CLIPBOARD_FORMAT_AVAILABLE			(BOXED_BASE+21)
#define BOXED_MAP_VIRTUAL_KEY_EX					(BOXED_BASE+22)
#define BOXED_MSG_WAIT_FOR_MULTIPLE_OBJECTS_EX		(BOXED_BASE+23)
#define BOXED_SET_CAPTURE							(BOXED_BASE+24)
#define BOXED_SET_CLIPBOARD_DATA					(BOXED_BASE+25)
#define BOXED_SET_CURSOR							(BOXED_BASE+26)
#define BOXED_SET_CURSOR_POS						(BOXED_BASE+27)
#define BOXED_SET_FOCUS								(BOXED_BASE+28)
#define BOXED_SET_LAYERED_WINDOW_ATTRIBUTES			(BOXED_BASE+29)
#define BOXED_SET_PARENT							(BOXED_BASE+30)
#define BOXED_SET_WINDOW_RGN						(BOXED_BASE+31)
#define BOXED_SET_WINDOW_STYLE						(BOXED_BASE+32)
#define BOXED_SET_WINDOW_TEXT						(BOXED_BASE+33)
#define BOXED_SHOW_WINDOW							(BOXED_BASE+34)
#define BOXED_SYS_COMMAND							(BOXED_BASE+35)
#define BOXED_SYSTEM_PARAMETERS_INFO				(BOXED_BASE+36)
#define BOXED_TO_UNICODE_EX							(BOXED_BASE+37)
#define BOXED_UPDATE_LAYERED_WINDOW					(BOXED_BASE+38)
#define BOXED_VK_KEY_SCAN_EX						(BOXED_BASE+39)
#define BOXED_WINDOW_MESSAGE						(BOXED_BASE+40)
#define BOXED_WINDOW_POS_CHANGED					(BOXED_BASE+41)
#define BOXED_WINDOW_POS_CHANGING					(BOXED_BASE+42)

#define BOXED_GET_DEVICE_GAMMA_RAMP					(BOXED_BASE+43)
#define BOXED_SET_DEVICE_GAMMA_RAMP					(BOXED_BASE+44)
#define BOXED_GET_DEVICE_CAPS						(BOXED_BASE+45)

#define BOXED_WINE_NOTIFY_ICON						(BOXED_BASE+46)

#define BOXED_IME_CONFIGURE							(BOXED_BASE+47)
#define BOXED_IME_CONVERSION_LIST					(BOXED_BASE+48)
#define BOXED_IME_DESTROY							(BOXED_BASE+49)
#define BOXED_IME_ENUM_REGISTER_WORD				(BOXED_BASE+50)
#define BOXED_IME_ESCAPE							(BOXED_BASE+51)
#define BOXED_IME_GET_IME_MENU_ITEMS				(BOXED_BASE+52)
#define BOXED_IME_GET_REGISTER_WORD_STYLE			(BOXED_BASE+53)
#define BOXED_IME_INQUIRE							(BOXED_BASE+54)
#define BOXED_IME_PROCESS_KEY						(BOXED_BASE+55)
#define BOXED_IME_REGISTER_WORD						(BOXED_BASE+56)
#define BOXED_IME_SELECT							(BOXED_BASE+57)
#define BOXED_IME_SET_ACTIVE_CONTEXT				(BOXED_BASE+58)
#define BOXED_IME_SET_COMPOSITION_STRING			(BOXED_BASE+59)
#define BOXED_IME_TO_ASCII_EX						(BOXED_BASE+60)
#define BOXED_IME_UNREGISTER_WORD					(BOXED_BASE+61)
#define BOXED_NOTIFY_IME							(BOXED_BASE+62)

#define BOXED_GL_COPY_CONTEXT						(BOXED_BASE+63)
#define BOXED_GL_CREATE_CONTEXT						(BOXED_BASE+64)
#define BOXED_GL_DELETE_CONTEXT						(BOXED_BASE+65)
#define BOXED_GL_DESCRIBE_PIXEL_FORMAT				(BOXED_BASE+66)
#define BOXED_GL_GET_PIXEL_FORMAT					(BOXED_BASE+67)
#define BOXED_GL_GET_PROC_ADDRESS					(BOXED_BASE+68)
#define BOXED_GL_MAKE_CURRENT						(BOXED_BASE+69)
#define BOXED_GL_SET_PIXEL_FORMAT					(BOXED_BASE+70)
#define BOXED_GL_SHARE_LISTS						(BOXED_BASE+71)
#define BOXED_GL_SWAP_BUFFERS						(BOXED_BASE+72)

#define BOXED_GET_KEYBOARD_LAYOUT_LIST				(BOXED_BASE+73)
#define BOXED_REGISTER_HOT_KEY						(BOXED_BASE+74)
#define BOXED_UNREGISTER_HOT_KEY					(BOXED_BASE+75)
#define BOXED_SET_SURFACE							(BOXED_BASE+76)
#define BOXED_GET_SURFACE							(BOXED_BASE+77)
#define BOXED_FLUSH_SURFACE							(BOXED_BASE+78)

#define BOXED_CREATE_DC                             (BOXED_BASE+79)

#define CALL_0(index) __asm__("push %0\n\tint $0x98\n\taddl $4, %%esp"::"i"(index):"%eax"); 
#define CALL_1(index, arg1) __asm__("push %1\n\tpush %0\n\tint $0x98\n\taddl $8, %%esp"::"i"(index), "g"(arg1):"%eax"); 
#define CALL_2(index, arg1,arg2) __asm__("push %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $12, %%esp"::"i"(index), "g"(arg1), "g"(arg2):"%eax");
#define CALL_3(index, arg1,arg2,arg3) __asm__("push %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $16, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3):"%eax");
#define CALL_4(index, arg1,arg2,arg3,arg4) __asm__("push %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $20, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4):"%eax");
#define CALL_5(index, arg1,arg2,arg3,arg4,arg5) __asm__("push %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $24, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5):"%eax");
#define CALL_6(index, arg1,arg2,arg3,arg4,arg5,arg6) __asm__("push %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $28, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6):"%eax");
#define CALL_7(index, arg1,arg2,arg3,arg4,arg5,arg6,arg7) __asm__("push %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $32, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7):"%eax");

#define CALL_NORETURN_0(index) __asm__("push %0\n\tint $0x98\n\taddl $4, %%esp"::"i"(index)); 
#define CALL_NORETURN_1(index, arg1) __asm__("push %1\n\tpush %0\n\tint $0x98\n\taddl $8, %%esp"::"i"(index), "g"(arg1)); 
#define CALL_NORETURN_2(index, arg1, arg2) __asm__("push %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $12, %%esp"::"i"(index), "g"(arg1), "g"(arg2)); 
#define CALL_NORETURN_3(index, arg1, arg2, arg3) __asm__("push %3\n\rpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $16, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3)); 
#define CALL_NORETURN_4(index, arg1, arg2, arg3, arg4) __asm__("push %4\n\tpush %3\n\rpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $20, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4)); 
#define CALL_NORETURN_5(index, arg1, arg2, arg3, arg4, arg5) __asm__("push %5\n\tpush %4\n\tpush %3\n\rpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $24, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5)); 
#define CALL_NORETURN_7(index, arg1,arg2,arg3,arg4,arg5,arg6,arg7) __asm__("push %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $32, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7));
#define CALL_NORETURN_8(index, arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8) __asm__("push %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $36, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8));
#define CALL_NORETURN_9(index, arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9) __asm__("push %9\n\tpush %8\n\tpush %7\n\tpush %6\n\tpush %5\n\tpush %4\n\tpush %3\n\tpush %2\n\tpush %1\n\tpush %0\n\tint $0x98\n\taddl $40, %%esp"::"i"(index), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), "g"(arg5), "g"(arg6), "g"(arg7), "g"(arg8), "g"(arg9));

INT boxeddrv_GetDeviceCaps(PHYSDEV dev, INT cap);

static inline BOOL can_activate_window(HWND hwnd)
{
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);

    if (!(style & WS_VISIBLE)) {
        TRACE("not visible\n");
        return FALSE;
    }
    if ((style & (WS_POPUP|WS_CHILD)) == WS_CHILD) {
        TRACE("child or popup\n");
        return FALSE;
    }
    if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) {
        TRACE("not activate\n");
        return FALSE;
    }
    if (hwnd == GetDesktopWindow()) {
        TRACE("desktop\n");
        return FALSE;
    }
    if (style & WS_DISABLED) {
        TRACE("disabled\n");
    }
    TRACE("yes\n");
    return TRUE;
}

int CDECL boxeddrv_AcquireClipboard(HWND hwnd) {
	TRACE("hwnd=%p\n", hwnd);
	CALL_1(BOXED_ACQUIRE_CLIPBOARD, hwnd);
}

HKL CDECL boxeddrv_ActivateKeyboardLayout(HKL hkl, UINT flags) {
	TRACE("hkl=%p flags=0x%08x\n", hkl, flags);
	CALL_2(BOXED_ACTIVATE_KEYBOARD_LAYOUT, hkl, flags);
}

void CDECL boxeddrv_Beep(void) {
	TRACE("\n");
	CALL_NORETURN_0(BOXED_BEEP);
}

LONG CDECL boxeddrv_ChangeDisplaySettingsEx(LPCWSTR devname, LPDEVMODEW devmode, HWND hwnd, DWORD flags, LPVOID lpvoid) {
	TRACE("devname=%s devmode=%p hwnd=%p flags=0x%08x %p\n", debugstr_w(devname), devmode, hwnd, flags, lpvoid);
	CALL_5(BOXED_CHANGE_DISPLAY_SETTINGS_EX, devname, devmode, hwnd, flags, lpvoid);
}

BOOL CDECL boxeddrv_ClipCursor(LPCRECT clip) {
	TRACE("clip=%s\n", wine_dbgstr_rect(clip));
	CALL_1(BOXED_CLIP_CURSOR, clip);
}

INT CDECL boxeddrv_CountClipboardFormats(void) {
	TRACE("\n");
	CALL_0(BOXED_COUNT_CLIPBOARD_FORMATS);
}

void initDesktop(HWND hwnd) {
    unsigned int width, height;

    TRACE("%p\n", hwnd);

    /* retrieve the real size of the desktop */
    SERVER_START_REQ(get_window_rectangles)
    {
        req->handle = wine_server_user_handle(hwnd);
        req->relative = COORDS_CLIENT;
        wine_server_call(req);
        width  = reply->window.right;
        height = reply->window.bottom;
    }
    SERVER_END_REQ;

    if (!width && !height)  /* not initialized yet */
    {
        width = boxeddrv_GetDeviceCaps(NULL, DESKTOPHORZRES); 
		height = boxeddrv_GetDeviceCaps(NULL, DESKTOPVERTRES);

        SERVER_START_REQ(set_window_pos)
        {
            req->handle        = wine_server_user_handle(hwnd);
            req->previous      = 0;
            req->swp_flags     = SWP_NOZORDER;
            req->window.left   = 0;
            req->window.top    = 0;
            req->window.right  = width;
            req->window.bottom = height;
            req->client        = req->window;
            wine_server_call(req);
        }
        SERVER_END_REQ;
    }
}
BOOL CDECL boxeddrv_CreateDesktopWindow(HWND hwnd) {
	CALL_1(BOXED_CREATE_DESKTOP_WINDOW, hwnd);
	initDesktop(hwnd);
	return TRUE;
}

BOOL CDECL boxeddrv_CreateWindow(HWND hwnd) {
	TRACE("hwnd=%p\n", hwnd);
	CALL_1(BOXED_CREATE_WINDOW, hwnd);
}

void CDECL boxeddrv_DestroyCursorIcon(HCURSOR cursor) {
	TRACE("cursor=%p\n", cursor);
	CALL_NORETURN_1(BOXED_DESTROY_CURSOR_ICON, cursor);
}

void CDECL boxeddrv_DestroyWindow(HWND hwnd) {
	TRACE("hwnd=%p\n", hwnd);
	CALL_NORETURN_1(BOXED_DESTROY_WINDOW, hwnd);

    if (hwnd == GetForegroundWindow())
    {
        SendMessageW(hwnd, WM_CANCELMODE, 0, 0);
        if (hwnd == GetForegroundWindow())
            SetForegroundWindow(GetDesktopWindow());
    }
}

void CDECL boxeddrv_EmptyClipboard(BOOL keepunowned) {
	TRACE("keepunowned=%d\n", keepunowned);
	CALL_NORETURN_1(BOXED_EMPTY_CLIPBOARD, keepunowned);
}

void CDECL boxeddrv_EndClipboardUpdate(void) {
	TRACE("\n");
	CALL_NORETURN_0(BOXED_END_CLIPBOARD_UPDATE);
}

UINT CDECL boxeddrv_EnumClipboardFormats(UINT prev_format) {
	TRACE("prev_format=%d\n", prev_format);
	CALL_1(BOXED_ENUM_CLIPBOARD_FORMATS, prev_format);
}

INT boxeddrv_GetDeviceCaps(PHYSDEV dev, INT cap);
BOOL CDECL boxeddrv_EnumDisplayMonitors(HDC hdc, LPRECT rect, MONITORENUMPROC proc, LPARAM lparam) {
	RECT r;
	r.left = 0;
	r.right = boxeddrv_GetDeviceCaps(0, HORZRES);
	r.top = 0;
	r.bottom = boxeddrv_GetDeviceCaps(0, VERTRES);

	TRACE("hdc=%p rect=%s proc=%p lparam=0x%08x\n", hdc, wine_dbgstr_rect(rect), proc, (int)lparam);
	if (hdc) {
		POINT origin;
		RECT limit;
		RECT monrect = r;

		if (!GetDCOrgEx(hdc, &origin)) return FALSE;
		if (GetClipBox(hdc, &limit) == ERROR) return FALSE;

		if (rect && !IntersectRect(&limit, &limit, rect)) return TRUE;

		if (IntersectRect(&monrect, &monrect, &limit)) {
			if (!proc((HMONITOR)1, hdc, &monrect, lparam))
				return FALSE;
		}
	}
	else {
		RECT monrect = r;
		RECT unused;

		if (!rect || IntersectRect(&unused, &monrect, rect)) {
            TRACE("calling proc hdc=%p monrect=%s proc=%p lparam=0x%08x\n", hdc, wine_dbgstr_rect(&monrect), proc, (int)lparam);
			if (!proc((HMONITOR)1, hdc, &monrect, lparam))
				return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CDECL boxeddrv_EnumDisplaySettingsEx(LPCWSTR devname, DWORD mode, LPDEVMODEW devmode, DWORD flags) {
	TRACE("devname=%s mode=%d devmode=%p flags=0x%08x\n", debugstr_w(devname), mode, devmode, flags);
	CALL_4(BOXED_ENUM_DISPLAY_SETTINGS_EX, devname, mode, devmode, flags);
}

HANDLE CDECL boxeddrv_GetClipboardData(UINT desired_format) {
	TRACE("desired_format=%d\n", desired_format);
	CALL_1(BOXED_GET_CLIPBOARD_DATA, desired_format);
}

BOOL CDECL boxeddrv_GetCursorPos(LPPOINT pos) {
	TRACE("pos=%p\n", pos);
	CALL_1(BOXED_GET_CURSOR_POS, pos);
}

HKL CDECL boxeddrv_GetKeyboardLayout(DWORD thread_id) {
	TRACE("thread_id=%d\n", thread_id);
	CALL_1(BOXED_GET_KEYBOARD_LAYOUT, thread_id);
}

BOOL CDECL boxeddrv_GetKeyboardLayoutName(LPWSTR name) {
	TRACE("name=%s\n", debugstr_w(name));
	CALL_1(BOXED_GET_KEYBOARD_LAYOUT_NAME, name);
}

INT CDECL boxeddrv_GetKeyNameText(LONG lparam, LPWSTR buffer, INT size) {
	TRACE("lparam=0x%08x buffer=%p size=%d\n", lparam, buffer, size);
	CALL_3(BOXED_GET_KEY_NAME, lparam, buffer, size);
}

BOOL CDECL boxeddrv_GetMonitorInfo(HMONITOR monitor, LPMONITORINFO info) {
    static const WCHAR adapter_name[] = { '\\','\\','.','\\','D','I','S','P','L','A','Y','1',0 };

	TRACE("monitor=%p info=%p\n", monitor, info);
    SetRect(&info->rcMonitor, 0, 0, boxeddrv_GetDeviceCaps(NULL, DESKTOPHORZRES), boxeddrv_GetDeviceCaps(NULL, DESKTOPVERTRES));
    SetRect(&info->rcWork, 0, 0, boxeddrv_GetDeviceCaps(NULL, DESKTOPHORZRES), boxeddrv_GetDeviceCaps(NULL, DESKTOPVERTRES));
    info->dwFlags = MONITORINFOF_PRIMARY;

    if (info->cbSize >= sizeof(MONITORINFOEXW))
        lstrcpyW(((MONITORINFOEXW*)info)->szDevice, adapter_name);
    return TRUE;
	//CALL_2(BOXED_GET_MONITOR_INFO, monitor, info);
}

BOOL CDECL boxeddrv_IsClipboardFormatAvailable(UINT desired_format) {
	TRACE("desired_format=%d\n", desired_format);
	CALL_1(BOXED_IS_CLIPBOARD_FORMAT_AVAILABLE, desired_format);
}

UINT CDECL boxeddrv_MapVirtualKeyEx(UINT wCode, UINT wMapType, HKL hkl) {
	TRACE("wCode=%d wMapType=%d hkl=%p\n", wCode, wMapType, hkl);
	CALL_3(BOXED_MAP_VIRTUAL_KEY_EX, wCode, wMapType, hkl);
}

DWORD CDECL boxeddrv_MsgWaitForMultipleObjectsEx(DWORD count, const HANDLE *handles, DWORD timeout, DWORD mask, DWORD flags) {
	TRACE("count=%d handles=%p timeout=0x%08x mask=0x%08x flags=0x%08x\n", count, handles, timeout, mask, flags);
    CALL_5(BOXED_MSG_WAIT_FOR_MULTIPLE_OBJECTS_EX, count, handles, timeout, mask, flags);
    if (!count && !timeout) return WAIT_TIMEOUT;
        return WaitForMultipleObjectsEx(count, handles, flags & MWMO_WAITALL, timeout, flags & MWMO_ALERTABLE);	
}

void CDECL boxeddrv_SetCapture(HWND hwnd, UINT flags) {
	TRACE("hwnd=%p flags=0x%08x\n", hwnd, flags);
	CALL_NORETURN_2(BOXED_SET_CAPTURE, hwnd, flags);
}

BOOL CDECL boxeddrv_SetClipboardData(UINT format_id, HANDLE data, BOOL owner) {
	TRACE("format_id=%d data=%p owner=%d\n", format_id, data, owner);
	CALL_3(BOXED_SET_CLIPBOARD_DATA, format_id, data, owner);
}

void CDECL boxeddrv_SetCursor(HCURSOR cursor) {
	TRACE("cursor=%p\n", cursor);
	CALL_NORETURN_1(BOXED_SET_CURSOR, cursor);
}

BOOL CDECL boxeddrv_SetCursorPos(INT x, INT y) {
	TRACE("x=%d y=%d\n", x, y);
	CALL_2(BOXED_SET_CURSOR_POS, x, y);
}

void CDECL boxeddrv_SetFocus(HWND hwnd) {
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);    

	TRACE("hwnd=%p\n", hwnd);	
    if (!(style & WS_MINIMIZE)) {
        BOOL shouldActivate = FALSE;
        HWND parent = GetAncestor(hwnd, GA_ROOT);
        
        CALL_NORETURN_2(BOXED_SET_FOCUS, parent, &shouldActivate);

        TRACE("shouldActivate=%s", shouldActivate?"TRUE":"FALSE");
        if (shouldActivate && can_activate_window(parent))
        {
            // simulate a mouse click on the caption to find out whether the window wants to be activated 
            LRESULT ma = SendMessageW(hwnd, WM_MOUSEACTIVATE, (WPARAM)parent, MAKELONG(HTCAPTION,WM_LBUTTONDOWN));
            if (ma != MA_NOACTIVATEANDEAT && ma != MA_NOACTIVATE)
            {
                TRACE("setting foreground window to %p\n", parent);            
                SetForegroundWindow(parent);            
            }
        }
    }
}

void CDECL boxeddrv_SetLayeredWindowAttributes(HWND hwnd, COLORREF key, BYTE alpha, DWORD flags) {
	TRACE("hwnd=%p key=0x%08x alpha=0x%02x flags=0x%08x\n", hwnd, key, alpha, flags);
	CALL_NORETURN_4(BOXED_SET_LAYERED_WINDOW_ATTRIBUTES, hwnd, key, alpha, flags);
}

void CDECL boxeddrv_SetParent(HWND hwnd, HWND parent, HWND old_parent) {
	TRACE("hwnd=%p parent=%p old_parent=%p\n", hwnd, parent, old_parent);
	CALL_NORETURN_3(BOXED_SET_PARENT, hwnd, parent, old_parent);
}

int CDECL boxeddrv_SetWindowRgn(HWND hwnd, HRGN hrgn, BOOL redraw) {
	TRACE("hwnd=%p hrgn=%p redraw=%d\n", hwnd, hrgn, redraw);
	CALL_3(BOXED_SET_WINDOW_RGN, hwnd, hrgn, redraw);
}

void CDECL boxeddrv_SetWindowStyle(HWND hwnd, INT offset, STYLESTRUCT *style) {
    HWND hwndFocus;

	TRACE("hwnd=%p offset=%d style=%p\n", hwnd, offset, style);
	CALL_NORETURN_3(BOXED_SET_WINDOW_STYLE, hwnd, offset, style);
    hwndFocus = GetFocus();
    if (hwndFocus && (hwnd == hwndFocus || IsChild(hwnd, hwndFocus)))
        boxeddrv_SetFocus(hwnd);
}

void CDECL boxeddrv_SetWindowText(HWND hwnd, LPCWSTR text) {
	TRACE("hwnd=%p text=%s\n", hwnd, debugstr_w(text));
	CALL_NORETURN_2(BOXED_SET_WINDOW_TEXT, hwnd, text);
}

UINT CDECL boxeddrv_ShowWindow(HWND hwnd, INT cmd, RECT *rect, UINT swp) {
    HWND hwndFocus;
    UINT result;

	TRACE("hwnd=%p cmd=%d rect=%s swp=0x%08x\n", hwnd, cmd, wine_dbgstr_rect(rect), swp);
    if (IsRectEmpty(rect)) return swp;
	CALL_NORETURN_5(BOXED_SHOW_WINDOW, hwnd, cmd, rect, swp, &result);    
    hwndFocus = GetFocus();
    if (hwndFocus && (hwnd == hwndFocus || IsChild(hwnd, hwndFocus)))
        boxeddrv_SetFocus(hwnd);
    return result;
}

LRESULT CDECL boxeddrv_SysCommand(HWND hwnd, WPARAM wparam, LPARAM lparam) {
	TRACE("hwnd=%p wparam=0x%08x lparam=0x%08x\n", hwnd, (int)wparam, (int)lparam);
	CALL_3(BOXED_SYS_COMMAND, hwnd, wparam, lparam);
}

BOOL CDECL boxeddrv_SystemParametersInfo(UINT action, UINT int_param, void *ptr_param, UINT flags) {
	TRACE("action=%d int_param=%d ptr_param=%p flags=0x%08x\n", action, int_param, ptr_param, flags);
	CALL_4(BOXED_SYSTEM_PARAMETERS_INFO, action, int_param, ptr_param, flags);
}

INT CDECL boxeddrv_ToUnicodeEx(UINT virtKey, UINT scanCode, const BYTE *lpKeyState, LPWSTR bufW, int bufW_size, UINT flags, HKL hkl) {
	CALL_7(BOXED_TO_UNICODE_EX, virtKey, scanCode, lpKeyState, bufW, bufW_size, flags, hkl);
}

BOOL CDECL boxeddrv_UpdateLayeredWindow(HWND hwnd, const UPDATELAYEREDWINDOWINFO *info, const RECT *window_rect) {
	CALL_3(BOXED_UPDATE_LAYERED_WINDOW, hwnd, info, window_rect);
}

SHORT CDECL boxeddrv_VkKeyScanEx(WCHAR wChar, HKL hkl) {
	CALL_2(BOXED_VK_KEY_SCAN_EX, wChar, hkl);
}

LRESULT CDECL boxeddrv_WindowMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	CALL_4(BOXED_WINDOW_MESSAGE, hwnd, msg, wp, lp);
}

void boxeddrv_SetSurface(HWND hwnd, struct window_surface *surface) {
	CALL_NORETURN_2(BOXED_SET_SURFACE, hwnd, surface);
}

struct window_surface* boxeddrv_GetSurface(HWND hwnd) {
	CALL_1(BOXED_GET_SURFACE, hwnd);
}

void CDECL boxeddrv_WindowPosChanged(HWND hwnd, HWND insert_after, UINT swp_flags, const RECT *window_rect, const RECT *client_rect, const RECT *visible_rect, const RECT *valid_rects, struct window_surface *surface) {
	DWORD new_style = GetWindowLongW(hwnd, GWL_STYLE);
    struct window_surface* oldSurface = boxeddrv_GetSurface(hwnd);

	TRACE("hwnd=%p insert_after=%p swp_flags=0x%08x window_rect=%s client_rect=%s visible_rect=%s valid_rects=%s surface=%p style=0x%08x\n", hwnd, insert_after, swp_flags, wine_dbgstr_rect(window_rect), wine_dbgstr_rect(client_rect), wine_dbgstr_rect(visible_rect), wine_dbgstr_rect(valid_rects), surface, new_style);
    if (surface) {
        TRACE("     using new surface %p (ref=%d)\n", surface, surface->ref);
        window_surface_add_ref(surface);
    }
    if (oldSurface) {
        TRACE("     releasing old surface %p (ref=%d)\n", oldSurface, oldSurface->ref);
        window_surface_release(oldSurface);
    }
	boxeddrv_SetSurface(hwnd, surface);	
	CALL_NORETURN_8(BOXED_WINDOW_POS_CHANGED, hwnd, insert_after, swp_flags, window_rect, client_rect, visible_rect, valid_rects, new_style);
}

void surface_clip_to_visible_rect(struct window_surface *window_surface, const RECT *visible_rect);
struct window_surface *create_surface(HWND window, const RECT *rect, struct window_surface *old_surface, BOOL use_alpha);
void CDECL boxeddrv_WindowPosChanging(HWND hwnd, HWND insert_after, UINT swp_flags, const RECT *window_rect, const RECT *client_rect, RECT *visible_rect, struct window_surface **surface) {
    DWORD style = GetWindowLongW(hwnd, GWL_STYLE); 
    struct window_surface *oldSurface = NULL;
    HWND parent = GetAncestor(hwnd, GA_PARENT);

	TRACE("hwnd=%p (parent=%p) insert_after=%p swp_flags=0x%08x window_rect=%s client_rect=%s visible_rect=%s surface=%p\n", hwnd, parent, insert_after, swp_flags, wine_dbgstr_rect(window_rect), wine_dbgstr_rect(client_rect), wine_dbgstr_rect(visible_rect), surface);     

    if (GetWindowThreadProcessId(hwnd, NULL) != GetCurrentThreadId()) return;

    if (!parent)  /* desktop */
    {
        return;
    }

    /* don't create wnd for HWND_MESSAGE windows */
    if (parent != GetDesktopWindow() && !GetAncestor(parent, GA_PARENT)) return;
    
    if (*surface)  {
        oldSurface = *surface;
        TRACE("     setting old surface %p (ref=%d)\n", *surface, (*surface)->ref);
    }
    *surface = NULL;
    
    *visible_rect = *window_rect;
    if (swp_flags & SWP_HIDEWINDOW) return;    
	CALL_NORETURN_7(BOXED_WINDOW_POS_CHANGING, hwnd, insert_after, swp_flags, window_rect, client_rect, visible_rect, surface);

    if (parent != GetDesktopWindow()) {
        return; // don't create surface
    }
	if (*surface) {
        int surfaceWidth = (*surface)->rect.right - (*surface)->rect.left;
        int surfaceHeight = (*surface)->rect.bottom - (*surface)->rect.top;
        int windowWidth = window_rect->right - window_rect->left;
        int windowHeight = window_rect->bottom - window_rect->top;

        if (oldSurface)  {
            TRACE("     releasing old surface %p (ref=%d)\n", oldSurface, oldSurface->ref);
            window_surface_release(oldSurface);
        }
        TRACE("     checking existing surface %p (ref=%d)\n", *surface, (*surface)->ref);
        if (surfaceWidth==windowWidth && surfaceHeight==windowHeight) {
		    // use existing surface
		    surface_clip_to_visible_rect(*surface, visible_rect);
            window_surface_add_ref(*surface);
            return;
        }
    }
    if (oldSurface)  {
        TRACE("     releasing old surface %p (ref=%d)\n", oldSurface, oldSurface->ref);
        window_surface_release(oldSurface);
    }
	if ((swp_flags & SWP_SHOWWINDOW) || (style & WS_VISIBLE)) {
        RECT rc;
        rc.left = 0;
        rc.right = window_rect->right - window_rect->left;
        rc.top = 0;
        rc.bottom = window_rect->bottom - window_rect->top;
		*surface = create_surface(hwnd, &rc, *surface, FALSE);
        TRACE("     created new surface %p (ref=%d)\n", *surface, (*surface)->ref);
	}
}

void boxeddrv_FlushSurface(HWND hwnd, void* bits, int xOrg, int yOrg, int width, int height, RECT* rect, RECT* rects, int rectCount) {
	TRACE("hwnd=%p bits=%p width=%d height=%d rect=%s rects=%p rectCount=%d\n", hwnd, bits, width, height, wine_dbgstr_rect(rect), rects, rectCount);
	CALL_NORETURN_9(BOXED_FLUSH_SURFACE, hwnd, bits, xOrg, yOrg, width, height, rect, rects, rectCount);
}

BOOL boxeddrv_GetDeviceGammaRamp(PHYSDEV dev, LPVOID ramp) {
	TRACE("dev=%p ramp=%p\n", dev, ramp);
	CALL_2(BOXED_GET_DEVICE_GAMMA_RAMP, dev, ramp);
}

BOOL boxeddrv_SetDeviceGammaRamp(PHYSDEV dev, LPVOID ramp) {
	TRACE("dev=%p ramp=%p\n", dev, ramp);
	CALL_2(BOXED_SET_DEVICE_GAMMA_RAMP, dev, ramp);
}

INT boxeddrv_GetDeviceCaps(PHYSDEV dev, INT cap) {
	TRACE("dev=%p cap=%d\n", dev, cap);
	switch (cap) {
	case PDEVICESIZE:
		return sizeof(BOXEDDRV_PDEVICE);
	}
	CALL_2(BOXED_GET_DEVICE_CAPS, dev, cap);
}

int CDECL wine_notify_icon(DWORD msg, NOTIFYICONDATAW *data) {
	CALL_2(BOXED_WINE_NOTIFY_ICON, msg, data);
}

BOOL WINAPI ImeConfigure(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData) {
	CALL_4(BOXED_IME_CONFIGURE, hKL, hWnd, dwMode, lpData);
}

DWORD WINAPI ImeConversionList(HIMC hIMC, LPCWSTR lpSource, LPCANDIDATELIST lpCandList, DWORD dwBufLen, UINT uFlag) {
	CALL_5(BOXED_IME_CONVERSION_LIST, hIMC, lpSource, lpCandList, dwBufLen, uFlag);
}

BOOL WINAPI ImeDestroy(UINT uForce) {
	CALL_1(BOXED_IME_DESTROY, uForce);
}

UINT WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROCW lpfnEnumProc, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister, LPVOID lpData) {
	CALL_5(BOXED_IME_ENUM_REGISTER_WORD, lpfnEnumProc, lpszReading, dwStyle, lpszRegister, lpData);
}

LRESULT WINAPI ImeEscape(HIMC hIMC, UINT uSubFunc, LPVOID lpData) {
	CALL_3(BOXED_IME_ESCAPE, hIMC, uSubFunc, lpData);
}

DWORD WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType, LPIMEMENUITEMINFOW lpImeParentMenu, LPIMEMENUITEMINFOW lpImeMenu, DWORD dwSize) {
	CALL_6(BOXED_IME_GET_IME_MENU_ITEMS, hIMC, dwFlags, dwType, lpImeParentMenu, lpImeMenu, dwSize);
}

UINT WINAPI ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUFW lpStyleBuf) {
	CALL_2(BOXED_IME_GET_REGISTER_WORD_STYLE, nItem, lpStyleBuf);
}

BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPWSTR lpszUIClass, LPCWSTR lpszOption) {
	CALL_3(BOXED_IME_INQUIRE, lpIMEInfo, lpszUIClass, lpszOption);
}

BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT vKey, LPARAM lKeyData, const LPBYTE lpbKeyState) {
	CALL_4(BOXED_IME_PROCESS_KEY, hIMC, vKey, lKeyData, lpbKeyState);
}

BOOL WINAPI ImeRegisterWord(LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister) {
	CALL_3(BOXED_IME_REGISTER_WORD, lpszReading, dwStyle, lpszRegister);
}

BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect) {
	CALL_2(BOXED_IME_SELECT, hIMC, fSelect);
}

BOOL WINAPI ImeSetActiveContext(HIMC hIMC, BOOL fFlag) {
	CALL_2(BOXED_IME_SET_ACTIVE_CONTEXT, hIMC, fFlag);
}

BOOL WINAPI ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen) {
	CALL_6(BOXED_IME_SET_COMPOSITION_STRING, hIMC, dwIndex, lpComp, dwCompLen, lpRead, dwReadLen);
}

UINT WINAPI ImeToAsciiEx(UINT uVKey, UINT uScanCode, const LPBYTE lpbKeyState, LPDWORD lpdwTransKey, UINT fuState, HIMC hIMC) {
	CALL_6(BOXED_IME_TO_ASCII_EX, uVKey, uScanCode, lpbKeyState, lpdwTransKey, fuState, hIMC);
}

BOOL WINAPI ImeUnregisterWord(LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszUnregister) {
	CALL_3(BOXED_IME_UNREGISTER_WORD, lpszReading, dwStyle, lpszUnregister);
}

BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue) {
	CALL_4(BOXED_NOTIFY_IME, hIMC, dwAction, dwIndex, dwValue);
}

static BOOL boxeddrv_wglCopyContext(struct wgl_context *src, struct wgl_context *dst, UINT mask) {
    printf("boxeddrv_wglCopyContext src=%p dst=%p mask=%X\n", src, dst, mask);
	CALL_3(BOXED_GL_COPY_CONTEXT, src, dst, mask);
}

static struct wgl_context *boxeddrv_wglCreateContext(HDC hdc) {
    printf("boxeddrv_wglCreateContext hdc=%X\n", (int)hdc);
	CALL_5(BOXED_GL_CREATE_CONTEXT, WindowFromDC(hdc), 0, 0, 0, 0);
}

static void boxeddrv_wglDeleteContext(struct wgl_context *context) {
    printf("boxeddrv_wglDeleteContext context=%p\n", context);
	CALL_NORETURN_1(BOXED_GL_DELETE_CONTEXT, context);
}

static int boxeddrv_wglDescribePixelFormat(HDC hdc, int fmt, UINT size, PIXELFORMATDESCRIPTOR *descr) {
    printf("boxeddrv_wglDescribePixelFormat hdc=%X fmt=%d size=%d descr=%p\n", (int)hdc, fmt, size, descr);
	CALL_4(BOXED_GL_DESCRIBE_PIXEL_FORMAT, hdc, fmt, size, descr);
}

static int boxeddrv_wglGetPixelFormat(HDC hdc) {
    printf("boxeddrv_wglGetPixelFormat hdc=%X\n", (int)hdc);
	CALL_1(BOXED_GL_GET_PIXEL_FORMAT, hdc);
}

static struct wgl_context *boxeddrv_wglCreateContextAttribsARB(HDC hdc, struct wgl_context *share_context, const int *attrib_list);
static PROC boxeddrv_wglGetProcAddress(const char *proc) {
    printf("boxeddrv_wglGetProcAddress %s\n", proc);
    //if (!strcmp(proc, "wglCreateContextAttribsARB"))
    //    return (PROC)boxeddrv_wglCreateContextAttribsARB;
	//CALL_1(BOXED_GL_GET_PROC_ADDRESS, proc);
    return NULL;
}

static BOOL boxeddrv_wglMakeCurrent(HDC hdc, struct wgl_context *context) {
    printf("boxeddrv_wglMakeCurrent hdc=%X context=%p\n",(int)hdc, context);
	CALL_2(BOXED_GL_MAKE_CURRENT, WindowFromDC(hdc), context);
}

static BOOL boxeddrv_wglSetPixelFormat(HDC hdc, int fmt, const PIXELFORMATDESCRIPTOR *descr) {
    printf("boxeddrv_wglSetPixelFormat hdc=%X fmt=%d descr=%p\n", (int)hdc, fmt, descr);
	CALL_3(BOXED_GL_SET_PIXEL_FORMAT, WindowFromDC(hdc), fmt, descr);
}

static BOOL boxeddrv_wglShareLists(struct wgl_context *org, struct wgl_context *dest) {
    printf("boxeddrv_wglShareLists org=%p dest=%p\n", org, dest);
	CALL_2(BOXED_GL_SHARE_LISTS, org, dest);
}

static BOOL boxeddrv_wglSwapBuffers(HDC hdc) {
	CALL_1(BOXED_GL_SWAP_BUFFERS, hdc);
}

UINT CDECL boxeddrv_GetKeyboardLayoutList(INT size, HKL *list) {
	CALL_2(BOXED_GET_KEYBOARD_LAYOUT_LIST, size, list);
}

BOOL CDECL boxeddrv_RegisterHotKey(HWND hwnd, UINT mod_flags, UINT vkey) {
	CALL_3(BOXED_REGISTER_HOT_KEY, hwnd, mod_flags, vkey);
}

void CDECL boxeddrv_UnregisterHotKey(HWND hwnd, UINT modifiers, UINT vkey) {
	CALL_NORETURN_3(BOXED_UNREGISTER_HOT_KEY, hwnd, modifiers, vkey);
}

static struct opengl_funcs opengl_funcs =
{
	{
		boxeddrv_wglCopyContext,          /* p_wglCopyContext */
		boxeddrv_wglCreateContext,        /* p_wglCreateContext */
		boxeddrv_wglDeleteContext,        /* p_wglDeleteContext */
		boxeddrv_wglDescribePixelFormat,  /* p_wglDescribePixelFormat */
		boxeddrv_wglGetPixelFormat,       /* p_wglGetPixelFormat */
		boxeddrv_wglGetProcAddress,       /* p_wglGetProcAddress */
		boxeddrv_wglMakeCurrent,          /* p_wglMakeCurrent */
		boxeddrv_wglSetPixelFormat,       /* p_wglSetPixelFormat */
		boxeddrv_wglShareLists,           /* p_wglShareLists */
		boxeddrv_wglSwapBuffers,          /* p_wglSwapBuffers */
	}
};

int initOpengl() {
    static int init_done;
    static void *opengl_handle;

    char buffer[200];
    unsigned int i;

    if (init_done) return (opengl_handle != NULL);
    init_done = 1;

    /* No need to load any other libraries as according to the ABI, libGL should be self-sufficient
       and include all dependencies */
    opengl_handle = wine_dlopen("libGL.so.1", RTLD_NOW|RTLD_GLOBAL, buffer, sizeof(buffer));
    if (opengl_handle == NULL)
    {
        ERR( "Failed to load libGL: %s\n", buffer );
        ERR( "OpenGL support is disabled.\n");
        return FALSE;
    }

    for (i = 0; i < sizeof(opengl_func_names)/sizeof(opengl_func_names[0]); i++)
    {
        if (!(((void **)&opengl_funcs.gl)[i] = wine_dlsym( opengl_handle, opengl_func_names[i], NULL, 0 )))
        {
            ERR( "%s not found in libGL, disabling OpenGL.\n", opengl_func_names[i] );
            goto failed;
        }
    }
    return TRUE;

failed:
    wine_dlclose(opengl_handle, NULL, 0);
    opengl_handle = NULL;
    return FALSE;
}

/**********************************************************************
*              macdrv_wine_get_wgl_driver
*/
struct opengl_funcs *boxeddrv_wine_get_wgl_driver(PHYSDEV dev, UINT version)
{
	if (version != WINE_WGL_DRIVER_VERSION)
	{
		ERR("version mismatch, opengl32 wants %u but boxeddrv has %u\n", version, WINE_WGL_DRIVER_VERSION);
		return NULL;
	}

    if (initOpengl())
	    return &opengl_funcs;
    return NULL;
}

static inline BOXEDDRV_PDEVICE *get_boxeddrv_dev(PHYSDEV dev)
{
	return (BOXEDDRV_PDEVICE*)dev;
}

static BOXEDDRV_PDEVICE *create_boxed_physdev(void)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BOXEDDRV_PDEVICE));
}


/**********************************************************************
*              DeleteDC (BOXEDDRV.@)
*/
static BOOL boxeddrv_DeleteDC(PHYSDEV dev)
{
	BOXEDDRV_PDEVICE *physDev = get_boxeddrv_dev(dev);

	TRACE("hdc %p\n", dev->hdc);

	HeapFree(GetProcessHeap(), 0, physDev);
	return TRUE;
}

static BOOL boxeddrv_CreateDC(PHYSDEV *pdev, LPCWSTR driver, LPCWSTR device, LPCWSTR output, const DEVMODEW* initData);
static BOOL boxeddrv_CreateCompatibleDC(PHYSDEV orig, PHYSDEV *pdev);

static const struct gdi_dc_funcs boxeddrv_funcs =
{
    NULL,                                   /* pAbortDoc */
    NULL,                                   /* pAbortPath */
    NULL,                                   /* pAlphaBlend */
    NULL,                                   /* pAngleArc */
    NULL,                                   /* pArc */
    NULL,                                   /* pArcTo */
    NULL,                                   /* pBeginPath */
    NULL,                                   /* pBlendImage */
    NULL,                                   /* pChord */
    NULL,                                   /* pCloseFigure */
    boxeddrv_CreateCompatibleDC,            /* pCreateCompatibleDC */
    boxeddrv_CreateDC,                      /* pCreateDC */
    boxeddrv_DeleteDC,                      /* pDeleteDC */
    NULL,                                   /* pDeleteObject */
    NULL,                                   /* pDeviceCapabilities */
    NULL,                                   /* pEllipse */
    NULL,                                   /* pEndDoc */
    NULL,                                   /* pEndPage */
    NULL,                                   /* pEndPath */
    NULL,                                   /* pEnumFonts */
    NULL,                                   /* pEnumICMProfiles */
    NULL,                                   /* pExcludeClipRect */
    NULL,                                   /* pExtDeviceMode */
    NULL,                                   /* pExtEscape */
    NULL,                                   /* pExtFloodFill */
    NULL,                                   /* pExtSelectClipRgn */
    NULL,                                   /* pExtTextOut */
    NULL,                                   /* pFillPath */
    NULL,                                   /* pFillRgn */
    NULL,                                   /* pFlattenPath */
    NULL,                                   /* pFontIsLinked */
    NULL,                                   /* pFrameRgn */
    NULL,                                   /* pGdiComment */
#if WINE_GDI_DRIVER_VERSION==46
    NULL,                                   /* pGdiRealizationInfo */
#endif
    NULL,                                   /* pGetBoundsRect */
    NULL,                                   /* pGetCharABCWidths */
    NULL,                                   /* pGetCharABCWidthsI */
    NULL,                                   /* pGetCharWidth */
    boxeddrv_GetDeviceCaps,                 /* pGetDeviceCaps */
    boxeddrv_GetDeviceGammaRamp,            /* pGetDeviceGammaRamp */
    NULL,                                   /* pGetFontData */
#if WINE_GDI_DRIVER_VERSION==47
    NULL,                                   /* pGetFontRealizationInfo */
#endif
    NULL,                                   /* pGetFontUnicodeRanges */
    NULL,                                   /* pGetGlyphIndices */
    NULL,                                   /* pGetGlyphOutline */
    NULL,                                   /* pGetICMProfile */
    NULL,                                   /* pGetImage */
    NULL,                                   /* pGetKerningPairs */
    NULL,                                   /* pGetNearestColor */
    NULL,                                   /* pGetOutlineTextMetrics */
    NULL,                                   /* pGetPixel */
    NULL,                                   /* pGetSystemPaletteEntries */
    NULL,                                   /* pGetTextCharsetInfo */
    NULL,                                   /* pGetTextExtentExPoint */
    NULL,                                   /* pGetTextExtentExPointI */
    NULL,                                   /* pGetTextFace */
    NULL,                                   /* pGetTextMetrics */
    NULL,                                   /* pGradientFill */
    NULL,                                   /* pIntersectClipRect */
    NULL,                                   /* pInvertRgn */
    NULL,                                   /* pLineTo */
    NULL,                                   /* pModifyWorldTransform */
    NULL,                                   /* pMoveTo */
    NULL,                                   /* pOffsetClipRgn */
    NULL,                                   /* pOffsetViewportOrg */
    NULL,                                   /* pOffsetWindowOrg */
    NULL,                                   /* pPaintRgn */
    NULL,                                   /* pPatBlt */
    NULL,                                   /* pPie */
    NULL,                                   /* pPolyBezier */
    NULL,                                   /* pPolyBezierTo */
    NULL,                                   /* pPolyDraw */
    NULL,                                   /* pPolyPolygon */
    NULL,                                   /* pPolyPolyline */
    NULL,                                   /* pPolygon */
    NULL,                                   /* pPolyline */
    NULL,                                   /* pPolylineTo */
    NULL,                                   /* pPutImage */
    NULL,                                   /* pRealizeDefaultPalette */
    NULL,                                   /* pRealizePalette */
    NULL,                                   /* pRectangle */
    NULL,                                   /* pResetDC */
    NULL,                                   /* pRestoreDC */
    NULL,                                   /* pRoundRect */
    NULL,                                   /* pSaveDC */
    NULL,                                   /* pScaleViewportExt */
    NULL,                                   /* pScaleWindowExt */
    NULL,                                   /* pSelectBitmap */
    NULL,                                   /* pSelectBrush */
    NULL,                                   /* pSelectClipPath */
    NULL,                                   /* pSelectFont */
    NULL,                                   /* pSelectPalette */
    NULL,                                   /* pSelectPen */
    NULL,                                   /* pSetArcDirection */
    NULL,                                   /* pSetBkColor */
    NULL,                                   /* pSetBkMode */
    NULL,                                   /* pSetBoundsRect */
    NULL,                                   /* pSetDCBrushColor */
    NULL,                                   /* pSetDCPenColor */
    NULL,                                   /* pSetDIBitsToDevice */
    NULL,                                   /* pSetDeviceClipping */
    boxeddrv_SetDeviceGammaRamp,            /* pSetDeviceGammaRamp */
    NULL,                                   /* pSetLayout */
    NULL,                                   /* pSetMapMode */
    NULL,                                   /* pSetMapperFlags */
    NULL,                                   /* pSetPixel */
    NULL,                                   /* pSetPolyFillMode */
    NULL,                                   /* pSetROP2 */
    NULL,                                   /* pSetRelAbs */
    NULL,                                   /* pSetStretchBltMode */
    NULL,                                   /* pSetTextAlign */
    NULL,                                   /* pSetTextCharacterExtra */
    NULL,                                   /* pSetTextColor */
    NULL,                                   /* pSetTextJustification */
    NULL,                                   /* pSetViewportExt */
    NULL,                                   /* pSetViewportOrg */
    NULL,                                   /* pSetWindowExt */
    NULL,                                   /* pSetWindowOrg */
    NULL,                                   /* pSetWorldTransform */
    NULL,                                   /* pStartDoc */
    NULL,                                   /* pStartPage */
    NULL,                                   /* pStretchBlt */
    NULL,                                   /* pStretchDIBits */
    NULL,                                   /* pStrokeAndFillPath */
    NULL,                                   /* pStrokePath */
    NULL,                                   /* pUnrealizePalette */
    NULL,                                   /* pWidenPath */
    boxeddrv_wine_get_wgl_driver,           /* wine_get_wgl_driver */
    GDI_PRIORITY_GRAPHICS_DRV               /* priority */
};

/**********************************************************************
*              CreateDC (BOXEDDRV.@)
*/
static BOOL boxeddrv_CreateDC(PHYSDEV *pdev, LPCWSTR driver, LPCWSTR device,
	LPCWSTR output, const DEVMODEW* initData)
{
	BOXEDDRV_PDEVICE *physDev = create_boxed_physdev();

	TRACE("pdev %p hdc %p driver %s device %s output %s initData %p\n", pdev,
		(*pdev)->hdc, debugstr_w(driver), debugstr_w(device), debugstr_w(output),
		initData);

	if (!physDev) return FALSE;

	push_dc_driver(pdev, &physDev->dev, &boxeddrv_funcs);
	CALL_NORETURN_1(BOXED_CREATE_DC, physDev);
	TRACE("priority=%d\n", boxeddrv_funcs.priority);
	return TRUE;
}


/**********************************************************************
*              CreateCompatibleDC (BOXEDDRV.@)
*/
static BOOL boxeddrv_CreateCompatibleDC(PHYSDEV orig, PHYSDEV *pdev)
{
	BOXEDDRV_PDEVICE *physDev = create_boxed_physdev();

	TRACE("orig %p orig->hdc %p pdev %p pdev->hdc %p\n", orig, (orig ? orig->hdc : NULL), pdev,
		((pdev && *pdev) ? (*pdev)->hdc : NULL));

	if (!physDev) return FALSE;

	push_dc_driver(pdev, &physDev->dev, &boxeddrv_funcs);
    CALL_NORETURN_1(BOXED_CREATE_DC, physDev);
	return TRUE;
}

/******************************************************************************
 *              boxeddrv_get_gdi_driver
 */
const struct gdi_dc_funcs * CDECL boxeddrv_get_gdi_driver(unsigned int version)
{
    if (version != WINE_GDI_DRIVER_VERSION)
    {
        ERR("version mismatch, gdi32 wants %u but wineboxed has %u\n", version, WINE_GDI_DRIVER_VERSION);
        return NULL;
    }
    return &boxeddrv_funcs;
}

static struct wgl_context *boxeddrv_wglCreateContextAttribsARB(HDC hdc, struct wgl_context *share_context, const int *attrib_list)
{
    const int *iptr;
    int major = 1, minor = 0, profile = WGL_CONTEXT_CORE_PROFILE_BIT_ARB, flags = 0;

    printf("boxeddrv_wglCreateContextAttribsARB hdc=%p share_context=%p attrib_list=%p\n", hdc, share_context, attrib_list);

    for (iptr = attrib_list; iptr && *iptr; iptr += 2)
    {
        int attr = iptr[0];
        int value = iptr[1];

        TRACE("attribute %d.%d\n", attr, value);

        switch (attr)
        {
            case WGL_CONTEXT_MAJOR_VERSION_ARB:
                major = value;
                break;

            case WGL_CONTEXT_MINOR_VERSION_ARB:
                minor = value;
                break;

            case WGL_CONTEXT_LAYER_PLANE_ARB:
                WARN("WGL_CONTEXT_LAYER_PLANE_ARB attribute ignored\n");
                break;

            case WGL_CONTEXT_FLAGS_ARB:
                flags = value;
                if (flags & ~WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB)
                    WARN("WGL_CONTEXT_FLAGS_ARB attributes %#x ignored\n", flags & ~WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB);
                break;

            case WGL_CONTEXT_PROFILE_MASK_ARB:
                if (value != WGL_CONTEXT_CORE_PROFILE_BIT_ARB &&
                    value != WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB)
                {
                    WARN("WGL_CONTEXT_PROFILE_MASK_ARB bits %#x invalid\n", value);
                    SetLastError(ERROR_INVALID_PROFILE_ARB);
                    return NULL;
                }
                profile = value;
                break;            
            default:
                WARN("Unknown attribute %d.%d\n", attr, value);
                SetLastError(ERROR_INVALID_PARAMETER);
                return NULL;
        }
    }

    CALL_5(BOXED_GL_CREATE_CONTEXT, WindowFromDC(hdc), major, minor, profile, flags);
}
