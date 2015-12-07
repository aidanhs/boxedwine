#include <SDL.h>

// copied from sdl2
#ifdef PLATFORM_MSVC
#define SDL_AtomicSet(a, v)     _InterlockedExchange((long*)&(a)->value, (v))
#define SDL_AtomicAdd(a, v)     _InterlockedExchangeAdd((long*)a, (v))
#define SDL_AtomicCAS(a, oldval, newval) (_InterlockedCompareExchange((long*)&(a)->value, (newval), (oldval)) == (oldval))
#define SDL_AtomicSetPtr(a, v)  _InterlockedExchangePointer((a), (v))
#if _M_IX86
#define SDL_AtomicCASPtr(a, oldval, newval) (_InterlockedCompareExchange((long*)(a), (long)(newval), (long)(oldval)) == (long)(oldval))
#else
#define SDL_AtomicCASPtr(a, oldval, newval) (_InterlockedCompareExchangePointer((a), (newval), (oldval)) == (oldval))
#endif

#elif defined(__MACOSX__)
#include <libkern/OSAtomic.h>

#define SDL_AtomicCAS(a, oldval, newval) OSAtomicCompareAndSwap32Barrier((oldval), (newval), &(a)->value)
#ifdef __LP64__
#define SDL_AtomicCASPtr(a, oldval, newval) OSAtomicCompareAndSwap64Barrier((int64_t)(oldval), (int64_t)(newval), (int64_t*)(a))
#else
#define SDL_AtomicCASPtr(a, oldval, newval) OSAtomicCompareAndSwap32Barrier((int32_t)(oldval), (int32_t)(newval), (int32_t*)(a))
#endif

#elif defined(HAVE_GCC_ATOMICS)
#define SDL_AtomicSet(a, v)     __sync_lock_test_and_set(&(a)->value, v)
#define SDL_AtomicAdd(a, v)     __sync_fetch_and_add(a, v)
#define SDL_AtomicSetPtr(a, v)  __sync_lock_test_and_set(a, v)
#define SDL_AtomicCAS(a, oldval, newval) __sync_bool_compare_and_swap(&(a)->value, oldval, newval)
#define SDL_AtomicCASPtr(a, oldval, newval) __sync_bool_compare_and_swap(a, oldval, newval)
#endif

void *interlocked_cmpxchg_ptr( void **dest, void *xchg, void *compare ) {
#ifdef PLATFORM_MSVC
    return (void*)_InterlockedCompareExchange((long*)dest, xchg, compare);
#endif
}

int interlocked_xchg_add( int *dest, int incr ) {
    return _InterlockedExchangeAdd(dest, incr);
}

int interlocked_cmpxchg( int *dest, int xchg, int compare ) {
    return _InterlockedCompareExchange(dest, xchg, compare);
}