#include "cpu.h"
#include "kthread.h"
#include "kprocess.h"

#include <Windows.h>

#define LOG_KERNEL if (0) printf

void SetLastError_k(U32 error);
void FindClose_cpu(struct CPU* cpu) {
    U32 hFindFile = pop32(cpu);
    EAX = FindClose((HANDLE)hFindFile);
    if (!EAX) {
        SetLastError_k(GetLastError());
    }
}

void getFullNativePath(const char* currentDirectory, const char* localFile, char* nativeFile);

void FindFirstFileA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    U32 lpFindFileData = pop32(cpu);
    WIN32_FIND_DATA data;
    HANDLE hFind;
    char path[MAX_FILEPATH_LEN];

    EAX = 0;
    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpFileName), path);

    hFind = FindFirstFile(path, &data);

    if (hFind == INVALID_HANDLE_VALUE) {
        EAX =  (U32)hFind;
        SetLastError_k(GetLastError());
    } else {
        writed(lpFindFileData, data.dwFileAttributes); lpFindFileData+=4;
        writed(lpFindFileData, data.ftCreationTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftCreationTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastAccessTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastAccessTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastWriteTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastWriteTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.nFileSizeHigh); lpFindFileData+=4;
        writed(lpFindFileData, data.nFileSizeLow); lpFindFileData+=4;
        writed(lpFindFileData, data.dwReserved0); lpFindFileData+=4;
        writed(lpFindFileData, data.dwReserved1); lpFindFileData+=4;
        writeBufferToMemory(lpFindFileData, data.cFileName, MAX_PATH); lpFindFileData+=MAX_PATH;
        writeBufferToMemory(lpFindFileData, data.cAlternateFileName, 14); lpFindFileData+=14;
        EAX = (U32)hFind;
    }
    LOG_KERNEL("FindFirstFileA(%s, %X)=%d\n", getStringFromMemory(lpFileName), lpFindFileData, EAX);
}

void FindNextFileA_cpu(struct CPU* cpu) {
    U32 hFindFile = pop32(cpu);
    U32 lpFindFileData = pop32(cpu);
    U32 originalData = lpFindFileData;
    WIN32_FIND_DATA data;

    data.dwFileAttributes = readd(lpFindFileData); lpFindFileData+=4;
    data.ftCreationTime.dwLowDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.ftCreationTime.dwHighDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.ftLastAccessTime.dwLowDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.ftLastAccessTime.dwHighDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.ftLastWriteTime.dwLowDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.ftLastWriteTime.dwHighDateTime = readd(lpFindFileData); lpFindFileData+=4;
    data.nFileSizeHigh = readd(lpFindFileData); lpFindFileData+=4;
    data.nFileSizeLow = readd(lpFindFileData); lpFindFileData+=4;
    data.dwReserved0 = readd(lpFindFileData); lpFindFileData+=4;
    data.dwReserved1 = readd(lpFindFileData); lpFindFileData+=4;
    readBufferFromMemory(lpFindFileData, data.cFileName, MAX_PATH); lpFindFileData+=MAX_PATH;
    readBufferFromMemory(lpFindFileData, data.cAlternateFileName, 14); lpFindFileData+=14;

    EAX = (U32)FindNextFile((HANDLE)hFindFile, &data);
    if (!EAX) {
        SetLastError_k(GetLastError());
    } else {
        lpFindFileData = originalData;

        writed(lpFindFileData, data.dwFileAttributes); lpFindFileData+=4;
        writed(lpFindFileData, data.ftCreationTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftCreationTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastAccessTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastAccessTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastWriteTime.dwLowDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.ftLastWriteTime.dwHighDateTime); lpFindFileData+=4;
        writed(lpFindFileData, data.nFileSizeHigh); lpFindFileData+=4;
        writed(lpFindFileData, data.nFileSizeLow); lpFindFileData+=4;
        writed(lpFindFileData, data.dwReserved0); lpFindFileData+=4;
        writed(lpFindFileData, data.dwReserved1); lpFindFileData+=4;
        writeBufferToMemory(lpFindFileData, data.cFileName, MAX_PATH); lpFindFileData+=MAX_PATH;
        writeBufferToMemory(lpFindFileData, data.cAlternateFileName, 14); lpFindFileData+=14;
    }
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