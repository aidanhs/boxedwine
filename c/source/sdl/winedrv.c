#include "cpu.h"
#include "log.h"

int horz_res = 1024;
int vert_res = 768;
int default_horz_res = 1024;
int default_vert_res = 768;
int bits_per_pixel = 32;
int default_bits_per_pixel = 32;

void notImplemented(const char* s) {
	kwarn(s);
}

#define ARG1 peek32(cpu, 2)
#define ARG2 peek32(cpu, 3)
#define ARG3 peek32(cpu, 4)
#define ARG4 peek32(cpu, 5)
#define ARG5 peek32(cpu, 6)
#define ARG6 peek32(cpu, 7)
#define ARG7 peek32(cpu, 8)
#define ARG8 peek32(cpu, 9)


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

void boxeddrv_AcquireClipboard(struct CPU* cpu) {
	
}

// HKL CDECL drv_ActivateKeyboardLayout(HKL hkl, UINT flags)
void boxeddrv_ActivateKeyboardLayout(struct CPU* cpu) {
	notImplemented("boxeddrv_ActivateKeyboardLayout not implemented");
	EAX = ARG1;
}

// void CDECL drv_Beep(void)
void boxeddrv_Beep(struct CPU* cpu) {
	notImplemented("boxeddrv_Beep not implemented");
}

#define DISP_CHANGE_SUCCESSFUL 0
#define DISP_CHANGE_RESTART    1
#define DISP_CHANGE_FAILED     (-1)
#define DISP_CHANGE_BADMODE    (-2)
#define DISP_CHANGE_NOTUPDATED (-3)
#define DISP_CHANGE_BADFLAGS   (-4)
#define DISP_CHANGE_BADPARAM   (-5)
#define DISP_CHANGE_BADDUALVIEW (-6)

# define __MSABI_LONG(x)         x

#define DM_ORIENTATION          __MSABI_LONG(0x00000001)
#define DM_PAPERSIZE            __MSABI_LONG(0x00000002)
#define DM_PAPERLENGTH          __MSABI_LONG(0x00000004)
#define DM_PAPERWIDTH           __MSABI_LONG(0x00000008)
#define DM_SCALE                __MSABI_LONG(0x00000010)
#define DM_POSITION             __MSABI_LONG(0x00000020)
#define DM_NUP                  __MSABI_LONG(0x00000040)
#define DM_DISPLAYORIENTATION   __MSABI_LONG(0x00000080)
#define DM_COPIES               __MSABI_LONG(0x00000100)
#define DM_DEFAULTSOURCE        __MSABI_LONG(0x00000200)
#define DM_PRINTQUALITY         __MSABI_LONG(0x00000400)
#define DM_COLOR                __MSABI_LONG(0x00000800)
#define DM_DUPLEX               __MSABI_LONG(0x00001000)
#define DM_YRESOLUTION          __MSABI_LONG(0x00002000)
#define DM_TTOPTION             __MSABI_LONG(0x00004000)
#define DM_COLLATE              __MSABI_LONG(0x00008000)
#define DM_FORMNAME             __MSABI_LONG(0x00010000)
#define DM_LOGPIXELS            __MSABI_LONG(0x00020000)
#define DM_BITSPERPEL           __MSABI_LONG(0x00040000)
#define DM_PELSWIDTH            __MSABI_LONG(0x00080000)
#define DM_PELSHEIGHT           __MSABI_LONG(0x00100000)
#define DM_DISPLAYFLAGS         __MSABI_LONG(0x00200000)
#define DM_DISPLAYFREQUENCY     __MSABI_LONG(0x00400000)
#define DM_ICMMETHOD            __MSABI_LONG(0x00800000)
#define DM_ICMINTENT            __MSABI_LONG(0x01000000)
#define DM_MEDIATYPE            __MSABI_LONG(0x02000000)
#define DM_DITHERTYPE           __MSABI_LONG(0x04000000)
#define DM_PANNINGWIDTH         __MSABI_LONG(0x08000000)
#define DM_PANNINGHEIGHT        __MSABI_LONG(0x10000000)
#define DM_DISPLAYFIXEDOUTPUT   __MSABI_LONG(0x20000000)

// LONG CDECL drv_ChangeDisplaySettingsEx(LPCWSTR devname, LPDEVMODEW devmode, HWND hwnd, DWORD flags, LPVOID lpvoid)
void boxeddrv_ChangeDisplaySettingsEx(struct CPU* cpu) {
	U32 devmode = ARG2;
	U32 bpp = 0;
	U32 width = 0;
	U32 height = 0;
	
	if (devmode)
	{
		U32 dmFields;
		U32 dmSize = read(MMU_PARAM_CPU devmode + 36);

		/* this is the minimal dmSize that XP accepts */
		if (read(MMU_PARAM_CPU devmode + 36) < 44)
			return DISP_CHANGE_FAILED;

		dmFields = readd(MMU_PARAM_CPU devmode + 40);

		if (dmFields & DM_BITSPERPEL) {
			bpp = readd(MMU_PARAM_CPU devmode + 104);
		}
		if (dmFields & DM_PELSWIDTH) {
			width = readd(MMU_PARAM_CPU devmode + 108);
		}
		if (dmFields & DM_PELSHEIGHT) {
			height = readd(MMU_PARAM_CPU devmode + 112);
		}
	}	
	if (!width || !height) {
		width = default_horz_res;
		height = default_vert_res;
	}
	if (!bpp) {
		bpp = default_bits_per_pixel;
	}
	// :TODO: 
	return DISP_CHANGE_SUCCESSFUL;
}

// BOOL CDECL drv_ClipCursor(LPCRECT clip)
void boxeddrv_ClipCursor(struct CPU* cpu) {
	notImplemented("boxeddrv_ClipCursor not implemented");
	EAX = 1;
}

// INT CDECL drv_CountClipboardFormats(void)
void boxeddrv_CountClipboardFormats(struct CPU* cpu) {
	notImplemented("boxeddrv_CountClipboardFormats not implemented");
	EAX = 0;
}

// BOOL CDECL drv_CreateDesktopWindow(HWND hwnd)
void boxeddrv_CreateDesktopWindow(struct CPU* cpu) {
	// setting up window pos was handled in driver
}

// BOOL CDECL drv_CreateWindow(HWND hwnd)
void boxeddrv_CreateWindow(struct CPU* cpu) {
	EAX = 1;
}

// void CDECL drv_DestroyCursorIcon(HCURSOR cursor)
void boxeddrv_DestroyCursorIcon(struct CPU* cpu) {
	
}

// void CDECL drv_DestroyWindow(HWND hwnd)
void boxeddrv_DestroyWindow(struct CPU* cpu) {
	notImplemented("boxeddrv_DestroyWindow not implemented");
}

// void CDECL drv_EmptyClipboard(void)
void boxeddrv_EmptyClipboard(struct CPU* cpu) {
	notImplemented("boxeddrv_EmptyClipboard not implemented");
}

//void CDECL drv_EndClipboardUpdate(void)
void boxeddrv_EndClipboardUpdate(struct CPU* cpu) {
	notImplemented("boxeddrv_EndClipboardUpdate not implemented");
}

// UINT CDECL drv_EnumClipboardFormats(UINT prev_format)
void boxeddrv_EnumClipboardFormats(struct CPU* cpu) {
	notImplemented("boxeddrv_EnumClipboardFormats not implemented");
	EAX = 0;
}

// BOOL CDECL drv_EnumDisplayMonitors(HDC hdc, LPRECT rect, MONITORENUMPROC proc, LPARAM lparam)
void boxeddrv_EnumDisplayMonitors(struct CPU* cpu) {
	notImplemented("drv_EnumDisplayMonitors not implemented");
	EAX = 0;
}

#define DMDFO_DEFAULT           0
#define DMDFO_STRETCH           1
#define DMDFO_CENTER            2

#define ENUM_CURRENT_SETTINGS  ((U32) -1)
#define ENUM_REGISTRY_SETTINGS ((U32) -2)

// BOOL CDECL macdrv_EnumDisplaySettingsEx(LPCWSTR devname, DWORD mode, LPDEVMODEW devmode, DWORD flags)
void boxeddrv_EnumDisplaySettingsEx(struct CPU* cpu) {
	U32 devmode = ARG3;

	writed(MMU_PARAM_CPU devmode + 40, DM_POSITION | DM_DISPLAYORIENTATION | DM_DISPLAYFIXEDOUTPUT | DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY);

	writed(MMU_PARAM_CPU devmode + 44, 0); // dmPosition.x
	writed(MMU_PARAM_CPU devmode + 48, 0); // dmPosition.y
	writed(MMU_PARAM_CPU devmode + 52, 0); // dmDisplayOrientation
	writed(MMU_PARAM_CPU devmode + 56, DMDFO_CENTER); // dmDisplayFixedOutput

	if (ARG2 == ENUM_REGISTRY_SETTINGS) {
		writed(MMU_PARAM_CPU devmode + 104, default_bits_per_pixel);
		writed(MMU_PARAM_CPU devmode + 108, default_horz_res);
		writed(MMU_PARAM_CPU devmode + 112, default_vert_res);
	}
	else {
		writed(MMU_PARAM_CPU devmode + 104, bits_per_pixel);
		writed(MMU_PARAM_CPU devmode + 108, horz_res);
		writed(MMU_PARAM_CPU devmode + 112, vert_res);
	}
	writed(MMU_PARAM_CPU devmode + 116, 0); // dmDisplayFlags
	writed(MMU_PARAM_CPU devmode + 120, 60); // dmDisplayFrequency
	EAX = 1;
}

// HANDLE CDECL drv_GetClipboardData(UINT desired_format)
void boxeddrv_GetClipboardData(struct CPU* cpu) {
	notImplemented("boxeddrv_GetClipboardData not implemented");
	EAX = 0;
}

// BOOL CDECL drv_GetCursorPos(LPPOINT pos)
void boxeddrv_GetCursorPos(struct CPU* cpu) {
	U32 pos = ARG1;
	writed(MMU_PARAM_CPU pos, 0);
	writed(MMU_PARAM_CPU pos+4, 0);
	EAX = 1;
	notImplemented("boxeddrv_GetCursorPos not implemented");
}

// HKL CDECL drv_GetKeyboardLayout(DWORD thread_id)
void boxeddrv_GetKeyboardLayout(struct CPU* cpu) {
	notImplemented("boxeddrv_GetKeyboardLayout not implemented");
	EAX = 0x1409; 
}

// BOOL CDECL drv_GetKeyboardLayoutName(LPWSTR name)
void boxeddrv_GetKeyboardLayoutName(struct CPU* cpu) {
	writeNativeString(MMU_PARAM_CPU ARG1, "Unknown");
	EAX = 1;
	notImplemented("boxeddrv_GetKeyboardLayoutName not implemented");
}

// INT CDECL drv_GetKeyNameText(LONG lparam, LPWSTR buffer, INT size)
void boxeddrv_GetKeyNameText(struct CPU* cpu) {
	notImplemented("boxeddrv_GetKeyNameText not implemented");
	EAX = 6;
	writeNativeString(MMU_PARAM_CPU ARG2, "bogus");
}

// BOOL CDECL drv_GetMonitorInfo(HMONITOR monitor, LPMONITORINFO info)
void boxeddrv_GetMonitorInfo(struct CPU* cpu) {
	notImplemented("boxeddrv_GetMonitorInfo not implemented");
	EAX = 0;
}

// BOOL CDECL drv_IsClipboardFormatAvailable(UINT desired_format)
void boxeddrv_IsClipboardFormatAvailable(struct CPU* cpu) {
	notImplemented("boxeddrv_IsClipboardFormatAvailable not implemented");
	EAX = 0;
}

// UINT CDECL drv_MapVirtualKeyEx(UINT wCode, UINT wMapType, HKL hkl)
void boxeddrv_MapVirtualKeyEx(struct CPU* cpu) {
	notImplemented("boxeddrv_MapVirtualKeyEx not implemented");
	EAX = 0;
}

// DWORD CDECL drv_MsgWaitForMultipleObjectsEx(DWORD count, const HANDLE *handles, DWORD timeout, DWORD mask, DWORD flags)
void boxeddrv_MsgWaitForMultipleObjectsEx(struct CPU* cpu) {
	notImplemented("boxeddrv_MsgWaitForMultipleObjectsEx not implemented");
	EAX = 0;
}

// void CDECL drv_SetCapture(HWND hwnd, UINT flags)
void boxeddrv_SetCapture(struct CPU* cpu) {
	notImplemented("boxeddrv_SetCapture not implemented");
}

// BOOL CDECL drv_SetClipboardData(UINT format_id, HANDLE data, BOOL owner)
void boxeddrv_SetClipboardData(struct CPU* cpu) {
	notImplemented("boxeddrv_SetClipboardData not implemented");
	EAX = 0;
}

// void CDECL drv_SetCursor(HCURSOR cursor)
void boxeddrv_SetCursor(struct CPU* cpu) {
	notImplemented("boxeddrv_SetCursor not implemented");
}

// BOOL CDECL drv_SetCursorPos(INT x, INT y)
void boxeddrv_SetCursorPos(struct CPU* cpu) {
	notImplemented("boxeddrv_SetCursorPos not implemented");
	EAX = 0;
}

// void CDECL drv_SetFocus(HWND hwnd)
void boxeddrv_SetFocus(struct CPU* cpu) {
	notImplemented("boxeddrv_SetFocus not implemented");
}

// void CDECL drv_SetLayeredWindowAttributes(HWND hwnd, COLORREF key, BYTE alpha, DWORD flags)
void boxeddrv_SetLayeredWindowAttributes(struct CPU* cpu) {
	notImplemented("boxeddrv_SetLayeredWindowAttributes not implemented");
}

// void CDECL drv_SetParent(HWND hwnd, HWND parent, HWND old_parent)
void boxeddrv_SetParent(struct CPU* cpu) {
	notImplemented("boxeddrv_SetParent not implemented");
}

// void CDECL drv_SetWindowRgn(HWND hwnd, HRGN hrgn, BOOL redraw)
void boxeddrv_SetWindowRgn(struct CPU* cpu) {
	notImplemented("boxeddrv_SetWindowRgn not implemented");	
}

// void CDECL drv_SetWindowStyle(HWND hwnd, INT offset, STYLESTRUCT *style)
void boxeddrv_SetWindowStyle(struct CPU* cpu) {
	notImplemented("boxeddrv_SetWindowStyle not implemented");
}

// void CDECL drv_SetWindowText(HWND hwnd, LPCWSTR text)
void boxeddrv_SetWindowText(struct CPU* cpu) {
	notImplemented("drv_SetWindowText not implemented");
}

// UINT CDECL drv_ShowWindow(HWND hwnd, INT cmd, RECT *rect, UINT swp)
void boxeddrv_ShowWindow(struct CPU* cpu) {
	notImplemented("boxeddrv_ShowWindow not implemented");
	EAX = 0;
}

// LRESULT CDECL macdrv_SysCommand(HWND hwnd, WPARAM wparam, LPARAM lparam)
void boxeddrv_SysCommand(struct CPU* cpu) {
	notImplemented("boxeddrv_SysCommand not implemented");
	EAX = 0;
}

#define SPI_GETSCREENSAVEACTIVE   16
#define SPI_SETSCREENSAVEACTIVE   17

// BOOL CDECL SystemParametersInfo(UINT action, UINT int_param, void *ptr_param, UINT flags)
void boxeddrv_SystemParametersInfo(struct CPU* cpu) {
	switch (ARG1)
	{
	case SPI_GETSCREENSAVEACTIVE:
		*(U32*)ARG3 = FALSE;
		EAX = 1;
		return;
	case SPI_SETSCREENSAVEACTIVE:
		break;
	}
	EAX = 0;
}

// INT CDECL drv_ToUnicodeEx(UINT virtKey, UINT scanCode, const BYTE *lpKeyState, LPWSTR bufW, int bufW_size, UINT flags, HKL hkl)
void boxeddrv_ToUnicodeEx(struct CPU* cpu) {
	notImplemented("boxeddrv_ToUnicodeEx not implemented");
	EAX = 0;
}

// BOOL CDECL drv_UpdateLayeredWindow(HWND hwnd, const UPDATELAYEREDWINDOWINFO *info, const RECT *window_rect)
void boxeddrv_UpdateLayeredWindow(struct CPU* cpu) {
	notImplemented("boxeddrv_UpdateLayeredWindow not implemented");
	EAX = 0;
}

// SHORT CDECL drv_VkKeyScanEx(WCHAR wChar, HKL hkl)
void boxeddrv_VkKeyScanEx(struct CPU* cpu) {
	notImplemented("boxeddrv_VkKeyScanEx not implemented");
	EAX = 0;
}

// LRESULT CDECL drv_WindowMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
void boxeddrv_WindowMessage(struct CPU* cpu) {
	notImplemented("boxeddrv_WindowMessage not implemented");
	EAX = 0;
}

// void CDECL drv_WindowPosChanged(HWND hwnd, HWND insert_after, UINT swp_flags, const RECT *window_rect, const RECT *client_rect, const RECT *visible_rect, const RECT *valid_rects, struct window_surface *surface)
void boxeddrv_WindowPosChanged(struct CPU* cpu) {
	notImplemented("boxeddrv_WindowPosChanged not implemented");
}

// void CDECL drv_WindowPosChanging(HWND hwnd, HWND insert_after, UINT swp_flags, const RECT *window_rect, const RECT *client_rect, RECT *visible_rect, struct window_surface **surface)
void boxeddrv_WindowPosChanging(struct CPU* cpu) {
	notImplemented("boxeddrv_WindowPosChanging not implemented");
}

// BOOL drv_GetDeviceGammaRamp(PHYSDEV dev, LPVOID ramp)
void boxeddrv_GetDeviceGammaRamp(struct CPU* cpu) {
	notImplemented("boxeddrv_GetDeviceGammaRamp not implemented");
	EAX = 0;
}

// BOOL drv_SetDeviceGammaRamp(PHYSDEV dev, LPVOID ramp)
void boxeddrv_SetDeviceGammaRamp(struct CPU* cpu) {
	notImplemented("boxeddrv_SetDeviceGammaRamp not implemented");
	EAX = 0;
}

/* CURVECAPS */
#define CC_NONE           0x0000
#define CC_CIRCLES        0x0001
#define CC_PIE            0x0002
#define CC_CHORD         0x0004
#define CC_ELLIPSES       0x0008
#define CC_WIDE           0x0010
#define CC_STYLED         0x0020
#define CC_WIDESTYLED     0x0040
#define CC_INTERIORS      0x0080
#define CC_ROUNDRECT      0x0100

/* LINECAPS */
#define LC_NONE           0x0000
#define LC_POLYLINE       0x0002
#define LC_MARKER         0x0004
#define LC_POLYMARKER     0x0008
#define LC_WIDE           0x0010
#define LC_STYLED         0x0020
#define LC_WIDESTYLED     0x0040
#define LC_INTERIORS      0x0080

/* POLYGONALCAPS */
#define PC_NONE           0x0000
#define PC_POLYGON        0x0001
#define PC_RECTANGLE      0x0002
#define PC_WINDPOLYGON    0x0004
#define PC_TRAPEZOID      0x0004
#define PC_SCANLINE       0x0008
#define PC_WIDE           0x0010
#define PC_STYLED         0x0020
#define PC_WIDESTYLED     0x0040
#define PC_INTERIORS      0x0080
#define PC_POLYPOLYGON    0x0100
#define PC_PATHS          0x0200

/* TEXTCAPS */
#define TC_OP_CHARACTER   0x0001
#define TC_OP_STROKE      0x0002
#define TC_CP_STROKE      0x0004
#define TC_CR_90          0x0008
#define TC_CR_ANY         0x0010
#define TC_SF_X_YINDEP    0x0020
#define TC_SA_DOUBLE      0x0040
#define TC_SA_INTEGER     0x0080
#define TC_SA_CONTIN      0x0100
#define TC_EA_DOUBLE      0x0200
#define TC_IA_ABLE        0x0400
#define TC_UA_ABLE        0x0800
#define TC_SO_ABLE        0x1000
#define TC_RA_ABLE        0x2000
#define TC_VA_ABLE        0x4000
#define TC_RESERVED       0x8000
#define TC_SCROLLBLT      0x00010000

/* CLIPCAPS */
#define CP_NONE           0x0000
#define CP_RECTANGLE      0x0001
#define CP_REGION         0x0002

/* RASTERCAPS */
#define RC_NONE           0x0000
#define RC_BITBLT         0x0001
#define RC_BANDING        0x0002
#define RC_SCALING        0x0004
#define RC_BITMAP64       0x0008
#define RC_GDI20_OUTPUT   0x0010
#define RC_GDI20_STATE    0x0020
#define RC_SAVEBITMAP     0x0040
#define RC_DI_BITMAP      0x0080
#define RC_PALETTE        0x0100
#define RC_DIBTODEV       0x0200
#define RC_BIGFONT        0x0400
#define RC_STRETCHBLT     0x0800
#define RC_FLOODFILL      0x1000
#define RC_STRETCHDIB     0x2000
#define RC_OP_DX_OUTPUT   0x4000
#define RC_DEVBITS        0x8000

/* SHADEBLENDCAPS */
#define SB_NONE           0x0000
#define SB_CONST_ALPHA    0x0001
#define SB_PIXEL_ALPHA    0x0002
#define SB_PREMULT_ALPHA  0x0004
#define SB_GRAD_RECT      0x0010
#define SB_GRAD_TRI       0x0020

/* Device parameters for GetDeviceCaps() */
#define DRIVERVERSION     0
#define TECHNOLOGY        2
#define HORZSIZE          4
#define VERTSIZE          6
#define HORZRES           8
#define VERTRES           10
#define BITSPIXEL         12
#define PLANES            14
#define NUMBRUSHES        16
#define NUMPENS           18
#define NUMMARKERS        20
#define NUMFONTS          22
#define NUMCOLORS         24
#define PDEVICESIZE       26
#define CURVECAPS         28
#define LINECAPS          30
#define POLYGONALCAPS     32
#define TEXTCAPS          34
#define CLIPCAPS          36
#define RASTERCAPS        38
#define ASPECTX           40
#define ASPECTY           42
#define ASPECTXY          44
#define LOGPIXELSX        88
#define LOGPIXELSY        90
#define CAPS1             94
#define SIZEPALETTE       104
#define NUMRESERVED       106
#define COLORRES          108

#define PHYSICALWIDTH     110
#define PHYSICALHEIGHT    111
#define PHYSICALOFFSETX   112
#define PHYSICALOFFSETY   113
#define SCALINGFACTORX    114
#define SCALINGFACTORY    115
#define VREFRESH          116
#define DESKTOPVERTRES    117
#define DESKTOPHORZRES    118
#define BLTALIGNMENT      119
#define SHADEBLENDCAPS    120
#define COLORMGMTCAPS     121

// Args: PHYSDEV dev, INT cap
// return: INT
void boxeddrv_GetDeviceCaps(struct CPU* cpu) {
	S32 ret;

	switch (ARG2) {
	case DRIVERVERSION:
		ret = 0x300;
		break;
	case TECHNOLOGY:
		ret = 1; // DT_RASDISPLAY;
		break;
	case HORZSIZE:
		ret = 320; // 17 inch monitor?
		break;
	case VERTSIZE:
		ret = 240; // 17 inch monitor?
		break;
	case HORZRES:
		ret = horz_res;
		break;
	case VERTRES:
		ret = vert_res;
		break;
	case DESKTOPHORZRES:
		ret = horz_res;
		break;
	case DESKTOPVERTRES:
		ret = vert_res;
		break;
	case BITSPIXEL:
		ret = bits_per_pixel;
		break;
	case PLANES:
		ret = 1;
		break;
	case NUMBRUSHES:
		ret = -1;
		break;
	case NUMPENS:
		ret = -1;
		break;
	case NUMMARKERS:
		ret = 0;
		break;
	case NUMFONTS:
		ret = 0;
		break;
	case NUMCOLORS:
		/* MSDN: Number of entries in the device's color table, if the device has
		* a color depth of no more than 8 bits per pixel.For devices with greater
		* color depths, -1 is returned. */
		ret = (bits_per_pixel > 8) ? -1 : (1 << bits_per_pixel);
		break;
	case CURVECAPS:
		ret = (CC_CIRCLES | CC_PIE | CC_CHORD | CC_ELLIPSES | CC_WIDE |
			CC_STYLED | CC_WIDESTYLED | CC_INTERIORS | CC_ROUNDRECT);
		break;
	case LINECAPS:
		ret = (LC_POLYLINE | LC_MARKER | LC_POLYMARKER | LC_WIDE |
			LC_STYLED | LC_WIDESTYLED | LC_INTERIORS);
		break;
	case POLYGONALCAPS:
		ret = (PC_POLYGON | PC_RECTANGLE | PC_WINDPOLYGON | PC_SCANLINE |
			PC_WIDE | PC_STYLED | PC_WIDESTYLED | PC_INTERIORS);
		break;
	case TEXTCAPS:
		ret = (TC_OP_CHARACTER | TC_OP_STROKE | TC_CP_STROKE |
			TC_CR_ANY | TC_SF_X_YINDEP | TC_SA_DOUBLE | TC_SA_INTEGER |
			TC_SA_CONTIN | TC_UA_ABLE | TC_SO_ABLE | TC_RA_ABLE | TC_VA_ABLE);
		break;
	case CLIPCAPS:
		ret = CP_REGION;
		break;
	case COLORRES:
		/* The observed correspondence between BITSPIXEL and COLORRES is:
		* BITSPIXEL: 8  -> COLORRES: 18
		* BITSPIXEL: 16 -> COLORRES: 16
		* BITSPIXEL: 24 -> COLORRES: 24
		* (note that bits_per_pixel is never 24)
		* BITSPIXEL: 32 -> COLORRES: 24 */
		ret = (bits_per_pixel <= 8) ? 18 : (bits_per_pixel == 32) ? 24 : bits_per_pixel;
		break;
	case RASTERCAPS:
		ret = (RC_BITBLT | RC_BANDING | RC_SCALING | RC_BITMAP64 | RC_DI_BITMAP |
			RC_DIBTODEV | RC_BIGFONT | RC_STRETCHBLT | RC_STRETCHDIB | RC_DEVBITS |
			(bits_per_pixel <= 8 ? RC_PALETTE : 0));
		break;
	case SHADEBLENDCAPS:
		ret = (SB_GRAD_RECT | SB_GRAD_TRI | SB_CONST_ALPHA | SB_PIXEL_ALPHA);
		break;
	case ASPECTX:
	case ASPECTY:
		ret = 36;
		break;
	case ASPECTXY:
		ret = 51;
		break;
	case LOGPIXELSX:
		ret = 96;
		break;
	case LOGPIXELSY:
		ret = 96;
		break;
	case CAPS1:
		kwarn("CAPS1 is unimplemented, will return 0\n");
		/* please see wingdi.h for the possible bit-flag values that need
		to be returned. */
		ret = 0;
		break;
	case SIZEPALETTE:
		ret = bits_per_pixel <= 8 ? 1 << bits_per_pixel : 0;
		break;
	case NUMRESERVED:
	case PHYSICALWIDTH:
	case PHYSICALHEIGHT:
	case PHYSICALOFFSETX:
	case PHYSICALOFFSETY:
	case SCALINGFACTORX:
	case SCALINGFACTORY:
	case VREFRESH:
	case BLTALIGNMENT:
		ret = 0;
		break;
	default:
		kwarn("unsupported capability %d %d %d %d, will return 0\n", ARG1, ARG2, ARG3, ARG4);
		ret = 0;
		break;
	}
	EAX = ret;
}

// int CDECL wine_notify_icon(DWORD msg, NOTIFYICONDATAW *data)
void wine_notify_icon(struct CPU* cpu) {
	notImplemented("wine_notify_icon not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeConfigure(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
void boxeddrv_ImeConfigure(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeConfigure not implemented");
	EAX = 0;
}

// DWORD WINAPI ImeConversionList(HIMC hIMC, LPCWSTR lpSource, LPCANDIDATELIST lpCandList, DWORD dwBufLen, UINT uFlag)
void boxeddrv_ImeConversionList(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeConversionList not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeDestroy(UINT uForce)
void boxeddrv_ImeDestroy(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeDestroy not implemented");
	EAX = 1;
}

// UINT WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROCW lpfnEnumProc, LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister, LPVOID lpData)
void boxeddrv_ImeEnumRegisterWord(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeEnumRegisterWord not implemented");
	EAX = 0;
}

// LRESULT WINAPI ImeEscape(HIMC hIMC, UINT uSubFunc, LPVOID lpData)
void boxeddrv_ImeEscape(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeEscape not implemented");
	EAX = 0;
}

// DWORD WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType, LPIMEMENUITEMINFOW lpImeParentMenu, LPIMEMENUITEMINFOW lpImeMenu, DWORD dwSize)
void boxeddrv_ImeGetImeMenuItems(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeGetImeMenuItems not implemented");
	EAX = 0;
}

// UINT WINAPI ImeGetRegisterWordStyle(UINT nItem, LPSTYLEBUFW lpStyleBuf)
void boxeddrv_ImeGetRegisterWordStyle(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeGetRegisterWordStyle not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPWSTR lpszUIClass, LPCWSTR lpszOption)
void boxeddrv_ImeInquire(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeInquire not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT vKey, LPARAM lKeyData, const LPBYTE lpbKeyState)
void boxeddrv_ImeProcessKey(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeProcessKey not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeRegisterWord(LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszRegister)
void boxeddrv_ImeRegisterWord(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeRegisterWord not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect)
void boxeddrv_ImeSelect(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeSelect not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeSetActiveContext(HIMC hIMC, BOOL fFlag)
void boxeddrv_ImeSetActiveContext(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeSetActiveContext not implemented");
	EAX = 1;
}

// BOOL WINAPI ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPCVOID lpComp, DWORD dwCompLen, LPCVOID lpRead, DWORD dwReadLen)
void boxeddrv_ImeSetCompositionString(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeSetCompositionString not implemented");
	EAX = 0;
}

// UINT WINAPI ImeToAsciiEx(UINT uVKey, UINT uScanCode, const LPBYTE lpbKeyState, LPDWORD lpdwTransKey, UINT fuState, HIMC hIMC)
void boxeddrv_ImeToAsciiEx(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeToAsciiEx not implemented");
	EAX = 0;
}

// BOOL WINAPI ImeUnregisterWord(LPCWSTR lpszReading, DWORD dwStyle, LPCWSTR lpszUnregister)
void boxeddrv_ImeUnregisterWord(struct CPU* cpu) {
	notImplemented("boxeddrv_ImeUnregisterWord not implemented");
	EAX = 0;
}

// BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
void boxeddrv_NotifyIME(struct CPU* cpu) {
	notImplemented("boxeddrv_NotifyIME not implemented");
	EAX = 0;
}

void boxeddrv_wglCopyContext(struct CPU* cpu) {
	
}

void boxeddrv_wglCreateContext(struct CPU* cpu) {
	
}

void boxeddrv_wglDeleteContext(struct CPU* cpu) {
	
}

void boxeddrv_wglDescribePixelFormat(struct CPU* cpu) {
	
}

void boxeddrv_wglGetPixelFormat(struct CPU* cpu) {
	
}

void boxeddrv_wglGetProcAddress(struct CPU* cpu) {
	
}

void boxeddrv_wglMakeCurrent(struct CPU* cpu) {
	
}

void boxeddrv_wglSetPixelFormat(struct CPU* cpu) {
	
}

void boxeddrv_wglShareLists(struct CPU* cpu) {
	
}

void boxeddrv_wglSwapBuffers(struct CPU* cpu) {
	
}

void boxeddrv_GetKeyboardLayoutList(struct CPU* cpu) {
	
}

void boxeddrv_RegisterHotKey(struct CPU* cpu) {
	
}

void boxeddrv_UnregisterHotKey(struct CPU* cpu) {
	
}

#include "kalloc.h"

Int99Callback* wine_callback;
int wine_callbackSize;

void initWine() {
	wine_callback = kalloc(sizeof(Int99Callback) * 76);
	wine_callback[BOXED_ACQUIRE_CLIPBOARD] = boxeddrv_AcquireClipboard;
	wine_callback[BOXED_ACTIVATE_KEYBOARD_LAYOUT] = boxeddrv_ActivateKeyboardLayout;
	wine_callback[BOXED_BEEP] = boxeddrv_Beep;
	wine_callback[BOXED_CHANGE_DISPLAY_SETTINGS_EX] = boxeddrv_ChangeDisplaySettingsEx;
	wine_callback[BOXED_CLIP_CURSOR] = boxeddrv_ClipCursor;
	wine_callback[BOXED_COUNT_CLIPBOARD_FORMATS] = boxeddrv_CountClipboardFormats;
	wine_callback[BOXED_CREATE_DESKTOP_WINDOW] = boxeddrv_CreateDesktopWindow;
	wine_callback[BOXED_CREATE_WINDOW] = boxeddrv_CreateWindow;
	wine_callback[BOXED_DESTROY_CURSOR_ICON] = boxeddrv_DestroyCursorIcon;
	wine_callback[BOXED_DESTROY_WINDOW] = boxeddrv_DestroyWindow;
	wine_callback[BOXED_EMPTY_CLIPBOARD] = boxeddrv_EmptyClipboard;
	wine_callback[BOXED_END_CLIPBOARD_UPDATE] = boxeddrv_EndClipboardUpdate;
	wine_callback[BOXED_ENUM_CLIPBOARD_FORMATS] = boxeddrv_EnumClipboardFormats;
	wine_callback[BOXED_ENUM_DISPLAY_MONITORS] = boxeddrv_EnumDisplayMonitors;
	wine_callback[BOXED_ENUM_DISPLAY_SETTINGS_EX] = boxeddrv_EnumDisplaySettingsEx;
	wine_callback[BOXED_GET_CLIPBOARD_DATA] = boxeddrv_GetClipboardData;
	wine_callback[BOXED_GET_CURSOR_POS] = boxeddrv_GetCursorPos;
	wine_callback[BOXED_GET_KEYBOARD_LAYOUT] = boxeddrv_GetKeyboardLayout;
	wine_callback[BOXED_GET_KEYBOARD_LAYOUT_NAME] = boxeddrv_GetKeyboardLayoutName;
	wine_callback[BOXED_GET_KEY_NAME] = boxeddrv_GetKeyNameText;
	wine_callback[BOXED_GET_MONITOR_INFO] = boxeddrv_GetMonitorInfo;
	wine_callback[BOXED_IS_CLIPBOARD_FORMAT_AVAILABLE] = boxeddrv_IsClipboardFormatAvailable;
	wine_callback[BOXED_MAP_VIRTUAL_KEY_EX] = boxeddrv_MapVirtualKeyEx;
	wine_callback[BOXED_MSG_WAIT_FOR_MULTIPLE_OBJECTS_EX] = boxeddrv_MsgWaitForMultipleObjectsEx;
	wine_callback[BOXED_SET_CAPTURE] = boxeddrv_SetCapture;
	wine_callback[BOXED_SET_CLIPBOARD_DATA] = boxeddrv_SetClipboardData;
	wine_callback[BOXED_SET_CURSOR] = boxeddrv_SetCursor;
	wine_callback[BOXED_SET_CURSOR_POS] = boxeddrv_SetCursorPos;
	wine_callback[BOXED_SET_FOCUS] = boxeddrv_SetFocus;
	wine_callback[BOXED_SET_LAYERED_WINDOW_ATTRIBUTES] = boxeddrv_SetLayeredWindowAttributes;
	wine_callback[BOXED_SET_PARENT] = boxeddrv_SetParent;
	wine_callback[BOXED_SET_WINDOW_RGN] = boxeddrv_SetWindowRgn;
	wine_callback[BOXED_SET_WINDOW_STYLE] = boxeddrv_SetWindowStyle;
	wine_callback[BOXED_SET_WINDOW_TEXT] = boxeddrv_SetWindowText;
	wine_callback[BOXED_SHOW_WINDOW] = boxeddrv_ShowWindow;
	wine_callback[BOXED_SYS_COMMAND] = boxeddrv_SysCommand;
	wine_callback[BOXED_SYSTEM_PARAMETERS_INFO] = boxeddrv_SystemParametersInfo;
	wine_callback[BOXED_TO_UNICODE_EX] = boxeddrv_ToUnicodeEx;
	wine_callback[BOXED_UPDATE_LAYERED_WINDOW] = boxeddrv_UpdateLayeredWindow;
	wine_callback[BOXED_VK_KEY_SCAN_EX] = boxeddrv_VkKeyScanEx;
	wine_callback[BOXED_WINDOW_MESSAGE] = boxeddrv_WindowMessage;
	wine_callback[BOXED_WINDOW_POS_CHANGED] = boxeddrv_WindowPosChanged;
	wine_callback[BOXED_WINDOW_POS_CHANGING] = boxeddrv_WindowPosChanging;
	wine_callback[BOXED_GET_DEVICE_GAMMA_RAMP] = boxeddrv_GetDeviceGammaRamp;
	wine_callback[BOXED_SET_DEVICE_GAMMA_RAMP] = boxeddrv_SetDeviceGammaRamp;
	wine_callback[BOXED_GET_DEVICE_CAPS] = boxeddrv_GetDeviceCaps;
	wine_callback[BOXED_WINE_NOTIFY_ICON] = wine_notify_icon;
	wine_callback[BOXED_IME_CONFIGURE] = boxeddrv_ImeConfigure;
	wine_callback[BOXED_IME_CONVERSION_LIST] = boxeddrv_ImeConversionList;
	wine_callback[BOXED_IME_DESTROY] = boxeddrv_ImeDestroy;
	wine_callback[BOXED_IME_ENUM_REGISTER_WORD] = boxeddrv_ImeEnumRegisterWord;
	wine_callback[BOXED_IME_ESCAPE] = boxeddrv_ImeEscape;
	wine_callback[BOXED_IME_GET_IME_MENU_ITEMS] = boxeddrv_ImeGetImeMenuItems;
	wine_callback[BOXED_IME_GET_REGISTER_WORD_STYLE] = boxeddrv_ImeGetRegisterWordStyle;
	wine_callback[BOXED_IME_INQUIRE] = boxeddrv_ImeInquire;
	wine_callback[BOXED_IME_PROCESS_KEY] = boxeddrv_ImeProcessKey;
	wine_callback[BOXED_IME_REGISTER_WORD] = boxeddrv_ImeRegisterWord;
	wine_callback[BOXED_IME_SELECT] = boxeddrv_ImeSelect;
	wine_callback[BOXED_IME_SET_ACTIVE_CONTEXT] = boxeddrv_ImeSetActiveContext;
	wine_callback[BOXED_IME_SET_COMPOSITION_STRING] = boxeddrv_ImeSetCompositionString;
	wine_callback[BOXED_IME_TO_ASCII_EX] = boxeddrv_ImeToAsciiEx;
	wine_callback[BOXED_IME_UNREGISTER_WORD] = boxeddrv_ImeUnregisterWord;
	wine_callback[BOXED_NOTIFY_IME] = boxeddrv_NotifyIME;
	wine_callback[BOXED_GL_COPY_CONTEXT] = boxeddrv_wglCopyContext;
	wine_callback[BOXED_GL_CREATE_CONTEXT] = boxeddrv_wglCreateContext;
	wine_callback[BOXED_GL_DELETE_CONTEXT] = boxeddrv_wglDeleteContext;
	wine_callback[BOXED_GL_DESCRIBE_PIXEL_FORMAT] = boxeddrv_wglDescribePixelFormat;
	wine_callback[BOXED_GL_GET_PIXEL_FORMAT] = boxeddrv_wglGetPixelFormat;
	wine_callback[BOXED_GL_GET_PROC_ADDRESS] = boxeddrv_wglGetProcAddress;
	wine_callback[BOXED_GL_MAKE_CURRENT] = boxeddrv_wglMakeCurrent;
	wine_callback[BOXED_GL_SET_PIXEL_FORMAT] = boxeddrv_wglSetPixelFormat;
	wine_callback[BOXED_GL_SHARE_LISTS] = boxeddrv_wglShareLists;
	wine_callback[BOXED_GL_SWAP_BUFFERS] = boxeddrv_wglSwapBuffers;
	wine_callback[BOXED_GET_KEYBOARD_LAYOUT_LIST] = boxeddrv_GetKeyboardLayoutList;
	wine_callback[BOXED_REGISTER_HOT_KEY] = boxeddrv_RegisterHotKey;
	wine_callback[BOXED_UNREGISTER_HOT_KEY] = boxeddrv_UnregisterHotKey;
	wine_callbackSize = 76;
}