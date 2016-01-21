#include "../include/winbase.h"
#include <SDL.h>
#include "kprocess.h"
#include "kmodule.h"

DWORD WINAPI GetTickCount_k(void) {
    return SDL_GetTicks();
}

DWORD WINAPI GetModuleFileNameA_k(HMODULE hModule, LPSTR lpFileName, DWORD size) {    
    struct KModule* module;
    struct KProcess* process = currentProcess();
    DWORD len;

    if (!hModule)
        module = process->mainModule;
    else {
        if ((int)hModule>=MAX_HANDLES || process->handles[(int)hModule].type != HANDLE_MODULE) {
            SetLastError_k(ERROR_INVALID_HANDLE);
            return 0;
        }
        module = process->handles[(int)hModule].module;
    }
    len = strlen(module->localPath)+1;
    if (len>size) {
        SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        len=size;
    } else {
        SetLastError_k(0);
    }
    strncpy(lpFileName, module->localPath, len);
    lpFileName[len-1]=0;
    return len;
}

DWORD WINAPI GetModuleFileNameW_k( HMODULE hModule, LPWSTR lpFileName, DWORD size ) {
    struct KModule* module;
    struct KProcess* process = currentProcess();
    DWORD len;
    DWORD result;

    if (!hModule)
        module = process->mainModule;
    else {
        if ((int)hModule>=MAX_HANDLES || process->handles[(int)hModule].type != HANDLE_MODULE) {
            SetLastError_k(ERROR_INVALID_HANDLE);
            return 0;
        }
        module = process->handles[(int)hModule].module;
    }
    len = strlen(module->localPath)+1;
    if (len>size) {
        SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
        len=size;
    } else {
        SetLastError_k(0);
    }
    result = MultiByteToWideChar_k(GetACP_k(), 0, module->localPath, len, lpFileName, size);
    if (result<len) {
        SetLastError_k(ERROR_INSUFFICIENT_BUFFER);
    } else {
        SetLastError_k(0);
    }
    return result;
}

/***********************************************************************
 *           MulDiv   (KERNEL32.@)
 * RETURNS
 *	Result of multiplication and division
 *	-1: Overflow occurred or Divisor was 0
 */
INT WINAPI MulDiv_k( INT nMultiplicand, INT nMultiplier, INT nDivisor)
{
    LONGLONG ret;

    if (!nDivisor) return -1;

    /* We want to deal with a positive divisor to simplify the logic. */
    if (nDivisor < 0)
    {
      nMultiplicand = - nMultiplicand;
      nDivisor = -nDivisor;
    }

    /* If the result is positive, we "add" to round. else, we subtract to round. */
    if ( ( (nMultiplicand <  0) && (nMultiplier <  0) ) ||
         ( (nMultiplicand >= 0) && (nMultiplier >= 0) ) )
      ret = (((LONGLONG)nMultiplicand * nMultiplier) + (nDivisor/2)) / nDivisor;
    else
      ret = (((LONGLONG)nMultiplicand * nMultiplier) - (nDivisor/2)) / nDivisor;

    if ((ret > 2147483647) || (ret < -2147483647)) return -1;
    return (INT)ret;
}

/***********************************************************************
 * 			GetSystemInfo            	[KERNEL32.@]
 *
 * Get information about the system.
 *
 * RETURNS
 *  Nothing.
 */
VOID WINAPI GetSystemInfo_k(LPSYSTEM_INFO si)
{
    si->wProcessorArchitecture  = 0; // PROCESSOR_ARCHITECTURE_INTEL
    si->wReserved               = 0;
    si->dwPageSize                  = 4096;
    si->lpMinimumApplicationAddress = (LPVOID)0x1000;
    si->lpMaximumApplicationAddress = (LPVOID)0x7FFFFFFF;
    si->dwActiveProcessorMask       = 0x01;
    si->dwNumberOfProcessors        = 1;
    si->dwProcessorType             = 586;
    si->dwAllocationGranularity     = 4096;
    si->wProcessorLevel             = 5;
    si->wProcessorRevision          = 0;
}