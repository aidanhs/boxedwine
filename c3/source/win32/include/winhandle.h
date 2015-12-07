#ifndef __WINHANDLE_H__
#define __WINHANDLE_H__

struct MutexHandleData {
    char name[260];
    struct kmutex* mutex;
};

struct WinHandle {
    U32 type;
    U32 refCount;
    union {
        U32 data;
        struct MutexHandleData* pMutexData;
        struct KModule* module;
        void* semaphore;
        char* nativeDirectoryPath;
    };
};

#define HANDLE_UNUSED 0
#define HANDLE_FILE 1
#define HANDLE_MUTEX 2
#define HANDLE_PROCESS 3
#define HANDLE_MODULE 4
#define HANDLE_SEMAPHORE 5
#define HANDLE_DIRECTORY 6
#endif