#include "config.h"
#include "wine/port.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define _ADVAPI32_
#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"
#include "winternl.h"
#include "winuser.h"
#include "sddl.h"

#include "wine/unicode.h"
#include "wine/debug.h"

HMODULE advapi32dll;

typedef LSTATUS(WINAPI *pfnRegCloseKey)(HKEY hkey);
pfnRegCloseKey pRegCloseKey;

typedef LSTATUS(WINAPI *pfnRegCreateKeyExA)(HKEY hkey, LPCSTR name, DWORD reserved, LPSTR class, DWORD options, REGSAM access, SECURITY_ATTRIBUTES *sa, PHKEY retkey, LPDWORD dispos);
pfnRegCreateKeyExA pRegCreateKeyExA;

typedef LSTATUS(WINAPI *pfnRegCreateKeyExW)(HKEY hkey, LPCWSTR name, DWORD reserved, LPWSTR class, DWORD options, REGSAM access, SECURITY_ATTRIBUTES *sa, PHKEY retkey, LPDWORD dispos);
pfnRegCreateKeyExW pRegCreateKeyExW;

typedef LSTATUS(WINAPI *pfnRegDeleteKeyExA)(HKEY hkey, LPCSTR name, REGSAM access, DWORD reserved);
pfnRegDeleteKeyExA pRegDeleteKeyExA;

typedef LSTATUS(WINAPI *pfnRegDeleteKeyExW)(HKEY hkey, LPCWSTR name, REGSAM access, DWORD reserved);
pfnRegDeleteKeyExW pRegDeleteKeyExW;

typedef LSTATUS(WINAPI *pfnRegDeleteTreeA)(HKEY hKey, LPCSTR lpszSubKey);
pfnRegDeleteTreeA pRegDeleteTreeA;

typedef LSTATUS(WINAPI *pfnRegDeleteTreeW)(HKEY hKey, LPCWSTR lpszSubKey);
pfnRegDeleteTreeW pRegDeleteTreeW;

typedef LSTATUS(WINAPI *pfnRegDeleteValueA)(HKEY hkey, LPCSTR name);
pfnRegDeleteValueA pRegDeleteValueA;

typedef LSTATUS(WINAPI *pfnRegDeleteValueW)(HKEY hkey, LPCWSTR name);
pfnRegDeleteValueW pRegDeleteValueW;

typedef LSTATUS(WINAPI *pfnRegEnumKeyExA)(HKEY hkey, DWORD index, LPSTR name, LPDWORD name_len, LPDWORD reserved, LPSTR class, LPDWORD class_len, FILETIME *ft);
pfnRegEnumKeyExA pRegEnumKeyExA;

typedef LSTATUS(WINAPI *pfnRegEnumKeyExW)(HKEY hkey, DWORD index, LPWSTR name, LPDWORD name_len, LPDWORD reserved, LPWSTR class, LPDWORD class_len, FILETIME *ft);
pfnRegEnumKeyExW pRegEnumKeyExW;

typedef LSTATUS(WINAPI *pfnRegEnumValueA)(HKEY hkey, DWORD index, LPSTR value, LPDWORD val_count, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count);
pfnRegEnumValueA pRegEnumValueA;

typedef LSTATUS(WINAPI *pfnRegEnumValueW)(HKEY hkey, DWORD index, LPWSTR value, LPDWORD val_count, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count);
pfnRegEnumValueW pRegEnumValueW;

typedef LSTATUS(WINAPI *pfnRegFlushKey)(HKEY hkey);
pfnRegFlushKey pRegFlushKey;

typedef LSTATUS(WINAPI *pfnRegGetKeySecurity)(HKEY hkey, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, LPDWORD lpcbSecurityDescriptor);
pfnRegGetKeySecurity pRegGetKeySecurity;

typedef LSTATUS(WINAPI *pfnRegGetValueA)(HKEY hKey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);
pfnRegGetValueA pRegGetValueA;

typedef LSTATUS(WINAPI *pfnRegGetValueW)(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);
pfnRegGetValueW pRegGetValueW;

typedef LSTATUS(WINAPI *pfnRegLoadKeyA)(HKEY hkey, LPCSTR subkey, LPCSTR filename);
pfnRegLoadKeyA pRegLoadKeyA;

typedef LSTATUS(WINAPI *pfnRegLoadKeyW)(HKEY hkey, LPCWSTR subkey, LPCWSTR filename);
pfnRegLoadKeyW pRegLoadKeyW;

typedef LSTATUS(WINAPI *pfnRegLoadMUIStringA)(HKEY hKey, LPCSTR pszValue, LPSTR pszBuffer, DWORD cbBuffer, LPDWORD pcbData, DWORD dwFlags, LPCSTR pszBaseDir);
pfnRegLoadMUIStringA pRegLoadMUIStringA;

typedef LSTATUS(WINAPI *pfnRegLoadMUIStringW)(HKEY hKey, LPCWSTR pwszValue, LPWSTR pwszBuffer, DWORD cbBuffer, LPDWORD pcbData, DWORD dwFlags, LPCWSTR pwszBaseDir);
pfnRegLoadMUIStringW pRegLoadMUIStringW;

typedef LSTATUS(WINAPI *pfnRegNotifyChangeKeyValue)(HKEY hkey, BOOL fWatchSubTree, DWORD fdwNotifyFilter, HANDLE hEvent, BOOL fAsync);
pfnRegNotifyChangeKeyValue pRegNotifyChangeKeyValue;

typedef LSTATUS(WINAPI *pfnRegOpenCurrentUser)(REGSAM access, PHKEY retkey);
pfnRegOpenCurrentUser pRegOpenCurrentUser;

typedef LSTATUS(WINAPI *pfnRegOpenKeyExA)(HKEY hkey, LPCSTR name, DWORD options, REGSAM access, PHKEY retkey);
pfnRegOpenKeyExA pRegOpenKeyExA;

typedef LSTATUS(WINAPI *pfnRegOpenKeyExW)(HKEY hkey, LPCWSTR name, DWORD options, REGSAM access, PHKEY retkey);
pfnRegOpenKeyExW pRegOpenKeyExW;

typedef LSTATUS(WINAPI *pfnRegOpenUserClassesRoot)(HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY phkResult);
pfnRegOpenUserClassesRoot pRegOpenUserClassesRoot;

typedef LSTATUS(WINAPI *pfnRegQueryInfoKeyA)(HKEY hkey, LPSTR class, LPDWORD class_len, LPDWORD reserved, LPDWORD subkeys, LPDWORD max_subkey, LPDWORD max_class, LPDWORD values, LPDWORD max_value, LPDWORD max_data, LPDWORD security, FILETIME *modif);
pfnRegQueryInfoKeyA pRegQueryInfoKeyA;

typedef LSTATUS(WINAPI *pfnRegQueryInfoKeyW)(HKEY hkey, LPWSTR class, LPDWORD class_len, LPDWORD reserved, LPDWORD subkeys, LPDWORD max_subkey, LPDWORD max_class, LPDWORD values, LPDWORD max_value, LPDWORD max_data, LPDWORD security, FILETIME *modif);
pfnRegQueryInfoKeyW pRegQueryInfoKeyW;

typedef LSTATUS(WINAPI *pfnRegQueryValueExA)(HKEY hkey, LPCSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count);
pfnRegQueryValueExA pRegQueryValueExA;

typedef LSTATUS(WINAPI *pfnRegQueryValueExW)(HKEY hkey, LPCWSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count);
pfnRegQueryValueExW pRegQueryValueExW;

typedef LSTATUS(WINAPI *pfnRegRestoreKeyA)(HKEY hkey, LPCSTR lpFile, DWORD dwFlags);
pfnRegRestoreKeyA pRegRestoreKeyA;

typedef LSTATUS(WINAPI *pfnRegRestoreKeyW)(HKEY hkey, LPCWSTR lpFile, DWORD dwFlags);
pfnRegRestoreKeyW pRegRestoreKeyW;

typedef LSTATUS(WINAPI *pfnRegSetKeySecurity)(HKEY hkey, SECURITY_INFORMATION SecurityInfo, PSECURITY_DESCRIPTOR pSecurityDesc);
pfnRegSetKeySecurity pRegSetKeySecurity;

typedef LSTATUS(WINAPI *pfnRegSetValueExA)(HKEY hkey, LPCSTR name, DWORD reserved, DWORD type, const BYTE *data, DWORD count);
pfnRegSetValueExA pRegSetValueExA;

typedef LSTATUS(WINAPI *pfnRegSetValueExW)(HKEY hkey, LPCWSTR name, DWORD reserved, DWORD type, const BYTE *data, DWORD count);
pfnRegSetValueExW pRegSetValueExW;

typedef LSTATUS(WINAPI *pfnRegUnLoadKeyA)(HKEY hkey, LPCSTR lpSubKey);
pfnRegUnLoadKeyA pRegUnLoadKeyA;

typedef LSTATUS(WINAPI *pfnRegUnLoadKeyW)(HKEY hkey, LPCWSTR lpSubKey);
pfnRegUnLoadKeyW pRegUnLoadKeyW;

HMODULE getAdvapi32() {
    if (advapi32dll)
        return advapi32dll;
    advapi32dll = LoadLibraryA("advapi32.dll");
    return advapi32dll;
}
LSTATUS WINAPI RegCloseKey(HKEY hkey) {
    if (!pRegCloseKey)
        pRegCloseKey = (pfnRegCloseKey)GetProcAddress(getAdvapi32(), "RegCloseKey");
    return pRegCloseKey(hkey);
}

LSTATUS WINAPI RegCreateKeyExA(HKEY hkey, LPCSTR name, DWORD reserved, LPSTR class, DWORD options, REGSAM access, SECURITY_ATTRIBUTES *sa, PHKEY retkey, LPDWORD dispos) {
    if (!pRegCreateKeyExA)
        pRegCreateKeyExA = (pfnRegCreateKeyExA)GetProcAddress(getAdvapi32(), "RegCreateKeyExA");
    return pRegCreateKeyExA(hkey, name, reserved, class, options, access, sa, retkey, dispos);
}

LSTATUS WINAPI RegCreateKeyExW(HKEY hkey, LPCWSTR name, DWORD reserved, LPWSTR class, DWORD options, REGSAM access, SECURITY_ATTRIBUTES *sa, PHKEY retkey, LPDWORD dispos) {
    if (!pRegCreateKeyExW)
        pRegCreateKeyExW = (pfnRegCreateKeyExW)GetProcAddress(getAdvapi32(), "RegCreateKeyExW");
    return pRegCreateKeyExW(hkey, name, reserved, class, options, access, sa, retkey, dispos);
}

LSTATUS WINAPI RegDeleteKeyExA(HKEY hkey, LPCSTR name, REGSAM access, DWORD reserved) {
    if (!pRegDeleteKeyExA)
        pRegDeleteKeyExA = (pfnRegDeleteKeyExA)GetProcAddress(getAdvapi32(), "RegDeleteKeyExA");
    return pRegDeleteKeyExA(hkey, name, access, reserved);
}

LSTATUS WINAPI RegDeleteKeyExW(HKEY hkey, LPCWSTR name, REGSAM access, DWORD reserved) {
    if (!pRegDeleteKeyExW)
        pRegDeleteKeyExW = (pfnRegDeleteKeyExW)GetProcAddress(getAdvapi32(), "RegDeleteKeyExW");
    return pRegDeleteKeyExW(hkey, name, access, reserved);
}

LSTATUS WINAPI RegDeleteTreeA(HKEY hKey, LPCSTR lpszSubKey) {
    if (!pRegDeleteTreeA)
        pRegDeleteTreeA = (pfnRegDeleteTreeA)GetProcAddress(getAdvapi32(), "RegDeleteTreeA");
    return pRegDeleteTreeA(hKey, lpszSubKey);
}
LSTATUS WINAPI RegDeleteTreeW(HKEY hKey, LPCWSTR lpszSubKey) {
    if (!pRegDeleteTreeW)
        pRegDeleteTreeW = (pfnRegDeleteTreeW)GetProcAddress(getAdvapi32(), "RegDeleteTreeW");
    return pRegDeleteTreeW(hKey, lpszSubKey);
}

LSTATUS WINAPI RegDeleteValueA(HKEY hkey, LPCSTR name) {
    if (!pRegDeleteValueA)
        pRegDeleteValueA = (pfnRegDeleteValueA)GetProcAddress(getAdvapi32(), "RegDeleteValueA");
    return pRegDeleteValueA(hkey, name);
}

LSTATUS WINAPI RegDeleteValueW(HKEY hkey, LPCWSTR name) {
    if (!pRegDeleteValueW)
        pRegDeleteValueW = (pfnRegDeleteValueW)GetProcAddress(getAdvapi32(), "RegDeleteValueW");
    return pRegDeleteValueW(hkey, name);
}

LSTATUS WINAPI RegEnumKeyExA(HKEY hkey, DWORD index, LPSTR name, LPDWORD name_len, LPDWORD reserved, LPSTR class, LPDWORD class_len, FILETIME *ft) {
    if (!pRegEnumKeyExA)
        pRegEnumKeyExA = (pfnRegEnumKeyExA)GetProcAddress(getAdvapi32(), "RegEnumKeyExA");
    return pRegEnumKeyExA(hkey, index, name, name_len, reserved, class, class_len, ft);
}

LSTATUS WINAPI RegEnumKeyExW(HKEY hkey, DWORD index, LPWSTR name, LPDWORD name_len, LPDWORD reserved, LPWSTR class, LPDWORD class_len, FILETIME *ft) {
    if (!pRegEnumKeyExW)
        pRegEnumKeyExW = (pfnRegEnumKeyExW)GetProcAddress(getAdvapi32(), "RegEnumKeyExW");
    return pRegEnumKeyExW(hkey, index, name, name_len, reserved, class, class_len, ft);
}

LSTATUS WINAPI RegEnumValueA(HKEY hkey, DWORD index, LPSTR value, LPDWORD val_count, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count) {
    if (!pRegEnumValueA)
        pRegEnumValueA = (pfnRegEnumValueA)GetProcAddress(getAdvapi32(), "RegEnumValueA");
    return pRegEnumValueA(hkey, index, value, val_count, reserved, type, data, count);
}

LSTATUS WINAPI RegEnumValueW(HKEY hkey, DWORD index, LPWSTR value, LPDWORD val_count, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count) {
    if (!pRegEnumValueW)
        pRegEnumValueW = (pfnRegEnumValueW)GetProcAddress(getAdvapi32(), "RegEnumValueW");
    return pRegEnumValueW(hkey, index, value, val_count, reserved, type, data, count);
}

LSTATUS WINAPI RegFlushKey(HKEY hkey) {
    if (!pRegFlushKey)
        pRegFlushKey = (pfnRegFlushKey)GetProcAddress(getAdvapi32(), "RegFlushKey");
    return pRegFlushKey(hkey);
}

LSTATUS WINAPI RegGetKeySecurity(HKEY hkey, SECURITY_INFORMATION SecurityInformation, PSECURITY_DESCRIPTOR pSecurityDescriptor, LPDWORD lpcbSecurityDescriptor) {
    if (!pRegGetKeySecurity)
        pRegGetKeySecurity = (pfnRegGetKeySecurity)GetProcAddress(getAdvapi32(), "RegGetKeySecurity");
    return pRegGetKeySecurity(hkey, SecurityInformation, pSecurityDescriptor, lpcbSecurityDescriptor);
}

LSTATUS WINAPI RegGetValueA(HKEY hKey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (!pRegGetValueA)
        pRegGetValueA = (pfnRegGetValueA)GetProcAddress(getAdvapi32(), "RegGetValueA");
    return pRegGetValueA(hKey, pszSubKey, pszValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS WINAPI RegGetValueW(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (!pRegGetValueW)
        pRegGetValueW = (pfnRegGetValueW)GetProcAddress(getAdvapi32(), "RegGetValueW");
    return pRegGetValueW(hKey, pszSubKey, pszValue, dwFlags, pdwType, pvData, pcbData);
}

LSTATUS WINAPI RegLoadKeyA(HKEY hkey, LPCSTR subkey, LPCSTR filename) {
    if (!pRegLoadKeyA)
        pRegLoadKeyA = (pfnRegLoadKeyA)GetProcAddress(getAdvapi32(), "RegLoadKeyA");
    return pRegLoadKeyA(hkey, subkey, filename);
}

LSTATUS WINAPI RegLoadKeyW(HKEY hkey, LPCWSTR subkey, LPCWSTR filename) {
    if (!pRegLoadKeyW)
        pRegLoadKeyW = (pfnRegLoadKeyW)GetProcAddress(getAdvapi32(), "RegLoadKeyW");
    return pRegLoadKeyW(hkey, subkey, filename);
}

LSTATUS WINAPI RegLoadMUIStringA(HKEY hKey, LPCSTR pszValue, LPSTR pszBuffer, DWORD cbBuffer, LPDWORD pcbData, DWORD dwFlags, LPCSTR pszBaseDir) {
    if (!pRegLoadMUIStringA)
        pRegLoadMUIStringA = (pfnRegLoadMUIStringA)GetProcAddress(getAdvapi32(), "RegLoadMUIStringA");
    return pRegLoadMUIStringA(hKey, pszValue, pszBuffer, cbBuffer, pcbData, dwFlags, pszBaseDir);
}

LSTATUS WINAPI RegLoadMUIStringW(HKEY hKey, LPCWSTR pwszValue, LPWSTR pwszBuffer, DWORD cbBuffer, LPDWORD pcbData, DWORD dwFlags, LPCWSTR pwszBaseDir) {
    if (!pRegLoadMUIStringW)
        pRegLoadMUIStringW = (pfnRegLoadMUIStringW)GetProcAddress(getAdvapi32(), "RegLoadMUIStringW");
    return pRegLoadMUIStringW(hKey, pwszValue, pwszBuffer, cbBuffer, pcbData, dwFlags, pwszBaseDir);
}

LSTATUS WINAPI RegNotifyChangeKeyValue(HKEY hkey, BOOL fWatchSubTree, DWORD fdwNotifyFilter, HANDLE hEvent, BOOL fAsync) {
    if (!pRegNotifyChangeKeyValue)
        pRegNotifyChangeKeyValue = (pfnRegNotifyChangeKeyValue)GetProcAddress(getAdvapi32(), "RegNotifyChangeKeyValue");
    return pRegNotifyChangeKeyValue(hkey, fWatchSubTree, fdwNotifyFilter, hEvent, fAsync);
}

LSTATUS WINAPI RegOpenCurrentUser(REGSAM access, PHKEY retkey) {
    if (!pRegOpenCurrentUser)
        pRegOpenCurrentUser = (pfnRegOpenCurrentUser)GetProcAddress(getAdvapi32(), "RegOpenCurrentUser");
    return pRegOpenCurrentUser(access, retkey);
}

LSTATUS WINAPI RegOpenKeyExA(HKEY hkey, LPCSTR name, DWORD options, REGSAM access, PHKEY retkey) {
    if (!pRegOpenKeyExA)
        pRegOpenKeyExA = (pfnRegOpenKeyExA)GetProcAddress(getAdvapi32(), "RegOpenKeyExA");
    return pRegOpenKeyExA(hkey, name, options, access, retkey);
}

LSTATUS WINAPI RegOpenKeyExW(HKEY hkey, LPCWSTR name, DWORD options, REGSAM access, PHKEY retkey) {
    if (!pRegOpenKeyExW)
        pRegOpenKeyExW = (pfnRegOpenKeyExW)GetProcAddress(getAdvapi32(), "RegOpenKeyExW");
    return pRegOpenKeyExW(hkey, name, options, access, retkey);
}

LSTATUS WINAPI RegOpenUserClassesRoot(HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY phkResult) {
    if (!pRegOpenUserClassesRoot)
        pRegOpenUserClassesRoot = (pfnRegOpenUserClassesRoot)GetProcAddress(getAdvapi32(), "RegOpenUserClassesRoot");
    return pRegOpenUserClassesRoot(hToken, dwOptions, samDesired, phkResult);
}

LSTATUS WINAPI RegQueryInfoKeyA(HKEY hkey, LPSTR class, LPDWORD class_len, LPDWORD reserved, LPDWORD subkeys, LPDWORD max_subkey, LPDWORD max_class, LPDWORD values, LPDWORD max_value, LPDWORD max_data, LPDWORD security, FILETIME *modif) {
    if (!pRegQueryInfoKeyA)
        pRegQueryInfoKeyA = (pfnRegQueryInfoKeyA)GetProcAddress(getAdvapi32(), "RegQueryInfoKeyA");
    return pRegQueryInfoKeyA(hkey, class, class_len, reserved, subkeys, max_subkey, max_class, values, max_value, max_data, security, modif);
}

LSTATUS WINAPI RegQueryInfoKeyW(HKEY hkey, LPWSTR class, LPDWORD class_len, LPDWORD reserved, LPDWORD subkeys, LPDWORD max_subkey, LPDWORD max_class, LPDWORD values, LPDWORD max_value, LPDWORD max_data, LPDWORD security, FILETIME *modif) {
    if (!pRegQueryInfoKeyW)
        pRegQueryInfoKeyW = (pfnRegQueryInfoKeyW)GetProcAddress(getAdvapi32(), "RegQueryInfoKeyW");
    return pRegQueryInfoKeyW(hkey, class, class_len, reserved, subkeys, max_subkey, max_class, values, max_value, max_data, security, modif);
}

LSTATUS WINAPI RegQueryValueExA(HKEY hkey, LPCSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count) {
    if (!pRegQueryValueExA)
        pRegQueryValueExA = (pfnRegQueryValueExA)GetProcAddress(getAdvapi32(), "RegQueryValueExA");
    return pRegQueryValueExA(hkey, name, reserved, type, data, count);
}

LSTATUS WINAPI RegQueryValueExW(HKEY hkey, LPCWSTR name, LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count) {
    if (!pRegQueryValueExW)
        pRegQueryValueExW = (pfnRegQueryValueExW)GetProcAddress(getAdvapi32(), "RegQueryValueExW");
    return pRegQueryValueExW(hkey, name, reserved, type, data, count);
}

LSTATUS WINAPI RegRestoreKeyA(HKEY hkey, LPCSTR lpFile, DWORD dwFlags) {
    if (!pRegRestoreKeyA)
        pRegRestoreKeyA = (pfnRegRestoreKeyA)GetProcAddress(getAdvapi32(), "RegRestoreKeyA");
    return pRegRestoreKeyA(hkey, lpFile, dwFlags);
}

LSTATUS WINAPI RegRestoreKeyW(HKEY hkey, LPCWSTR lpFile, DWORD dwFlags) {
    if (!pRegRestoreKeyW)
        pRegRestoreKeyW = (pfnRegRestoreKeyW)GetProcAddress(getAdvapi32(), "RegRestoreKeyW");
    return pRegRestoreKeyW(hkey, lpFile, dwFlags);
}

LSTATUS WINAPI RegSetKeySecurity(HKEY hkey, SECURITY_INFORMATION SecurityInfo, PSECURITY_DESCRIPTOR pSecurityDesc) {
    if (!pRegSetKeySecurity)
        pRegSetKeySecurity = (pfnRegSetKeySecurity)GetProcAddress(getAdvapi32(), "RegSetKeySecurity");
    return pRegSetKeySecurity(hkey, SecurityInfo, pSecurityDesc);
}

LSTATUS WINAPI RegSetValueExA(HKEY hkey, LPCSTR name, DWORD reserved, DWORD type, const BYTE *data, DWORD count) {
    if (!pRegSetValueExA)
        pRegSetValueExA = (pfnRegSetValueExA)GetProcAddress(getAdvapi32(), "RegSetValueExA");
    return pRegSetValueExA(hkey, name, reserved, type, data, count);
}

LSTATUS WINAPI RegSetValueExW(HKEY hkey, LPCWSTR name, DWORD reserved, DWORD type, const BYTE *data, DWORD count) {
    if (!pRegSetValueExW)
        pRegSetValueExW = (pfnRegSetValueExW)GetProcAddress(getAdvapi32(), "RegSetValueExW");
    return pRegSetValueExW(hkey, name, reserved, type, data, count);
}

LSTATUS WINAPI RegUnLoadKeyA(HKEY hkey, LPCSTR lpSubKey) {
    if (!pRegUnLoadKeyA)
        pRegUnLoadKeyA = (pfnRegUnLoadKeyA)GetProcAddress(getAdvapi32(), "RegUnLoadKeyA");
    return pRegUnLoadKeyA(hkey, lpSubKey);
}

LSTATUS WINAPI RegUnLoadKeyW(HKEY hkey, LPCWSTR lpSubKey) {
    if (!pRegUnLoadKeyW)
        pRegUnLoadKeyW = (pfnRegUnLoadKeyW)GetProcAddress(getAdvapi32(), "RegUnLoadKeyA");
    return pRegUnLoadKeyW(hkey, lpSubKey);
}