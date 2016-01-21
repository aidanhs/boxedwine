/*
 * Window stations and desktops
 *
 * Copyright 2002 Alexandre Julliard
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
#include "../include/ntstatus.h"
#include "../include/unicode.h"

#include "user_private.h"
#include "pbl.h"
#include "kprocess.h"
#include "kalloc.h"

PblMap* winstations;

/* callback for enumeration functions */
struct enum_proc_lparam
{
    NAMEENUMPROCA func;
    LPARAM        lparam;
};

static BOOL CALLBACK enum_names_WtoA( LPWSTR name, LPARAM lparam )
{
    struct enum_proc_lparam *data = (struct enum_proc_lparam *)lparam;
    char buffer[MAX_PATH];

    if (!WideCharToMultiByte_k( CP_ACP, 0, name, -1, buffer, sizeof(buffer), NULL, NULL ))
        return FALSE;
    return data->func( buffer, data->lparam );
}


/***********************************************************************
 *              CreateWindowStationA  (USER32.@)
 */
HWINSTA WINAPI CreateWindowStationA_k( LPCSTR name, DWORD reserved, ACCESS_MASK access,
                                     LPSECURITY_ATTRIBUTES sa )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateWindowStationW_k( NULL, reserved, access, sa );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateWindowStationW_k( buffer, reserved, access, sa );
}

/***********************************************************************
 *              CreateWindowStationW  (USER32.@)
 */
HWINSTA WINAPI CreateWindowStationW_k( LPCWSTR name, DWORD reserved, ACCESS_MASK access,
                                     LPSECURITY_ATTRIBUTES sa )
{
    struct winstation *winstation;
    WCHAR lowercase_name[MAX_PATH];

    if (memchrW( name, '\\', strlenW(name) ))  /* no backslash allowed in name */
    {
        SetLastError_k( STATUS_INVALID_PARAMETER );
        return NULL;
    }
    strcpyW(lowercase_name, name);
    strlwrW(lowercase_name);
    if (!winstations) {
        winstations = pblMapNewHashMap();
    }
    
    winstation = pblMapGet(winstations, lowercase_name, (strlenW(lowercase_name)+1)*2, NULL);
    if (!winstation) {
        struct winstation w;
        w.atom_table = NULL;
        w.clipboard = NULL;
        w.desktops = pblMapNewHashMap();
        w.flags = 0;
        strcpyW(w.name, name);
        winstation = pblMapPut(winstations, lowercase_name, (strlenW(lowercase_name)+1)*2, &w, sizeof(struct winstation), NULL);
    }
    return (HWINSTA)winstation;
}


/******************************************************************************
 *              OpenWindowStationA  (USER32.@)
 */
HWINSTA WINAPI OpenWindowStationA_k( LPCSTR name, BOOL inherit, ACCESS_MASK access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenWindowStationW_k( NULL, inherit, access );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenWindowStationW_k( buffer, inherit, access );
}


/******************************************************************************
 *              OpenWindowStationW  (USER32.@)
 */
HWINSTA WINAPI OpenWindowStationW_k( LPCWSTR name, BOOL inherit, ACCESS_MASK access )
{
    WCHAR lowercase_name[MAX_PATH];

    strcpyW(lowercase_name, name);
    strlwrW(lowercase_name);
    return (HWINSTA)pblMapGet(winstations, lowercase_name, (strlenW(lowercase_name)+1)*2, NULL);
}


/***********************************************************************
 *              CloseWindowStation  (USER32.@)
 */
BOOL WINAPI CloseWindowStation_k( HWINSTA handle )
{
    return TRUE;
}


/******************************************************************************
 *              GetProcessWindowStation  (USER32.@)
 */
HWINSTA WINAPI GetProcessWindowStation_k(void)
{
    return (HWINSTA)currentProcess()->winstation;
}


/***********************************************************************
 *              SetProcessWindowStation  (USER32.@)
 */
BOOL WINAPI SetProcessWindowStation_k( HWINSTA handle )
{
    currentProcess()->winstation = (struct winstation*)handle;
    return TRUE;
}


/******************************************************************************
 *              EnumWindowStationsA  (USER32.@)
 */
BOOL WINAPI EnumWindowStationsA_k( WINSTAENUMPROCA func, LPARAM lparam )
{
    struct enum_proc_lparam data;
    data.func   = func;
    data.lparam = lparam;
    return EnumWindowStationsW_k( enum_names_WtoA, (LPARAM)&data );
}


/******************************************************************************
 *              EnumWindowStationsW  (USER32.@)
 */
BOOL WINAPI EnumWindowStationsW_k( WINSTAENUMPROCW func, LPARAM lparam )
{
    PblIterator* it = pblMapIteratorNew(winstations);
    BOOL ret = TRUE;

    while (ret && it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        WCHAR* name = pblMapEntryKey(entry);
        ret = func( name, lparam );
    }
    pblIteratorFree(it);
    return ret;
}


/***********************************************************************
 *              CreateDesktopA   (USER32.@)
 */
HDESK WINAPI CreateDesktopA_k( LPCSTR name, LPCSTR device, LPDEVMODEA devmode,
                             DWORD flags, ACCESS_MASK access, LPSECURITY_ATTRIBUTES sa )
{
    WCHAR buffer[MAX_PATH];

    if (device || devmode)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }
    if (!name) return CreateDesktopW_k( NULL, NULL, NULL, flags, access, sa );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateDesktopW_k( buffer, NULL, NULL, flags, access, sa );
}


/***********************************************************************
 *              CreateDesktopW   (USER32.@)
 */
HDESK WINAPI CreateDesktopW_k( LPCWSTR name, LPCWSTR device, LPDEVMODEW devmode,
                             DWORD flags, ACCESS_MASK access, LPSECURITY_ATTRIBUTES sa )
{
    DWORD len = name ? strlenW(name) : 0;
    struct desktop* desktop;
    struct winstation* winstation = currentProcess()->winstation;
    WCHAR lowercase_name[MAX_PATH];

    strcpyW(lowercase_name, name);
    strlwrW(lowercase_name);
    if (device || devmode)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }
    if (len >= MAX_PATH)
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    if (!winstation) {
        return 0;
    }

    if (name) {
        desktop = pblMapGet(winstation->desktops, (void*)lowercase_name, (len+1)*2, NULL);
        if (desktop)
            return (HDESK)desktop;
    }
    desktop = kalloc(sizeof(struct desktop));
    desktop->winstation = winstation;
    pblMapPut(winstation->desktops, (void*)lowercase_name, (len+1)*2, desktop, sizeof(struct desktop), NULL);
    kfree(desktop);
    return (HDESK)desktop;
}


/******************************************************************************
 *              OpenDesktopA   (USER32.@)
 */
HDESK WINAPI OpenDesktopA_k( LPCSTR name, DWORD flags, BOOL inherit, ACCESS_MASK access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenDesktopW_k( NULL, flags, inherit, access );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenDesktopW_k( buffer, flags, inherit, access );
}


/******************************************************************************
 *              OpenDesktopW   (USER32.@)
 */
HDESK WINAPI OpenDesktopW_k( LPCWSTR name, DWORD flags, BOOL inherit, ACCESS_MASK access )
{
    WCHAR lowercase_name[MAX_PATH];

    strcpyW(lowercase_name, name);
    strlwrW(lowercase_name);
    return pblMapGet(currentProcess()->winstation->desktops, (void*)lowercase_name, (strlenW(lowercase_name)+1)*2, NULL);
}


/***********************************************************************
 *              CloseDesktop  (USER32.@)
 */
BOOL WINAPI CloseDesktop_k( HDESK handle )
{
    return TRUE;
}


/******************************************************************************
 *              GetThreadDesktop   (USER32.@)
 */
HDESK WINAPI GetThreadDesktop_k( DWORD threadId )
{
    struct KThread* thread = getThread(threadId);
    if (!thread) {
        SetLastError_k(ERROR_INVALID_THREAD_ID);
        return 0;
    }
    return (HDESK)thread->input->desktop;
}


/******************************************************************************
 *              SetThreadDesktop   (USER32.@)
 */
BOOL WINAPI SetThreadDesktop_k( HDESK handle )
{
    struct KThread* thread = currentThread();
    if (!thread) {
        SetLastError_k(ERROR_INVALID_THREAD_ID);
        return 0;
    }
    thread->input->desktop = (struct desktop*)handle;
    thread->Win32ClientInfo->top_window = 0;
    thread->Win32ClientInfo->msg_window = 0;
    thread->Win32ClientInfo->key_state_time = 0;
   return TRUE;
}


/******************************************************************************
 *              EnumDesktopsA   (USER32.@)
 */
BOOL WINAPI EnumDesktopsA_k( HWINSTA winsta, DESKTOPENUMPROCA func, LPARAM lparam )
{
    struct enum_proc_lparam data;
    data.func   = func;
    data.lparam = lparam;
    return EnumDesktopsW_k( winsta, enum_names_WtoA, (LPARAM)&data );
}


/******************************************************************************
 *              EnumDesktopsW   (USER32.@)
 */
BOOL WINAPI EnumDesktopsW_k( HWINSTA winsta, DESKTOPENUMPROCW func, LPARAM lparam )
{
    PblIterator* it = pblMapIteratorNew(((struct winstation*)winsta)->desktops);
    BOOL ret = TRUE;

    while (ret && it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        WCHAR* name = pblMapEntryKey(entry);
        ret = func( name, lparam );
    }
    pblIteratorFree(it);
    return ret;
}


/******************************************************************************
 *              OpenInputDesktop   (USER32.@)
 */
HDESK WINAPI OpenInputDesktop_k( DWORD flags, BOOL inherit, ACCESS_MASK access )
{
    FIXME( "(%x,%i,%x): stub\n", flags, inherit, access );
    SetLastError_k( ERROR_CALL_NOT_IMPLEMENTED );
    return 0;
}


/***********************************************************************
 *              GetUserObjectInformationA   (USER32.@)
 */
BOOL WINAPI GetUserObjectInformationA_k( HANDLE handle, INT index, LPVOID info, DWORD len, LPDWORD needed )
{
    /* check for information types returning strings */
    if (index == UOI_TYPE || index == UOI_NAME)
    {
        WCHAR buffer[MAX_PATH];
        DWORD lenA, lenW;

        if (!GetUserObjectInformationW_k( handle, index, buffer, sizeof(buffer), &lenW )) return FALSE;
        lenA = WideCharToMultiByte_k( CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL );
        if (needed) *needed = lenA;
        if (lenA > len)
        {
            /* If the buffer length supplied by the caller is insufficient, Windows returns a
               'needed' length based upon the Unicode byte length, so we should do similarly. */
            if (needed) *needed = lenW;

            SetLastError_k( ERROR_INSUFFICIENT_BUFFER );
            return FALSE;
        }
        if (info) WideCharToMultiByte_k( CP_ACP, 0, buffer, -1, info, len, NULL, NULL );
        return TRUE;
    }
    return GetUserObjectInformationW_k( handle, index, info, len, needed );
}


/***********************************************************************
 *              GetUserObjectInformationW   (USER32.@)
 */
BOOL WINAPI GetUserObjectInformationW_k( HANDLE handle, INT index, LPVOID info, DWORD len, LPDWORD needed )
{
    FIXME("GetUserObjectInformationW not imlemented");
    SetLastError_k( ERROR_CALL_NOT_IMPLEMENTED );
    return 0;
}


/******************************************************************************
 *              SetUserObjectInformationA   (USER32.@)
 */
BOOL WINAPI SetUserObjectInformationA_k( HANDLE handle, INT index, LPVOID info, DWORD len )
{
    return SetUserObjectInformationW_k( handle, index, info, len );
}


/******************************************************************************
 *              SetUserObjectInformationW   (USER32.@)
 */
BOOL WINAPI SetUserObjectInformationW_k( HANDLE handle, INT index, LPVOID info, DWORD len )
{
    FIXME("SetUserObjectInformationW not imlemented");
    SetLastError_k( ERROR_CALL_NOT_IMPLEMENTED );
    return 0;
}


/***********************************************************************
 *              GetUserObjectSecurity   (USER32.@)
 */
BOOL WINAPI GetUserObjectSecurity_k( HANDLE handle, PSECURITY_INFORMATION info,
                                   PSECURITY_DESCRIPTOR sid, DWORD len, LPDWORD needed )
{
    FIXME( "(%p %p %p len=%d %p),stub!\n", handle, info, sid, len, needed );
    if (needed)
        *needed = sizeof(SECURITY_DESCRIPTOR);
    if (len < sizeof(SECURITY_DESCRIPTOR))
    {
        SetLastError_k( ERROR_INSUFFICIENT_BUFFER );
        return FALSE;
    }
    return InitializeSecurityDescriptor_k(sid, SECURITY_DESCRIPTOR_REVISION);
}

/***********************************************************************
 *              SetUserObjectSecurity   (USER32.@)
 */
BOOL WINAPI SetUserObjectSecurity_k( HANDLE handle, PSECURITY_INFORMATION info,
                                   PSECURITY_DESCRIPTOR sid )
{
    FIXME( "(%p,%p,%p),stub!\n", handle, info, sid );
    return TRUE;
}
