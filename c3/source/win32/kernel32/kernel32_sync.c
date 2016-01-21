/*
 * Kernel synchronization objects
 *
 * Copyright 1998 Alexandre Julliard
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
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "../include/winbase.h"
#include "../include/ntstatus.h"
#include "../include/unicode.h"
#include "../include/winioctl.h"
#include "../include/ddk/wdm.h"
#include "kthread.h"

/* check if current version is NT or Win95 */
static INLINE int is_version_nt(void)
{
    return !(GetVersion_k() & 0x80000000);
}

/* returns directory handle to \\BaseNamedObjects */
HANDLE get_BaseNamedObjects_handle(void)
{
    static HANDLE handle = NULL;
    static const WCHAR basenameW[] =
        {'\\','B','a','s','e','N','a','m','e','d','O','b','j','e','c','t','s',0};
    UNICODE_STRING str;
    OBJECT_ATTRIBUTES attr;

    if (!handle)
    {
        HANDLE dir;

        RtlInitUnicodeString_k(&str, basenameW);
        InitializeObjectAttributes_k(&attr, &str, 0, 0, NULL);
        NtOpenDirectoryObject_k(&dir, DIRECTORY_CREATE_OBJECT|DIRECTORY_TRAVERSE, &attr);
        if (InterlockedCompareExchangePointer_k( &handle, dir, 0 ) != 0)
        {
            /* someone beat us here... */
            CloseHandle_k( dir );
        }
    }
    return handle;
}

/* helper for kernel32->ntdll timeout format conversion */
static INLINE PLARGE_INTEGER get_nt_timeout( PLARGE_INTEGER pTime, DWORD timeout )
{
    if (timeout == INFINITE) return NULL;
    pTime->QuadPart = (ULONGLONG)timeout * -10000;
    return pTime;
}

/***********************************************************************
 *              Sleep  (KERNEL32.@)
 */
VOID WINAPI Sleep_k( DWORD timeout )
{
    SleepEx_k( timeout, FALSE );
}

/******************************************************************************
 *              SleepEx   (KERNEL32.@)
 */
DWORD WINAPI SleepEx_k( DWORD timeout, BOOL alertable )
{
    NTSTATUS status;
    LARGE_INTEGER time;

    status = NtDelayExecution_k( alertable, get_nt_timeout( &time, timeout ) );
    if (status == STATUS_USER_APC) return WAIT_IO_COMPLETION;
    return 0;
}


/***********************************************************************
 *		SwitchToThread (KERNEL32.@)
 */
BOOL WINAPI SwitchToThread_k(void)
{
    return (NtYieldExecution_k() != STATUS_NO_YIELD_PERFORMED);
}


/***********************************************************************
 *           WaitForSingleObject   (KERNEL32.@)
 */
DWORD WINAPI WaitForSingleObject_k( HANDLE handle, DWORD timeout )
{
    return WaitForMultipleObjectsEx_k( 1, &handle, FALSE, timeout, FALSE );
}


/***********************************************************************
 *           WaitForSingleObjectEx   (KERNEL32.@)
 */
DWORD WINAPI WaitForSingleObjectEx_k( HANDLE handle, DWORD timeout,
                                    BOOL alertable )
{
    return WaitForMultipleObjectsEx_k( 1, &handle, FALSE, timeout, alertable );
}


/***********************************************************************
 *           WaitForMultipleObjects   (KERNEL32.@)
 */
DWORD WINAPI WaitForMultipleObjects_k( DWORD count, const HANDLE *handles,
                                     BOOL wait_all, DWORD timeout )
{
    return WaitForMultipleObjectsEx_k( count, handles, wait_all, timeout, FALSE );
}


/***********************************************************************
 *           WaitForMultipleObjectsEx   (KERNEL32.@)
 */
DWORD WINAPI WaitForMultipleObjectsEx_k( DWORD count, const HANDLE *handles,
                                       BOOL wait_all, DWORD timeout,
                                       BOOL alertable )
{
    NTSTATUS status;
    HANDLE hloc[MAXIMUM_WAIT_OBJECTS];
    LARGE_INTEGER time;
    unsigned int i;

    if (count > MAXIMUM_WAIT_OBJECTS)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return WAIT_FAILED;
    }
    for (i = 0; i < count; i++)
    {
        if ((handles[i] == (HANDLE)STD_INPUT_HANDLE) ||
            (handles[i] == (HANDLE)STD_OUTPUT_HANDLE) ||
            (handles[i] == (HANDLE)STD_ERROR_HANDLE))
            hloc[i] = GetStdHandle_k( HandleToULong(handles[i]) );
        else
            hloc[i] = handles[i];

        /* yes, even screen buffer console handles are waitable, and are
         * handled as a handle to the console itself !!
         */
        if (is_console_handle(hloc[i]))
        {
            if (VerifyConsoleIoHandle_k(hloc[i]))
                hloc[i] = GetConsoleInputWaitHandle();
        }
    }

    status = NtWaitForMultipleObjects_k( count, hloc, wait_all, alertable,
                                       get_nt_timeout( &time, timeout ) );

    if (HIWORD(status))  /* is it an error code? */
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        status = WAIT_FAILED;
    }
    return status;
}


/***********************************************************************
 *           RegisterWaitForSingleObject   (KERNEL32.@)
 */
BOOL WINAPI RegisterWaitForSingleObject_k(PHANDLE phNewWaitObject, HANDLE hObject,
                WAITORTIMERCALLBACK Callback, PVOID Context,
                ULONG dwMilliseconds, ULONG dwFlags)
{
    NTSTATUS status;

    TRACE("%p %p %p %p %d %d\n",
          phNewWaitObject,hObject,Callback,Context,dwMilliseconds,dwFlags);

    status = RtlRegisterWait_k( phNewWaitObject, hObject, Callback, Context, dwMilliseconds, dwFlags );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}

/***********************************************************************
 *           RegisterWaitForSingleObjectEx   (KERNEL32.@)
 */
HANDLE WINAPI RegisterWaitForSingleObjectEx_k( HANDLE hObject, 
                WAITORTIMERCALLBACK Callback, PVOID Context,
                ULONG dwMilliseconds, ULONG dwFlags ) 
{
    NTSTATUS status;
    HANDLE hNewWaitObject;

    TRACE("%p %p %p %d %d\n",
          hObject,Callback,Context,dwMilliseconds,dwFlags);

    status = RtlRegisterWait_k( &hNewWaitObject, hObject, Callback, Context, dwMilliseconds, dwFlags );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return NULL;
    }
    return hNewWaitObject;
}

/***********************************************************************
 *           UnregisterWait   (KERNEL32.@)
 */
BOOL WINAPI UnregisterWait_k( HANDLE WaitHandle ) 
{
    NTSTATUS status;

    TRACE("%p\n",WaitHandle);

    status = RtlDeregisterWait_k( WaitHandle );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}

/***********************************************************************
 *           UnregisterWaitEx   (KERNEL32.@)
 */
BOOL WINAPI UnregisterWaitEx_k( HANDLE WaitHandle, HANDLE CompletionEvent ) 
{
    NTSTATUS status;

    TRACE("%p %p\n",WaitHandle, CompletionEvent);

    status = RtlDeregisterWaitEx_k( WaitHandle, CompletionEvent );
    if (status != STATUS_SUCCESS) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/***********************************************************************
 *           SignalObjectAndWait  (KERNEL32.@)
 *
 * Makes it possible to atomically signal any of the synchronization
 * objects (semaphore, mutex, event) and wait on another.
 */
DWORD WINAPI SignalObjectAndWait_k( HANDLE hObjectToSignal, HANDLE hObjectToWaitOn,
                                  DWORD dwMilliseconds, BOOL bAlertable )
{
    NTSTATUS status;
    LARGE_INTEGER timeout;

    TRACE("%p %p %d %d\n", hObjectToSignal,
          hObjectToWaitOn, dwMilliseconds, bAlertable);

    status = NtSignalAndWaitForSingleObject_k( hObjectToSignal, hObjectToWaitOn, bAlertable,
                                             get_nt_timeout( &timeout, dwMilliseconds ) );
    if (HIWORD(status))
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        status = WAIT_FAILED;
    }
    return status;
}

/***********************************************************************
 *           InitializeCriticalSection   (KERNEL32.@)
 *
 * Initialise a critical section before use.
 *
 * PARAMS
 *  crit [O] Critical section to initialise.
 *
 * RETURNS
 *  Nothing. If the function fails an exception is raised.
 */
void WINAPI InitializeCriticalSection_k( CRITICAL_SECTION *crit )
{
    InitializeCriticalSectionEx_k( crit, 0, 0 );
}

/***********************************************************************
 *           InitializeCriticalSectionAndSpinCount   (KERNEL32.@)
 *
 * Initialise a critical section with a spin count.
 *
 * PARAMS
 *  crit      [O] Critical section to initialise.
 *  spincount [I] Number of times to spin upon contention.
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: Nothing. If the function fails an exception is raised.
 *
 * NOTES
 *  spincount is ignored on uni-processor systems.
 */
BOOL WINAPI InitializeCriticalSectionAndSpinCount_k( CRITICAL_SECTION *crit, DWORD spincount )
{
    return InitializeCriticalSectionEx_k( crit, spincount, 0 );
}

/***********************************************************************
 *           InitializeCriticalSectionEx   (KERNEL32.@)
 *
 * Initialise a critical section with a spin count and flags.
 *
 * PARAMS
 *  crit      [O] Critical section to initialise.
 *  spincount [I] Number of times to spin upon contention.
 *  flags     [I] CRITICAL_SECTION_ flags from winbase.h.
 *
 * RETURNS
 *  Success: TRUE.
 *  Failure: Nothing. If the function fails an exception is raised.
 *
 * NOTES
 *  spincount is ignored on uni-processor systems.
 */
BOOL WINAPI InitializeCriticalSectionEx_k( CRITICAL_SECTION *crit, DWORD spincount, DWORD flags )
{
    NTSTATUS ret = RtlInitializeCriticalSectionEx_k( crit, spincount, flags );
    if (ret) RtlRaiseStatus_k( ret );
    return !ret;
}

/***********************************************************************
 *           MakeCriticalSectionGlobal   (KERNEL32.@)
 */
void WINAPI MakeCriticalSectionGlobal_k( CRITICAL_SECTION *crit )
{
    /* let's assume that only one thread at a time will try to do this */
    HANDLE sem = crit->LockSemaphore;
    if (!sem) NtCreateSemaphore_k( &sem, SEMAPHORE_ALL_ACCESS, NULL, 0, 1 );
    crit->LockSemaphore = ConvertToGlobalHandle_k( sem );
    RtlFreeHeap_k( GetProcessHeap_k(), 0, crit->DebugInfo );
    crit->DebugInfo = NULL;
}


/***********************************************************************
 *           ReinitializeCriticalSection   (KERNEL32.@)
 *
 * Initialise an already used critical section.
 *
 * PARAMS
 *  crit [O] Critical section to initialise.
 *
 * RETURNS
 *  Nothing.
 */
void WINAPI ReinitializeCriticalSection_k( CRITICAL_SECTION *crit )
{
    if ( !crit->LockSemaphore )
        RtlInitializeCriticalSection_k( crit );
}


/***********************************************************************
 *           UninitializeCriticalSection   (KERNEL32.@)
 *
 * UnInitialise a critical section after use.
 *
 * PARAMS
 *  crit [O] Critical section to uninitialise (destroy).
 *
 * RETURNS
 *  Nothing.
 */
void WINAPI UninitializeCriticalSection_k( CRITICAL_SECTION *crit )
{
    RtlDeleteCriticalSection_k( crit );
}


/***********************************************************************
 *           CreateEventA    (KERNEL32.@)
 */
HANDLE WINAPI CreateEventA_k( SECURITY_ATTRIBUTES *sa, BOOL manual_reset,
                                              BOOL initial_state, LPCSTR name )
{
    DWORD flags = 0;

    if (manual_reset) flags |= CREATE_EVENT_MANUAL_RESET;
    if (initial_state) flags |= CREATE_EVENT_INITIAL_SET;
    return CreateEventExA_k( sa, name, flags, EVENT_ALL_ACCESS );
}


/***********************************************************************
 *           CreateEventW    (KERNEL32.@)
 */
HANDLE WINAPI CreateEventW_k( SECURITY_ATTRIBUTES *sa, BOOL manual_reset,
                                              BOOL initial_state, LPCWSTR name )
{
    DWORD flags = 0;

    if (manual_reset) flags |= CREATE_EVENT_MANUAL_RESET;
    if (initial_state) flags |= CREATE_EVENT_INITIAL_SET;
    return CreateEventExW_k( sa, name, flags, EVENT_ALL_ACCESS );
}


/***********************************************************************
 *           CreateEventExA    (KERNEL32.@)
 */
HANDLE WINAPI CreateEventExA_k( SECURITY_ATTRIBUTES *sa, LPCSTR name, DWORD flags, DWORD access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateEventExW_k( sa, NULL, flags, access );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateEventExW_k( sa, buffer, flags, access );
}


/***********************************************************************
 *           CreateEventExW    (KERNEL32.@)
 */
HANDLE WINAPI CreateEventExW_k( SECURITY_ATTRIBUTES *sa, LPCWSTR name, DWORD flags, DWORD access )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    /* one buggy program needs this
     * ("Van Dale Groot woordenboek der Nederlandse taal")
     */
    if (sa && IsBadReadPtr_k(sa,sizeof(SECURITY_ATTRIBUTES)))
    {
        ERR("Bad security attributes pointer %p\n",sa);
        SetLastError_k( ERROR_INVALID_PARAMETER);
        return 0;
    }

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = OBJ_OPENIF | ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtCreateEvent_k( &ret, access, &attr,
                            (flags & CREATE_EVENT_MANUAL_RESET) ? NotificationEvent : SynchronizationEvent,
                            (flags & CREATE_EVENT_INITIAL_SET) != 0 );
    if (status == STATUS_OBJECT_NAME_EXISTS)
        SetLastError_k( ERROR_ALREADY_EXISTS );
    else
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return ret;
}


/***********************************************************************
 *           OpenEventA    (KERNEL32.@)
 */
HANDLE WINAPI OpenEventA_k( DWORD access, BOOL inherit, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenEventW_k( access, inherit, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenEventW_k( access, inherit, buffer );
}


/***********************************************************************
 *           OpenEventW    (KERNEL32.@)
 */
HANDLE WINAPI OpenEventW_k( DWORD access, BOOL inherit, LPCWSTR name )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    if (!is_version_nt()) access = EVENT_ALL_ACCESS;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = inherit ? OBJ_INHERIT : 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtOpenEvent_k( &ret, access, &attr );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return ret;
}

/***********************************************************************
 *           PulseEvent    (KERNEL32.@)
 */
BOOL WINAPI PulseEvent_k( HANDLE handle )
{
    NTSTATUS status;

    if ((status = NtPulseEvent_k( handle, NULL )))
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}


/***********************************************************************
 *           SetEvent    (KERNEL32.@)
 */
BOOL WINAPI SetEvent_k( HANDLE handle )
{
    NTSTATUS status;

    if ((status = NtSetEvent_k( handle, NULL )))
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}


/***********************************************************************
 *           ResetEvent    (KERNEL32.@)
 */
BOOL WINAPI ResetEvent_k( HANDLE handle )
{
    NTSTATUS status;

    if ((status = NtResetEvent_k( handle, NULL )))
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}


/***********************************************************************
 *           CreateMutexA   (KERNEL32.@)
 */
HANDLE WINAPI CreateMutexA_k( SECURITY_ATTRIBUTES *sa, BOOL owner, LPCSTR name )
{
    return CreateMutexExA_k( sa, name, owner ? CREATE_MUTEX_INITIAL_OWNER : 0, MUTEX_ALL_ACCESS );
}


/***********************************************************************
 *           CreateMutexW   (KERNEL32.@)
 */
HANDLE WINAPI CreateMutexW_k( SECURITY_ATTRIBUTES *sa, BOOL owner, LPCWSTR name )
{
    return CreateMutexExW_k( sa, name, owner ? CREATE_MUTEX_INITIAL_OWNER : 0, MUTEX_ALL_ACCESS );
}


/***********************************************************************
 *           CreateMutexExA   (KERNEL32.@)
 */
HANDLE WINAPI CreateMutexExA_k( SECURITY_ATTRIBUTES *sa, LPCSTR name, DWORD flags, DWORD access )
{
    ANSI_STRING nameA;
    NTSTATUS status;
    struct KThread* thread = currentThread();

    if (!name) return CreateMutexExW_k( sa, NULL, flags, access );

    RtlInitAnsiString_k( &nameA, name );
    status = RtlAnsiStringToUnicodeString_k( thread->StaticUnicodeString, &nameA, FALSE );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateMutexExW_k( sa, thread->StaticUnicodeString->Buffer, flags, access );
}


/***********************************************************************
 *           CreateMutexExW   (KERNEL32.@)
 */
HANDLE WINAPI CreateMutexExW_k( SECURITY_ATTRIBUTES *sa, LPCWSTR name, DWORD flags, DWORD access )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = OBJ_OPENIF | ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtCreateMutant_k( &ret, access, &attr, (flags & CREATE_MUTEX_INITIAL_OWNER) != 0 );
    if (status == STATUS_OBJECT_NAME_EXISTS)
        SetLastError_k( ERROR_ALREADY_EXISTS );
    else
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return ret;
}


/***********************************************************************
 *           OpenMutexA   (KERNEL32.@)
 */
HANDLE WINAPI OpenMutexA_k( DWORD access, BOOL inherit, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenMutexW_k( access, inherit, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenMutexW_k( access, inherit, buffer );
}


/***********************************************************************
 *           OpenMutexW   (KERNEL32.@)
 */
HANDLE WINAPI OpenMutexW_k( DWORD access, BOOL inherit, LPCWSTR name )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    if (!is_version_nt()) access = MUTEX_ALL_ACCESS;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = inherit ? OBJ_INHERIT : 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtOpenMutant_k( &ret, access, &attr );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return ret;
}


/***********************************************************************
 *           ReleaseMutex   (KERNEL32.@)
 */
BOOL WINAPI ReleaseMutex_k( HANDLE handle )
{
    NTSTATUS    status;

    status = NtReleaseMutant_k(handle, NULL);
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}


/*
 * Semaphores
 */


/***********************************************************************
 *           CreateSemaphoreA   (KERNEL32.@)
 */
HANDLE WINAPI CreateSemaphoreA_k( SECURITY_ATTRIBUTES *sa, LONG initial, LONG max, LPCSTR name )
{
    return CreateSemaphoreExA_k( sa, initial, max, name, 0, SEMAPHORE_ALL_ACCESS );
}


/***********************************************************************
 *           CreateSemaphoreW   (KERNEL32.@)
 */
HANDLE WINAPI CreateSemaphoreW_k( SECURITY_ATTRIBUTES *sa, LONG initial, LONG max, LPCWSTR name )
{
    return CreateSemaphoreExW_k( sa, initial, max, name, 0, SEMAPHORE_ALL_ACCESS );
}


/***********************************************************************
 *           CreateSemaphoreExA   (KERNEL32.@)
 */
HANDLE WINAPI CreateSemaphoreExA_k( SECURITY_ATTRIBUTES *sa, LONG initial, LONG max,
                                                    LPCSTR name, DWORD flags, DWORD access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateSemaphoreExW_k( sa, initial, max, NULL, flags, access );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateSemaphoreExW_k( sa, initial, max, buffer, flags, access );
}


/***********************************************************************
 *           CreateSemaphoreExW   (KERNEL32.@)
 */
HANDLE WINAPI CreateSemaphoreExW_k( SECURITY_ATTRIBUTES *sa, LONG initial, LONG max,
                                                    LPCWSTR name, DWORD flags, DWORD access )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = OBJ_OPENIF | ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtCreateSemaphore_k( &ret, access, &attr, initial, max );
    if (status == STATUS_OBJECT_NAME_EXISTS)
        SetLastError_k( ERROR_ALREADY_EXISTS );
    else
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return ret;
}


/***********************************************************************
 *           OpenSemaphoreA   (KERNEL32.@)
 */
HANDLE WINAPI OpenSemaphoreA_k( DWORD access, BOOL inherit, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenSemaphoreW_k( access, inherit, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenSemaphoreW_k( access, inherit, buffer );
}


/***********************************************************************
 *           OpenSemaphoreW   (KERNEL32.@)
 */
HANDLE WINAPI OpenSemaphoreW_k( DWORD access, BOOL inherit, LPCWSTR name )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    if (!is_version_nt()) access = SEMAPHORE_ALL_ACCESS;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = inherit ? OBJ_INHERIT : 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtOpenSemaphore_k( &ret, access, &attr );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return ret;
}


/***********************************************************************
 *           ReleaseSemaphore   (KERNEL32.@)
 */
BOOL WINAPI ReleaseSemaphore_k( HANDLE handle, LONG count, LONG *previous )
{
    NTSTATUS status = NtReleaseSemaphore_k( handle, count, (PULONG)previous );
    if (status) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}


/*
 * Jobs
 */

/******************************************************************************
 *		CreateJobObjectW (KERNEL32.@)
 */
HANDLE WINAPI CreateJobObjectW_k( LPSECURITY_ATTRIBUTES sa, LPCWSTR name )
{
    HANDLE ret = 0;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = OBJ_OPENIF | ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtCreateJobObject_k( &ret, JOB_OBJECT_ALL_ACCESS, &attr );
    if (status == STATUS_OBJECT_NAME_EXISTS)
        SetLastError_k( ERROR_ALREADY_EXISTS );
    else
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return ret;
}

/******************************************************************************
 *		CreateJobObjectA (KERNEL32.@)
 */
HANDLE WINAPI CreateJobObjectA_k( LPSECURITY_ATTRIBUTES attr, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateJobObjectW_k( attr, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateJobObjectW_k( attr, buffer );
}

/******************************************************************************
 *		OpenJobObjectW (KERNEL32.@)
 */
HANDLE WINAPI OpenJobObjectW_k( DWORD access, BOOL inherit, LPCWSTR name )
{
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = inherit ? OBJ_INHERIT : 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtOpenJobObject_k( &ret, access, &attr );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return ret;
}

/******************************************************************************
 *		OpenJobObjectA (KERNEL32.@)
 */
HANDLE WINAPI OpenJobObjectA_k( DWORD access, BOOL inherit, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenJobObjectW_k( access, inherit, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenJobObjectW_k( access, inherit, buffer );
}

/******************************************************************************
 *		TerminateJobObject (KERNEL32.@)
 */
BOOL WINAPI TerminateJobObject_k( HANDLE job, UINT exit_code )
{
    NTSTATUS status = NtTerminateJobObject_k( job, exit_code );
    if (status) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/******************************************************************************
 *		QueryInformationJobObject (KERNEL32.@)
 */
BOOL WINAPI QueryInformationJobObject_k( HANDLE job, JOBOBJECTINFOCLASS class, LPVOID info,
                                       DWORD len, DWORD *ret_len )
{
    NTSTATUS status = NtQueryInformationJobObject_k( job, class, info, len, ret_len );
    if (status) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/******************************************************************************
 *		SetInformationJobObject (KERNEL32.@)
 */
BOOL WINAPI SetInformationJobObject_k( HANDLE job, JOBOBJECTINFOCLASS class, LPVOID info, DWORD len )
{
    NTSTATUS status = NtSetInformationJobObject_k( job, class, info, len );
    if (status) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/******************************************************************************
 *		AssignProcessToJobObject (KERNEL32.@)
 */
BOOL WINAPI AssignProcessToJobObject_k( HANDLE job, HANDLE process )
{
    NTSTATUS status = NtAssignProcessToJobObject_k( job, process );
    if (status) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/******************************************************************************
 *		IsProcessInJob (KERNEL32.@)
 */
BOOL WINAPI IsProcessInJob_k( HANDLE process, HANDLE job, PBOOL result )
{
    NTSTATUS status = NtIsProcessInJob_k( job, process );
    switch(status)
    {
    case STATUS_PROCESS_IN_JOB:
        *result = TRUE;
        return TRUE;
    case STATUS_PROCESS_NOT_IN_JOB:
        *result = FALSE;
        return TRUE;
    default:
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
}


/*
 * Timers
 */


/***********************************************************************
 *           CreateWaitableTimerA    (KERNEL32.@)
 */
HANDLE WINAPI CreateWaitableTimerA_k( SECURITY_ATTRIBUTES *sa, BOOL manual, LPCSTR name )
{
    return CreateWaitableTimerExA_k( sa, name, manual ? CREATE_WAITABLE_TIMER_MANUAL_RESET : 0,
                                   TIMER_ALL_ACCESS );
}


/***********************************************************************
 *           CreateWaitableTimerW    (KERNEL32.@)
 */
HANDLE WINAPI CreateWaitableTimerW_k( SECURITY_ATTRIBUTES *sa, BOOL manual, LPCWSTR name )
{
    return CreateWaitableTimerExW_k( sa, name, manual ? CREATE_WAITABLE_TIMER_MANUAL_RESET : 0,
                                   TIMER_ALL_ACCESS );
}


/***********************************************************************
 *           CreateWaitableTimerExA    (KERNEL32.@)
 */
HANDLE WINAPI CreateWaitableTimerExA_k( SECURITY_ATTRIBUTES *sa, LPCSTR name, DWORD flags, DWORD access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateWaitableTimerExW_k( sa, NULL, flags, access );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return CreateWaitableTimerExW_k( sa, buffer, flags, access );
}


/***********************************************************************
 *           CreateWaitableTimerExW    (KERNEL32.@)
 */
HANDLE WINAPI CreateWaitableTimerExW_k( SECURITY_ATTRIBUTES *sa, LPCWSTR name, DWORD flags, DWORD access )
{
    HANDLE handle;
    NTSTATUS status;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = OBJ_OPENIF | ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtCreateTimer_k( &handle, access, &attr,
                 (flags & CREATE_WAITABLE_TIMER_MANUAL_RESET) ? NotificationTimer : SynchronizationTimer );
    if (status == STATUS_OBJECT_NAME_EXISTS)
        SetLastError_k( ERROR_ALREADY_EXISTS );
    else
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    return handle;
}


/***********************************************************************
 *           OpenWaitableTimerA    (KERNEL32.@)
 */
HANDLE WINAPI OpenWaitableTimerA_k( DWORD access, BOOL inherit, LPCSTR name )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return OpenWaitableTimerW_k( access, inherit, NULL );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return OpenWaitableTimerW_k( access, inherit, buffer );
}


/***********************************************************************
 *           OpenWaitableTimerW    (KERNEL32.@)
 */
HANDLE WINAPI OpenWaitableTimerW_k( DWORD access, BOOL inherit, LPCWSTR name )
{
    HANDLE handle;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    if (!is_version_nt()) access = TIMER_ALL_ACCESS;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = NULL;
    attr.Attributes               = inherit ? OBJ_INHERIT : 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    if (name)
    {
        RtlInitUnicodeString_k( &nameW, name );
        attr.ObjectName = &nameW;
        attr.RootDirectory = get_BaseNamedObjects_handle();
    }

    status = NtOpenTimer_k(&handle, access, &attr);
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return handle;
}


/***********************************************************************
 *           SetWaitableTimer    (KERNEL32.@)
 */
BOOL WINAPI SetWaitableTimer_k( HANDLE handle, const LARGE_INTEGER *when, LONG period,
                              PTIMERAPCROUTINE callback, LPVOID arg, BOOL resume )
{
    NTSTATUS status = NtSetTimer_k(handle, when, (PTIMER_APC_ROUTINE)callback,
                                 arg, resume, period, NULL);

    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        if (status != STATUS_TIMER_RESUME_IGNORED) return FALSE;
    }
    return TRUE;
}


/***********************************************************************
 *           CancelWaitableTimer    (KERNEL32.@)
 */
BOOL WINAPI CancelWaitableTimer_k( HANDLE handle )
{
    NTSTATUS status;

    status = NtCancelTimer_k(handle, NULL);
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}


/***********************************************************************
 *           CreateTimerQueue  (KERNEL32.@)
 */
HANDLE WINAPI CreateTimerQueue_k(void)
{
    HANDLE q;
    NTSTATUS status = RtlCreateTimerQueue_k(&q);

    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return NULL;
    }

    return q;
}


/***********************************************************************
 *           DeleteTimerQueueEx  (KERNEL32.@)
 */
BOOL WINAPI DeleteTimerQueueEx_k(HANDLE TimerQueue, HANDLE CompletionEvent)
{
    NTSTATUS status = RtlDeleteTimerQueueEx_k(TimerQueue, CompletionEvent);

    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
 *           DeleteTimerQueue  (KERNEL32.@)
 */
BOOL WINAPI DeleteTimerQueue_k(HANDLE TimerQueue)
{
    return DeleteTimerQueueEx_k(TimerQueue, NULL);
}

/***********************************************************************
 *           CreateTimerQueueTimer  (KERNEL32.@)
 *
 * Creates a timer-queue timer. This timer expires at the specified due
 * time (in ms), then after every specified period (in ms). When the timer
 * expires, the callback function is called.
 *
 * RETURNS
 *   nonzero on success or zero on failure
 */
BOOL WINAPI CreateTimerQueueTimer_k( PHANDLE phNewTimer, HANDLE TimerQueue,
                                   WAITORTIMERCALLBACK Callback, PVOID Parameter,
                                   DWORD DueTime, DWORD Period, ULONG Flags )
{
    NTSTATUS status = RtlCreateTimer_k(phNewTimer, TimerQueue, Callback,
                                     Parameter, DueTime, Period, Flags);

    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
 *           ChangeTimerQueueTimer  (KERNEL32.@)
 *
 * Changes the times at which the timer expires.
 *
 * RETURNS
 *   nonzero on success or zero on failure
 */
BOOL WINAPI ChangeTimerQueueTimer_k( HANDLE TimerQueue, HANDLE Timer,
                                   ULONG DueTime, ULONG Period )
{
    NTSTATUS status = RtlUpdateTimer_k(TimerQueue, Timer, DueTime, Period);

    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }

    return TRUE;
}

/***********************************************************************
 *           DeleteTimerQueueTimer  (KERNEL32.@)
 *
 * Cancels a timer-queue timer.
 *
 * RETURNS
 *   nonzero on success or zero on failure
 */
BOOL WINAPI DeleteTimerQueueTimer_k( HANDLE TimerQueue, HANDLE Timer,
                                   HANDLE CompletionEvent )
{
    NTSTATUS status = RtlDeleteTimer_k(TimerQueue, Timer, CompletionEvent);
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}


/*
 * Pipes
 */


/***********************************************************************
 *           CreateNamedPipeA   (KERNEL32.@)
 */
HANDLE WINAPI CreateNamedPipeA_k( LPCSTR name, DWORD dwOpenMode,
                                DWORD dwPipeMode, DWORD nMaxInstances,
                                DWORD nOutBufferSize, DWORD nInBufferSize,
                                DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES attr )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return CreateNamedPipeW_k( NULL, dwOpenMode, dwPipeMode, nMaxInstances,
                                        nOutBufferSize, nInBufferSize, nDefaultTimeOut, attr );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return INVALID_HANDLE_VALUE;
    }
    return CreateNamedPipeW_k( buffer, dwOpenMode, dwPipeMode, nMaxInstances,
                             nOutBufferSize, nInBufferSize, nDefaultTimeOut, attr );
}


/***********************************************************************
 *           CreateNamedPipeW   (KERNEL32.@)
 */
HANDLE WINAPI CreateNamedPipeW_k( LPCWSTR name, DWORD dwOpenMode,
                                DWORD dwPipeMode, DWORD nMaxInstances,
                                DWORD nOutBufferSize, DWORD nInBufferSize,
                                DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES sa )
{
    HANDLE handle;
    UNICODE_STRING nt_name;
    OBJECT_ATTRIBUTES attr;
    DWORD access, options, sharing;
    BOOLEAN pipe_type, read_mode, non_block;
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    LARGE_INTEGER timeout;

    TRACE("(%s, %#08x, %#08x, %d, %d, %d, %d, %p)\n",
          debugstr_w(name), dwOpenMode, dwPipeMode, nMaxInstances,
          nOutBufferSize, nInBufferSize, nDefaultTimeOut, sa );

    if (!RtlDosPathNameToNtPathName_U_k( name, &nt_name, NULL, NULL ))
    {
        SetLastError_k( ERROR_PATH_NOT_FOUND );
        return INVALID_HANDLE_VALUE;
    }
    if (nt_name.Length >= MAX_PATH * sizeof(WCHAR) )
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        RtlFreeUnicodeString_k( &nt_name );
        return INVALID_HANDLE_VALUE;
    }

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = &nt_name;
    attr.Attributes               = OBJ_CASE_INSENSITIVE |
                                    ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;

    switch(dwOpenMode & 3)
    {
    case PIPE_ACCESS_INBOUND:
        sharing = FILE_SHARE_WRITE;
        access  = GENERIC_READ;
        break;
    case PIPE_ACCESS_OUTBOUND:
        sharing = FILE_SHARE_READ;
        access  = GENERIC_WRITE;
        break;
    case PIPE_ACCESS_DUPLEX:
        sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
        access  = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return INVALID_HANDLE_VALUE;
    }
    access |= SYNCHRONIZE;
    options = 0;
    if (dwOpenMode & FILE_FLAG_WRITE_THROUGH) options |= FILE_WRITE_THROUGH;
    if (!(dwOpenMode & FILE_FLAG_OVERLAPPED)) options |= FILE_SYNCHRONOUS_IO_NONALERT;
    pipe_type = (dwPipeMode & PIPE_TYPE_MESSAGE) != 0;
    read_mode = (dwPipeMode & PIPE_READMODE_MESSAGE) != 0;
    non_block = (dwPipeMode & PIPE_NOWAIT) != 0;
    if (nMaxInstances >= PIPE_UNLIMITED_INSTANCES) nMaxInstances = ~0U;

    timeout.QuadPart = (ULONGLONG)nDefaultTimeOut * -10000;

    SetLastError_k(0);

    status = NtCreateNamedPipeFile_k(&handle, access, &attr, &iosb, sharing,
                                   FILE_OVERWRITE_IF, options, pipe_type,
                                   read_mode, non_block, nMaxInstances,
                                   nInBufferSize, nOutBufferSize, &timeout);

    RtlFreeUnicodeString_k( &nt_name );
    if (status)
    {
        handle = INVALID_HANDLE_VALUE;
        SetLastError_k( RtlNtStatusToDosError_k(status) );
    }
    return handle;
}


/***********************************************************************
 *           PeekNamedPipe   (KERNEL32.@)
 */
BOOL WINAPI PeekNamedPipe_k( HANDLE hPipe, LPVOID lpvBuffer, DWORD cbBuffer,
                           LPDWORD lpcbRead, LPDWORD lpcbAvail, LPDWORD lpcbMessage )
{
    FILE_PIPE_PEEK_BUFFER local_buffer;
    FILE_PIPE_PEEK_BUFFER *buffer = &local_buffer;
    IO_STATUS_BLOCK io;
    NTSTATUS status;

    if (cbBuffer && !(buffer = HeapAlloc_k( GetProcessHeap_k(), 0,
                                          FIELD_OFFSET( FILE_PIPE_PEEK_BUFFER, Data[cbBuffer] ))))
    {
        SetLastError_k( ERROR_NOT_ENOUGH_MEMORY );
        return FALSE;
    }

    status = NtFsControlFile_k( hPipe, 0, NULL, NULL, &io, FSCTL_PIPE_PEEK, NULL, 0,
                              buffer, FIELD_OFFSET( FILE_PIPE_PEEK_BUFFER, Data[cbBuffer] ) );
    if (!status)
    {
        ULONG read_size = io.Information - FIELD_OFFSET( FILE_PIPE_PEEK_BUFFER, Data );
        if (lpcbAvail) *lpcbAvail = buffer->ReadDataAvailable;
        if (lpcbRead) *lpcbRead = read_size;
        if (lpcbMessage) *lpcbMessage = 0;  /* FIXME */
        if (lpvBuffer) memcpy( lpvBuffer, buffer->Data, read_size );
    }
    else SetLastError_k( RtlNtStatusToDosError_k(status) );

    if (buffer != &local_buffer) HeapFree_k( GetProcessHeap_k(), 0, buffer );
    return !status;
}

/***********************************************************************
 *           WaitNamedPipeA   (KERNEL32.@)
 */
BOOL WINAPI WaitNamedPipeA_k (LPCSTR name, DWORD nTimeOut)
{
    WCHAR buffer[MAX_PATH];

    if (!name) return WaitNamedPipeW_k( NULL, nTimeOut );

    if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return WaitNamedPipeW_k( buffer, nTimeOut );
}


/***********************************************************************
 *           WaitNamedPipeW   (KERNEL32.@)
 *
 *  Waits for a named pipe instance to become available
 *
 *  PARAMS
 *   name     [I] Pointer to a named pipe name to wait for
 *   nTimeOut [I] How long to wait in ms
 *
 *  RETURNS
 *   TRUE: Success, named pipe can be opened with CreateFile
 *   FALSE: Failure, GetLastError can be called for further details
 */
BOOL WINAPI WaitNamedPipeW_k(LPCWSTR name, DWORD nTimeOut)
{
    static const WCHAR leadin[] = {'\\','?','?','\\','P','I','P','E','\\'};
    NTSTATUS status;
    UNICODE_STRING nt_name, pipe_dev_name;
    FILE_PIPE_WAIT_FOR_BUFFER *pipe_wait;
    IO_STATUS_BLOCK iosb;
    OBJECT_ATTRIBUTES attr;
    ULONG sz_pipe_wait;
    HANDLE pipe_dev;

    TRACE("%s 0x%08x\n",debugstr_w(name),nTimeOut);

    if (!RtlDosPathNameToNtPathName_U_k( name, &nt_name, NULL, NULL ))
        return FALSE;

    if (nt_name.Length >= MAX_PATH * sizeof(WCHAR) ||
        nt_name.Length < sizeof(leadin) ||
        strncmpiW( nt_name.Buffer, leadin, sizeof(leadin)/sizeof(WCHAR)) != 0)
    {
        RtlFreeUnicodeString_k( &nt_name );
        SetLastError_k( ERROR_PATH_NOT_FOUND );
        return FALSE;
    }

    sz_pipe_wait = sizeof(*pipe_wait) + nt_name.Length - sizeof(leadin) - sizeof(WCHAR);
    if (!(pipe_wait = HeapAlloc_k( GetProcessHeap_k(), 0,  sz_pipe_wait)))
    {
        RtlFreeUnicodeString_k( &nt_name );
        SetLastError_k( ERROR_OUTOFMEMORY );
        return FALSE;
    }

    pipe_dev_name.Buffer = nt_name.Buffer;
    pipe_dev_name.Length = sizeof(leadin);
    pipe_dev_name.MaximumLength = sizeof(leadin);
    InitializeObjectAttributes_k(&attr,&pipe_dev_name, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile_k( &pipe_dev, FILE_READ_ATTRIBUTES, &attr,
                         &iosb, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         FILE_SYNCHRONOUS_IO_NONALERT);
    if (status != ERROR_SUCCESS)
    {
        HeapFree_k( GetProcessHeap_k(), 0, pipe_wait);
        RtlFreeUnicodeString_k( &nt_name );
        SetLastError_k( ERROR_PATH_NOT_FOUND );
        return FALSE;
    }

    pipe_wait->TimeoutSpecified = !(nTimeOut == NMPWAIT_USE_DEFAULT_WAIT);
    if (nTimeOut == NMPWAIT_WAIT_FOREVER)
        pipe_wait->Timeout.QuadPart = ((ULONGLONG)0x7fffffff << 32) | 0xffffffff;
    else
        pipe_wait->Timeout.QuadPart = (ULONGLONG)nTimeOut * -10000;
    pipe_wait->NameLength = nt_name.Length - sizeof(leadin);
    memcpy(pipe_wait->Name, nt_name.Buffer + sizeof(leadin)/sizeof(WCHAR),
           pipe_wait->NameLength);
    RtlFreeUnicodeString_k( &nt_name );

    status = NtFsControlFile_k( pipe_dev, NULL, NULL, NULL, &iosb, FSCTL_PIPE_WAIT,
                              pipe_wait, sz_pipe_wait, NULL, 0 );

    HeapFree_k( GetProcessHeap_k(), 0, pipe_wait );
    NtClose_k( pipe_dev );

    if(status != STATUS_SUCCESS)
    {
        SetLastError_k(RtlNtStatusToDosError_k(status));
        return FALSE;
    }
    else
        return TRUE;
}


/***********************************************************************
 *           ConnectNamedPipe   (KERNEL32.@)
 *
 *  Connects to a named pipe
 *
 *  Parameters
 *  hPipe: A handle to a named pipe returned by CreateNamedPipe
 *  overlapped: Optional OVERLAPPED struct
 *
 *  Return values
 *  TRUE: Success
 *  FALSE: Failure, GetLastError can be called for further details
 */
BOOL WINAPI ConnectNamedPipe_k(HANDLE hPipe, LPOVERLAPPED overlapped)
{
    NTSTATUS status;
    IO_STATUS_BLOCK status_block;
    LPVOID   cvalue = NULL;

    TRACE("(%p,%p)\n", hPipe, overlapped);

    if(overlapped)
    {
        overlapped->Internal = STATUS_PENDING;
        overlapped->InternalHigh = 0;
        if (((ULONG_PTR)overlapped->hEvent & 1) == 0) cvalue = overlapped;
    }

    status = NtFsControlFile_k(hPipe, overlapped ? overlapped->hEvent : NULL, NULL, cvalue,
                             overlapped ? (IO_STATUS_BLOCK *)overlapped : &status_block,
                             FSCTL_PIPE_LISTEN, NULL, 0, NULL, 0);

    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError_k( RtlNtStatusToDosError_k(status) );
    return FALSE;
}

/***********************************************************************
 *           DisconnectNamedPipe   (KERNEL32.@)
 *
 *  Disconnects from a named pipe
 *
 *  Parameters
 *  hPipe: A handle to a named pipe returned by CreateNamedPipe
 *
 *  Return values
 *  TRUE: Success
 *  FALSE: Failure, GetLastError can be called for further details
 */
BOOL WINAPI DisconnectNamedPipe_k(HANDLE hPipe)
{
    NTSTATUS status;
    IO_STATUS_BLOCK io_block;

    TRACE("(%p)\n",hPipe);

    status = NtFsControlFile_k(hPipe, 0, NULL, NULL, &io_block, FSCTL_PIPE_DISCONNECT,
                             NULL, 0, NULL, 0);
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError_k( RtlNtStatusToDosError_k(status) );
    return FALSE;
}

/***********************************************************************
 *           TransactNamedPipe   (KERNEL32.@)
 *
 * BUGS
 *  should be done as a single operation in the wineserver or kernel
 */
BOOL WINAPI TransactNamedPipe_k(
    HANDLE handle, LPVOID write_buf, DWORD write_size, LPVOID read_buf,
    DWORD read_size, LPDWORD bytes_read, LPOVERLAPPED overlapped)
{
    BOOL r;
    DWORD count;

    TRACE("%p %p %d %p %d %p %p\n",
          handle, write_buf, write_size, read_buf,
          read_size, bytes_read, overlapped);

    if (overlapped)
    {
        FIXME("Doesn't support overlapped operation as yet\n");
        return FALSE;
    }

    r = WriteFile_k(handle, write_buf, write_size, &count, NULL);
    if (r)
        r = ReadFile_k(handle, read_buf, read_size, bytes_read, NULL);

    return r;
}

/***********************************************************************
 *           GetNamedPipeInfo   (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeInfo_k(
    HANDLE hNamedPipe, LPDWORD lpFlags, LPDWORD lpOutputBufferSize,
    LPDWORD lpInputBufferSize, LPDWORD lpMaxInstances)
{
    FILE_PIPE_LOCAL_INFORMATION fpli;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    status = NtQueryInformationFile_k(hNamedPipe, &iosb, &fpli, sizeof(fpli),
                                    FilePipeLocalInformation);
    if (status)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }

    if (lpFlags)
    {
        *lpFlags = (fpli.NamedPipeEnd & FILE_PIPE_SERVER_END) ?
            PIPE_SERVER_END : PIPE_CLIENT_END;
        *lpFlags |= (fpli.NamedPipeType & FILE_PIPE_TYPE_MESSAGE) ?
            PIPE_TYPE_MESSAGE : PIPE_TYPE_BYTE;
    }

    if (lpOutputBufferSize) *lpOutputBufferSize = fpli.OutboundQuota;
    if (lpInputBufferSize) *lpInputBufferSize = fpli.InboundQuota;
    if (lpMaxInstances) *lpMaxInstances = fpli.MaximumInstances;

    return TRUE;
}

/***********************************************************************
 *           GetNamedPipeHandleStateA  (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeHandleStateA_k(
    HANDLE hNamedPipe, LPDWORD lpState, LPDWORD lpCurInstances,
    LPDWORD lpMaxCollectionCount, LPDWORD lpCollectDataTimeout,
    LPSTR lpUsername, DWORD nUsernameMaxSize)
{
    FIXME("%p %p %p %p %p %p %d\n",
          hNamedPipe, lpState, lpCurInstances,
          lpMaxCollectionCount, lpCollectDataTimeout,
          lpUsername, nUsernameMaxSize);

    return FALSE;
}

/***********************************************************************
 *           GetNamedPipeHandleStateW  (KERNEL32.@)
 */
BOOL WINAPI GetNamedPipeHandleStateW_k(
    HANDLE hNamedPipe, LPDWORD lpState, LPDWORD lpCurInstances,
    LPDWORD lpMaxCollectionCount, LPDWORD lpCollectDataTimeout,
    LPWSTR lpUsername, DWORD nUsernameMaxSize)
{
    FIXME("%p %p %p %p %p %p %d\n",
          hNamedPipe, lpState, lpCurInstances,
          lpMaxCollectionCount, lpCollectDataTimeout,
          lpUsername, nUsernameMaxSize);

    return FALSE;
}

/***********************************************************************
 *           SetNamedPipeHandleState  (KERNEL32.@)
 */
BOOL WINAPI SetNamedPipeHandleState_k(
    HANDLE hNamedPipe, LPDWORD lpMode, LPDWORD lpMaxCollectionCount,
    LPDWORD lpCollectDataTimeout)
{
    /* should be a fixme, but this function is called a lot by the RPC
     * runtime, and it slows down InstallShield a fair bit. */
    WARN("stub: %p %p/%d %p %p\n",
          hNamedPipe, lpMode, lpMode ? *lpMode : 0, lpMaxCollectionCount, lpCollectDataTimeout);
    return FALSE;
}

/***********************************************************************
 *           CallNamedPipeA  (KERNEL32.@)
 */
BOOL WINAPI CallNamedPipeA_k(
    LPCSTR lpNamedPipeName, LPVOID lpInput, DWORD dwInputSize,
    LPVOID lpOutput, DWORD dwOutputSize,
    LPDWORD lpBytesRead, DWORD nTimeout)
{
    DWORD len;
    LPWSTR str = NULL;
    BOOL ret;

    TRACE("%s %p %d %p %d %p %d\n",
           debugstr_a(lpNamedPipeName), lpInput, dwInputSize,
           lpOutput, dwOutputSize, lpBytesRead, nTimeout);

    if( lpNamedPipeName )
    {
        len = MultiByteToWideChar_k( CP_ACP, 0, lpNamedPipeName, -1, NULL, 0 );
        str = HeapAlloc_k( GetProcessHeap_k(), 0, len*sizeof(WCHAR) );
        MultiByteToWideChar_k( CP_ACP, 0, lpNamedPipeName, -1, str, len );
    }
    ret = CallNamedPipeW_k( str, lpInput, dwInputSize, lpOutput,
                          dwOutputSize, lpBytesRead, nTimeout );
    if( lpNamedPipeName )
        HeapFree_k( GetProcessHeap_k(), 0, str );

    return ret;
}

/***********************************************************************
 *           CallNamedPipeW  (KERNEL32.@)
 */
BOOL WINAPI CallNamedPipeW_k(
    LPCWSTR lpNamedPipeName, LPVOID lpInput, DWORD lpInputSize,
    LPVOID lpOutput, DWORD lpOutputSize,
    LPDWORD lpBytesRead, DWORD nTimeout)
{
    HANDLE pipe;
    BOOL ret;
    DWORD mode;

    TRACE("%s %p %d %p %d %p %d\n",
          debugstr_w(lpNamedPipeName), lpInput, lpInputSize,
          lpOutput, lpOutputSize, lpBytesRead, nTimeout);

    pipe = CreateFileW_k(lpNamedPipeName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        ret = WaitNamedPipeW_k(lpNamedPipeName, nTimeout);
        if (!ret)
            return FALSE;
        pipe = CreateFileW_k(lpNamedPipeName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (pipe == INVALID_HANDLE_VALUE)
            return FALSE;
    }

    mode = PIPE_READMODE_MESSAGE;
    ret = SetNamedPipeHandleState_k(pipe, &mode, NULL, NULL);

    /* Currently SetNamedPipeHandleState() is a stub returning FALSE */
    if (ret) FIXME("Now that SetNamedPipeHandleState() is more than a stub, please update CallNamedPipeW\n");
    /*
    if (!ret)
    {
        CloseHandle(pipe);
        return FALSE;
    }*/

    ret = TransactNamedPipe_k(pipe, lpInput, lpInputSize, lpOutput, lpOutputSize, lpBytesRead, NULL);
    CloseHandle_k(pipe);
    if (!ret)
        return FALSE;

    return TRUE;
}

/******************************************************************
 *		CreatePipe (KERNEL32.@)
 *
 */
BOOL WINAPI CreatePipe_k( PHANDLE hReadPipe, PHANDLE hWritePipe,
                        LPSECURITY_ATTRIBUTES sa, DWORD size )
{
    static unsigned     index /* = 0 */;
    WCHAR               name[64];
    HANDLE              hr, hw;
    unsigned            in_index = index;
    UNICODE_STRING      nt_name;
    OBJECT_ATTRIBUTES   attr;
    NTSTATUS            status;
    IO_STATUS_BLOCK     iosb;
    LARGE_INTEGER       timeout;

    *hReadPipe = *hWritePipe = INVALID_HANDLE_VALUE;

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = &nt_name;
    attr.Attributes               = OBJ_CASE_INSENSITIVE |
                                    ((sa && sa->bInheritHandle) ? OBJ_INHERIT : 0);
    attr.SecurityDescriptor       = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;

    timeout.QuadPart = (ULONGLONG)NMPWAIT_USE_DEFAULT_WAIT * -10000;
    /* generate a unique pipe name (system wide) */
    do
    {
        static const WCHAR nameFmt[] = { '\\','?','?','\\','p','i','p','e',
         '\\','W','i','n','3','2','.','P','i','p','e','s','.','%','0','8','l',
         'u','.','%','0','8','u','\0' };

        snprintfW(name, sizeof(name) / sizeof(name[0]), nameFmt,
                  GetCurrentProcessId_k(), ++index);
        RtlInitUnicodeString_k(&nt_name, name);
        status = NtCreateNamedPipeFile_k(&hr, GENERIC_READ | SYNCHRONIZE, &attr, &iosb,
                                       FILE_SHARE_WRITE, FILE_OVERWRITE_IF,
                                       FILE_SYNCHRONOUS_IO_NONALERT,
                                       FALSE, FALSE, FALSE, 
                                       1, size, size, &timeout);
        if (status)
        {
            SetLastError_k( RtlNtStatusToDosError_k(status) );
            hr = INVALID_HANDLE_VALUE;
        }
    } while (hr == INVALID_HANDLE_VALUE && index != in_index);
    /* from completion sakeness, I think system resources might be exhausted before this happens !! */
    if (hr == INVALID_HANDLE_VALUE) return FALSE;

    status = NtOpenFile_k(&hw, GENERIC_WRITE | SYNCHRONIZE, &attr, &iosb, 0,
                        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE);

    if (status) 
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        NtClose_k(hr);
        return FALSE;
    }

    *hReadPipe = hr;
    *hWritePipe = hw;
    return TRUE;
}


/******************************************************************************
 * CreateMailslotA [KERNEL32.@]
 *
 * See CreateMailslotW.
 */
HANDLE WINAPI CreateMailslotA_k( LPCSTR lpName, DWORD nMaxMessageSize,
                               DWORD lReadTimeout, LPSECURITY_ATTRIBUTES sa )
{
    DWORD len;
    HANDLE handle;
    LPWSTR name = NULL;

    TRACE("%s %d %d %p\n", debugstr_a(lpName),
          nMaxMessageSize, lReadTimeout, sa);

    if( lpName )
    {
        len = MultiByteToWideChar_k( CP_ACP, 0, lpName, -1, NULL, 0 );
        name = HeapAlloc_k( GetProcessHeap_k(), 0, len*sizeof(WCHAR) );
        MultiByteToWideChar_k( CP_ACP, 0, lpName, -1, name, len );
    }

    handle = CreateMailslotW_k( name, nMaxMessageSize, lReadTimeout, sa );

    HeapFree_k( GetProcessHeap_k(), 0, name );

    return handle;
}


/******************************************************************************
 * CreateMailslotW [KERNEL32.@]
 *
 * Create a mailslot with specified name.
 *
 * PARAMS
 *    lpName          [I] Pointer to string for mailslot name
 *    nMaxMessageSize [I] Maximum message size
 *    lReadTimeout    [I] Milliseconds before read time-out
 *    sa              [I] Pointer to security structure
 *
 * RETURNS
 *    Success: Handle to mailslot
 *    Failure: INVALID_HANDLE_VALUE
 */
HANDLE WINAPI CreateMailslotW_k( LPCWSTR lpName, DWORD nMaxMessageSize,
                               DWORD lReadTimeout, LPSECURITY_ATTRIBUTES sa )
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    OBJECT_ATTRIBUTES attr;
    UNICODE_STRING nameW;
    LARGE_INTEGER timeout;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    TRACE("%s %d %d %p\n", debugstr_w(lpName),
          nMaxMessageSize, lReadTimeout, sa);

    if (!RtlDosPathNameToNtPathName_U_k( lpName, &nameW, NULL, NULL ))
    {
        SetLastError_k( ERROR_PATH_NOT_FOUND );
        return INVALID_HANDLE_VALUE;
    }

    if (nameW.Length >= MAX_PATH * sizeof(WCHAR) )
    {
        SetLastError_k( ERROR_FILENAME_EXCED_RANGE );
        RtlFreeUnicodeString_k( &nameW );
        return INVALID_HANDLE_VALUE;
    }

    attr.Length = sizeof(attr);
    attr.RootDirectory = 0;
    attr.Attributes = OBJ_CASE_INSENSITIVE;
    attr.ObjectName = &nameW;
    attr.SecurityDescriptor = sa ? sa->lpSecurityDescriptor : NULL;
    attr.SecurityQualityOfService = NULL;

    if (lReadTimeout != MAILSLOT_WAIT_FOREVER)
        timeout.QuadPart = (ULONGLONG) lReadTimeout * -10000;
    else
        timeout.QuadPart = ((LONGLONG)0x7fffffff << 32) | 0xffffffff;

    status = NtCreateMailslotFile_k( &handle, GENERIC_READ | SYNCHRONIZE, &attr,
                                   &iosb, 0, 0, nMaxMessageSize, &timeout );
    if (status)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        handle = INVALID_HANDLE_VALUE;
    }

    RtlFreeUnicodeString_k( &nameW );
    return handle;
}


/******************************************************************************
 * GetMailslotInfo [KERNEL32.@]
 *
 * Retrieve information about a mailslot.
 *
 * PARAMS
 *    hMailslot        [I] Mailslot handle
 *    lpMaxMessageSize [O] Address of maximum message size
 *    lpNextSize       [O] Address of size of next message
 *    lpMessageCount   [O] Address of number of messages
 *    lpReadTimeout    [O] Address of read time-out
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
BOOL WINAPI GetMailslotInfo_k( HANDLE hMailslot, LPDWORD lpMaxMessageSize,
                               LPDWORD lpNextSize, LPDWORD lpMessageCount,
                               LPDWORD lpReadTimeout )
{
    FILE_MAILSLOT_QUERY_INFORMATION info;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    TRACE("%p %p %p %p %p\n",hMailslot, lpMaxMessageSize,
          lpNextSize, lpMessageCount, lpReadTimeout);

    status = NtQueryInformationFile_k( hMailslot, &iosb, &info, sizeof info,
                                     FileMailslotQueryInformation );

    if( status != STATUS_SUCCESS )
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }

    if( lpMaxMessageSize )
        *lpMaxMessageSize = info.MaximumMessageSize;
    if( lpNextSize )
        *lpNextSize = info.NextMessageSize;
    if( lpMessageCount )
        *lpMessageCount = info.MessagesAvailable;
    if( lpReadTimeout )
    {
        if (info.ReadTimeout.QuadPart == (((LONGLONG)0x7fffffff << 32) | 0xffffffff))
            *lpReadTimeout = MAILSLOT_WAIT_FOREVER;
        else
            *lpReadTimeout = (DWORD)(info.ReadTimeout.QuadPart / -10000);
    }
    return TRUE;
}


/******************************************************************************
 * SetMailslotInfo [KERNEL32.@]
 *
 * Set the read timeout of a mailslot.
 *
 * PARAMS
 *  hMailslot     [I] Mailslot handle
 *  dwReadTimeout [I] Timeout in milliseconds.
 *
 * RETURNS
 *    Success: TRUE
 *    Failure: FALSE
 */
BOOL WINAPI SetMailslotInfo_k( HANDLE hMailslot, DWORD dwReadTimeout)
{
    FILE_MAILSLOT_SET_INFORMATION info;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;

    TRACE("%p %d\n", hMailslot, dwReadTimeout);

    if (dwReadTimeout != MAILSLOT_WAIT_FOREVER)
        info.ReadTimeout.QuadPart = (ULONGLONG)dwReadTimeout * -10000;
    else
        info.ReadTimeout.QuadPart = ((LONGLONG)0x7fffffff << 32) | 0xffffffff;
    status = NtSetInformationFile_k( hMailslot, &iosb, &info, sizeof info,
                                   FileMailslotSetInformation );
    if( status != STATUS_SUCCESS )
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return FALSE;
    }
    return TRUE;
}


/******************************************************************************
 *		CreateIoCompletionPort (KERNEL32.@)
 */
HANDLE WINAPI CreateIoCompletionPort_k(HANDLE hFileHandle, HANDLE hExistingCompletionPort,
                                     ULONG_PTR CompletionKey, DWORD dwNumberOfConcurrentThreads)
{
    NTSTATUS status;
    HANDLE ret = 0;

    TRACE("(%p, %p, %08lx, %08x)\n",
          hFileHandle, hExistingCompletionPort, CompletionKey, dwNumberOfConcurrentThreads);

    if (hExistingCompletionPort && hFileHandle == INVALID_HANDLE_VALUE)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (hExistingCompletionPort)
        ret = hExistingCompletionPort;
    else
    {
        status = NtCreateIoCompletion_k( &ret, IO_COMPLETION_ALL_ACCESS, NULL, dwNumberOfConcurrentThreads );
        if (status != STATUS_SUCCESS) goto fail;
    }

    if (hFileHandle != INVALID_HANDLE_VALUE)
    {
        FILE_COMPLETION_INFORMATION info;
        IO_STATUS_BLOCK iosb;

        info.CompletionPort = ret;
        info.CompletionKey = CompletionKey;
        status = NtSetInformationFile_k( hFileHandle, &iosb, &info, sizeof(info), FileCompletionInformation );
        if (status != STATUS_SUCCESS) goto fail;
    }

    return ret;

fail:
    if (ret && !hExistingCompletionPort)
        CloseHandle_k( ret );
    SetLastError_k( RtlNtStatusToDosError_k(status) );
    return 0;
}

/******************************************************************************
 *		GetQueuedCompletionStatus (KERNEL32.@)
 */
BOOL WINAPI GetQueuedCompletionStatus_k( HANDLE CompletionPort, LPDWORD lpNumberOfBytesTransferred,
                                       PULONG_PTR pCompletionKey, LPOVERLAPPED *lpOverlapped,
                                       DWORD dwMilliseconds )
{
    NTSTATUS status;
    IO_STATUS_BLOCK iosb;
    LARGE_INTEGER wait_time;

    TRACE("(%p,%p,%p,%p,%d)\n",
          CompletionPort,lpNumberOfBytesTransferred,pCompletionKey,lpOverlapped,dwMilliseconds);

    *lpOverlapped = NULL;

    status = NtRemoveIoCompletion_k( CompletionPort, pCompletionKey, (PULONG_PTR)lpOverlapped,
                                   &iosb, get_nt_timeout( &wait_time, dwMilliseconds ) );
    if (status == STATUS_SUCCESS)
    {
        *lpNumberOfBytesTransferred = iosb.Information;
        if (iosb.Status >= 0) return TRUE;
        SetLastError_k( RtlNtStatusToDosError_k(iosb.Status) );
        return FALSE;
    }

    if (status == STATUS_TIMEOUT) SetLastError_k( WAIT_TIMEOUT );
    else SetLastError_k( RtlNtStatusToDosError_k(status) );
    return FALSE;
}


/******************************************************************************
 *		PostQueuedCompletionStatus (KERNEL32.@)
 */
BOOL WINAPI PostQueuedCompletionStatus_k( HANDLE CompletionPort, DWORD dwNumberOfBytes,
                                        ULONG_PTR dwCompletionKey, LPOVERLAPPED lpOverlapped)
{
    NTSTATUS status;

    TRACE("%p %d %08lx %p\n", CompletionPort, dwNumberOfBytes, dwCompletionKey, lpOverlapped );

    status = NtSetIoCompletion_k( CompletionPort, dwCompletionKey, (ULONG_PTR)lpOverlapped,
                                STATUS_SUCCESS, dwNumberOfBytes );

    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError_k( RtlNtStatusToDosError_k(status) );
    return FALSE;
}

/******************************************************************************
 *		BindIoCompletionCallback (KERNEL32.@)
 */
BOOL WINAPI BindIoCompletionCallback_k( HANDLE FileHandle, LPOVERLAPPED_COMPLETION_ROUTINE Function, ULONG Flags)
{
    NTSTATUS status;

    TRACE("(%p, %p, %d)\n", FileHandle, Function, Flags);

    status = RtlSetIoCompletionCallback_k( FileHandle, (PRTL_OVERLAPPED_COMPLETION_ROUTINE)Function, Flags );
    if (status == STATUS_SUCCESS) return TRUE;
    SetLastError_k( RtlNtStatusToDosError_k(status) );
    return FALSE;
}


/***********************************************************************
 *           CreateMemoryResourceNotification   (KERNEL32.@)
 */
HANDLE WINAPI CreateMemoryResourceNotification_k(MEMORY_RESOURCE_NOTIFICATION_TYPE type)
{
    static const WCHAR lowmemW[] =
        {'\\','K','e','r','n','e','l','O','b','j','e','c','t','s',
         '\\','L','o','w','M','e','m','o','r','y','C','o','n','d','i','t','i','o','n',0};
    static const WCHAR highmemW[] =
        {'\\','K','e','r','n','e','l','O','b','j','e','c','t','s',
         '\\','H','i','g','h','M','e','m','o','r','y','C','o','n','d','i','t','i','o','n',0};
    HANDLE ret;
    UNICODE_STRING nameW;
    OBJECT_ATTRIBUTES attr;
    NTSTATUS status;

    switch (type)
    {
    case LowMemoryResourceNotification:
        RtlInitUnicodeString_k( &nameW, lowmemW );
        break;
    case HighMemoryResourceNotification:
        RtlInitUnicodeString_k( &nameW, highmemW );
        break;
    default:
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }

    attr.Length                   = sizeof(attr);
    attr.RootDirectory            = 0;
    attr.ObjectName               = &nameW;
    attr.Attributes               = 0;
    attr.SecurityDescriptor       = NULL;
    attr.SecurityQualityOfService = NULL;
    status = NtOpenEvent_k( &ret, EVENT_ALL_ACCESS, &attr );
    if (status != STATUS_SUCCESS)
    {
        SetLastError_k( RtlNtStatusToDosError_k(status) );
        return 0;
    }
    return ret;
}

/***********************************************************************
 *          QueryMemoryResourceNotification   (KERNEL32.@)
 */
BOOL WINAPI QueryMemoryResourceNotification_k(HANDLE handle, PBOOL state)
{
    switch (WaitForSingleObject_k( handle, 0 ))
    {
    case WAIT_OBJECT_0:
        *state = TRUE;
        return TRUE;
    case WAIT_TIMEOUT:
        *state = FALSE;
        return TRUE;
    }
    SetLastError_k( ERROR_INVALID_PARAMETER );
    return FALSE;
}

/***********************************************************************
 *           InitOnceBeginInitialize    (KERNEL32.@)
 */
BOOL WINAPI InitOnceBeginInitialize_k( INIT_ONCE *once, DWORD flags, BOOL *pending, void **context )
{
    NTSTATUS status = RtlRunOnceBeginInitialize_k( once, flags, context );
    if (status >= 0) *pending = (status == STATUS_PENDING);
    else SetLastError_k( RtlNtStatusToDosError_k(status) );
    return status >= 0;
}

/***********************************************************************
 *           InitOnceComplete    (KERNEL32.@)
 */
BOOL WINAPI InitOnceComplete_k( INIT_ONCE *once, DWORD flags, void *context )
{
    NTSTATUS status = RtlRunOnceComplete_k( once, flags, context );
    if (status != STATUS_SUCCESS) SetLastError_k( RtlNtStatusToDosError_k(status) );
    return !status;
}

/***********************************************************************
 *           InitOnceExecuteOnce    (KERNEL32.@)
 */
BOOL WINAPI InitOnceExecuteOnce_k( INIT_ONCE *once, PINIT_ONCE_FN func, void *param, void **context )
{
    return !RtlRunOnceExecuteOnce_k( once, (PRTL_RUN_ONCE_INIT_FN)func, param, context );
}

#ifdef __i386__

/***********************************************************************
 *		InterlockedCompareExchange (KERNEL32.@)
 */
/* LONG WINAPI InterlockedCompareExchange( PLONG dest, LONG xchg, LONG compare ); */
__ASM_STDCALL_FUNC(InterlockedCompareExchange, 12,
                  "movl 12(%esp),%eax\n\t"
                  "movl 8(%esp),%ecx\n\t"
                  "movl 4(%esp),%edx\n\t"
                  "lock; cmpxchgl %ecx,(%edx)\n\t"
                  "ret $12")

/***********************************************************************
 *		InterlockedExchange (KERNEL32.@)
 */
/* LONG WINAPI InterlockedExchange( PLONG dest, LONG val ); */
__ASM_STDCALL_FUNC(InterlockedExchange, 8,
                  "movl 8(%esp),%eax\n\t"
                  "movl 4(%esp),%edx\n\t"
                  "lock; xchgl %eax,(%edx)\n\t"
                  "ret $8")

/***********************************************************************
 *		InterlockedExchangeAdd (KERNEL32.@)
 */
/* LONG WINAPI InterlockedExchangeAdd( PLONG dest, LONG incr ); */
__ASM_STDCALL_FUNC(InterlockedExchangeAdd, 8,
                  "movl 8(%esp),%eax\n\t"
                  "movl 4(%esp),%edx\n\t"
                  "lock; xaddl %eax,(%edx)\n\t"
                  "ret $8")

/***********************************************************************
 *		InterlockedIncrement (KERNEL32.@)
 */
/* LONG WINAPI InterlockedIncrement( PLONG dest ); */
__ASM_STDCALL_FUNC(InterlockedIncrement, 4,
                  "movl 4(%esp),%edx\n\t"
                  "movl $1,%eax\n\t"
                  "lock; xaddl %eax,(%edx)\n\t"
                  "incl %eax\n\t"
                  "ret $4")

/***********************************************************************
 *		InterlockedDecrement (KERNEL32.@)
 */
__ASM_STDCALL_FUNC(InterlockedDecrement, 4,
                  "movl 4(%esp),%edx\n\t"
                  "movl $-1,%eax\n\t"
                  "lock; xaddl %eax,(%edx)\n\t"
                  "decl %eax\n\t"
                  "ret $4")

#endif  /* __i386__ */
