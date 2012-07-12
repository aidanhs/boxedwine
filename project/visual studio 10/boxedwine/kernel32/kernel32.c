#include "config.h"
#include "wine/port.h"

#include <stdlib.h>
#include "windef.h"
#include "winbase.h"
#include "winternl.h"

char* __wine_spec_file_name="KERNEL32.DLL";
IMAGE_NT_HEADERS __wine_spec_nt_header;

void setImageNtHeaders(void* p) {
	memcpy(&__wine_spec_nt_header, p, sizeof(IMAGE_NT_HEADERS));
}

void _fini() {
}

//__ASM_STDCALL_FUNC( SetLastError, 4,
//                   "movl 4(%esp),%eax\n\t"
//                   ".byte 0x64\n\t"
//                   "movl %eax,0x34\n\t"
//                   "ret $4" )
void WINAPI SetLastError( DWORD error ) /* [in] Per-thread error code */
{
    NtCurrentTeb()->LastErrorValue = error;
}


//__ASM_STDCALL_FUNC( GetLastError, 0, ".byte 0x64\n\tmovl 0x34,%eax\n\tret" )
DWORD WINAPI GetLastError(void)
{
    return NtCurrentTeb()->LastErrorValue;
}

//__ASM_STDCALL_FUNC( GetCurrentProcessId, 0, ".byte 0x64\n\tmovl 0x20,%eax\n\tret" )
DWORD WINAPI GetCurrentProcessId(void)
{
    return HandleToULong(NtCurrentTeb()->ClientId.UniqueProcess);
}

//__ASM_STDCALL_FUNC( GetCurrentThreadId, 0, ".byte 0x64\n\tmovl 0x24,%eax\n\tret" )
DWORD WINAPI GetCurrentThreadId(void)
{
    return HandleToULong(NtCurrentTeb()->ClientId.UniqueThread);
}

//__ASM_STDCALL_FUNC( GetProcessHeap, 0, ".byte 0x64\n\tmovl 0x30,%eax\n\tmovl 0x18(%eax),%eax\n\tret");
HANDLE WINAPI GetProcessHeap(void)
{
    return NtCurrentTeb()->Peb->ProcessHeap;
}

//__ASM_STDCALL_FUNC(InterlockedCompareExchange, 12,
//                  "movl 12(%esp),%eax\n\t"
//                  "movl 8(%esp),%ecx\n\t"
//                  "movl 4(%esp),%edx\n\t"
//                  "lock; cmpxchgl %ecx,(%edx)\n\t"
//                  "ret $12")
__declspec(naked) LONG WINAPI InterlockedCompareExchange(PLONG dest, LONG xchg, LONG compare)
{
    __asm mov eax, 12[esp];
    __asm mov ecx, 8[esp];
    __asm mov edx, 4[esp];
    __asm lock cmpxchg [edx], ecx;
    __asm ret;
}

//__ASM_STDCALL_FUNC(InterlockedCompareExchange, 12,
//                  "movl 12(%esp),%eax\n\t"
//                  "movl 8(%esp),%ecx\n\t"
//                  "movl 4(%esp),%edx\n\t"
//                  "lock; cmpxchgl %ecx,(%edx)\n\t"
//                  "ret $12")
__declspec(naked) LONG WINAPI InterlockedExchange( PLONG dest, LONG xchg)
{
	__asm mov eax, 8[esp];
    __asm mov edx, 4[esp];
    __asm lock xchg [edx], eax;
    __asm ret;
}

void WINAPI ExitProcess( DWORD status )
{
    LdrShutdownProcess();
    NtTerminateProcess(GetCurrentProcess(), status);
    exit(status);
}

DWORD call_process_entry( PEB *peb, LPTHREAD_START_ROUTINE entry )
{
    return entry( peb );
}

WINBASEAPI PVOID WINAPI _imp__DecodePointer(PVOID p)
{
	return RtlDecodePointer(p);
}

WINBASEAPI PVOID WINAPI _imp__EncodePointer(PVOID p)
{
	return RtlEncodePointer(p);
}

LONG WINAPI InterlockedIncrement( LONG volatile *dest )
{
    return InterlockedExchangeAdd( dest, 1 ) + 1;
}

LONG WINAPI InterlockedDecrement( LONG volatile *dest )
{
    return InterlockedExchangeAdd( dest, -1 ) - 1;
}

LONG WINAPI InterlockedExchangeAdd( LONG volatile *dest, LONG incr )
{
    extern int interlocked_xchg_add( int *dest, int incr );
    return interlocked_xchg_add( (int *)dest, incr );
}

void WINAPI DebugBreak(void)
{
    DbgBreakPoint();
}