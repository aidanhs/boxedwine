#include "config.h"
#include "wine/port.h"

#include <stdlib.h>
#include "windef.h"
#include "winbase.h"
#include "winternl.h"

HMODULE advapi32;
HMODULE getAdvapi32() {
	if (!advapi32)
		advapi32 = LoadLibraryA("advapi32.dll");
	return advapi32;
}

typedef int (*pfn1)(int);
typedef int (*pfn2)(int,int);
typedef int (*pfn3)(int,int,int);
typedef int (*pfn4)(int,int,int,int);
typedef int (*pfn5)(int,int,int,int,int);
typedef int (*pfn6)(int,int,int,int,int,int);
typedef int (*pfn7)(int,int,int,int,int,int,int);
typedef int (*pfn8)(int,int,int,int,int,int,int,int);
typedef int (*pfn9)(int,int,int,int,int,int,int,int,int);
typedef int (*pfn12)(int,int,int,int,int,int,int,int,int,int,int,int);
#define FUNC1(x) pfn1 pfn##x; int advapi32##x(int i) {if (!pfn##x) pfn##x=(pfn1)GetProcAddress(getAdvapi32(), #x); return pfn##x(i);}
#define FUNC2(x) pfn2 pfn##x; int advapi32##x(int i1, int i2) {if (!pfn##x) pfn##x=(pfn2)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2);}
#define FUNC3(x) pfn3 pfn##x; int advapi32##x(int i1, int i2, int i3) {if (!pfn##x) pfn##x=(pfn3)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3);}
#define FUNC4(x) pfn4 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4) {if (!pfn##x) pfn##x=(pfn4)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4);}
#define FUNC5(x) pfn5 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5) {if (!pfn##x) pfn##x=(pfn5)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5);}
#define FUNC6(x) pfn6 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5, int i6) {if (!pfn##x) pfn##x=(pfn6)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5, i6);}
#define FUNC7(x) pfn7 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5, int i6, int i7) {if (!pfn##x) pfn##x=(pfn7)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5, i6, i7);}
#define FUNC8(x) pfn8 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8) {if (!pfn##x) pfn##x=(pfn8)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5, i6, i7, i8);}
#define FUNC9(x) pfn9 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9) {if (!pfn##x) pfn##x=(pfn9)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5, i6, i7, i8, i9);}
#define FUNC12(x) pfn12 pfn##x; int advapi32##x(int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10, int i11, int i12) {if (!pfn##x) pfn##x=(pfn12)GetProcAddress(getAdvapi32(), #x); return pfn##x(i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);}

FUNC1(RegCloseKey)
FUNC9(RegCreateKeyExA)
FUNC9(RegCreateKeyExW)
FUNC4(RegDeleteKeyExA)
FUNC4(RegDeleteKeyExW)
FUNC2(RegDeleteTreeA)
FUNC2(RegDeleteTreeW)
FUNC2(RegDeleteValueA)
FUNC2(RegDeleteValueW)
FUNC8(RegEnumKeyExA)
FUNC8(RegEnumKeyExW)
FUNC8(RegEnumValueA)
FUNC8(RegEnumValueW)
FUNC1(RegFlushKey)
FUNC4(RegGetKeySecurity)
FUNC7(RegGetValueA)
FUNC7(RegGetValueW)
FUNC3(RegLoadKeyA)
FUNC3(RegLoadKeyW)
FUNC7(RegLoadMUIStringA)
FUNC7(RegLoadMUIStringW)
FUNC5(RegNotifyChangeKeyValue)
FUNC2(RegOpenCurrentUser)
FUNC5(RegOpenKeyExA)
FUNC5(RegOpenKeyExW)
FUNC4(RegOpenUserClassesRoot)
FUNC12(RegQueryInfoKeyA)
FUNC12(RegQueryInfoKeyW)
FUNC6(RegQueryValueExA)
FUNC6(RegQueryValueExW)
FUNC3(RegRestoreKeyA)
FUNC3(RegRestoreKeyW)
FUNC3(RegSetKeySecurity)
FUNC6(RegSetValueExA)
FUNC6(RegSetValueExW)
FUNC2(RegUnLoadKeyA)
FUNC2(RegUnLoadKeyW)