#include "cpu.h"
#include "kthread.h"
#include "kprocess.h"

#include <Windows.h>

#define LOG_KERNEL if (0) printf

void SetLastError_k(U32 error);
BOOL FindClose_k( HANDLE handle ) {
    BOOL result = FindClose((HANDLE)handle);
    if (!result) {
        SetLastError_k(GetLastError());
    }
    return result;
}

void getFullNativePath(const char* currentDirectory, const char* localFile, char* nativeFile);

HANDLE WINAPI FindFirstFileA_k( LPCSTR lpFileName, WIN32_FIND_DATAA *lpFindData ) {
    HANDLE hFind;
    char path[MAX_FILEPATH_LEN];

    getFullNativePath(currentThread()->process->currentDirectory, lpFileName, path);

    hFind = FindFirstFileA(path, lpFindData);
    if (hFind == INVALID_HANDLE_VALUE)
        SetLastError_k(GetLastError());
    return hFind;
}

HANDLE WINAPI FindFirstFileW_k( LPCWSTR lpFileName, WIN32_FIND_DATAW *lpFindData ) {
}

BOOL WINAPI FindNextFileA_k( HANDLE handle, WIN32_FIND_DATAA *data ) {
    BOOL result = FindNextFileA(handle, data);
    if (!result) {
        SetLastError_k(GetLastError());
    }
    return result;
}

BOOL WINAPI FindNextFileW_k( HANDLE handle, WIN32_FIND_DATAW *data ) {
}

typedef struct timeval_internal {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval_internal * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}