/*
 * Win32 critical sections
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

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif
#include <time.h>
#include "../include/ntstatus.h"
#include "../include/winbase.h"
#include "kprocess.h"
#include <SDL.h>

static __inline LONG interlocked_inc( PLONG dest )
{
    return interlocked_xchg_add( dest, 1 ) + 1;
}

static __inline LONG interlocked_dec( PLONG dest )
{
    return interlocked_xchg_add( dest, -1 ) - 1;
}

/***********************************************************************
 *           get_semaphore
 */
static __inline HANDLE get_semaphore( RTL_CRITICAL_SECTION *crit )
{
    HANDLE ret = crit->LockSemaphore;
    if (!ret)
    {
        HANDLE sem;
        if (NtCreateSemaphore_k( &sem, SEMAPHORE_ALL_ACCESS, NULL, 0, 1 )) return 0;
        if (!(ret =  interlocked_cmpxchg_ptr( &crit->LockSemaphore, sem, 0 )))
            ret = sem;
        else
            NtClose_k(sem);  /* somebody beat us to it */
    }
    return ret;
}

/***********************************************************************
 *           wait_semaphore
 */
static __inline NTSTATUS wait_semaphore( RTL_CRITICAL_SECTION *crit, int timeout )
{
    NTSTATUS ret;

    /* debug info is cleared by MakeCriticalSectionGlobal */
    if (!crit->DebugInfo)
    {
        HANDLE sem = get_semaphore( crit );
        struct KProcess* process = currentProcess();
        if (SDL_SemWaitTimeout((SDL_sem*)process->handles[(int)sem].semaphore, timeout*1000)==SDL_MUTEX_TIMEDOUT) {
            ret = STATUS_TIMEOUT;
        }
        ret = STATUS_WAIT_0;
    }
    return ret;
}

/***********************************************************************
 *           RtlInitializeCriticalSection   (NTDLL.@)
 *
 * Initialises a new critical section.
 *
 * PARAMS
 *  crit [O] Critical section to initialise
 *
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSectionAndSpinCount(), RtlDeleteCriticalSection(),
 *  RtlEnterCriticalSection(), RtlLeaveCriticalSection(),
 *  RtlTryEnterCriticalSection(), RtlSetCriticalSectionSpinCount()
 */
NTSTATUS WINAPI RtlInitializeCriticalSection_k( RTL_CRITICAL_SECTION *crit )
{
    return RtlInitializeCriticalSectionEx_k( crit, 0, 0 );
}

/***********************************************************************
 *           RtlInitializeCriticalSectionAndSpinCount   (NTDLL.@)
 *
 * Initialises a new critical section with a given spin count.
 *
 * PARAMS
 *   crit      [O] Critical section to initialise
 *   spincount [I] Spin count for crit
 * 
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * NOTES
 *  Available on NT4 SP3 or later.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlDeleteCriticalSection(),
 *  RtlEnterCriticalSection(), RtlLeaveCriticalSection(),
 *  RtlTryEnterCriticalSection(), RtlSetCriticalSectionSpinCount()
 */
NTSTATUS WINAPI RtlInitializeCriticalSectionAndSpinCount( RTL_CRITICAL_SECTION *crit, ULONG spincount )
{
    return RtlInitializeCriticalSectionEx_k( crit, spincount, 0 );
}

/***********************************************************************
 *           RtlInitializeCriticalSectionEx   (NTDLL.@)
 *
 * Initialises a new critical section with a given spin count and flags.
 *
 * PARAMS
 *   crit      [O] Critical section to initialise.
 *   spincount [I] Number of times to spin upon contention.
 *   flags     [I] RTL_CRITICAL_SECTION_FLAG_ flags from winnt.h.
 *
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * NOTES
 *  Available on Vista or later.
 *
 * SEE
 *  RtlInitializeCriticalSection(), RtlDeleteCriticalSection(),
 *  RtlEnterCriticalSection(), RtlLeaveCriticalSection(),
 *  RtlTryEnterCriticalSection(), RtlSetCriticalSectionSpinCount()
 */
NTSTATUS WINAPI RtlInitializeCriticalSectionEx_k( RTL_CRITICAL_SECTION *crit, ULONG spincount, ULONG flags )
{
    if (flags & (RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN|RTL_CRITICAL_SECTION_FLAG_STATIC_INIT))
        FIXME("(%p,%u,0x%08x) semi-stub\n", crit, spincount, flags);

    /* FIXME: if RTL_CRITICAL_SECTION_FLAG_STATIC_INIT is given, we should use
     * memory from a static pool to hold the debug info. Then heap.c could pass
     * this flag rather than initialising the process heap CS by hand. If this
     * is done, then debug info should be managed through Rtlp[Allocate|Free]DebugInfo
     * so (e.g.) MakeCriticalSectionGlobal() doesn't free it using HeapFree().
     */
    if (flags & RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO)
        crit->DebugInfo = NULL;
    else
        crit->DebugInfo = HeapAlloc(GetProcessHeap(), 0, sizeof(RTL_CRITICAL_SECTION_DEBUG));

    if (crit->DebugInfo)
    {
        crit->DebugInfo->Type = 0;
        crit->DebugInfo->CreatorBackTraceIndex = 0;
        crit->DebugInfo->CriticalSection = crit;
        crit->DebugInfo->ProcessLocksList.Blink = &(crit->DebugInfo->ProcessLocksList);
        crit->DebugInfo->ProcessLocksList.Flink = &(crit->DebugInfo->ProcessLocksList);
        crit->DebugInfo->EntryCount = 0;
        crit->DebugInfo->ContentionCount = 0;
        memset( crit->DebugInfo->Spare, 0, sizeof(crit->DebugInfo->Spare) );
    }
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;
    crit->LockSemaphore  = 0;
    /*if (NtCurrentTeb_k()->Peb->NumberOfProcessors <= 1)*/ spincount = 0;
    crit->SpinCount = spincount & ~0x80000000;
    return STATUS_SUCCESS;
}

/***********************************************************************
 *           RtlSetCriticalSectionSpinCount   (NTDLL.@)
 *
 * Sets the spin count of a critical section.
 *
 * PARAMS
 *   crit      [I/O] Critical section
 *   spincount [I] Spin count for crit
 *
 * RETURNS
 *  The previous spin count.
 *
 * NOTES
 *  If the system is not SMP, spincount is ignored and set to 0.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlLeaveCriticalSection(), RtlTryEnterCriticalSection()
 */
ULONG WINAPI RtlSetCriticalSectionSpinCount( RTL_CRITICAL_SECTION *crit, ULONG spincount )
{
    ULONG oldspincount = crit->SpinCount;
    /*if (NtCurrentTeb_k()->Peb->NumberOfProcessors <= 1)*/ spincount = 0;
    crit->SpinCount = spincount;
    return oldspincount;
}

/***********************************************************************
 *           RtlDeleteCriticalSection   (NTDLL.@)
 *
 * Frees the resources used by a critical section.
 *
 * PARAMS
 *  crit [I/O] Critical section to free
 *
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlLeaveCriticalSection(), RtlTryEnterCriticalSection()
 */
NTSTATUS WINAPI RtlDeleteCriticalSection( RTL_CRITICAL_SECTION *crit )
{
    crit->LockCount      = -1;
    crit->RecursionCount = 0;
    crit->OwningThread   = 0;
    if (crit->DebugInfo)
    {
        /* only free the ones we made in here */
        if (!crit->DebugInfo->Spare[0])
        {
            HeapFree( GetProcessHeap(), 0, crit->DebugInfo );
            crit->DebugInfo = NULL;
        }
        NtClose_k( crit->LockSemaphore );
    }
    else NtClose_k( crit->LockSemaphore );
    crit->LockSemaphore = 0;
    return STATUS_SUCCESS;
}


/***********************************************************************
 *           RtlpWaitForCriticalSection   (NTDLL.@)
 *
 * Waits for a busy critical section to become free.
 * 
 * PARAMS
 *  crit [I/O] Critical section to wait for
 *
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * NOTES
 *  Use RtlEnterCriticalSection() instead of this function as it is often much
 *  faster.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlLeaveCriticalSection(), RtlTryEnterCriticalSection()
 */
NTSTATUS WINAPI RtlpWaitForCriticalSection( RTL_CRITICAL_SECTION *crit )
{
    for (;;)
    {
        NTSTATUS status = wait_semaphore( crit, 5 );

        if ( status == STATUS_TIMEOUT )
        {
            const char *name = NULL;
            if (crit->DebugInfo) name = (char *)crit->DebugInfo->Spare[0];
            if (!name) name = "?";
            ERR( "section %p %s wait timed out in thread %04x, blocked by %04x, retrying (60 sec)\n", crit, debugstr_a(name), GetCurrentThreadId_k(), HandleToULong(crit->OwningThread) );
            status = wait_semaphore( crit, 60 );
            if ( status == STATUS_TIMEOUT)
            {
                ERR( "section %p %s wait timed out in thread %04x, blocked by %04x, retrying (5 min)\n", crit, debugstr_a(name), GetCurrentThreadId_k(), HandleToULong(crit->OwningThread) );
                status = wait_semaphore( crit, 300 );
            }
        }
        if (status == STATUS_WAIT_0) break;
    }
    if (crit->DebugInfo) crit->DebugInfo->ContentionCount++;
    return STATUS_SUCCESS;
}


/***********************************************************************
 *           RtlpUnWaitCriticalSection   (NTDLL.@)
 *
 * Notifies other threads waiting on the busy critical section that it has
 * become free.
 * 
 * PARAMS
 *  crit [I/O] Critical section
 *
 * RETURNS
 *  Success: STATUS_SUCCESS.
 *  Failure: Any error returned by NtReleaseSemaphore()
 *
 * NOTES
 *  Use RtlLeaveCriticalSection() instead of this function as it is often much
 *  faster.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlLeaveCriticalSection(), RtlTryEnterCriticalSection()
 */
NTSTATUS WINAPI RtlpUnWaitCriticalSection_k( RTL_CRITICAL_SECTION *crit )
{
    NTSTATUS ret;

    /* debug info is cleared by MakeCriticalSectionGlobal */
    if (!crit->DebugInfo)
    {
        HANDLE sem = get_semaphore( crit );
        ret = NtReleaseSemaphore_k( sem, 1, NULL );
    }
    if (ret) RtlRaiseStatus_k( ret );
    return ret;
}


/***********************************************************************
 *           RtlEnterCriticalSection   (NTDLL.@)
 *
 * Enters a critical section, waiting for it to become available if necessary.
 *
 * PARAMS
 *  crit [I/O] Critical section to enter
 *
 * RETURNS
 *  STATUS_SUCCESS. The critical section is held by the caller.
 *  
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlSetCriticalSectionSpinCount(),
 *  RtlLeaveCriticalSection(), RtlTryEnterCriticalSection()
 */
NTSTATUS WINAPI RtlEnterCriticalSection_k( RTL_CRITICAL_SECTION *crit )
{
    if (crit->SpinCount)
    {
        ULONG count;

        if (RtlTryEnterCriticalSection_k( crit )) return STATUS_SUCCESS;
        for (count = crit->SpinCount; count > 0; count--)
        {
            if (crit->LockCount > 0) break;  /* more than one waiter, don't bother spinning */
            if (crit->LockCount == -1)       /* try again */
            {
                if (interlocked_cmpxchg( &crit->LockCount, 0, -1 ) == -1) goto done;
            }
            SDL_Delay(0);
        }
    }

    if (interlocked_inc( &crit->LockCount ))
    {
        if (crit->OwningThread == ULongToHandle(GetCurrentThreadId_k()))
        {
            crit->RecursionCount++;
            return STATUS_SUCCESS;
        }

        /* Now wait for it */
        RtlpWaitForCriticalSection( crit );
    }
done:
    crit->OwningThread   = ULongToHandle(GetCurrentThreadId_k());
    crit->RecursionCount = 1;
    return STATUS_SUCCESS;
}


/***********************************************************************
 *           RtlTryEnterCriticalSection   (NTDLL.@)
 *
 * Tries to enter a critical section without waiting.
 *
 * PARAMS
 *  crit [I/O] Critical section to enter
 *
 * RETURNS
 *  Success: TRUE. The critical section is held by the caller.
 *  Failure: FALSE. The critical section is currently held by another thread.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlLeaveCriticalSection(), RtlSetCriticalSectionSpinCount()
 */
BOOL WINAPI RtlTryEnterCriticalSection_k( RTL_CRITICAL_SECTION *crit )
{
    BOOL ret = FALSE;
    if (interlocked_cmpxchg( &crit->LockCount, 0, -1 ) == -1)
    {
        crit->OwningThread   = ULongToHandle(GetCurrentThreadId_k());
        crit->RecursionCount = 1;
        ret = TRUE;
    }
    else if (crit->OwningThread == ULongToHandle(GetCurrentThreadId_k()))
    {
        interlocked_inc( &crit->LockCount );
        crit->RecursionCount++;
        ret = TRUE;
    }
    return ret;
}


/***********************************************************************
 *           RtlLeaveCriticalSection   (NTDLL.@)
 *
 * Leaves a critical section.
 *
 * PARAMS
 *  crit [I/O] Critical section to leave.
 *
 * RETURNS
 *  STATUS_SUCCESS.
 *
 * SEE
 *  RtlInitializeCriticalSectionEx(),
 *  RtlInitializeCriticalSection(), RtlInitializeCriticalSectionAndSpinCount(),
 *  RtlDeleteCriticalSection(), RtlEnterCriticalSection(),
 *  RtlSetCriticalSectionSpinCount(), RtlTryEnterCriticalSection()
 */
NTSTATUS WINAPI RtlLeaveCriticalSection_k( RTL_CRITICAL_SECTION *crit )
{
    if (--crit->RecursionCount) interlocked_dec( &crit->LockCount );
    else
    {
        crit->OwningThread = 0;
        if (interlocked_dec( &crit->LockCount ) >= 0)
        {
            /* someone is waiting */
            RtlpUnWaitCriticalSection_k( crit );
        }
    }
    return STATUS_SUCCESS;
}
