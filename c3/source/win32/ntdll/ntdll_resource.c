#include "../include/winbase.h"
#include "../include/ntstatus.h"

static LCID user_lcid=1033, system_lcid=1033;
static LANGID user_ui_language=1033, system_ui_language=1033;

NTSTATUS WINAPI NtQueryDefaultLocale_k( BOOLEAN user, LCID *lcid )
{
    *lcid = user ? user_lcid : system_lcid;
    return STATUS_SUCCESS;
}

/**********************************************************************
 *	NtQueryDefaultUILanguage  (NTDLL.@)
 */
NTSTATUS WINAPI NtQueryDefaultUILanguage_k( LANGID *lang )
{
    *lang = user_ui_language;
    return STATUS_SUCCESS;
}

NTSTATUS WINAPI NtQueryInstallUILanguage_k( LANGID *lang )
{
    *lang = system_ui_language;
    return STATUS_SUCCESS;
}