#include "../include/winbase.h"
#include "kprocess.h"

VOID WINAPI GetStartupInfoA_k( LPSTARTUPINFOA info )
{
    *info = *currentProcess()->startup_infoA;
}


VOID WINAPI GetStartupInfoW_k( LPSTARTUPINFOW info )
{
    *info = *currentProcess()->startup_infoW;
}