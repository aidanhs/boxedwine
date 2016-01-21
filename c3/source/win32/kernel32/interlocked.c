#include "../include/winbase.h"

LONG WINAPI InterlockedCompareExchange_k( LONG volatile *dest, LONG xchg, LONG compare )
{
    return _InterlockedCompareExchange( dest, xchg, compare );
}

LONG WINAPI InterlockedDecrement_k( LONG volatile *dest )
{
    return _InterlockedDecrement( dest );
}

LONG WINAPI InterlockedExchange_k( LONG volatile *dest, LONG val )
{
    return _InterlockedExchange( dest, val );
}

LONG WINAPI InterlockedExchangeAdd_k( LONG volatile *dest, LONG incr )
{
    return _InterlockedExchangeAdd( dest, incr );
}

LONG WINAPI InterlockedIncrement_k( LONG volatile *dest )
{
    return _InterlockedIncrement( dest );
}

PVOID WINAPI InterlockedCompareExchangePointer_k( PVOID volatile *dest, PVOID xchg, PVOID compare )
{
    return _InterlockedCompareExchangePointer( dest, xchg, compare );
}

PVOID WINAPI InterlockedExchangePointer_k( PVOID volatile *dest, PVOID val )
{
    return (PVOID)_InterlockedExchange( dest, val );
}
