# GDI driver

@ cdecl wine_get_gdi_driver(long) boxeddrv_get_gdi_driver

# USER driver

@ cdecl ActivateKeyboardLayout(long long) boxeddrv_ActivateKeyboardLayout
@ cdecl Beep() boxeddrv_Beep
@ cdecl ChangeDisplaySettingsEx(ptr ptr long long long) boxeddrv_ChangeDisplaySettingsEx
@ cdecl ClipCursor(ptr) boxeddrv_ClipCursor
@ cdecl CountClipboardFormats() boxeddrv_CountClipboardFormats
@ cdecl CreateDesktopWindow(long) boxeddrv_CreateDesktopWindow
@ cdecl CreateWindow(long) boxeddrv_CreateWindow
@ cdecl DestroyCursorIcon(long) boxeddrv_DestroyCursorIcon
@ cdecl DestroyWindow(long) boxeddrv_DestroyWindow
@ cdecl EmptyClipboard() boxeddrv_EmptyClipboard
@ cdecl EndClipboardUpdate() boxeddrv_EndClipboardUpdate
@ cdecl EnumClipboardFormats(long) boxeddrv_EnumClipboardFormats
@ cdecl EnumDisplayMonitors(long ptr ptr long) boxeddrv_EnumDisplayMonitors
@ cdecl EnumDisplaySettingsEx(ptr long ptr long) boxeddrv_EnumDisplaySettingsEx
@ cdecl GetClipboardData(long) boxeddrv_GetClipboardData
@ cdecl GetCursorPos(ptr) boxeddrv_GetCursorPos
@ cdecl GetKeyboardLayout(long) boxeddrv_GetKeyboardLayout
@ cdecl GetKeyboardLayoutList(long ptr) boxeddrv_GetKeyboardLayoutList
@ cdecl GetKeyboardLayoutName(ptr) boxeddrv_GetKeyboardLayoutName
@ cdecl GetKeyNameText(long ptr long) boxeddrv_GetKeyNameText
@ cdecl GetMonitorInfo(long ptr) boxeddrv_GetMonitorInfo
@ cdecl IsClipboardFormatAvailable(long) boxeddrv_IsClipboardFormatAvailable
@ cdecl MapVirtualKeyEx(long long long) boxeddrv_MapVirtualKeyEx
@ cdecl MsgWaitForMultipleObjectsEx(long ptr long long long) boxeddrv_MsgWaitForMultipleObjectsEx
@ cdecl RegisterHotKey(long long long) boxeddrv_RegisterHotKey
@ cdecl SetCapture(long long) boxeddrv_SetCapture
@ cdecl SetClipboardData(long long long) boxeddrv_SetClipboardData
@ cdecl SetCursor(long) boxeddrv_SetCursor
@ cdecl SetCursorPos(long long) boxeddrv_SetCursorPos
@ cdecl SetFocus(long) boxeddrv_SetFocus
@ cdecl SetLayeredWindowAttributes(long long long long) boxeddrv_SetLayeredWindowAttributes
@ cdecl SetParent(long long long) boxeddrv_SetParent
@ cdecl SetWindowRgn(long long long) boxeddrv_SetWindowRgn
@ cdecl SetWindowStyle(ptr long ptr) boxeddrv_SetWindowStyle
@ cdecl SetWindowText(long wstr) boxeddrv_SetWindowText
@ cdecl ShowWindow(long long ptr long) boxeddrv_ShowWindow
@ cdecl SysCommand(long long long) boxeddrv_SysCommand
@ cdecl SystemParametersInfo(long long ptr long) boxeddrv_SystemParametersInfo
@ cdecl ToUnicodeEx(long long ptr ptr long long long) boxeddrv_ToUnicodeEx
@ cdecl UnregisterHotKey(long long long) boxeddrv_UnregisterHotKey
@ cdecl UpdateLayeredWindow(long ptr ptr) boxeddrv_UpdateLayeredWindow
@ cdecl VkKeyScanEx(long long) boxeddrv_VkKeyScanEx
@ cdecl WindowMessage(long long long long) boxeddrv_WindowMessage
@ cdecl WindowPosChanged(long long long ptr ptr ptr ptr ptr) boxeddrv_WindowPosChanged
@ cdecl WindowPosChanging(long long long ptr ptr ptr ptr) boxeddrv_WindowPosChanging

# System tray
@ cdecl wine_notify_icon(long ptr)

# IME
@ stdcall ImeConfigure(long long long ptr)
@ stdcall ImeConversionList(long wstr ptr long long)
@ stdcall ImeDestroy(long)
@ stdcall ImeEnumRegisterWord(ptr wstr long wstr ptr)
@ stdcall ImeEscape(long long ptr)
@ stdcall ImeGetImeMenuItems(long long long ptr ptr long)
@ stdcall ImeGetRegisterWordStyle(long ptr)
@ stdcall ImeInquire(ptr wstr wstr)
@ stdcall ImeProcessKey(long long long ptr)
@ stdcall ImeRegisterWord(wstr long wstr)
@ stdcall ImeSelect(long long)
@ stdcall ImeSetActiveContext(long long)
@ stdcall ImeSetCompositionString(long long ptr long ptr long)
@ stdcall ImeToAsciiEx(long long ptr ptr long long)
@ stdcall ImeUnregisterWord(wstr long wstr)
@ stdcall NotifyIME(long long long long)
