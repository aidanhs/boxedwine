#include "kmodule.h"
#include "kthread.h"
#include "kalloc.h"
#include "kprocess.h"
#include "kmutex.h"
#include "loader.h"
#include "pathutils.h"

#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include UNISTD

#include "../include/winerror.h"
#include "../include/winbase.h"
#include "../include/unicode.h"
#include "../include/ntstatus.h"

extern PblMap* processMap;

#define LOG_KERNEL if (0) printf

/*
Failed to import CreateEventA from kernel32.dll
Failed to import CreateThread from kernel32.dll
Failed to import ExitThread from kernel32.dll
Failed to import GetCurrentProcessId from kernel32.dll
Failed to import GetCurrentThreadId from kernel32.dll
Failed to import GetCurrentThread from kernel32.dll
Failed to import GetModuleHandleA from kernel32.dll
Failed to import SetEvent from kernel32.dll
Failed to import TlsAlloc from kernel32.dll
Failed to import TlsFree from kernel32.dll
Failed to import TlsGetValue from kernel32.dll
Failed to import TlsSetValue from kernel32.dll
*/

struct KProcess* currentProcess() {
    return currentThread()->process;
}

DWORD WINAPI GetCurrentThreadId_k(void) {
    return currentThread()->id;
}

DWORD NtCurrentTeb_k() {
    return currentThread()->cpu.segAddress[FS];
}

DWORD WINAPI GetLastError_k() {
    return readd(NtCurrentTeb_k()+TEB_LAST_ERROR);
}

void SetLastError_k(DWORD error) {
    writed(NtCurrentTeb_k()+TEB_LAST_ERROR, error);
}

void setError(struct CPU* cpu, U32 e) {
    writed(cpu->segAddress[FS]+TEB_LAST_ERROR, e);
}

U64 unixToWinTime(time_t seconds) {
    return seconds * 10000000l + 11644473600l;
}

void CloseHandle_cpu(struct CPU* cpu) {
    U32 handle = pop32(cpu);

    LOG_KERNEL("CloseHandle(%d)\n", handle);
    if (!freeHandle(cpu->thread->process, handle)) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
    } else {
        setError(cpu, 0);
        EAX = TRUE;
    }
}

void CompareStringA_cpu(struct CPU* cpu) {
    U32 Locale = pop32(cpu);
    U32 dwCmpFlags = pop32(cpu);
    U32 lpString1 = pop32(cpu);
    S32 cchCount1 = (S32)pop32(cpu);
    U32 lpString2 = pop32(cpu);
    S32 cchCount2 = (S32)pop32(cpu);
    
    EAX = CompareStringA_k(Locale, dwCmpFlags, getStringFromMemory(lpString1), cchCount1, getStringFromMemory(lpString2), cchCount2);
    LOG_KERNEL("CompareStringA(Locale=%d, dwCmpFlags=%X, lpString1=%s, cchCount1=%d, lpString2=%s, cchCount2=%d)=%d\n", Locale, dwCmpFlags, getStringFromMemory(lpString1), cchCount1, getStringFromMemory(lpString2), cchCount2, EAX);
}

void CompareStringW_cpu(struct CPU* cpu) {
    U32 Locale = pop32(cpu);
    U32 dwCmpFlags = pop32(cpu);
    U32 lpString1 = pop32(cpu);
    S32 cchCount1 = (S32)pop32(cpu);
    U32 lpString2 = pop32(cpu);
    S32 cchCount2 = (S32)pop32(cpu);
    U16* src1;
    U16* src2;
    U32 len1 = stringLengthW(lpString1)+1;
    U32 len2 = stringLengthW(lpString2)+1;

    src1 = kalloc(len1*2);
    src2 = kalloc(len2*2);
    readArray16FromMemory(cchCount1, src1, cchCount1);
    readArray16FromMemory(cchCount2, src2, cchCount2);
    EAX = CompareStringW_k(Locale, dwCmpFlags, src1, cchCount1, src2, cchCount2);
    LOG_KERNEL("CompareStringW(Locale=%d, dwCmpFlags=%X, lpString1=%S, cchCount1=%d, lpString2=%S, cchCount2=%d)=%d\n", Locale, dwCmpFlags, src1, cchCount1, src2, cchCount2, EAX);
}

void CreateDirectoryA_cpu(struct CPU* cpu) {
    U32 lpPathName = pop32(cpu);
    U32 lpSecurityAttributes = pop32(cpu);
    char path[MAX_FILEPATH_LEN];

    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpPathName), path);
    if (mkdir(path)==0)
        EAX = TRUE;
    else {
        if (errno==EEXIST)
            setError(cpu, ERROR_ALREADY_EXISTS);
        else
            setError(cpu, ERROR_PATH_NOT_FOUND);
        EAX = FALSE;
    }
    LOG_KERNEL("CreateDirectoryA(%s, %X)=%d\n", lpPathName, lpSecurityAttributes);
}

void CreateFileA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    U32 dwDesiredAccess = pop32(cpu);
    U32 dwShareMode = pop32(cpu);
    U32 lpSecurityAttributes = pop32(cpu);
    U32 dwCreationDisposition = pop32(cpu);
    U32 dwFlagsAndAttributes = pop32(cpu);
    U32 hTemplateFile = pop32(cpu);
    char nativePath[MAX_FILEPATH_LEN];
    struct stat st;
    U32 openFlags = O_BINARY;
    int f;

    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpFileName), nativePath);
    
    if ((dwDesiredAccess & GENERIC_READ) && (dwDesiredAccess & GENERIC_WRITE)) {
        openFlags |= O_RDWR;
    } else if (dwDesiredAccess & GENERIC_WRITE) {
        openFlags |= O_WRONLY;
    } else {
        openFlags |= O_RDONLY;
    }
    switch (dwCreationDisposition) {
        case 1: // CREATE_NEW
            if (stat(nativePath, &st)==0) {
                setError(cpu, ERROR_FILE_EXISTS);
                EAX = (U32)INVALID_HANDLE_VALUE;
                break;
            }
            openFlags |= O_CREAT | O_TRUNC;
            break;
        case 2: // CREATE_ALWAYS
            if (stat(nativePath, &st)==0) {
                setError(cpu, ERROR_ALREADY_EXISTS);
                unlink(nativePath);
            } else {
                setError(cpu, 0);
            }
            openFlags |= O_CREAT | O_TRUNC;
            break;
        case 3: // OPEN_EXISTING
            if (stat(nativePath, &st)!=0) {
                setError(cpu, ERROR_FILE_NOT_FOUND);
                EAX = (U32)INVALID_HANDLE_VALUE;
                break;
            }
            if (S_ISDIR(st.st_mode)) {
                EAX = allocHandle(cpu->thread->process, HANDLE_DIRECTORY);

                cpu->thread->process->handles[EAX].nativeDirectoryPath = kalloc(strlen(nativePath)+1);
                strcpy(cpu->thread->process->handles[EAX].nativeDirectoryPath, nativePath);
                return;
            }
            break;
        case 4: // OPEN_ALWAYS
            if (stat(nativePath, &st)==0) {
                setError(cpu, ERROR_ALREADY_EXISTS);
            } else {
                setError(cpu, 0);
            }
            openFlags = O_CREAT;
            break;
        case 5: // TRUNCATE_EXISTING
            if (stat(nativePath, &st)!=0) {
                setError(cpu, ERROR_FILE_NOT_FOUND);
                EAX = (U32)INVALID_HANDLE_VALUE;
                break;
            }
            openFlags = O_TRUNC;
            break;
    }
    if (EAX != (U32)INVALID_HANDLE_VALUE) {
        f = open(nativePath, openFlags, S_IREAD | _S_IWRITE);
        if (f<0) {
            setError(cpu, ERROR_ACCESS_DENIED);
            EAX = (U32)INVALID_HANDLE_VALUE;
        } else {
            EAX = allocHandle(cpu->thread->process, HANDLE_FILE);
            cpu->thread->process->handles[EAX].data = f;
        }
    }
    LOG_KERNEL("CreateFileA(%s, %X, %X, %X, %X, %X, %X)=%d\n", getStringFromMemory(lpFileName), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile, EAX);
}

void CreateMutexA_cpu(struct CPU* cpu) {
    U32 lpMutexAttributes = pop32(cpu);
    U32 bInitialOwner = pop32(cpu);
    U32 lpName = pop32(cpu);
    struct KProcess* process = cpu->thread->process;

    if (!lockProcess(cpu->thread, process))
        return;
    setError(cpu, 0);
    if (lpName) {
        const char* name = getStringFromMemory(lpName);
        int i;

        for (i=0;i<MAX_HANDLES;i++) {
            if (process->handles[i].type == HANDLE_MUTEX) {
                if (!stricmp(process->handles[i].pMutexData->name, name)) {
                    EAX = i;
                    setError(cpu, ERROR_ALREADY_EXISTS);
                    process->handles[i].refCount++;
                    return;
                }
            }
        }
    }
    EAX = allocHandle(process, HANDLE_MUTEX);
    process->handles[EAX].pMutexData = kalloc(sizeof(struct MutexHandleData));
    if (lpName) {
        strcpy(process->handles[EAX].pMutexData->name, getStringFromMemory(lpName));
    }
    process->handles[EAX].pMutexData->mutex = KCreateMutex();
    if (bInitialOwner) {
        KLockMutex(process->handles[EAX].pMutexData->mutex);
    }
    unlockProcess(process);

    if (lpName) {
        LOG_KERNEL("CreateMutexA(%X, %d, %s)=%X\n", lpMutexAttributes, bInitialOwner, getStringFromMemory(lpName));
    } else {
        LOG_KERNEL("CreateMutexA(%X, %d, 0)=%X\n", lpMutexAttributes, bInitialOwner);
    }
}

void DeleteCriticalSection_cpu(struct CPU* cpu) {
    U32 pCriticalSection = pop32(cpu);
    struct kmutex* mutex = (struct kmutex*)readd(pCriticalSection);
    KDestroyMutex(mutex);
    LOG_KERNEL("DeleteCriticalSection(%X)\n", pCriticalSection);
}

void DeleteFileA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    char path[MAX_FILEPATH_LEN];
    struct stat st;

    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpFileName), path);
    if (stat(path, &st)) {
        setError(cpu, ERROR_FILE_NOT_FOUND);
        EAX = FALSE;
    } else if (unlink(path)==0) {
        EAX = TRUE;
    } else {
        setError(cpu, ERROR_ACCESS_DENIED);
        EAX = FALSE;
    }
    LOG_KERNEL("DeleteFileA(%s)=%d\n", getStringFromMemory(lpFileName), EAX);
}

void EnterCriticalSection_cpu(struct CPU* cpu) {
    U32 pCriticalSection = pop32(cpu);
    if (pCriticalSection) {
        struct kmutex* mutex = (struct kmutex*)readd(pCriticalSection);
        cpu->thread->waitingOnMutex = mutex;
        LOG_KERNEL("EnterCriticalSection(%X)...", pCriticalSection);
        KLockMutex(mutex);
        cpu->thread->waitingOnMutex = 0;
        LOG_KERNEL("OK\n", pCriticalSection);
    }
}

void ExitProcess_cpu(struct CPU* cpu) {
    U32 exitCode = pop32(cpu);
    LOG_KERNEL("ExitProcess(%d)\n", exitCode);
    exitProcess(cpu->thread, cpu->thread->process, exitCode);
}

void FileTimeToDosDateTime_cpu(struct CPU* cpu) {
    time_t t = readq(pop32(cpu)) / 10000000 - 11644473600l;
    U32 pDate = pop32(cpu);
    U32 pTime = pop32(cpu);
    struct tm* tm;

    tm = gmtime( &t );
    if (tm) {
        if (pDate)
            writed(pDate,  (tm->tm_sec / 2) + (tm->tm_min << 5) + (tm->tm_hour << 11));
        if (pTime)
            writed(pTime, tm->tm_mday + ((tm->tm_mon + 1) << 5) + ((tm->tm_year - 80) << 9));
        EAX = TRUE;
    } else {
        EAX = FALSE;
    }
    LOG_KERNEL("FileTimeToDosDateTime(time_t=%d)=%d\n", t, EAX);
}

void FileTimeToLocalFileTime_cpu(struct CPU* cpu) {
    U32 lpFileTime = pop32(cpu);
    U32 lpLocalFileTime = pop32(cpu);
    writeq(lpLocalFileTime, readq(lpFileTime));
    EAX = 1;
    LOG_KERNEL("FileTimeToLocalFileTime(%X, %X)=%d\n", lpFileTime, lpLocalFileTime, EAX);
}

void FileTimeToSystemTime_cpu(struct CPU* cpu) {
    U32 lpFileTime = pop32(cpu);
    U32 lpSystemTime = pop32(cpu);
    FILETIME i;
    SYSTEMTIME o;

    i.dwLowDateTime = readd(lpFileTime);
    i.dwHighDateTime = readd(lpFileTime+4);

    EAX = FileTimeToSystemTime_k(&i, &o);
    writeBufferToMemory(lpSystemTime, (unsigned char*)&o, sizeof(SYSTEMTIME));

    LOG_KERNEL("FileTimeToSystemTime(%X, %X)=%d\n", lpFileTime, lpSystemTime, EAX);
}

void FindClose_cpu(struct CPU* cpu) {
    U32 hFindFile = pop32(cpu);
    EAX = FindClose_k((HANDLE)hFindFile);
}

void FindFirstFileA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    U32 lpFindFileData = pop32(cpu);
    WIN32_FIND_DATAA data;
    char path[MAX_FILEPATH_LEN];

    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpFileName), path);

    EAX = (U32)FindFirstFileA_k(path, &data);

    if (EAX != (U32)INVALID_HANDLE_VALUE) {
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
    LOG_KERNEL("FindFirstFileA(%s, %X)=%d\n", getStringFromMemory(lpFileName), lpFindFileData, EAX);
}

void FindNextFileA_cpu(struct CPU* cpu) {
    U32 hFindFile = pop32(cpu);
    U32 lpFindFileData = pop32(cpu);
    U32 originalData = lpFindFileData;
    WIN32_FIND_DATAA data;

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

    EAX = (U32)FindNextFileA_k((HANDLE)hFindFile, &data);
    if (EAX) {
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

void FlushFileBuffers_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);

    if (hFile>=MAX_HANDLES || cpu->thread->process->handles[hFile].type!=HANDLE_FILE) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
    } else {
        EAX = TRUE;
    }
    LOG_KERNEL("FlushFileBuffers(%X)=%d\n", hFile, EAX);
}

void FreeEnvironmentStringsA_cpu(struct CPU* cpu) {
    U32 lpszEnvironmentBlock = pop32(cpu);
    kfree((void*)lpszEnvironmentBlock);
    EAX = 1;
    LOG_KERNEL("FreeEnvironmentStringsA(%X)=%d\n", lpszEnvironmentBlock, EAX);
}

void FreeEnvironmentStringsW_cpu(struct CPU* cpu) {
    U32 lpszEnvironmentBlock = pop32(cpu);
    kfree((void*)lpszEnvironmentBlock);
    EAX = 1;
    LOG_KERNEL("FreeEnvironmentStringsW(%X)=%d\n", lpszEnvironmentBlock, EAX);
}

void GetACP_cpu(struct CPU* cpu) {
    EAX = GetACP_k();
    LOG_KERNEL("GetACP=%d\n", EAX);
}

void GetCommandLineA_cpu(struct CPU* cpu) {
    EAX = (U32)cpu->thread->process->commandLine;
    LOG_KERNEL("GetCommandLineA=%X %s\n", EAX, cpu->thread->process->commandLine);
}

void GetConsoleMode_cpu(struct CPU* cpu) {
    EAX = 2; // ENABLE_LINE_INPUT
    LOG_KERNEL("GetConsoleMode=%d\n", EAX);
}

void GetCPInfo_cpu(struct CPU* cpu) {
    U32 CodePage = pop32(cpu);
    U32 lpCPInfo = pop32(cpu);
    CPINFO info;

    EAX = GetCPInfo_k(CodePage, &info);
    writed(lpCPInfo, info.MaxCharSize);lpCPInfo+=sizeof(UINT);
    writeBufferToMemory(lpCPInfo, info.DefaultChar, MAX_DEFAULTCHAR);lpCPInfo+=MAX_DEFAULTCHAR;
    writeBufferToMemory(lpCPInfo, info.LeadByte, MAX_LEADBYTES);
    LOG_KERNEL("GetCPInfo(%d, %X)=%d\n", CodePage, lpCPInfo, EAX);
}

void GetCurrentDirectoryA_cpu(struct CPU* cpu) {
    U32 nBufferLength = pop32(cpu);
    U32 lpBuffer = pop32(cpu);  
    U32 len = strlen(cpu->thread->process->currentDirectory);

    if (len>nBufferLength)
        len=nBufferLength;
    else {
        writeb(lpBuffer+len, 0);
    }
    writeBufferToMemory(lpBuffer, cpu->thread->process->currentDirectory, len);
    EAX = len;
    LOG_KERNEL("GetCurrentDirectoryA(%d, %X)=%d %s\n", nBufferLength, lpBuffer, EAX, cpu->thread->process->currentDirectory);
}

void GetCurrentProcess_cpu(struct CPU* cpu) {
    EAX = -1;
    LOG_KERNEL("GetCurrentProcess=%d\n", EAX);
}

void GetDriveTypeA_cpu(struct CPU* cpu) {
    U32 lpRootPathName = pop32(cpu);
    EAX = (U32)GetDriveTypeA_k((LPCSTR)lpRootPathName);
    LOG_KERNEL("GetDriveTypeA(%s)=%d\n", getStringFromMemory(lpRootPathName), EAX);
}

void GetEnvironmentStrings_cpu(struct CPU* cpu) {
    PblIterator* it;
    U32 len = 0;
    char* result;
    char* p;

    if (!lockProcess(cpu->thread, cpu->thread->process))
        return;

    it = pblMapIteratorNew(cpu->thread->process->environmentMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        const char* key = pblMapEntryKey(entry);
        const char* value = pblMapEntryValue(entry);
        len+=strlen(key);
        len++; // =
        len+=strlen(value);
        len++; // NULL;
    }
    pblIteratorFree(it);

    len++; // double NULL;
    result = (char*)kalloc(len);    
    p = result;

    LOG_KERNEL("GetEnvironmentStrings=");
    it = pblMapIteratorNew(cpu->thread->process->environmentMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        const char* key = pblMapEntryKey(entry);
        const char* value = pblMapEntryValue(entry);
        strcpy(p, key);
        p+=strlen(key);
        strcpy(p, "=");
        p++;
        strcpy(p, value);
        p+=strlen(value);
        p[0]=0;
        p++;
        LOG_KERNEL("%s=%s;", key, value);
    }
    p[0]=0;
    EAX = (U32)result;
    LOG_KERNEL("\n");
}

void GetEnvironmentStringsW_cpu(struct CPU* cpu) {
    PblIterator* it;
    U32 len = 0;
    unsigned short* result;
    unsigned short* p;

    if (!lockProcess(cpu->thread, cpu->thread->process))
        return;

    it = pblMapIteratorNew(cpu->thread->process->environmentMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        const char* key = pblMapEntryKey(entry);
        const char* value = pblMapEntryValue(entry);
        len+=strlen(key);
        len++; // =
        len+=strlen(value);
        len++; // NULL;
    }
    pblIteratorFree(it);

    len++; // double NULL;
    result = (unsigned short*)kalloc(len*2);    
    p = result;

    LOG_KERNEL("GetEnvironmentStringsW=");
    it = pblMapIteratorNew(cpu->thread->process->environmentMap);
    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        const char* key = pblMapEntryKey(entry);
        const char* value = pblMapEntryValue(entry);
        strcpyA2W(p, key);
        p+=strlen(key);
        strcpyA2W(p, "=");
        p++;
        strcpyA2W(p, value);
        p+=strlen(value);
        p[0]=0;
        p++;
        LOG_KERNEL("%s=%s;", key, value);
    }
    p[0]=0;
    EAX = (U32)result;
    LOG_KERNEL("\n");
}

void GetFileAttributesA_cpu(struct CPU* cpu) {
    U32  lpFileName = pop32(cpu);
    const char* pFileName = getStringFromMemory(lpFileName);    
    char path[MAX_FILEPATH_LEN];
    struct stat st;

    EAX = 0;
    getFullNativePath(cpu->thread->process->currentDirectory, pFileName, path);
    if (stat(path, &st)==0) {
        if (S_ISDIR(st.st_mode))
            EAX = 0x10; // FILE_ATTRIBUTE_DIRECTORY
        else
            EAX = 0x80; // FILE_ATTRIBUTE_NORMAL
    } else {
        setError(cpu, ERROR_PATH_NOT_FOUND);
        EAX = INVALID_FILE_ATTRIBUTES;
    }
    LOG_KERNEL("GetFileAttributesA(%s)=%d\n", pFileName, EAX);
}

void GetFileTime_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);
    U32 pCreateTime = pop32(cpu);
    U32 pAccessTime = pop32(cpu);
    U32 pModifiedTime = pop32(cpu);
    struct stat s;

    if (hFile>=MAX_HANDLES || (cpu->thread->process->handles[hFile].type!=HANDLE_FILE && cpu->thread->process->handles[hFile].type!=HANDLE_DIRECTORY)) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = 0;
    } else {
        if (cpu->thread->process->handles[hFile].type==HANDLE_FILE) {
            if (fstat(cpu->thread->process->handles[hFile].data, &s)==0) {                
                EAX = TRUE;
            } else {
                setError(cpu, ERROR_INVALID_PARAMETER);
                EAX = FALSE;
            }
        } else {
            if (stat(cpu->thread->process->handles[hFile].nativeDirectoryPath, &s)==0) {                
                EAX = TRUE;
            } else {
                setError(cpu, ERROR_INVALID_PARAMETER);
                EAX = FALSE;
            }
        }
        if (EAX) {
            if (pCreateTime)
                writeq(pCreateTime, unixToWinTime(s.st_ctime));
            if (pAccessTime)
                writeq(pAccessTime, unixToWinTime(s.st_atime));
            if (pModifiedTime)
                writeq(pModifiedTime, unixToWinTime(s.st_mtime));
        }
    }
    LOG_KERNEL("GetFileTime(%X, %X, %X, %X)=%X\n", hFile, pCreateTime, pAccessTime, pModifiedTime, EAX);
}

void GetFileType_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);

    if (hFile>=MAX_HANDLES || (cpu->thread->process->handles[hFile].type!=HANDLE_FILE && cpu->thread->process->handles[hFile].type!=HANDLE_DIRECTORY)) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = 0;
    } else {
        setError(cpu, 0);
        if (cpu->thread->process->handles[hFile].data<=2)
            EAX = 2; // FILE_TYPE_CHAR
        else
            EAX = 1; // FILE_TYPE_DISK
    }
    LOG_KERNEL("GetFileType(%X)=%d\n", hFile, EAX);
}

void GetFullPathNameA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    U32 nBufferLength = pop32(cpu);
    U32 lpBuffer = pop32(cpu);
    U32 lpFilePart = pop32(cpu);
    const char* result;

    const char* fileName = getStringFromMemory(lpFileName);
    if (fileName[1]==':') {
        U32 len = strlen(fileName)+1;
        if (len<=nBufferLength)
            writeBufferToMemory(lpBuffer, fileName, strlen(fileName)+1);
        EAX = len;
    } else if (fileName[0]=='/' || fileName[0]=='\\') {
        U32 len = strlen(fileName)+3;
        if (len<=nBufferLength) {
            writeBufferToMemory(lpBuffer, "C:", 2);
            writeBufferToMemory(lpBuffer+2, fileName, strlen(fileName)+1);
        }
        EAX = len;
    } else {
        U32 dirLen = strlen(cpu->thread->process->currentDirectory);
        U32 len = dirLen + strlen(fileName)+1;
        if (len<=nBufferLength) {
            writeBufferToMemory(lpBuffer, cpu->thread->process->currentDirectory, dirLen);
            writeBufferToMemory(lpBuffer+dirLen, fileName, strlen(fileName)+1);
        }
        EAX = len;
    }
    result = getStringFromMemory(lpBuffer);
    if (lpFilePart) {
        int i=0;
        for (i=(int)EAX-1;i>=0;i--) {
            if (result[i]=='/' || result[i]=='\\') {
                if (i==(int)EAX-1) {
                    writed(lpFilePart, 0);
                } else {
                    writed(lpFilePart, (U32)result+i+1);
                }
                break;
            }
        }
    }
    LOG_KERNEL("GetFullPathNameA(%s, %d, %X, %X)=%d lpBuffer=%s", fileName, nBufferLength, lpBuffer, lpFilePart, EAX, result);
    if (lpFilePart) {
        LOG_KERNEL(" lpFilePart=%s", getStringFromMemory(readd(lpFilePart)));
    }
    LOG_KERNEL("\n");
}

void GetLastError_cpu(struct CPU* cpu) {
    EAX = readd(cpu->segAddress[FS]+TEB_LAST_ERROR);
    LOG_KERNEL("GetLastError=%d", EAX);
}

void GetModuleFileNameA_cpu(struct CPU* cpu) {
    U32 hModule = pop32(cpu);
    U32 lpFilename = pop32(cpu);
    U32 nSize = pop32(cpu);
    EAX = GetModuleFileNameA_k((HMODULE)hModule, (LPSTR)lpFilename, nSize);
    LOG_KERNEL("GetModuleFileNameA(%d, %X, %d)=%d %s\n", hModule, lpFilename, nSize, EAX, (EAX?getStringFromMemory(lpFilename):""));
}

void GetOEMCP_cpu(struct CPU* cpu) {
    EAX = 437;
    LOG_KERNEL("GetOEMCP=%d\n", EAX);
}

void GetProcAddress_cpu(struct CPU* cpu) {
    U32 hModule = pop32(cpu);
    U32 lpProcName = pop32(cpu);
    struct KModule* module;

    if (hModule>=MAX_HANDLES || cpu->thread->process->handles[hModule].type != HANDLE_MODULE) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = 0;
    } else {
        module = cpu->thread->process->handles[hModule].module;
        EAX = (U32)getProcByName(module, getStringFromMemory(lpProcName));
    }
    LOG_KERNEL("GetProcAddress(%d, %s)=%X\n", hModule, getStringFromMemory(lpProcName), EAX);
}

void GetProcessHeap_cpu(struct CPU* cpu) {
    EAX = 1;
    LOG_KERNEL("GetProcessHeap=%d\n", EAX);
}

void GetStartupInfoA_cpu(struct CPU* cpu) {
    U32 lpStartupInfo = pop32(cpu);
    STARTUPINFOA info;

    info.cb = sizeof(STARTUPINFOA);
    GetStartupInfoA_k(&info);

    writed(lpStartupInfo, info.cb); lpStartupInfo+=4; // cb
    writed(lpStartupInfo, (U32)info.lpReserved); lpStartupInfo+=4; // lpReserved
    writed(lpStartupInfo, (U32)info.lpDesktop); lpStartupInfo+=4; // lpDesktop
    writed(lpStartupInfo, (U32)info.lpTitle); lpStartupInfo+=4; // lpTitle
    writed(lpStartupInfo, info.dwX); lpStartupInfo+=4; // dwX
    writed(lpStartupInfo, info.dwY); lpStartupInfo+=4; // dwY
    writed(lpStartupInfo, info.dwXSize); lpStartupInfo+=4; // dwXSize
    writed(lpStartupInfo, info.dwYSize); lpStartupInfo+=4; // dwYSize
    writed(lpStartupInfo, info.dwXCountChars); lpStartupInfo+=4; // dwXCountChars
    writed(lpStartupInfo, info.dwYCountChars); lpStartupInfo+=4; // dwYCountChars
    writed(lpStartupInfo, info.dwFillAttribute); lpStartupInfo+=4; // dwFillAttribute
    writed(lpStartupInfo, info.dwFlags); lpStartupInfo+=4; // dwFlags
    writew(lpStartupInfo, info.wShowWindow); lpStartupInfo+=2; // wShowWindow
    writew(lpStartupInfo, info.cbReserved2); lpStartupInfo+=2; // cbReserved2
    writed(lpStartupInfo, (U32)info.lpReserved2); lpStartupInfo+=4; // lpReserved2
    writed(lpStartupInfo, (U32)info.hStdInput); lpStartupInfo+=4; // hStdInput
    writed(lpStartupInfo, (U32)info.hStdOutput); lpStartupInfo+=4; // hStdOutput
    writed(lpStartupInfo, (U32)info.hStdError); // hStdError
    LOG_KERNEL("GetStartupInfoA(%X)\n", lpStartupInfo);
}

void GetStdHandle_cpu(struct CPU* cpu) {
    S32 nStdHandle = (S32)pop32(cpu);

    if (nStdHandle == -10)
        EAX = cpu->thread->process->stdInHandle;
    else if (nStdHandle == -11)
        EAX = cpu->thread->process->stdOutHandle;
    else if (nStdHandle == -12)
        EAX = cpu->thread->process->stdErrHandle;
    else {
        EAX = (U32)INVALID_HANDLE_VALUE;
        setError(cpu, ERROR_INVALID_HANDLE);
    }
    LOG_KERNEL("GetStdHandle(%d)=%d\n", nStdHandle, EAX);
}

void GetStringTypeA_cpu(struct CPU* cpu) {
    U32 Locale = pop32(cpu);
    U32 dwInfoType = pop32(cpu);
    U32 lpSrcStr = pop32(cpu);
    S32 cchSrc = (S32)pop32(cpu);
    U32 lpCharType = pop32(cpu);
    U32 len;
    U16* data;

    if (cchSrc>=0)
        len = cchSrc;
    else {
        len = stringLengthW(lpSrcStr)+1;
    }
    data = (U16*)kalloc(len*2);
    EAX = GetStringTypeA_k(Locale, dwInfoType, getStringFromMemory(lpSrcStr), len, data);
    if (EAX) {
        writeArray16ToMemory(lpCharType, data, len);
    }
    kfree(data);
    LOG_KERNEL("GetStringTypeA(%d, %d, %s, %d, %X)=%d\n", Locale, dwInfoType, (lpSrcStr?getStringFromMemory(lpSrcStr):"NULL"), cchSrc, lpCharType, EAX);
}

void GetStringTypeW_cpu(struct CPU* cpu) {
    U32 dwInfoType = pop32(cpu);
    U32 lpSrcStr = pop32(cpu);
    S32 cchSrc = (S32)pop32(cpu);
    U32 lpCharType = pop32(cpu);
    U32 len;
    U16* str;
    U16* data;
    if (cchSrc>=0)
        len = cchSrc;
    else {
        len = stringLengthW(lpSrcStr)+1;
    }
    str = (U16*)kalloc(len*2+2);
    data = (U16*)kalloc(len*2);
    readArray16FromMemory(lpSrcStr, str, len);

    EAX = GetStringTypeW_k(dwInfoType, str, len, data);
    if (EAX) {
        writeArray16ToMemory(lpCharType, data, len);
    }
    LOG_KERNEL("GetStringTypeW(%d, %S, %d, %X)=%d\n", dwInfoType, str, cchSrc, lpCharType, EAX);
    kfree(str);
    kfree(data);    
}

void GetTimeZoneInformation_cpu(struct CPU* cpu) {
    U32 lpTimeZoneInformation = pop32(cpu);
    TIME_ZONE_INFORMATION info;

    EAX = GetTimeZoneInformation_k(&info);
    if (lpTimeZoneInformation)
        writeBufferToMemory(lpTimeZoneInformation, (unsigned char*)&info, sizeof(TIME_ZONE_INFORMATION));
}

void GetVersion_cpu(struct CPU* cpu) {
    EAX = 0x0A280105; // 5.2.2600 WinXP SP2
    LOG_KERNEL("GetVersion=%X\n", EAX);
}

void GetVolumeInformationA_cpu(struct CPU* cpu) {
    U32 lpRootPathName = pop32(cpu);
    U32 lpVolumeNameBuffer = pop32(cpu);
    U32 nVolumeNameSize = pop32(cpu);
    U32 lpVolumeSerialNumber = pop32(cpu);
    U32 lpMaximumComponentLength = pop32(cpu);
    U32 lpFileSystemFlags = pop32(cpu);
    U32 lpFileSystemNameBuffer = pop32(cpu);
    U32 nFileSystemNameSize = pop32(cpu);

    if (lpVolumeNameBuffer)
        writeBufferToMemory(lpVolumeNameBuffer, "BoxedWine", 10);
    if (lpVolumeSerialNumber)
        writed(lpVolumeSerialNumber, 1);
    if (lpMaximumComponentLength)
        writed(lpMaximumComponentLength, 255);
    if (lpFileSystemFlags)
        writed(lpFileSystemFlags, 0);
    if (lpFileSystemNameBuffer)
        writeBufferToMemory(lpFileSystemNameBuffer, "FAT", 4);
    EAX = TRUE;
    LOG_KERNEL("GetVolumeInformationA(%s, %X, %d, %X, %X, %X, %X, %X)=%d\n", getStringFromMemory(lpRootPathName), lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize, EAX);
}

void HeapAlloc_cpu(struct CPU* cpu) {
    U32 hHeap = pop32(cpu);
    U32 dwFlags = pop32(cpu);
    U32 dwBytes = pop32(cpu);
    EAX = (U32)kalloc(dwBytes);
    LOG_KERNEL("HeapAlloc(%d, %X, %d)=%X\n", hHeap, dwFlags, dwBytes, EAX);
}

void HeapCreate_cpu(struct CPU* cpu) {
    U32 flOptions = pop32(cpu);
    U32 dwInitialSize = pop32(cpu);
    U32 dwMaximumSize = pop32(cpu);
    EAX = 1;
    LOG_KERNEL("HeapCreate(%X, %d, %d)=%d\n", flOptions, dwInitialSize, dwMaximumSize, EAX);
}

void HeapDestroy_cpu(struct CPU* cpu) {
    U32 handle = pop32(cpu);
    EAX = 1;
    LOG_KERNEL("HeapDestroy(%d)=%d\n", handle, EAX);
}

void HeapFree_cpu(struct CPU* cpu) {
    U32 hHeap = pop32(cpu);
    U32 dwFlags = pop32(cpu);
    U32 lpMem = pop32(cpu);
    kfree((void*)lpMem);
    EAX = TRUE;
    LOG_KERNEL("HeapFree(%d, %X, %X)=%d\n", hHeap, dwFlags, lpMem, EAX);
}

void HeapReAlloc_cpu(struct CPU* cpu) {
    U32 hHeap = pop32(cpu);
    U32 dwFlags = pop32(cpu);
    U32 lpMem = pop32(cpu);
    U32 dwBytes = pop32(cpu);
    EAX = (U32)realloc((void*)lpMem, dwBytes);
    LOG_KERNEL("HeapReAlloc(%d, %X, %X, %d)=%d\n", hHeap, dwFlags, lpMem, dwBytes, EAX);
}

void InitializeCriticalSection_cpu(struct CPU* cpu) {
    U32 pCriticalSection = pop32(cpu);
    SDL_mutex* mutex = SDL_CreateMutex();
    writed(pCriticalSection, (U32)mutex);
    LOG_KERNEL("InitializeCriticalSection(%X)\n", pCriticalSection);
}

void InterlockedExchange_cpu(struct CPU* cpu) {
    U32 pTarget = pop32(cpu);
    U32 value = pop32(cpu);

    KLockMutex(cpu->thread->process->interlockedMutex);
    EAX = readd(pTarget);
    writed(pTarget, value);
    KUnlockMutex(cpu->thread->process->interlockedMutex);
    LOG_KERNEL("InterlockedExchange(%X, %X)=%X\n", pTarget, value, EAX);
}

void LCMapStringA_cpu(struct CPU* cpu) {
    U32 Locale = pop32(cpu);
    U32 dwMapFlags = pop32(cpu);
    U32 lpSrcStr = pop32(cpu);
    S32 cchSrc = (S32)pop32(cpu);
    U32 lpDestStr = (U32)pop32(cpu);
    S32 cchDest = (S32)pop32(cpu);
    U8* dst = 0;

    if (lpDestStr) {
        dst = (U8*)kalloc(cchDest);        
    }
    EAX = LCMapStringA_k(Locale, dwMapFlags, getStringFromMemory(lpSrcStr), cchSrc, dst, cchDest);
    if (dst) {
        writeBufferToMemory(lpDestStr, dst, cchDest);
        kfree(dst);
    }
    LOG_KERNEL("LCMapStringA(%d, %X, %s, %d, %X, %d)=%d %s\n", Locale, dwMapFlags, (lpSrcStr?getStringFromMemory(lpSrcStr):"NULL"), cchSrc, lpDestStr, cchDest, EAX, (lpDestStr?getStringFromMemory(lpDestStr):""));
}

void LCMapStringW_cpu(struct CPU* cpu) {
    U32 Locale = pop32(cpu);
    U32 dwMapFlags = pop32(cpu);
    U32 lpSrcStr = pop32(cpu);
    S32 cchSrc = (S32)pop32(cpu);
    U32 lpDestStr = (U32)pop32(cpu);
    S32 cchDest = (S32)pop32(cpu);

    U16* dst = 0;
    U16* src = (U16*)kalloc(cchSrc*2);

    if (lpDestStr) {
        dst = (U16*)kalloc(cchDest*2);        
    }
    EAX = LCMapStringW_k(Locale, dwMapFlags, src, cchSrc, dst, cchDest);
    LOG_KERNEL("LCMapStringW(%d, %X, %S, %d, %X, %d)=%d %S\n", Locale, dwMapFlags, src, cchSrc, lpDestStr, cchDest, EAX, (dst?dst:L""));
    if (dst) {
        writeArray16ToMemory(lpDestStr, dst, cchDest);
        kfree(dst);
    }
    kfree(src);    
}

void LeaveCriticalSection_cpu(struct CPU* cpu) {
    U32 pCriticalSection = pop32(cpu);
    struct kmutex* mutex = (struct kmutex*)readd(pCriticalSection);
    KUnlockMutex(mutex);
    LOG_KERNEL("LeaveCriticalSection(%X)\n", pCriticalSection);
}

void LoadLibraryA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    struct KModule* module = loadModule(cpu->thread->process, getStringFromMemory(lpFileName));
    if (module)
        EAX = module->handle;
    else {
        setError(cpu, ERROR_MOD_NOT_FOUND);
        EAX = 0;
    }
    LOG_KERNEL("LoadLibraryA(%s)=%d\n", getStringFromMemory(lpFileName), EAX);
}

void lstrcmpiA_cpu(struct CPU* cpu) {
    U32 pStr1 = pop32(cpu);
    U32 pStr2 = pop32(cpu);
    
    EAX = lstrcmpiA(getStringFromMemory(pStr1), getStringFromMemory(pStr2));
    LOG_KERNEL("lstrcmpiA(%s, %s)=%d\n", getStringFromMemory(pStr1), getStringFromMemory(pStr2), EAX);
}

void lstrcpynA_cpu(struct CPU* cpu) {
    U32 pDst = pop32(cpu);
    U32 pSrc = pop32(cpu);
    U32 count = pop32(cpu);

    EAX = pDst;
    LOG_KERNEL("lstrcpynA(%s, %s, %d)=%d\n", getStringFromMemory(pDst), getStringFromMemory(pSrc), count, EAX);
    while (count > 1 && readb(pSrc)) {
        count--;
        writeb(pDst, readb(pSrc));
        pDst++;
        pSrc++;        
    }
    if (count)
        writeb(pDst, 0);    
}

void lstrlenA_cpu(struct CPU* cpu) {
    U32 pStr = pop32(cpu);
    U32 result = 0;
    U32 p = pStr;

    while (readb(p)) {
        p++;
        result++;
    }
    EAX = result;
    LOG_KERNEL("lstrlenA(%s)=%d\n", getStringFromMemory(pStr), EAX);
}

void MultiByteToWideChar_cpu(struct CPU* cpu) {
    U32 CodePage = pop32(cpu);
    U32 dwFlags = pop32(cpu);
    U32 lpMultiByteStr = pop32(cpu);
    S32 cbMultiByte = (S32)pop32(cpu);
    U32 lpWideCharStr = pop32(cpu);
    S32 cchWideChar = (S32)pop32(cpu);
    U16* dst = 0;
    
    if (cchWideChar && lpWideCharStr) {
        dst = (U16*)kalloc(cchWideChar*2);
    }
    EAX = MultiByteToWideChar_k(CodePage, dwFlags, getStringFromMemory(lpMultiByteStr), cbMultiByte, dst, cchWideChar);
    LOG_KERNEL("MultiByteToWideChar(%d, %X, %X, %d, %S, %d)=%d\n", CodePage, dwFlags, lpMultiByteStr, cbMultiByte, dst, cchWideChar, EAX);
    if (dst) {
        writeArray16ToMemory(lpWideCharStr, dst, cchWideChar);
        kfree(dst);
    }    
}

void ReadFile_cpu(struct CPU* cpu) {
    U32 f = pop32(cpu);
    U32 pBuffer = pop32(cpu);
    U32 len = pop32(cpu);
    U32 pRead = pop32(cpu);
    U32 pOverlapped = pop32(cpu);
    U8 buffer[4096];
    U32 amountRead = 0;
    
    if (f>=MAX_HANDLES || cpu->thread->process->handles[f].type!=HANDLE_FILE) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
        return;
    }
    EAX = TRUE;
    while (len) {
        U32 toRead = len;
        S32 result;

        if (toRead>4096)
            toRead = 4096;
        result = read(cpu->thread->process->handles[f].data, buffer, toRead);
        if (result>0) {
            writeBufferToMemory(pBuffer, buffer, result);
            pBuffer+=result;
            len-=result;
            amountRead+=result;
        } else if (result == 0) {
            U32 result = lseek(cpu->thread->process->handles[f].data, 0, SEEK_CUR);
            break;
        } else {
            if (amountRead==0) {
                EAX = FALSE;
                if (errno == EBADF) {
                    setError(cpu, ERROR_INVALID_HANDLE);
                } else {
                    setError(cpu, ERROR_NOT_READY);
                }
            }
            break;
        }
    }
    if (pRead) {
        writed(pRead, amountRead);
    }  
    LOG_KERNEL("ReadFile(%d, %X, %d, %X, %X)=%d\n", f, pBuffer, len, pRead, pOverlapped, EAX);
}

void ReleaseMutex_cpu(struct CPU* cpu) {
    U32 handle = pop32(cpu);
    if (handle >= MAX_HANDLES || cpu->thread->process->handles[handle].type != HANDLE_MUTEX) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;        
    } else {
        struct MutexHandleData* data = cpu->thread->process->handles[handle].pMutexData;
        KUnlockMutex(data->mutex);
        EAX = TRUE;
    }
    LOG_KERNEL("ReleaseMutex(%d)\n", handle, EAX);
}

void RtlUnwind_cpu(struct CPU* cpu) {
    U32 pTargetFrame = pop32(cpu);
    U32 pTargetIp = pop32(cpu);
    U32 pExceptionRecord = pop32(cpu);
    U32 pReturnValue = pop32(cpu);
    NOT_IMPLEMENTED("RtlUnwind");
}

void SetConsoleCtrlHandler_cpu(struct CPU* cpu) {
    U32 handlerRoutine = pop32(cpu);
    U32 add = pop32(cpu);

    NOT_IMPLEMENTED("SetConsoleCtrlHandler");
    EAX = TRUE;
}

void SetConsoleMode_cpu(struct CPU* cpu) {
    U32 hcon = pop32(cpu);
    U32 mode = pop32(cpu);

    NOT_IMPLEMENTED("SetConsoleMode");
    EAX = TRUE;
}

void SetEndOfFile_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);
    if (hFile >= MAX_HANDLES || cpu->thread->process->handles[hFile].type != HANDLE_MUTEX) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;        
    } else {
        if (ftruncate(cpu->thread->process->handles[hFile].data, lseek(cpu->thread->process->handles[hFile].data, 0, SEEK_CUR))==0)
            EAX = TRUE;
        else
            EAX = FALSE;
    }
    LOG_KERNEL("SetEndOfFile(%d)=%d\n", hFile, EAX);
}

void SetEnvironmentVariableA_cpu(struct CPU* cpu) {
    U32 lpName = pop32(cpu);
    U32 lpValue = pop32(cpu);

    pblMapAddStrStr(cpu->thread->process->environmentMap, (char*)getStringFromMemory(lpName), (char*)getStringFromMemory(lpValue));
    EAX = 1;
    LOG_KERNEL("SetEnvironmentVariableA(%s, %s)=%d\n", getStringFromMemory(lpName), getStringFromMemory(lpValue), EAX);
}

void SetFileAttributesA_cpu(struct CPU* cpu) {
    U32 lpFileName = pop32(cpu);
    U32 dwFileAttributes = pop32(cpu);
    char path[MAX_FILEPATH_LEN];
    struct stat st;

    getFullNativePath(cpu->thread->process->currentDirectory, getStringFromMemory(lpFileName), path);
    if (stat(path, &st)) {
        setError(cpu, ERROR_FILE_NOT_FOUND);
        EAX = FALSE;
    } else {
        // :TODO:
        EAX = TRUE;
    }
    LOG_KERNEL("SetFileAttributesA(%s, %X)=%d\n", getStringFromMemory(lpFileName), dwFileAttributes, EAX);
}

void SetFilePointer_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);
    U32 lDistanceToMove = pop32(cpu);
    U32 lpDistanceToMoveHigh = pop32(cpu);
    U32 dwMoveMethod = pop32(cpu);
    U64 offset = lDistanceToMove;
    U32 origin = 0;

    if (lpDistanceToMoveHigh)
        offset |= ((U64)readd(lpDistanceToMoveHigh) << 32);
    if (hFile>=MAX_HANDLES || cpu->thread->process->handles[hFile].type!=HANDLE_FILE) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
        return;
    }
    if (dwMoveMethod == 0) // FILE_BEGIN
        origin = SEEK_SET;
    else if (dwMoveMethod == 1) // FILE_CURRENT
        origin = SEEK_CUR;
    else if (dwMoveMethod == 2) // FILE_END
        origin = SEEK_END;
    else {
        setError(cpu, ERROR_INVALID_PARAMETER);
        EAX = INVALID_SET_FILE_POINTER;
        return;
    }
    setError(cpu, 0);
    offset = lseek64(cpu->thread->process->handles[hFile].data, offset, origin);
    if (lpDistanceToMoveHigh) {
        writed(lpDistanceToMoveHigh, offset >> 32);
    }
    EAX = (U32)offset;
    LOG_KERNEL("SetFilePointer(%d, %X, %X, %X)=%d", hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod, EAX);
}

void SetFileTime_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);
    U32 lpCreationTime = pop32(cpu);
    U32 lpLastAccessTime = pop32(cpu);
    U32 lpLastWriteTime = pop32(cpu);
    if (hFile>=MAX_HANDLES || (cpu->thread->process->handles[hFile].type!=HANDLE_FILE && cpu->thread->process->handles[hFile].type!=HANDLE_DIRECTORY)) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
        return;
    }
    NOT_IMPLEMENTED("SetFileTime");
    EAX = TRUE;
}

void SetHandleCount_cpu(struct CPU* cpu) {
    EAX = pop32(cpu);
    LOG_KERNEL("SetHandleCount(%d)=%d\n", EAX, EAX);
}

void SetLastError_cpu(struct CPU* cpu) {
    U32 error = pop32(cpu);
    setError(cpu, error);
    LOG_KERNEL("SetLastError(%d)\n", error);
}

void SetStdHandle_cpu(struct CPU* cpu) {
    S32 nStdHandle = (S32)pop32(cpu);
    U32 hHandle = pop32(cpu);

    if (hHandle>=MAX_HANDLES || cpu->thread->process->handles[hHandle].type==HANDLE_UNUSED) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
        return;
    }
    // :TODO: what happens to the reference counts
    if (nStdHandle == -10) {
        EAX = TRUE;
    } else if (nStdHandle == -11) {
        EAX = TRUE;
    } else if (nStdHandle == -12) {
        EAX = TRUE;
    } else {
        setError(cpu, ERROR_INVALID_PARAMETER);
        EAX = FALSE;
    }
    LOG_KERNEL("SetStdHandle(%d, %d)=%d\n", nStdHandle, hHandle, EAX);
}

void SetVolumeLabelA_cpu(struct CPU* cpu) {
    U32 lpRootPathName = pop32(cpu);
    U32 lpVolumeName = pop32(cpu);
    NOT_IMPLEMENTED("SetVolumeLabelA");
    EAX = 1;
}

void TerminateProcess_cpu(struct CPU* cpu) {
    U32 hProcess = pop32(cpu);
    U32 uExitCode = pop32(cpu);
    struct KProcess* process = 0;
    
    if (hProcess == 0xFFFFFFFF)
        process = cpu->thread->process;
    else {
        process = getProcess(hProcess);        
    }

    if (!process) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;   
    } else {
        terminateProcess(cpu->thread, process, uExitCode);
        setError(cpu, 0);
        EAX = TRUE;
    }
    LOG_KERNEL("TerminateProcess(%d, %d)\n", hProcess, uExitCode);
}

void UnhandledExceptionFilter_cpu(struct CPU* cpu) {
    U32 info = pop32(cpu);
    EAX = 0;
    NOT_IMPLEMENTED("UnhandledExceptionFilter");
}

void VirtualAlloc_cpu(struct CPU* cpu) {
    U32 lpAddress = pop32(cpu);
    U32 dwSize = pop32(cpu);
    U32 flAllocationType = pop32(cpu);
    U32 flProtect = pop32(cpu);

    if (lpAddress) {
        // MEM_RESERVE
        if (flAllocationType & 0x2000) {
            NOT_IMPLEMENTED("VirtualFree with lpAddress");
        }
        EAX = lpAddress;
    } else {
        EAX = (U32)kalloc(dwSize);
    }
    LOG_KERNEL("VirtualAlloc(%X, %X, %X, %X)=%X\n", lpAddress, dwSize, flAllocationType, flProtect, EAX);
}

void VirtualFree_cpu(struct CPU* cpu) {
    U32 lpAddress = pop32(cpu);
    U32 dwSize = pop32(cpu);
    U32 dwFreeType = pop32(cpu);
    kfree((void*)lpAddress);
    EAX = 1;
    LOG_KERNEL("VirtualFree(%X, %d, %X)=%d\n", lpAddress, dwSize, dwFreeType, EAX);
}

void WaitForSingleObject_cpu(struct CPU* cpu) {
    U32 handle = pop32(cpu);
    U32 timeout = pop32(cpu);

    LOG_KERNEL("WaitForSingleObject(%d, %d)", handle, timeout);
    if (handle>=MAX_HANDLES) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = 0xFFFFFFFF;        
    } else {
        switch (cpu->thread->process->handles[handle].type) {
        case HANDLE_MUTEX:
            cpu->thread->waitingOnMutex = cpu->thread->process->handles[handle].pMutexData->mutex;
            if (timeout == INFINITE) {
                KLockMutex(cpu->thread->process->handles[handle].pMutexData->mutex);
                EAX = 0;
            } else {
                if (KLockMutexTimeout(cpu->thread->process->handles[handle].pMutexData->mutex, timeout)==SDL_MUTEX_TIMEDOUT) {
                    EAX = WAIT_TIMEOUT;
                } else {
                    EAX = 0;
                }
            }
            cpu->thread->waitingOnMutex = 0;
            break;
        default:
            setError(cpu, ERROR_INVALID_HANDLE);
            EAX = 0xFFFFFFFF;        
            break;
        }
    }
    LOG_KERNEL("=%d\n", EAX);
}

void WideCharToMultiByte_cpu(struct CPU* cpu) {
    U32 page = pop32(cpu);
    U32 flags = pop32(cpu);
    U32 pSrc = pop32(cpu);
    S32 srclen = (S32)pop32(cpu);
    U32 pDst = pop32(cpu);
    U32 dstlen = pop32(cpu);
    U32 pDefchar = pop32(cpu);
    U32 pUsed = pop32(cpu);
    U16* src;
    U8* dst = 0;
    U32 len;
    BOOL used;
    U8* defChar = 0;

    if (srclen<0) {
        len = stringLengthW(pSrc)+1;
    } else {
        len = srclen;
    }
    if (pDst) {
        dst = getStringFromMemory(pDst);
    }
    if (pDefchar) {
        defChar = getStringFromMemory(pDefchar);
    }
    src = kalloc(len*2);
    readArray16FromMemory(pSrc, src, len);
    EAX = WideCharToMultiByte_k(page, flags, src, len, dst, dstlen, defChar, &used);    
    if (dst) {
        writeBufferToMemory(pDst, dst, dstlen);
    }
    if (pUsed) {
        writed(pUsed, used);
    }    
    LOG_KERNEL("WideCharToMultiByte(%d, %X, %S, %d, %X, %d, %X, %X)=%d\n", page, flags, src, srclen, pDst, dstlen, pDefchar, pUsed, EAX);
    kfree(src);    
}

void WriteFile_cpu(struct CPU* cpu) {
    U32 hFile = pop32(cpu);
    U32 lpBuffer = pop32(cpu);
    U32 nNumberOfBytesToWrite = pop32(cpu);
    U32 lpNumberOfBytesWritten = pop32(cpu);
    U32 lpOverlapped = pop32(cpu);
    U8 buffer[4096];
    S32 toWrite;
    U32 written = 0;

    if (hFile>=MAX_HANDLES || cpu->thread->process->handles[hFile].type!=HANDLE_FILE) {
        setError(cpu, ERROR_INVALID_HANDLE);
        EAX = FALSE;
    } else {
        EAX = TRUE;
        while (nNumberOfBytesToWrite) {
            toWrite = nNumberOfBytesToWrite;
            if (toWrite>4096)
                toWrite = 4096;
            readBufferFromMemory(lpBuffer, buffer, toWrite);
            toWrite = write(cpu->thread->process->handles[hFile].data, buffer, toWrite);
            if (toWrite==-1) {
                if (written==0) {
                    if (errno == EBADF) {
                        setError(cpu, ERROR_INVALID_HANDLE);
                    } else {
                        setError(cpu, ERROR_NOT_READY);
                    }
                }
                break;
            }
            written += toWrite;
            lpBuffer += toWrite;
            nNumberOfBytesToWrite -= toWrite;
        }
        if (lpNumberOfBytesWritten) {
            writed(lpNumberOfBytesWritten, written);
        }
    }
    LOG_KERNEL("WriteFile(%d, %X, %d, %X, %X)=%d\n", hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped, EAX);
}

struct KModule* createModule_kernel32() {
    struct KModule* result = createModule(0);
    strcpy(result->name, "kernel32.dll");
    strcpy(result->localPath, "c:\\Windows\\System32\\kernel32.dll");

    addBuiltInFunction(result, "CloseHandle", CloseHandle_cpu);        
    addBuiltInFunction(result, "CompareStringA", CompareStringA_cpu);
    addBuiltInFunction(result, "CompareStringW", CompareStringW_cpu);
    addBuiltInFunction(result, "CreateDirectoryA", CreateDirectoryA_cpu);
    addBuiltInFunction(result, "CreateFileA", CreateFileA_cpu);    
    addBuiltInFunction(result, "CreateMutexA", CreateMutexA_cpu);        
    addBuiltInFunction(result, "DeleteCriticalSection", DeleteCriticalSection_cpu);    
    addBuiltInFunction(result, "DeleteFileA", DeleteFileA_cpu);    
    addBuiltInFunction(result, "EnterCriticalSection", EnterCriticalSection_cpu);    
    addBuiltInFunction(result, "ExitProcess", EnterCriticalSection_cpu);    
    addBuiltInFunction(result, "FileTimeToDosDateTime", FileTimeToDosDateTime_cpu);
    addBuiltInFunction(result, "FileTimeToLocalFileTime", FileTimeToLocalFileTime_cpu);    
    addBuiltInFunction(result, "FileTimeToSystemTime", FileTimeToSystemTime_cpu);
    addBuiltInFunction(result, "FindClose", FindClose_cpu);
    addBuiltInFunction(result, "FindFirstFileA", FindFirstFileA_cpu);        
    addBuiltInFunction(result, "FindNextFileA", FindNextFileA_cpu);        
    addBuiltInFunction(result, "FlushFileBuffers", FlushFileBuffers_cpu);    
    addBuiltInFunction(result, "FreeEnvironmentStringsA", FreeEnvironmentStringsA_cpu);
    addBuiltInFunction(result, "FreeEnvironmentStringsW", FreeEnvironmentStringsW_cpu);
    addBuiltInFunction(result, "GetACP", GetACP_cpu);    
    addBuiltInFunction(result, "GetCommandLineA", GetCommandLineA_cpu);    
    addBuiltInFunction(result, "GetConsoleMode", GetConsoleMode_cpu);
    addBuiltInFunction(result, "GetCPInfo", GetCPInfo_cpu);
    addBuiltInFunction(result, "GetCurrentDirectoryA", GetCurrentDirectoryA_cpu);    
    addBuiltInFunction(result, "GetCurrentProcess", GetCurrentProcess_cpu);    
    addBuiltInFunction(result, "GetDriveTypeA", GetDriveTypeA_cpu);
    addBuiltInFunction(result, "GetEnvironmentStrings", GetEnvironmentStrings_cpu);
    addBuiltInFunction(result, "GetEnvironmentStringsW", GetEnvironmentStringsW_cpu);
    addBuiltInFunction(result, "GetFileAttributesA", GetFileAttributesA_cpu);
    addBuiltInFunction(result, "GetFileTime", GetFileTime_cpu);
    addBuiltInFunction(result, "GetFileType", GetFileType_cpu);
    addBuiltInFunction(result, "GetFullPathNameA", GetFullPathNameA_cpu);    
    addBuiltInFunction(result, "GetLastError", GetLastError_cpu);    
    addBuiltInFunction(result, "GetModuleFileNameA", GetModuleFileNameA_cpu);
    addBuiltInFunction(result, "GetOEMCP", GetOEMCP_cpu);
    addBuiltInFunction(result, "GetProcAddress", GetProcAddress_cpu);    
    addBuiltInFunction(result, "GetProcessHeap", GetProcessHeap_cpu);    
    addBuiltInFunction(result, "GetStartupInfoA", GetStartupInfoA_cpu);    
    addBuiltInFunction(result, "GetStdHandle", GetStdHandle_cpu);
    addBuiltInFunction(result, "GetStringTypeA", GetStringTypeA_cpu);
    addBuiltInFunction(result, "GetStringTypeW", GetStringTypeW_cpu);
    addBuiltInFunction(result, "GetTimeZoneInformation", GetTimeZoneInformation_cpu);        
    addBuiltInFunction(result, "GetVersion", GetVersion_cpu);
    addBuiltInFunction(result, "GetVolumeInformationA", GetVolumeInformationA_cpu);    
    addBuiltInFunction(result, "HeapAlloc", HeapAlloc_cpu);
    addBuiltInFunction(result, "HeapCreate", HeapCreate_cpu);
    addBuiltInFunction(result, "HeapDestroy", HeapDestroy_cpu);
    addBuiltInFunction(result, "HeapFree", HeapFree_cpu);
    addBuiltInFunction(result, "HeapReAlloc", HeapReAlloc_cpu);    
    addBuiltInFunction(result, "InitializeCriticalSection", InitializeCriticalSection_cpu);
    addBuiltInFunction(result, "InterlockedExchange", InterlockedExchange_cpu);    
    addBuiltInFunction(result, "LCMapStringA", LCMapStringA_cpu);
    addBuiltInFunction(result, "LCMapStringW", LCMapStringW_cpu);
    addBuiltInFunction(result, "LeaveCriticalSection", LeaveCriticalSection_cpu);
    addBuiltInFunction(result, "LoadLibraryA", LoadLibraryA_cpu);    
    addBuiltInFunction(result, "lstrcmpiA", lstrcmpiA_cpu);        
    addBuiltInFunction(result, "lstrcpynA", lstrcpynA_cpu);        
    addBuiltInFunction(result, "lstrlenA", lstrlenA_cpu);    
    addBuiltInFunction(result, "MultiByteToWideChar", MultiByteToWideChar_cpu);        
    addBuiltInFunction(result, "ReadFile", ReadFile_cpu);
    addBuiltInFunction(result, "ReleaseMutex", ReleaseMutex_cpu);
    addBuiltInFunction(result, "RtlUnwind", RtlUnwind_cpu);    
    addBuiltInFunction(result, "SetEndOfFile", SetEndOfFile_cpu);    
    addBuiltInFunction(result, "SetConsoleCtrlHandler", SetConsoleCtrlHandler_cpu);    
    addBuiltInFunction(result, "SetConsoleMode", SetConsoleMode_cpu);
    addBuiltInFunction(result, "SetEnvironmentVariableA", SetEnvironmentVariableA_cpu);
    addBuiltInFunction(result, "SetFileAttributesA", SetFileAttributesA_cpu);    
    addBuiltInFunction(result, "SetFilePointer", SetFilePointer_cpu);
    addBuiltInFunction(result, "SetFileTime", SetFileTime_cpu);    
    addBuiltInFunction(result, "SetHandleCount", SetHandleCount_cpu);    
    addBuiltInFunction(result, "SetLastError", SetLastError_cpu);
    addBuiltInFunction(result, "SetStdHandle", SetStdHandle_cpu);    
    addBuiltInFunction(result, "SetVolumeLabelA", SetVolumeLabelA_cpu);    
    addBuiltInFunction(result, "TerminateProcess", TerminateProcess_cpu);    
    addBuiltInFunction(result, "UnhandledExceptionFilter", UnhandledExceptionFilter_cpu);
    addBuiltInFunction(result, "VirtualAlloc", VirtualAlloc_cpu);    
    addBuiltInFunction(result, "VirtualFree", VirtualFree_cpu);    
    addBuiltInFunction(result, "WaitForSingleObject", WaitForSingleObject_cpu);
    addBuiltInFunction(result, "WideCharToMultiByte", WideCharToMultiByte_cpu);    
    addBuiltInFunction(result, "WriteFile", WriteFile_cpu);        
    return result;
}