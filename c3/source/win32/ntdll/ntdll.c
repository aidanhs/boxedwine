#include "../include/winbase.h"
#include "../include/ntstatus.h"
#include <SDL.h>
#include "kprocess.h"

NTSTATUS WINAPI NtCreateSemaphore_k(PHANDLE SemaphoreHandle, ACCESS_MASK access, const OBJECT_ATTRIBUTES *attr, LONG InitialCount, LONG MaximumCount ) {
    SDL_sem* sem = SDL_CreateSemaphore(InitialCount);
    struct KProcess* process = currentProcess();
    U32 handle = allocHandle(process, HANDLE_SEMAPHORE);
    process->handles[handle].semaphore = sem;
    *SemaphoreHandle = (HANDLE)handle;
    return 0;
}

NTSTATUS WINAPI NtReleaseSemaphore_k( HANDLE handle, ULONG count, PULONG previous ) {
    struct KProcess* process = currentProcess();
    SDL_sem* sem;

    if (process->handles[(int)handle].type!=HANDLE_SEMAPHORE) {
        return STATUS_INVALID_HANDLE;
    }
    sem = process->handles[(int)handle].semaphore;
    if (previous)
        *previous = SDL_SemValue(sem);
    SDL_SemPost(sem);
    return STATUS_SUCCESS;
}

NTSTATUS WINAPI NtClose_k( HANDLE Handle ) {
    if (freeHandle(currentProcess(), (int)Handle))
        return STATUS_SUCCESS;
    return STATUS_INVALID_HANDLE;
}

void WINAPI RtlRaiseStatus_k( NTSTATUS status ) {
    kpanic("RtlRaiseStatus not implemented");
}