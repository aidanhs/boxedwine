#ifndef __WINBASE_H__
#define __WINBASE_H__

#include "wineincludes.h"
#include "winerror.h"

#define FIXME printf
#define ERR printf

#define WAIT_TIMEOUT 0x102
#define MAX_PATH 260
#define INFINITE 0xFFFFFFFF

#define INVALID_FILE_ATTRIBUTES  (~0u)
#define INVALID_HANDLE_VALUE     (~0u)
#define INVALID_SET_FILE_POINTER (~0u)

#define GENERIC_READ               0x80000000
#define GENERIC_WRITE              0x40000000

void getFullNativePath(const char* currentDirectory, const char* localFile, char* nativeFile);

LPSTR WINAPI CharLowerA_k(LPSTR str);
LPWSTR WINAPI CharLowerW_k(LPWSTR x);
DWORD WINAPI CharLowerBuffA_k( LPSTR str, DWORD len );
DWORD WINAPI CharLowerBuffW_k( LPWSTR str, DWORD len );
LPSTR WINAPI CharUpperA_k(LPSTR str);
LPWSTR WINAPI CharUpperW_k(LPWSTR x);
DWORD WINAPI CharUpperBuffA_k( LPSTR str, DWORD len );
DWORD WINAPI CharUpperBuffW_k( LPWSTR str, DWORD len );
LPSTR WINAPI CharNextA_k( LPCSTR ptr );
LPSTR WINAPI CharNextExA_k( WORD codepage, LPCSTR ptr, DWORD flags );
LPWSTR WINAPI CharNextExW_k( WORD codepage, LPCWSTR ptr, DWORD flags );
LPWSTR WINAPI CharNextW_k(LPCWSTR x);
LPSTR WINAPI CharPrevA_k( LPCSTR start, LPCSTR ptr );
LPSTR WINAPI CharPrevExA_k( WORD codepage, LPCSTR start, LPCSTR ptr, DWORD flags );
LPSTR WINAPI CharPrevExW_k( WORD codepage, LPCWSTR start, LPCWSTR ptr, DWORD flags );
LPWSTR WINAPI CharPrevW_k(LPCWSTR start,LPCWSTR x);
BOOL WINAPI CharToOemA_k( LPCSTR s, LPSTR d );
BOOL WINAPI CharToOemBuffA_k( LPCSTR s, LPSTR d, DWORD len );
BOOL WINAPI CharToOemBuffW_k( LPCWSTR s, LPSTR d, DWORD len );
BOOL WINAPI CharToOemW_k( LPCWSTR s, LPSTR d );
INT WINAPI CompareStringA_k(LCID lcid, DWORD flags, LPCSTR str1, INT len1, LPCSTR str2, INT len2);
INT WINAPI CompareStringEx_k(LPCWSTR locale, DWORD flags, LPCWSTR str1, INT len1, LPCWSTR str2, INT len2, void* version, LPVOID reserved, LPARAM lParam);
INT WINAPI CompareStringW_k(LCID lcid, DWORD flags, LPCWSTR str1, INT len1, LPCWSTR str2, INT len2);
INT WINAPI CompareStringOrdinal_k(const WCHAR *str1, INT len1, const WCHAR *str2, INT len2, BOOL ignore_case);
LCID WINAPI ConvertDefaultLocale_k( LCID lcid );
BOOL WINAPI EnumSystemCodePagesA_k( CODEPAGE_ENUMPROCA lpfnCodePageEnum, DWORD flags );
BOOL WINAPI EnumSystemCodePagesW_k( CODEPAGE_ENUMPROCW lpfnCodePageEnum, DWORD flags );
BOOL WINAPI EnumSystemLocalesA_k( LOCALE_ENUMPROCA lpfnLocaleEnum, DWORD dwFlags );
BOOL WINAPI EnumSystemLocalesW_k( LOCALE_ENUMPROCW lpfnLocaleEnum, DWORD dwFlags );
BOOL WINAPI EnumSystemLocalesEx_k( LOCALE_ENUMPROCEX proc, DWORD flags, LPARAM lparam, LPVOID reserved );
BOOL WINAPI EnumUILanguagesA_k(UILANGUAGE_ENUMPROCA pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam);
BOOL WINAPI EnumUILanguagesW_k(UILANGUAGE_ENUMPROCW pUILangEnumProc, DWORD dwFlags, LONG_PTR lParam);
BOOL WINAPI FileTimeToSystemTime_k( const FILETIME *ft, LPSYSTEMTIME syst );
INT WINAPI FoldStringA_k(DWORD dwFlags, LPCSTR src, INT srclen, LPSTR dst, INT dstlen);
INT WINAPI FoldStringW_k(DWORD dwFlags, LPCWSTR src, INT srclen, LPWSTR dst, INT dstlen);                       
UINT WINAPI GetACP_k();
int WINAPI GetCalendarInfoW_k(LCID Locale, CALID Calendar, CALTYPE CalType, LPWSTR lpCalData, int cchData, LPDWORD lpValue);
BOOL WINAPI GetCPInfo_k( UINT codepage, LPCPINFO cpinfo );
BOOL WINAPI GetCPInfoExA_k( UINT codepage, DWORD dwFlags, LPCPINFOEXA cpinfo );
BOOL WINAPI GetCPInfoExW_k( UINT codepage, DWORD dwFlags, LPCPINFOEXW cpinfo );
INT WINAPI GetGeoInfoA_k(GEOID GeoId, GEOTYPE GeoType, LPSTR lpGeoData, int cchData, LANGID language);
INT WINAPI GetGeoInfoW_k(GEOID GeoId, GEOTYPE GeoType, LPWSTR lpGeoData, int cchData, LANGID language);
DWORD WINAPI GetLastError_k();
INT WINAPI GetLocaleInfoA_k( LCID lcid, LCTYPE lctype, LPSTR buffer, INT len );
INT WINAPI GetLocaleInfoW_k( LCID lcid, LCTYPE lctype, LPWSTR buffer, INT len );
UINT WINAPI GetOEMCP_k(void);
BOOL WINAPI GetStringTypeA_k( LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype );
BOOL WINAPI GetStringTypeW_k( DWORD type, LPCWSTR src, INT count, LPWORD chartype );
BOOL WINAPI GetStringTypeExW_k( LCID locale, DWORD type, LPCWSTR src, INT count, LPWORD chartype );
BOOL WINAPI GetStringTypeExA_k( LCID locale, DWORD type, LPCSTR src, INT count, LPWORD chartype );
LANGID WINAPI GetSystemDefaultLangID_k(void);
LCID WINAPI GetSystemDefaultLCID_k(void);
INT WINAPI GetSystemDefaultLocaleName_k(LPWSTR localename, INT len);
VOID WINAPI GetSystemTimeAsFileTime_k(LPFILETIME time);
LCID WINAPI GetThreadLocale_k(void);
DWORD WINAPI GetTimeZoneInformation_k( LPTIME_ZONE_INFORMATION tzinfo );
LANGID WINAPI GetUserDefaultLangID(void);
LCID WINAPI GetUserDefaultLCID_k(void);
INT WINAPI GetUserDefaultLocaleName_k(LPWSTR localename, int buffersize);
LANGID WINAPI GetUserDefaultUILanguage_k(void);
INT WINAPI IdnToAscii_k(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar, LPWSTR lpASCIICharStr, INT cchASCIIChar);
INT WINAPI IdnToNameprepUnicode_k(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar, LPWSTR lpNameprepCharStr, INT cchNameprepChar);
BOOL WINAPI InvalidateNLSCache_k(void);
BOOL WINAPI IsDBCSLeadByte_k( BYTE testchar );
BOOL WINAPI IsDBCSLeadByteEx_k( UINT codepage, BYTE testchar );
BOOL WINAPI IsCharAlphaA_k(CHAR x);
BOOL WINAPI IsCharAlphaW_k(WCHAR x);
BOOL WINAPI IsCharAlphaNumericA_k(CHAR x);
BOOL WINAPI IsCharAlphaNumericW_k(WCHAR x);
BOOL WINAPI IsCharLowerA_k(CHAR x);
BOOL WINAPI IsCharLowerW_k(WCHAR x);
BOOL WINAPI IsCharUpperA_k(CHAR x);
BOOL WINAPI IsCharUpperW_k(WCHAR x);
BOOL WINAPI IsNormalizedString_k(NORM_FORM NormForm, LPCWSTR lpString, INT cwLength);
BOOL WINAPI IsValidCodePage_k( UINT codepage );
BOOL WINAPI IsValidLocale_k( LCID lcid, DWORD flags );
BOOL WINAPI IsValidLocaleName_k( LPCWSTR locale );
INT WINAPI LCIDToLocaleName_k( LCID lcid, LPWSTR name, INT count, DWORD flags );
INT WINAPI LCMapStringA_k(LCID lcid, DWORD flags, LPCSTR src, INT srclen, LPSTR dst, INT dstlen);
INT WINAPI LCMapStringEx_k(LPCWSTR name, DWORD flags, LPCWSTR src, INT srclen, LPWSTR dst, INT dstlen, void* version, LPVOID reserved, LPARAM lparam);
INT WINAPI LCMapStringW_k(LCID lcid, DWORD flags, LPCWSTR src, INT srclen, LPWSTR dst, INT dstlen);
LCID WINAPI LocaleNameToLCID_k( LPCWSTR name, DWORD flags );
int WINAPI lstrcmpA_k(LPCSTR str1, LPCSTR str2);
int WINAPI lstrcmpiA_k(LPCSTR str1, LPCSTR str2);
int WINAPI lstrcmpW_k(LPCWSTR str1, LPCWSTR str2);
int WINAPI lstrcmpiW_k(LPCWSTR str1, LPCWSTR str2);
INT WINAPI MultiByteToWideChar_k( UINT page, DWORD flags, LPCSTR src, INT srclen, LPWSTR dst, INT dstlen );
INT WINAPI NormalizeString_k(NORM_FORM NormForm, LPCWSTR lpSrcString, INT cwSrcLength, LPWSTR lpDstString, INT cwDstLength);
NTSTATUS WINAPI NtClose_k( HANDLE Handle );
NTSTATUS WINAPI NtCreateSemaphore_k(PHANDLE SemaphoreHandle, ACCESS_MASK access, const OBJECT_ATTRIBUTES *attr, LONG InitialCount, LONG MaximumCount );
NTSTATUS WINAPI NtQueryDefaultLocale_k( BOOLEAN user, LCID *lcid );
NTSTATUS WINAPI NtQueryDefaultUILanguage_k( LANGID *lang );
NTSTATUS WINAPI NtQueryInstallUILanguage_k( LANGID *lang );
NTSTATUS WINAPI NtQuerySystemTime_k( PLARGE_INTEGER Time );
NTSTATUS WINAPI NtReleaseSemaphore_k( HANDLE handle, ULONG count, PULONG previous );
BOOL WINAPI OemToCharA_k( LPCSTR s, LPSTR d );
BOOL WINAPI OemToCharBuffA_k( LPCSTR s, LPSTR d, DWORD len );
BOOL WINAPI OemToCharBuffW_k( LPCSTR s, LPWSTR d, DWORD len );
BOOL WINAPI OemToCharW_k( LPCSTR s, LPWSTR d );
NTSTATUS WINAPI RtlEnterCriticalSection_k( RTL_CRITICAL_SECTION *crit );
NTSTATUS WINAPI RtlInitializeCriticalSection_k( RTL_CRITICAL_SECTION *crit );
NTSTATUS WINAPI RtlInitializeCriticalSectionAndSpinCount( RTL_CRITICAL_SECTION *crit, ULONG spincount );
NTSTATUS WINAPI RtlInitializeCriticalSectionEx_k( RTL_CRITICAL_SECTION *crit, ULONG spincount, ULONG flags );
NTSTATUS WINAPI RtlLeaveCriticalSection_k( RTL_CRITICAL_SECTION *crit );
void WINAPI RtlRaiseStatus_k( NTSTATUS status );
BOOL WINAPI RtlTryEnterCriticalSection_k( RTL_CRITICAL_SECTION *crit );
NTSTATUS WINAPI RtlpUnWaitCriticalSection_k( RTL_CRITICAL_SECTION *crit );
UINT WINAPI SetCPGlobal_k( UINT acp );
void SetLastError_k(UINT error);
BOOL WINAPI SetLocaleInfoA_k(LCID lcid, LCTYPE lctype, LPCSTR data);
BOOL WINAPI SetLocaleInfoW_k( LCID lcid, LCTYPE lctype, LPCWSTR data );
BOOL WINAPI SetThreadLocale_k( LCID lcid );
LANGID WINAPI SetThreadUILanguage_k( LANGID langid );
BOOL WINAPI SystemTimeToFileTime_k( const SYSTEMTIME *syst, LPFILETIME ft );
DWORD WINAPI VerLanguageNameA_k( DWORD wLang, LPSTR szLang, DWORD nSize );
DWORD WINAPI VerLanguageNameW_k( DWORD wLang, LPWSTR szLang, DWORD nSize );
INT WINAPI WideCharToMultiByte_k( UINT page, DWORD flags, LPCWSTR src, INT srclen, LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used );

#define TEB_THREAD_ID 0x24
#define TEB_LAST_ERROR 0x34
#define TEB_LOCALE 0xC4
#define TEB_LAST_STATUS 0xBF4
#define TEB_STATIC_UNICODE_BUFFER 0xC00

DWORD NtCurrentTeb_k();
struct KThread* currentThread();
struct KProcess* currentProcess();

NTSTATUS WINAPI RtlGetLastNtStatus_k(void);
NTSTATUS WINAPI RtlLocalTimeToSystemTime_k( const LARGE_INTEGER *LocalTime, PLARGE_INTEGER SystemTime);
ULONG WINAPI RtlNtStatusToDosError_k( NTSTATUS status );
NTSTATUS WINAPI RtlQueryTimeZoneInformation_k(RTL_TIME_ZONE_INFORMATION *tzinfo);
void WINAPI RtlSecondsSince1970ToTime_k( DWORD Seconds, LARGE_INTEGER *Time );
ULONG WINAPI RtlNtStatusToDosErrorNoTeb_k( NTSTATUS status );
NTSTATUS WINAPI RtlSystemTimeToLocalTime_k( const LARGE_INTEGER *SystemTime, PLARGE_INTEGER LocalTime );
BOOLEAN WINAPI RtlTimeFieldsToTime_k(PTIME_FIELDS tfTimeFields, PLARGE_INTEGER Time);
BOOLEAN WINAPI RtlTimeToSecondsSince1970_k( const LARGE_INTEGER *Time, LPDWORD Seconds );
VOID WINAPI RtlTimeToTimeFields_k(const LARGE_INTEGER *liTime, PTIME_FIELDS TimeFields);

DWORD WINAPI GetCurrentThreadId_k(void);

#endif