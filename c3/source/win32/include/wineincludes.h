#ifndef __WINEINCLUDES_H__
#define __WINEINCLUDES_H__

#undef BYTE
#undef WORD
#undef DWORD
#undef LONG

#include "kalloc.h"
#include "pbl.h"

#define DUMMYUNIONNAME
#define DUMMYSTRUCTNAME
#define CALLBACK __stdcall
#define CDECL __cdecl
#define CONST const 
#define DECLSPEC_HIDDEN
#define WINAPIV     __cdecl
#define __TRY __try
#define __EXCEPT(func) __except((func)(GetExceptionInformation()))
#define __FINALLY(func) __finally { (func)(!AbnormalTermination()); }
#define __ENDTRY /*nothing*/
#define __EXCEPT_PAGE_FAULT __except(GetExceptionCode_k() == STATUS_ACCESS_VIOLATION)
#define __EXCEPT_ALL __except(EXCEPTION_EXECUTE_HANDLER)

#  define __ms_va_list va_list
#  define __ms_va_start(list,arg) va_start(list,arg)
#  define __ms_va_end(list) va_end(list)

#define WINELIB_NAME_AW(func) \
    func##_must_be_suffixed_with_W_or_A_in_this_context \
    func##_must_be_suffixed_with_W_or_A_in_this_context

#undef strcasecmp
int strcasecmp( const char *str1, const char *str2 );
void *interlocked_cmpxchg_ptr( void **dest, void *xchg, void *compare );
int interlocked_xchg_add( int *dest, int incr );
int interlocked_cmpxchg( int *dest, int xchg, int compare );

#define debugstr_w(x) ""
#define debugstr_a(x) x

#define WARN printf
#define TRACE printf
#define FIXME_(x) printf
#define ERR_(x) printf
#define TRACE_(x) printf
#define WARN_(x) printf

#define FALSE 0
#define TRUE 1

typedef void                                   *LPVOID;
typedef const void                             *LPCVOID;
typedef int             BOOL,       *PBOOL,    *LPBOOL;
typedef unsigned char   BYTE,       *PBYTE,    *LPBYTE;
typedef unsigned char   UCHAR,      *PUCHAR;
typedef unsigned short  WORD,       *PWORD,    *LPWORD;
typedef unsigned short  USHORT,     *PUSHORT;
typedef int             INT,        *PINT,     *LPINT;
typedef unsigned int    UINT,       *PUINT;
typedef float           FLOAT,      *PFLOAT;
typedef char                        *PSZ;
typedef int                                    *LPLONG;
typedef unsigned int    DWORD,      *PDWORD,   *LPDWORD;
typedef unsigned int    ULONG,      *PULONG;

typedef unsigned short  WCHAR,      *PWCHAR;
typedef long          INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef long          LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR     DWORD_PTR, *PDWORD_PTR;

#ifndef VOID
#define VOID void
#endif
typedef VOID           *PVOID;
typedef VOID           *PVOID64;
typedef BYTE            BOOLEAN,    *PBOOLEAN;
typedef char            CHAR,       *PCHAR;
typedef short           SHORT,      *PSHORT;
typedef int             LONG,       *PLONG;

/* ANSI string types */
typedef CHAR           *PCH,        *LPCH,      *PNZCH;
typedef const CHAR     *PCCH,       *LPCCH,     *PCNZCH;
typedef CHAR           *PSTR,       *LPSTR,     *NPSTR;
typedef const CHAR     *PCSTR,      *LPCSTR;
typedef CHAR           *PZZSTR;
typedef const CHAR     *PCZZSTR;

/* Unicode string types */
typedef const WCHAR    *PCWCHAR,    *LPCUWCHAR, *PCUWCHAR;
typedef WCHAR          *PWCH,       *LPWCH;
typedef const WCHAR    *PCWCH,      *LPCWCH;
typedef WCHAR          *PNZWCH,     *PUNZWCH;
typedef const WCHAR    *PCNZWCH,    *PCUNZWCH;
typedef WCHAR          *PWSTR,      *LPWSTR,    *NWPSTR;
typedef const WCHAR    *PCWSTR,     *LPCWSTR;
typedef WCHAR          *PZZWSTR,    *PUZZWSTR;
typedef const WCHAR    *PCZZWSTR,   *PCUZZWSTR;
typedef PWSTR          *PZPWSTR;
typedef PCWSTR         *PZPCWSTR;

typedef signed __int64   LONGLONG,  *PLONGLONG;
typedef unsigned __int64 ULONGLONG, *PULONGLONG;
typedef signed __int64 INT64;

typedef WORD            ATOM;
typedef DWORD           COLORREF, *LPCOLORREF;
typedef ULONGLONG DWORDLONG;

typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;

typedef void *HANDLE;
typedef HANDLE *PHANDLE, *LPHANDLE;

typedef HANDLE HMODULE;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;
typedef HANDLE GLOBALHANDLE;
typedef HANDLE LOCALHANDLE;
typedef HANDLE HCURSOR;
typedef HANDLE HRSRC;

#define DECLARE_HANDLE(a) typedef HANDLE a
#define DECL_WINELIB_TYPE_AW(x)

typedef int HFILE;
DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HCOLORSPACE);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HENHMETAFILE);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HGLRC);
DECLARE_HANDLE(HHOOK);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HKEY);
typedef HKEY *PHKEY;
DECLARE_HANDLE(HKL);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HWINEVENTHOOK);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HWND);

typedef LONG NTSTATUS;

/* Unicode char type flags */
#define	CT_CTYPE1		0x0001	/* usual ctype */
#define	CT_CTYPE2		0x0002	/* bidirectional layout info */
#define	CT_CTYPE3		0x0004	/* textprocessing info */

/* Type 1 flags */
#define C1_UPPER		0x0001
#define C1_LOWER		0x0002
#define C1_DIGIT		0x0004
#define C1_SPACE		0x0008
#define C1_PUNCT		0x0010
#define C1_CNTRL		0x0020
#define C1_BLANK		0x0040
#define C1_XDIGIT		0x0080
#define C1_ALPHA		0x0100
#define C1_DEFINED		0x0200

/* Type 2 flags */
#define	C2_LEFTTORIGHT		0x0001
#define	C2_RIGHTTOLEFT		0x0002
#define	C2_EUROPENUMBER		0x0003
#define	C2_EUROPESEPARATOR	0x0004
#define	C2_EUROPETERMINATOR	0x0005
#define	C2_ARABICNUMBER		0x0006
#define	C2_COMMONSEPARATOR	0x0007
#define	C2_BLOCKSEPARATOR	0x0008
#define	C2_SEGMENTSEPARATOR	0x0009
#define	C2_WHITESPACE		0x000A
#define	C2_OTHERNEUTRAL		0x000B
#define	C2_NOTAPPLICABLE	0x0000

/* Type 3 flags */
#define	C3_NONSPACING		0x0001
#define	C3_DIACRITIC		0x0002
#define	C3_VOWELMARK		0x0004
#define	C3_SYMBOL		0x0008
#define	C3_KATAKANA		0x0010
#define	C3_HIRAGANA		0x0020
#define	C3_HALFWIDTH		0x0040
#define	C3_FULLWIDTH		0x0080
#define	C3_IDEOGRAPH		0x0100
#define	C3_KASHIDA		0x0200
#define	C3_LEXICAL		0x0400
#define	C3_ALPHA		0x8000
#define	C3_NOTAPPLICABLE	0x0000

/* Code page information.
 */
#define MAX_LEADBYTES     12
#define MAX_DEFAULTCHAR   2

#ifndef NULL
#define NULL (void*)0
#endif

#define MB_COMPOSITE                0x02
#define MB_USEGLYPHCHARS            0x04
#define MB_ERR_INVALID_CHARS        0x08

#define CP_ACP        0
#define CP_OEMCP      1
#define CP_MACCP      2
#define CP_THREAD_ACP 3
#define CP_SYMBOL     42
#define CP_UTF7       65000
#define CP_UTF8       65001

#define CP_UNIXCP     65010 /* Wine extension */

#define CP_INSTALLED 0x1
#define CP_SUPPORTED 0x2

/* String mapping flags */
#define LCMAP_LOWERCASE  0x00000100	/* Make lower-case */
#define LCMAP_UPPERCASE  0x00000200	/* Make upper-case */
#define LCMAP_SORTKEY    0x00000400	/* Create a sort key */
#define LCMAP_BYTEREV    0x00000800	/* Reverse the result */

#define LCMAP_HIRAGANA   0x00100000	/* Transform Japanese katakana into hiragana */
#define LCMAP_KATAKANA   0x00200000	/* Transform Japanese hiragana into katakana */
#define LCMAP_HALFWIDTH  0x00400000	/* Use single byte chars in output */
#define LCMAP_FULLWIDTH  0x00800000	/* Use double byte chars in output */

#define LCMAP_LINGUISTIC_CASING   0x01000000 /* Change case by using language context */
#define LCMAP_SIMPLIFIED_CHINESE  0x02000000 /* Transform Chinese traditional into simplified */
#define LCMAP_TRADITIONAL_CHINESE 0x04000000 /* Transform Chinese simplified into traditional */

#define NORM_IGNORECASE            0x00000001
#define NORM_IGNORENONSPACE        0x00000002
#define NORM_IGNORESYMBOLS         0x00000004
#define SORT_DIGITSASNUMBERS       0x00000008
#define LINGUISTIC_IGNORECASE      0x00000010
#define LINGUISTIC_IGNOREDIACRITIC 0x00000020
#define SORT_STRINGSORT            0x00001000 /* Take punctuation into account */
#define NORM_IGNOREKANATYPE        0x00010000
#define NORM_IGNOREWIDTH           0x00020000
#define NORM_LINGUISTIC_CASING     0x08000000

/* Locale flags */
#define LOCALE_NOUSEROVERRIDE         0x80000000
#define LOCALE_USE_CP_ACP             0x40000000
#define LOCALE_RETURN_NUMBER          0x20000000
#define LOCALE_RETURN_GENITIVE_NAMES  0x10000000

#define WINAPI _stdcall

typedef UINT_PTR        WPARAM;
typedef LONG_PTR        LPARAM;
typedef LONG_PTR        LRESULT;

typedef char            CCHAR;
typedef DWORD           LCID,       *PLCID;
typedef WORD            LANGID;
typedef DWORD		EXECUTION_STATE;
#define LOCALE_IDEFAULTANSICODEPAGE 0x1004

typedef DWORD CALID;
typedef DWORD CALTYPE;
typedef LONG  GEOID;
typedef DWORD GEOCLASS;
typedef DWORD GEOTYPE;
typedef DWORD LCTYPE;
typedef DWORD LGRPID;

typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR SIZE_T, *PSIZE_T;

#define WC_DISCARDNS         0x0010
#define WC_SEPCHARS          0x0020
#define WC_DEFAULTCHAR       0x0040
#define WC_ERR_INVALID_CHARS 0x0080
#define WC_COMPOSITECHECK    0x0200
#define WC_NO_BEST_FIT_CHARS 0x0400

/* CompareString results */
#define CSTR_LESS_THAN    1
#define CSTR_EQUAL        2
#define CSTR_GREATER_THAN 3

typedef struct
{
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
} CPINFO, *LPCPINFO;

/*
 * RTL_SYSTEM_TIME and RTL_TIME_ZONE_INFORMATION are the same as
 * the SYSTEMTIME and TIME_ZONE_INFORMATION structures defined
 * in winbase.h, however we need to define them separately so
 * winternl.h doesn't depend on winbase.h.  They are used by
 * RtlQueryTimeZoneInformation and RtlSetTimeZoneInformation.
 * The names are guessed; if anybody knows the real names, let me know.
 */
typedef struct _RTL_SYSTEM_TIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} RTL_SYSTEM_TIME, *PRTL_SYSTEM_TIME;

typedef struct _RTL_TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[32];
    RTL_SYSTEM_TIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    RTL_SYSTEM_TIME DaylightDate;
    LONG DaylightBias;
} RTL_TIME_ZONE_INFORMATION, *PRTL_TIME_ZONE_INFORMATION;

/* Defines for calendar handling */
#define CAL_NOUSEROVERRIDE     LOCALE_NOUSEROVERRIDE
#define CAL_USE_CP_ACP         LOCALE_USE_CP_ACP
#define CAL_RETURN_NUMBER      LOCALE_RETURN_NUMBER

#define CAL_ICALINTVALUE       0x01
#define CAL_SCALNAME           0x02
#define CAL_IYEAROFFSETRANGE   0x03
#define CAL_SERASTRING         0x04
#define CAL_SSHORTDATE         0x05
#define CAL_SLONGDATE          0x06
#define CAL_SDAYNAME1          0x07
#define CAL_SDAYNAME2          0x08
#define CAL_SDAYNAME3          0x09
#define CAL_SDAYNAME4          0x0a
#define CAL_SDAYNAME5          0x0b
#define CAL_SDAYNAME6          0x0c
#define CAL_SDAYNAME7          0x0d
#define CAL_SABBREVDAYNAME1    0x0e
#define CAL_SABBREVDAYNAME2    0x0f
#define CAL_SABBREVDAYNAME3    0x10
#define CAL_SABBREVDAYNAME4    0x11
#define CAL_SABBREVDAYNAME5    0x12
#define CAL_SABBREVDAYNAME6    0x13
#define CAL_SABBREVDAYNAME7    0x14
#define CAL_SMONTHNAME1        0x15
#define CAL_SMONTHNAME2        0x16
#define CAL_SMONTHNAME3        0x17
#define CAL_SMONTHNAME4        0x18
#define CAL_SMONTHNAME5        0x19
#define CAL_SMONTHNAME6        0x1a
#define CAL_SMONTHNAME7        0x1b
#define CAL_SMONTHNAME8        0x1c
#define CAL_SMONTHNAME9        0x1d
#define CAL_SMONTHNAME10       0x1e
#define CAL_SMONTHNAME11       0x1f
#define CAL_SMONTHNAME12       0x20
#define CAL_SMONTHNAME13       0x21
#define CAL_SABBREVMONTHNAME1  0x22
#define CAL_SABBREVMONTHNAME2  0x23
#define CAL_SABBREVMONTHNAME3  0x24
#define CAL_SABBREVMONTHNAME4  0x25
#define CAL_SABBREVMONTHNAME5  0x26
#define CAL_SABBREVMONTHNAME6  0x27
#define CAL_SABBREVMONTHNAME7  0x28
#define CAL_SABBREVMONTHNAME8  0x29
#define CAL_SABBREVMONTHNAME9  0x2a
#define CAL_SABBREVMONTHNAME10 0x2b
#define CAL_SABBREVMONTHNAME11 0x2c
#define CAL_SABBREVMONTHNAME12 0x2d
#define CAL_SABBREVMONTHNAME13 0x2e
#define CAL_SYEARMONTH         0x2f
#define CAL_ITWODIGITYEARMAX   0x30

/* Calendar types */
#define CAL_GREGORIAN              1
#define CAL_GREGORIAN_US           2
#define CAL_JAPAN                  3
#define CAL_TAIWAN                 4
#define CAL_KOREA                  5
#define CAL_HIJRI                  6
#define CAL_THAI                   7
#define CAL_HEBREW                 8
#define CAL_GREGORIAN_ME_FRENCH    9
#define CAL_GREGORIAN_ARABIC       10
#define CAL_GREGORIAN_XLIT_ENGLISH 11
#define CAL_GREGORIAN_XLIT_FRENCH  12

#define LOCALE_ALL                  0x00
#define LOCALE_WINDOWS              0x01
#define LOCALE_SUPPLEMENTAL         0x02
#define LOCALE_ALTERNATE_SORTS      0x04
#define LOCALE_REPLACEMENT          0x08
#define LOCALE_NEUTRALDATA          0x10
#define LOCALE_SPECIFICDATA         0x20

/* Locale flags */
#define LOCALE_NOUSEROVERRIDE         0x80000000
#define LOCALE_USE_CP_ACP             0x40000000
#define LOCALE_RETURN_NUMBER          0x20000000
#define LOCALE_RETURN_GENITIVE_NAMES  0x10000000

/* Locale information types */
#define LOCALE_ILANGUAGE            0x0001
#define LOCALE_SLANGUAGE            0x0002
#define LOCALE_SENGLANGUAGE         0x1001
#define LOCALE_SENGLISHLANGUAGENAME 0x1001
#define LOCALE_SABBREVLANGNAME      0x0003
#define LOCALE_SNATIVELANGNAME      0x0004
#define LOCALE_SNATIVELANGUAGENAME  0x0004
#define LOCALE_ICOUNTRY             0x0005
#define LOCALE_SCOUNTRY             0x0006
#define LOCALE_SLOCALIZEDCOUNTRYNAME 0x0006
#define LOCALE_SENGCOUNTRY          0x1002
#define LOCALE_SENGLISHCOUNTRYNAME  0x1002
#define LOCALE_SABBREVCTRYNAME      0x0007
#define LOCALE_SNATIVECTRYNAME      0x0008
#define LOCALE_SNATIVECOUNTRYNAME   0x0008
#define LOCALE_IDEFAULTLANGUAGE     0x0009
#define LOCALE_IDEFAULTCOUNTRY      0x000A
#define LOCALE_IDEFAULTCODEPAGE     0x000B
#define LOCALE_IDEFAULTANSICODEPAGE 0x1004
#define LOCALE_IDEFAULTMACCODEPAGE  0x1011
#define LOCALE_SLIST                0x000C
#define LOCALE_IMEASURE             0x000D
#define LOCALE_SDECIMAL             0x000E
#define LOCALE_STHOUSAND            0x000F
#define LOCALE_SGROUPING            0x0010
#define LOCALE_IDIGITS              0x0011
#define LOCALE_ILZERO               0x0012
#define LOCALE_INEGNUMBER           0x1010
#define LOCALE_SNATIVEDIGITS        0x0013
#define LOCALE_SCURRENCY            0x0014
#define LOCALE_SINTLSYMBOL          0x0015
#define LOCALE_SMONDECIMALSEP       0x0016
#define LOCALE_SMONTHOUSANDSEP      0x0017
#define LOCALE_SMONGROUPING         0x0018
#define LOCALE_ICURRDIGITS          0x0019
#define LOCALE_IINTLCURRDIGITS      0x001A
#define LOCALE_ICURRENCY            0x001B
#define LOCALE_INEGCURR             0x001C
#define LOCALE_SDATE                0x001D
#define LOCALE_STIME                0x001E
#define LOCALE_SSHORTDATE           0x001F
#define LOCALE_SLONGDATE            0x0020
#define LOCALE_STIMEFORMAT          0x1003
#define LOCALE_IDATE                0x0021
#define LOCALE_ILDATE               0x0022
#define LOCALE_ITIME                0x0023
#define LOCALE_ITIMEMARKPOSN        0x1005
#define LOCALE_ICENTURY             0x0024
#define LOCALE_ITLZERO              0x0025
#define LOCALE_IDAYLZERO            0x0026
#define LOCALE_IMONLZERO            0x0027
#define LOCALE_S1159                0x0028
#define LOCALE_S2359                0x0029
#define LOCALE_ICALENDARTYPE        0x1009
#define LOCALE_IOPTIONALCALENDAR    0x100B
#define LOCALE_IFIRSTDAYOFWEEK      0x100C
#define LOCALE_IFIRSTWEEKOFYEAR     0x100D
#define LOCALE_SDAYNAME1            0x002A
#define LOCALE_SDAYNAME2            0x002B
#define LOCALE_SDAYNAME3            0x002C
#define LOCALE_SDAYNAME4            0x002D
#define LOCALE_SDAYNAME5            0x002E
#define LOCALE_SDAYNAME6            0x002F
#define LOCALE_SDAYNAME7            0x0030
#define LOCALE_SABBREVDAYNAME1      0x0031
#define LOCALE_SABBREVDAYNAME2      0x0032
#define LOCALE_SABBREVDAYNAME3      0x0033
#define LOCALE_SABBREVDAYNAME4      0x0034
#define LOCALE_SABBREVDAYNAME5      0x0035
#define LOCALE_SABBREVDAYNAME6      0x0036
#define LOCALE_SABBREVDAYNAME7      0x0037
#define LOCALE_SMONTHNAME1          0x0038
#define LOCALE_SMONTHNAME2          0x0039
#define LOCALE_SMONTHNAME3          0x003A
#define LOCALE_SMONTHNAME4          0x003B
#define LOCALE_SMONTHNAME5          0x003C
#define LOCALE_SMONTHNAME6          0x003D
#define LOCALE_SMONTHNAME7          0x003E
#define LOCALE_SMONTHNAME8          0x003F
#define LOCALE_SMONTHNAME9          0x0040
#define LOCALE_SMONTHNAME10         0x0041
#define LOCALE_SMONTHNAME11         0x0042
#define LOCALE_SMONTHNAME12         0x0043
#define LOCALE_SMONTHNAME13         0x100E
#define LOCALE_SABBREVMONTHNAME1    0x0044
#define LOCALE_SABBREVMONTHNAME2    0x0045
#define LOCALE_SABBREVMONTHNAME3    0x0046
#define LOCALE_SABBREVMONTHNAME4    0x0047
#define LOCALE_SABBREVMONTHNAME5    0x0048
#define LOCALE_SABBREVMONTHNAME6    0x0049
#define LOCALE_SABBREVMONTHNAME7    0x004A
#define LOCALE_SABBREVMONTHNAME8    0x004B
#define LOCALE_SABBREVMONTHNAME9    0x004C
#define LOCALE_SABBREVMONTHNAME10   0x004D
#define LOCALE_SABBREVMONTHNAME11   0x004E
#define LOCALE_SABBREVMONTHNAME12   0x004F
#define LOCALE_SABBREVMONTHNAME13   0x100F
#define LOCALE_SPOSITIVESIGN        0x0050
#define LOCALE_SNEGATIVESIGN        0x0051
#define LOCALE_IPOSSIGNPOSN         0x0052
#define LOCALE_INEGSIGNPOSN         0x0053
#define LOCALE_IPOSSYMPRECEDES      0x0054
#define LOCALE_IPOSSEPBYSPACE       0x0055
#define LOCALE_INEGSYMPRECEDES      0x0056
#define LOCALE_INEGSEPBYSPACE       0x0057
#define	LOCALE_FONTSIGNATURE        0x0058
#define LOCALE_SISO639LANGNAME      0x0059
#define LOCALE_SISO3166CTRYNAME     0x005A
#define LOCALE_IGEOID               0x005B
#define LOCALE_SNAME                0x005C
#define LOCALE_SDURATION            0x005D
#define LOCALE_SKEYBOARDSTOINSTALL  0x005E
#define LOCALE_SSHORTESTDAYNAME1    0x0060
#define LOCALE_SSHORTESTDAYNAME2    0x0061
#define LOCALE_SSHORTESTDAYNAME3    0x0062
#define LOCALE_SSHORTESTDAYNAME4    0x0063
#define LOCALE_SSHORTESTDAYNAME5    0x0064
#define LOCALE_SSHORTESTDAYNAME6    0x0065
#define LOCALE_SSHORTESTDAYNAME7    0x0066
#define LOCALE_SISO639LANGNAME2     0x0067
#define LOCALE_SISO3166CTRYNAME2    0x0068
#define LOCALE_SNAN                 0x0069
#define LOCALE_SPOSINFINITY         0x006A
#define LOCALE_SNEGINFINITY         0x006B
#define LOCALE_SSCRIPTS             0x006C
#define LOCALE_SPARENT              0x006D
#define LOCALE_SCONSOLEFALLBACKNAME 0x006E
#define LOCALE_SLANGDISPLAYNAME     0x006F
#define LOCALE_SLOCALIZEDLANGUAGENAME 0x006F
#define LOCALE_IREADINGLAYOUT       0x0070
#define LOCALE_INEUTRAL             0x0071
#define LOCALE_SENGLISHDISPLAYNAME  0x0072
#define LOCALE_SNATIVEDISPLAYNAME   0x0073
#define LOCALE_INEGATIVEPERCENT     0x0074
#define LOCALE_IPOSITIVEPERCENT     0x0075
#define LOCALE_SPERCENT             0x0076
#define LOCALE_SPERMILLE            0x0077
#define LOCALE_SMONTHDAY            0x0078
#define LOCALE_SSHORTTIME           0x0079
#define LOCALE_SOPENTYPELANGUAGETAG 0X007A
#define LOCALE_SSORTLOCALE          0x007B

#define LOCALE_IDEFAULTEBCDICCODEPAGE 0x1012
#define LOCALE_IPAPERSIZE             0x100A
#define LOCALE_SENGCURRNAME           0x1007
#define LOCALE_SNATIVECURRNAME        0x1008
#define LOCALE_SYEARMONTH             0x1006
#define LOCALE_SSORTNAME              0x1013
#define LOCALE_IDIGITSUBSTITUTION     0x1014

typedef CONST char *PCSZ;

typedef short CSHORT;
typedef CSHORT *PCSHORT;

typedef struct _STRING {
  USHORT Length;
  USHORT MaximumLength;
  PCHAR Buffer;
} STRING, *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef const STRING *PCANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef const STRING *PCOEM_STRING;

typedef struct _UNICODE_STRING {
  USHORT Length;        /* bytes */
  USHORT MaximumLength; /* bytes */
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _TIME_FIELDS
{   CSHORT Year;
    CSHORT Month;
    CSHORT Day;
    CSHORT Hour;
    CSHORT Minute;
    CSHORT Second;
    CSHORT Milliseconds;
    CSHORT Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

typedef struct _SYSTEMTIME{
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _TIME_ZONE_INFORMATION{
        LONG Bias;
        WCHAR StandardName[32];
        SYSTEMTIME StandardDate;
        LONG StandardBias;
        WCHAR DaylightName[32];
        SYSTEMTIME DaylightDate;
        LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

typedef struct _TIME_DYNAMIC_ZONE_INFORMATION
{
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
    WCHAR TimeZoneKeyName[128];
    BOOLEAN DynamicDaylightTimeDisabled;
} DYNAMIC_TIME_ZONE_INFORMATION, *PDYNAMIC_TIME_ZONE_INFORMATION;

typedef struct _FILETIME
{
#ifdef WORDS_BIGENDIAN
  DWORD  dwHighDateTime;
  DWORD  dwLowDateTime;
#else
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
#endif
} FILETIME, *PFILETIME, *LPFILETIME;

#define TIME_ZONE_ID_INVALID	((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN    0
#define TIME_ZONE_ID_STANDARD   1
#define TIME_ZONE_ID_DAYLIGHT   2

typedef union _LARGE_INTEGER {
    struct {
#ifdef WORDS_BIGENDIAN
        LONG     HighPart;
        DWORD    LowPart;
#else
        DWORD    LowPart;
        LONG     HighPart;
#endif
    } u;
#ifndef NONAMELESSSTRUCT
    struct {
#ifdef WORDS_BIGENDIAN
        LONG     HighPart;
        DWORD    LowPart;
#else
        DWORD    LowPart;
        LONG     HighPart;
#endif
    };
#endif
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
#ifdef WORDS_BIGENDIAN
        DWORD    HighPart;
        DWORD    LowPart;
#else
        DWORD    LowPart;
        DWORD    HighPart;
#endif
    } u;
#ifndef NONAMELESSSTRUCT
    struct {
#ifdef WORDS_BIGENDIAN
        DWORD    HighPart;
        DWORD    LowPart;
#else
        DWORD    LowPart;
        DWORD    HighPart;
#endif
    };
#endif
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;


typedef struct _LIST_ENTRY {
  struct _LIST_ENTRY *Flink;
  struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#define CONTAINING_RECORD(address, type, field) \
  ((type *)((PCHAR)(address) - offsetof(type, field)))

typedef struct _RTL_CRITICAL_SECTION_DEBUG
{
  WORD   Type;
  WORD   CreatorBackTraceIndex;
  struct _RTL_CRITICAL_SECTION *CriticalSection;
  LIST_ENTRY ProcessLocksList;
  DWORD EntryCount;
  DWORD ContentionCount;
#ifdef __WINESRC__  /* in Wine we store the name here */
  DWORD_PTR Spare[8/sizeof(DWORD_PTR)];
#else
  DWORD Spare[ 2 ];
#endif
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;
}  RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

#define RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO 0x1000000
#define RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN  0x2000000
#define RTL_CRITICAL_SECTION_FLAG_STATIC_INIT   0x4000000
#define RTL_CRITICAL_SECTION_ALL_FLAG_BITS      0xFF000000
#define RTL_CRITICAL_SECTION_FLAG_RESERVED      (RTL_CRITICAL_SECTION_ALL_FLAG_BITS & ~0x7000000)

typedef struct _CLIENT_ID
{
   HANDLE UniqueProcess;
   HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    PVOID Handle;
} CURDIR, *PCURDIR;

typedef struct RTL_DRIVE_LETTER_CURDIR
{
    USHORT              Flags;
    USHORT              Length;
    ULONG               TimeStamp;
    UNICODE_STRING      DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct tagRTL_BITMAP {
    ULONG  SizeOfBitMap; /* Number of bits in the bitmap */
    PULONG Buffer; /* Bitmap data, assumed sized to a DWORD boundary */
} RTL_BITMAP, *PRTL_BITMAP;

typedef const RTL_BITMAP *PCRTL_BITMAP;

typedef struct tagRTL_BITMAP_RUN {
    ULONG StartingIndex; /* Bit position at which run starts */
    ULONG NumberOfBits;  /* Size of the run in bits */
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;

typedef const RTL_BITMAP_RUN *PCRTL_BITMAP_RUN;

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG               AllocationSize;
    ULONG               Size;
    ULONG               Flags;
    ULONG               DebugFlags;
    HANDLE              ConsoleHandle;
    ULONG               ConsoleFlags;
    HANDLE              hStdInput;
    HANDLE              hStdOutput;
    HANDLE              hStdError;
    CURDIR              CurrentDirectory;
    UNICODE_STRING      DllPath;
    UNICODE_STRING      ImagePathName;
    UNICODE_STRING      CommandLine;
    PWSTR               Environment;
    ULONG               dwX;
    ULONG               dwY;
    ULONG               dwXSize;
    ULONG               dwYSize;
    ULONG               dwXCountChars;
    ULONG               dwYCountChars;
    ULONG               dwFillAttribute;
    ULONG               dwFlags;
    ULONG               wShowWindow;
    UNICODE_STRING      WindowTitle;
    UNICODE_STRING      Desktop;
    UNICODE_STRING      ShellInfo;
    UNICODE_STRING      RuntimeInfo;
    RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

/* value for Flags field (FIXME: not the correct name) */
#define PROCESS_PARAMS_FLAG_NORMALIZED 1

typedef struct _PEB_LDR_DATA
{
    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

/***********************************************************************
 * PEB data structure
 */
typedef struct _PEB
{                                                                 /* win32/win64 */
    BOOLEAN                      InheritedAddressSpace;             /* 000/000 */
    BOOLEAN                      ReadImageFileExecOptions;          /* 001/001 */
    BOOLEAN                      BeingDebugged;                     /* 002/002 */
    BOOLEAN                      SpareBool;                         /* 003/003 */
    HANDLE                       Mutant;                            /* 004/008 */
    HMODULE                      ImageBaseAddress;                  /* 008/010 */
    PPEB_LDR_DATA                LdrData;                           /* 00c/018 */
    RTL_USER_PROCESS_PARAMETERS *ProcessParameters;                 /* 010/020 */
    PVOID                        SubSystemData;                     /* 014/028 */
    HANDLE                       ProcessHeap;                       /* 018/030 */
    PRTL_CRITICAL_SECTION        FastPebLock;                       /* 01c/038 */
    PVOID /*PPEBLOCKROUTINE*/    FastPebLockRoutine;                /* 020/040 */
    PVOID /*PPEBLOCKROUTINE*/    FastPebUnlockRoutine;              /* 024/048 */
    ULONG                        EnvironmentUpdateCount;            /* 028/050 */
    PVOID                        KernelCallbackTable;               /* 02c/058 */
    ULONG                        Reserved[2];                       /* 030/060 */
    PVOID /*PPEB_FREE_BLOCK*/    FreeList;                          /* 038/068 */
    ULONG                        TlsExpansionCounter;               /* 03c/070 */
    PRTL_BITMAP                  TlsBitmap;                         /* 040/078 */
    ULONG                        TlsBitmapBits[2];                  /* 044/080 */
    PVOID                        ReadOnlySharedMemoryBase;          /* 04c/088 */
    PVOID                        ReadOnlySharedMemoryHeap;          /* 050/090 */
    PVOID                       *ReadOnlyStaticServerData;          /* 054/098 */
    PVOID                        AnsiCodePageData;                  /* 058/0a0 */
    PVOID                        OemCodePageData;                   /* 05c/0a8 */
    PVOID                        UnicodeCaseTableData;              /* 060/0b0 */
    ULONG                        NumberOfProcessors;                /* 064/0b8 */
    ULONG                        NtGlobalFlag;                      /* 068/0bc */
    LARGE_INTEGER                CriticalSectionTimeout;            /* 070/0c0 */
    SIZE_T                       HeapSegmentReserve;                /* 078/0c8 */
    SIZE_T                       HeapSegmentCommit;                 /* 07c/0d0 */
    SIZE_T                       HeapDeCommitTotalFreeThreshold;    /* 080/0d8 */
    SIZE_T                       HeapDeCommitFreeBlockThreshold;    /* 084/0e0 */
    ULONG                        NumberOfHeaps;                     /* 088/0e8 */
    ULONG                        MaximumNumberOfHeaps;              /* 08c/0ec */
    PVOID                       *ProcessHeaps;                      /* 090/0f0 */
    PVOID                        GdiSharedHandleTable;              /* 094/0f8 */
    PVOID                        ProcessStarterHelper;              /* 098/100 */
    PVOID                        GdiDCAttributeList;                /* 09c/108 */
    PVOID                        LoaderLock;                        /* 0a0/110 */
    ULONG                        OSMajorVersion;                    /* 0a4/118 */
    ULONG                        OSMinorVersion;                    /* 0a8/11c */
    ULONG                        OSBuildNumber;                     /* 0ac/120 */
    ULONG                        OSPlatformId;                      /* 0b0/124 */
    ULONG                        ImageSubSystem;                    /* 0b4/128 */
    ULONG                        ImageSubSystemMajorVersion;        /* 0b8/12c */
    ULONG                        ImageSubSystemMinorVersion;        /* 0bc/130 */
    ULONG                        ImageProcessAffinityMask;          /* 0c0/134 */
    HANDLE                       GdiHandleBuffer[28];               /* 0c4/138 */
    ULONG                        unknown[6];                        /* 134/218 */
    PVOID                        PostProcessInitRoutine;            /* 14c/230 */
    PRTL_BITMAP                  TlsExpansionBitmap;                /* 150/238 */
    ULONG                        TlsExpansionBitmapBits[32];        /* 154/240 */
    ULONG                        SessionId;                         /* 1d4/2c0 */
    ULARGE_INTEGER               AppCompatFlags;                    /* 1d8/2c8 */
    ULARGE_INTEGER               AppCompatFlagsUser;                /* 1e0/2d0 */
    PVOID                        ShimData;                          /* 1e8/2d8 */
    PVOID                        AppCompatInfo;                     /* 1ec/2e0 */
    UNICODE_STRING               CSDVersion;                        /* 1f0/2e8 */
    PVOID                        ActivationContextData;             /* 1f8/2f8 */
    PVOID                        ProcessAssemblyStorageMap;         /* 1fc/300 */
    PVOID                        SystemDefaultActivationData;       /* 200/308 */
    PVOID                        SystemAssemblyStorageMap;          /* 204/310 */
    SIZE_T                       MinimumStackCommit;                /* 208/318 */
    PVOID                       *FlsCallback;                       /* 20c/320 */
    LIST_ENTRY                   FlsListHead;                       /* 210/328 */
    PRTL_BITMAP                  FlsBitmap;                         /* 218/338 */
    ULONG                        FlsBitmapBits[4];                  /* 21c/340 */
} PEB, *PPEB;

typedef enum _HARDERROR_RESPONSE_OPTION {
  OptionAbortRetryIgnore,
  OptionOk,
  OptionOkCancel,
  OptionRetryCancel,
  OptionYesNo,
  OptionYesNoCancel,
  OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE {
  ResponseReturnToCaller,
  ResponseNotHandled,
  ResponseAbort,
  ResponseCancel,
  ResponseIgnore,
  ResponseNo,
  ResponseOk,
  ResponseRetry,
  ResponseYes
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;

/*
 * Language IDs
 */

#define MAKELCID(l, s)		(MAKELONG(l, s))

#define MAKELANGID(p, s)        ((((WORD)(s))<<10) | (WORD)(p))
#define PRIMARYLANGID(l)        ((WORD)(l) & 0x3ff)
#define SUBLANGID(l)            ((WORD)(l) >> 10)

#define LANGIDFROMLCID(lcid)	((WORD)(lcid))
#define SORTIDFROMLCID(lcid)	((WORD)((((DWORD)(lcid)) >> 16) & 0x0f))

#define LANG_SYSTEM_DEFAULT	(MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT	(MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LOCALE_SYSTEM_DEFAULT	(MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT	(MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define LOCALE_NEUTRAL		(MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT))
#define LOCALE_INVARIANT	(MAKELCID(MAKELANGID(LANG_INVARIANT,SUBLANG_NEUTRAL),SORT_DEFAULT))
#define LOCALE_CUSTOM_DEFAULT      (MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_CUSTOM_DEFAULT),SORT_DEFAULT))
#define LOCALE_CUSTOM_UNSPECIFIED  (MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_CUSTOM_UNSPECIFIED),SORT_DEFAULT))
#define LOCALE_CUSTOM_UI_DEFAULT   (MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_UI_CUSTOM_DEFAULT),SORT_DEFAULT))
#define LOCALE_NAME_MAX_LENGTH     85

/* Predefined resource types */
#define RT_CURSOR         MAKEINTRESOURCE(1)
#define RT_BITMAP         MAKEINTRESOURCE(2)
#define RT_ICON           MAKEINTRESOURCE(3)
#define RT_MENU           MAKEINTRESOURCE(4)
#define RT_DIALOG         MAKEINTRESOURCE(5)
#define RT_STRING         MAKEINTRESOURCE(6)
#define RT_FONTDIR        MAKEINTRESOURCE(7)
#define RT_FONT           MAKEINTRESOURCE(8)
#define RT_ACCELERATOR    MAKEINTRESOURCE(9)
#define RT_RCDATA         MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE   MAKEINTRESOURCE(11)
#define RT_GROUP_CURSOR   MAKEINTRESOURCE(12)
#define RT_GROUP_ICON     MAKEINTRESOURCE(14)
#define RT_VERSION        MAKEINTRESOURCE(16)
#define RT_DLGINCLUDE     MAKEINTRESOURCE(17)
#define RT_PLUGPLAY       MAKEINTRESOURCE(19)
#define RT_VXD            MAKEINTRESOURCE(20)
#define RT_ANICURSOR      MAKEINTRESOURCE(21)
#define RT_ANIICON        MAKEINTRESOURCE(22)
#define RT_HTML           MAKEINTRESOURCE(23)

#define SORT_DEFAULT                     0x0

#define LANG_NEUTRAL        0x00
#define LANG_ENGLISH        0x09

#define LOCALE_IDEFAULTUNIXCODEPAGE   0x1030 /* Wine extension */

/* Sublanguage definitions */
#define SUBLANG_NEUTRAL                  0x00    /* language neutral */
#define SUBLANG_DEFAULT                  0x01    /* user default */
#define SUBLANG_SYS_DEFAULT              0x02    /* system default */
#define SUBLANG_CUSTOM_DEFAULT           0x03
#define SUBLANG_CUSTOM_UNSPECIFIED       0x04
#define SUBLANG_UI_CUSTOM_DEFAULT        0x05

#define LOCALE_NAME_USER_DEFAULT    NULL

#define IDN_ALLOW_UNASSIGNED        0x1
#define IDN_USE_STD3_ASCII_RULES    0x2

#define MAP_FOLDCZONE        0x0010
#define MAP_PRECOMPOSED      0x0020
#define MAP_COMPOSITE        0x0040
#define MAP_FOLDDIGITS       0x0080
#define MAP_EXPAND_LIGATURES 0x2000

typedef enum _NORM_FORM {
    NormalizationOther  = 0,
    NormalizationC      = 0x1,
    NormalizationD      = 0x2,
    NormalizationKC     = 0x5,
    NormalizationKD     = 0x6
} NORM_FORM;

/* Geographic Information types */
enum SYSGEOTYPE
{
    GEO_NATION = 1,
    GEO_LATITUDE,
    GEO_LONGITUDE,
    GEO_ISO2,
    GEO_ISO3,
    GEO_RFC1766,
    GEO_LCID,
    GEO_FRIENDLYNAME,
    GEO_OFFICIALNAME,
    GEO_TIMEZONES,
    GEO_OFFICIALLANGUAGES
};

enum SYSGEOCLASS
{
    GEOCLASS_REGION = 14,
    GEOCLASS_NATION = 16
};

typedef BOOL    (CALLBACK *CALINFO_ENUMPROCA)(LPSTR);
typedef BOOL    (CALLBACK *CALINFO_ENUMPROCW)(LPWSTR);
typedef BOOL    (CALLBACK *CALINFO_ENUMPROCEXA)(LPSTR,CALID);
typedef BOOL    (CALLBACK *CALINFO_ENUMPROCEXW)(LPWSTR,CALID);
typedef BOOL    (CALLBACK *CODEPAGE_ENUMPROCA)(LPSTR);
typedef BOOL    (CALLBACK *CODEPAGE_ENUMPROCW)(LPWSTR);
typedef BOOL    (CALLBACK *DATEFMT_ENUMPROCA)(LPSTR);
typedef BOOL    (CALLBACK *DATEFMT_ENUMPROCW)(LPWSTR);
typedef BOOL    (CALLBACK *DATEFMT_ENUMPROCEXA)(LPSTR,CALID);
typedef BOOL    (CALLBACK *DATEFMT_ENUMPROCEXW)(LPWSTR,CALID);
typedef BOOL    (CALLBACK *GEO_ENUMPROC)(GEOID);
typedef BOOL    (CALLBACK *LANGGROUPLOCALE_ENUMPROCA)(LGRPID,LCID,LPSTR,LONG_PTR);
typedef BOOL    (CALLBACK *LANGGROUPLOCALE_ENUMPROCW)(LGRPID,LCID,LPWSTR,LONG_PTR);
typedef BOOL    (CALLBACK *LANGUAGEGROUP_ENUMPROCA)(LGRPID,LPSTR,LPSTR,DWORD,LONG_PTR);
typedef BOOL    (CALLBACK *LANGUAGEGROUP_ENUMPROCW)(LGRPID,LPWSTR,LPWSTR,DWORD,LONG_PTR);
typedef BOOL    (CALLBACK *LOCALE_ENUMPROCA)(LPSTR);
typedef BOOL    (CALLBACK *LOCALE_ENUMPROCW)(LPWSTR);
typedef BOOL    (CALLBACK *LOCALE_ENUMPROCEX)(LPWSTR, DWORD, LPARAM);
typedef BOOL    (CALLBACK *TIMEFMT_ENUMPROCA)(LPSTR);
typedef BOOL    (CALLBACK *TIMEFMT_ENUMPROCW)(LPWSTR);
typedef BOOL    (CALLBACK *UILANGUAGE_ENUMPROCA)(LPSTR,LONG_PTR);
typedef BOOL    (CALLBACK *UILANGUAGE_ENUMPROCW)(LPWSTR,LONG_PTR);

#define LGRPID_WESTERN_EUROPE      0x01 /* Includes US and Africa */
#define LGRPID_CENTRAL_EUROPE      0x02
#define LGRPID_BALTIC              0x03
#define LGRPID_GREEK               0x04
#define LGRPID_CYRILLIC            0x05
#define LGRPID_TURKISH             0x06
#define LGRPID_JAPANESE            0x07
#define LGRPID_KOREAN              0x08
#define LGRPID_TRADITIONAL_CHINESE 0x09
#define LGRPID_SIMPLIFIED_CHINESE  0x0A
#define LGRPID_THAI                0x0B
#define LGRPID_HEBREW              0x0C
#define LGRPID_ARABIC              0x0D
#define LGRPID_VIETNAMESE          0x0E
#define LGRPID_INDIC               0x0F
#define LGRPID_GEORGIAN            0x10
#define LGRPID_ARMENIAN            0x11

#define LGRPID_INSTALLED            0x1
#define LGRPID_SUPPORTED            0x2

#define LCID_INSTALLED              0x1
#define LCID_SUPPORTED              0x2
#define LCID_ALTERNATE_SORTS        0x4

#define LOCALE_ALL                  0x00
#define LOCALE_WINDOWS              0x01
#define LOCALE_SUPPLEMENTAL         0x02
#define LOCALE_ALTERNATE_SORTS      0x04
#define LOCALE_REPLACEMENT          0x08
#define LOCALE_NEUTRALDATA          0x10
#define LOCALE_SPECIFICDATA         0x20

typedef struct _nlsversioninfo {
    DWORD dwNLSVersionInfoSize;
    DWORD dwNLSVersion;
    DWORD dwDefinedVersion;
} NLSVERSIONINFO, *LPNLSVERSIONINFO;

typedef struct _GUID
{
    unsigned int   Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;

typedef struct _nlsversioninfoex {
    DWORD dwNLSVersionInfoSize;
    DWORD dwNLSVersion;
    DWORD dwDefinedVersion;
    DWORD dwEffectiveId;
    GUID  guidCustomVersion;
} NLSVERSIONINFOEX, *LPNLSVERSIONINFOEX;

#define LOBYTE(w)              ((BYTE)((DWORD_PTR)(w) & 0xFF))
#define HIBYTE(w)              ((BYTE)((DWORD_PTR)(w) >> 8))

#define LOWORD(l)              ((WORD)((DWORD_PTR)(l) & 0xFFFF))
#define HIWORD(l)              ((WORD)((DWORD_PTR)(l) >> 16))

#define MAKEWORD(low,high)     ((WORD)(((BYTE)((DWORD_PTR)(low) & 0xFF)) | ((WORD)((BYTE)((DWORD_PTR)(high) & 0xFF))) << 8))
#define MAKELONG(low,high)     ((LONG)(((WORD)((DWORD_PTR)(low) & 0xFFFF)) | ((DWORD)((WORD)((DWORD_PTR)(high) & 0xFFFF))) << 16))

#define MAX_PATH        260

typedef struct
{
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
    WCHAR UnicodeDefaultChar;
    UINT CodePage;
    CHAR CodePageName[MAX_PATH];
} CPINFOEXA, *LPCPINFOEXA;

typedef struct
{
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
    WCHAR UnicodeDefaultChar;
    UINT CodePage;
    WCHAR CodePageName[MAX_PATH];
} CPINFOEXW, *LPCPINFOEXW;

#define LANG_ENGLISH        0x09
#define SUBLANG_ENGLISH_US                 0x01

static __inline LPWSTR WINAPI lstrcpynW( LPWSTR dst, LPCWSTR src, INT n )
{
    LPWSTR d = dst;
    LPCWSTR s = src;
    UINT count = n;

    while ((count > 1) && *s)
    {
        count--;
        *d++ = *s++;
    }
    if (count) *d = 0;
    return dst;
}

static __inline LPSTR WINAPI lstrcpynA( LPSTR dst, LPCSTR src, INT n )
{
    LPSTR d = dst;
    LPCSTR s = src;
    UINT count = n;

    while ((count > 1) && *s)
    {
        count--;
        *d++ = *s++;
    }
    if (count) *d = 0;
    return dst;
}

static __inline INT WINAPI lstrlenW( LPCWSTR str )
{
    const WCHAR *s = str;
    while (*s) s++;
    return s - str;
}

static __inline INT WINAPI lstrlenA( LPCSTR str )
{
    return strlen( str );
}

static __inline LPWSTR WINAPI lstrcpyW( LPWSTR dst, LPCWSTR src )
{
    WCHAR *p = dst;
    while ((*p++ = *src++));
    return dst;
}

static __inline LPSTR WINAPI lstrcpyA( LPSTR dst, LPCSTR src )
{
    return strcpy( dst, src );
}

static __inline LPWSTR WINAPI lstrcatW( LPWSTR dst, LPCWSTR src )
{
    WCHAR *p = dst;
    while (*p) p++;
    while ((*p++ = *src++));
    return dst;
}

static __inline LPSTR WINAPI lstrcatA( LPSTR dst, LPCSTR src )
{
    return strcat( dst, src );
}

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;       /* type SECURITY_DESCRIPTOR */
  PVOID SecurityQualityOfService; /* type SECURITY_QUALITY_OF_SERVICE */
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

typedef DWORD ACCESS_MASK, *PACCESS_MASK;

#define DELETE                     0x00010000
#define READ_CONTROL               0x00020000
#define WRITE_DAC                  0x00040000
#define WRITE_OWNER                0x00080000
#define SYNCHRONIZE                0x00100000
#define STANDARD_RIGHTS_REQUIRED   0x000f0000

#define STANDARD_RIGHTS_READ       READ_CONTROL
#define STANDARD_RIGHTS_WRITE      READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE    READ_CONTROL

#define STANDARD_RIGHTS_ALL        0x001f0000

#define SPECIFIC_RIGHTS_ALL        0x0000ffff

#define GENERIC_READ               0x80000000
#define GENERIC_WRITE              0x40000000
#define GENERIC_EXECUTE            0x20000000
#define GENERIC_ALL                0x10000000

#define MAXIMUM_ALLOWED            0x02000000
#define ACCESS_SYSTEM_SECURITY     0x01000000

#define EVENT_QUERY_STATE          0x0001
#define EVENT_MODIFY_STATE         0x0002
#define EVENT_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define SEMAPHORE_MODIFY_STATE     0x0002
#define SEMAPHORE_ALL_ACCESS       (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define MUTANT_QUERY_STATE         0x0001
#define MUTANT_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|MUTANT_QUERY_STATE)


#define SEMAPHORE_MODIFY_STATE     0x0002
#define SEMAPHORE_ALL_ACCESS       (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define HandleToULong(h)        ((ULONG)(ULONG_PTR)(h))
#define HandleToLong(h)         ((LONG)(LONG_PTR)(h))
#define ULongToHandle(ul)       ((HANDLE)(ULONG_PTR)(ul))
#define LongToHandle(l)         ((HANDLE)(LONG_PTR)(l))
#define PtrToUlong(p)           ((ULONG)(ULONG_PTR)(p))
#define PtrToLong(p)            ((LONG)(LONG_PTR)(p))
#define PtrToUint(p)            ((UINT)(UINT_PTR)(p))
#define PtrToInt(p)             ((INT)(INT_PTR)(p))
#define PtrToUshort(p)          ((USHORT)(ULONG_PTR)(p))
#define PtrToShort(p)           ((SHORT)(LONG_PTR)(p))
#define IntToPtr(i)             ((void *)(INT_PTR)((INT)i))
#define UIntToPtr(ui)           ((void *)(UINT_PTR)((UINT)ui))
#define LongToPtr(l)            ((void *)(LONG_PTR)((LONG)l))
#define ULongToPtr(ul)          ((void *)(ULONG_PTR)((ULONG)ul))

#define IS_INTRESOURCE(x)   (((ULONG_PTR)(x) >> 16) == 0)

/* The RECT structure */
typedef struct tagRECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;

typedef struct _RECTL
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECTL, *PRECTL, *LPRECTL;

typedef const RECTL *LPCRECTL;

#define FIRST_USER_HANDLE 0x0020
#define LAST_USER_HANDLE  0xffef

#define CCHDEVICENAME 32
#define CCHFORMNAME   32

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, *LPPOINT;

typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, *PLUID;

typedef struct _SECURITY_ATTRIBUTES
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef DWORD SECURITY_INFORMATION, *PSECURITY_INFORMATION;
typedef WORD SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;

/* The security descriptor structure */
typedef struct {
    BYTE Revision;
    BYTE Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    DWORD Owner;
    DWORD Group;
    DWORD Sacl;
    DWORD Dacl;
} SECURITY_DESCRIPTOR_RELATIVE, *PISECURITY_DESCRIPTOR_RELATIVE;

typedef PVOID PACCESS_TOKEN;
typedef PVOID PSECURITY_DESCRIPTOR;
typedef PVOID PSID;

typedef struct _ACL {
    BYTE AclRevision;
    BYTE Sbz1;
    WORD AclSize;
    WORD AceCount;
    WORD Sbz2;
} ACL, *PACL;

typedef struct {
    BYTE Revision;
    BYTE Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    PSID Owner;
    PSID Group;
    PACL Sacl;
    PACL Dacl;
} SECURITY_DESCRIPTOR, *PISECURITY_DESCRIPTOR;

#define SECURITY_DESCRIPTOR_MIN_LENGTH   (sizeof(SECURITY_DESCRIPTOR))

typedef struct tagSIZE
{
    LONG cx;
    LONG cy;
} SIZE, *PSIZE, *LPSIZE;

typedef struct _POINTL
{
    LONG x;
    LONG y;
} POINTL, *PPOINTL;

/* The POINTS structure */

typedef struct tagPOINTS
{
#ifdef WORDS_BIGENDIAN
    SHORT y;
    SHORT x;
#else
    SHORT x;
    SHORT y;
#endif
} POINTS, *PPOINTS, *LPPOINTS;

typedef SIZE SIZEL, *PSIZEL, *LPSIZEL;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

#define CS_VREDRAW             0x00000001
#define CS_HREDRAW             0x00000002
#define CS_KEYCVTWINDOW        0x00000004 /* DDK / Win16 */
#define CS_DBLCLKS             0x00000008
#define CS_OWNDC               0x00000020
#define CS_CLASSDC             0x00000040
#define CS_PARENTDC            0x00000080
#define CS_NOKEYCVT            0x00000100 /* DDK / Win16 */
#define CS_NOCLOSE             0x00000200
#define CS_SAVEBITS            0x00000800
#define CS_BYTEALIGNCLIENT     0x00001000
#define CS_BYTEALIGNWINDOW     0x00002000
#define CS_GLOBALCLASS         0x00004000
#define CS_IME                 0x00010000
#define CS_DROPSHADOW          0x00020000

typedef INT_PTR (CALLBACK *FARPROC)();
typedef INT_PTR (CALLBACK *NEARPROC)();
typedef INT_PTR (CALLBACK *PROC)();

#define FIELD_OFFSET(type, field) ((LONG)offsetof(type, field))

#define	DLL_PROCESS_DETACH	0	/* detach process (unload library) */
#define	DLL_PROCESS_ATTACH	1	/* attach process (load library) */
#define	DLL_THREAD_ATTACH	2	/* attach new thread */
#define	DLL_THREAD_DETACH	3	/* detach thread */

#define IS_TEXT_UNICODE_ASCII16		   0x0001
#define IS_TEXT_UNICODE_STATISTICS         0x0002
#define IS_TEXT_UNICODE_CONTROLS           0x0004
#define IS_TEXT_UNICODE_SIGNATURE	   0x0008
#define IS_TEXT_UNICODE_UNICODE_MASK       0x000F
#define IS_TEXT_UNICODE_REVERSE_ASCII16	   0x0010
#define IS_TEXT_UNICODE_REVERSE_STATISTICS 0x0020
#define IS_TEXT_UNICODE_REVERSE_CONTROLS   0x0040
#define IS_TEXT_UNICODE_REVERSE_SIGNATURE  0x0080
#define IS_TEXT_UNICODE_REVERSE_MASK       0x00F0
#define IS_TEXT_UNICODE_ILLEGAL_CHARS	   0x0100
#define IS_TEXT_UNICODE_ODD_LENGTH	   0x0200
#define IS_TEXT_UNICODE_DBCS_LEADBYTE      0x0400
#define IS_TEXT_UNICODE_NOT_UNICODE_MASK   0x0F00
#define IS_TEXT_UNICODE_NULL_BYTES         0x1000
#define IS_TEXT_UNICODE_NOT_ASCII_MASK     0xF000

# define __MSABI_LONG(x)         x

#define WS_OVERLAPPED          __MSABI_LONG(0x00000000)
#define WS_POPUP               __MSABI_LONG(0x80000000)
#define WS_CHILD               __MSABI_LONG(0x40000000)
#define WS_MINIMIZE            __MSABI_LONG(0x20000000)
#define WS_VISIBLE             __MSABI_LONG(0x10000000)
#define WS_DISABLED            __MSABI_LONG(0x08000000)
#define WS_CLIPSIBLINGS        __MSABI_LONG(0x04000000)
#define WS_CLIPCHILDREN        __MSABI_LONG(0x02000000)
#define WS_MAXIMIZE            __MSABI_LONG(0x01000000)
#define WS_BORDER              __MSABI_LONG(0x00800000)
#define WS_DLGFRAME            __MSABI_LONG(0x00400000)
#define WS_VSCROLL             __MSABI_LONG(0x00200000)
#define WS_HSCROLL             __MSABI_LONG(0x00100000)
#define WS_SYSMENU             __MSABI_LONG(0x00080000)
#define WS_THICKFRAME          __MSABI_LONG(0x00040000)
#define WS_GROUP               __MSABI_LONG(0x00020000)
#define WS_TABSTOP             __MSABI_LONG(0x00010000)
#define WS_MINIMIZEBOX         __MSABI_LONG(0x00020000)
#define WS_MAXIMIZEBOX         __MSABI_LONG(0x00010000)
#define WS_CAPTION             (WS_BORDER | WS_DLGFRAME)
#define WS_TILED               WS_OVERLAPPED
#define WS_ICONIC              WS_MINIMIZE
#define WS_SIZEBOX             WS_THICKFRAME
#define WS_OVERLAPPEDWINDOW    (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW         (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW         WS_CHILD
#define WS_TILEDWINDOW         WS_OVERLAPPEDWINDOW

#define WS_EX_DLGMODALFRAME    __MSABI_LONG(0x00000001)
#define WS_EX_DRAGDETECT       __MSABI_LONG(0x00000002) /* Undocumented */
#define WS_EX_NOPARENTNOTIFY   __MSABI_LONG(0x00000004)
#define WS_EX_TOPMOST          __MSABI_LONG(0x00000008)
#define WS_EX_ACCEPTFILES      __MSABI_LONG(0x00000010)
#define WS_EX_TRANSPARENT      __MSABI_LONG(0x00000020)
#define WS_EX_MDICHILD         __MSABI_LONG(0x00000040)
#define WS_EX_TOOLWINDOW       __MSABI_LONG(0x00000080)
#define WS_EX_WINDOWEDGE       __MSABI_LONG(0x00000100)
#define WS_EX_CLIENTEDGE       __MSABI_LONG(0x00000200)
#define WS_EX_CONTEXTHELP      __MSABI_LONG(0x00000400)
#define WS_EX_RIGHT            __MSABI_LONG(0x00001000)
#define WS_EX_LEFT             __MSABI_LONG(0x00000000)
#define WS_EX_RTLREADING       __MSABI_LONG(0x00002000)
#define WS_EX_LTRREADING       __MSABI_LONG(0x00000000)
#define WS_EX_LEFTSCROLLBAR    __MSABI_LONG(0x00004000)
#define WS_EX_RIGHTSCROLLBAR   __MSABI_LONG(0x00000000)
#define WS_EX_CONTROLPARENT    __MSABI_LONG(0x00010000)
#define WS_EX_STATICEDGE       __MSABI_LONG(0x00020000)
#define WS_EX_APPWINDOW        __MSABI_LONG(0x00040000)
#define WS_EX_LAYERED          __MSABI_LONG(0x00080000)
#define WS_EX_NOINHERITLAYOUT  __MSABI_LONG(0x00100000)
#define WS_EX_LAYOUTRTL        __MSABI_LONG(0x00400000)
#define WS_EX_COMPOSITED       __MSABI_LONG(0x02000000)
#define WS_EX_NOACTIVATE       __MSABI_LONG(0x08000000)

#define WS_EX_OVERLAPPEDWINDOW (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW    (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

#define WM_NULL                0x0000
#define WM_CREATE              0x0001
#define WM_DESTROY             0x0002
#define WM_MOVE                0x0003
#define WM_SIZEWAIT            0x0004 /* DDK / Win16 */
#define WM_SIZE                0x0005
#define WM_ACTIVATE            0x0006

/* WM_ACTIVATE wParam values */
#define WA_INACTIVE            0
#define WA_ACTIVE              1
#define WA_CLICKACTIVE         2

#define WM_SETFOCUS            0x0007
#define WM_KILLFOCUS           0x0008
#define WM_SETVISIBLE          0x0009 /* DDK / Win16 */
#define WM_ENABLE              0x000a
#define WM_SETREDRAW           0x000b
#define WM_SETTEXT             0x000c
#define WM_GETTEXT             0x000d
#define WM_GETTEXTLENGTH       0x000e
#define WM_PAINT               0x000f
#define WM_CLOSE               0x0010
#define WM_QUERYENDSESSION     0x0011
#define WM_QUIT                0x0012
#define WM_QUERYOPEN           0x0013
#define WM_ERASEBKGND          0x0014
#define WM_SYSCOLORCHANGE      0x0015
#define WM_ENDSESSION          0x0016
#define WM_SYSTEMERROR         0x0017 /* DDK / Win16 */
#define WM_SHOWWINDOW          0x0018
#define WM_CTLCOLOR            0x0019 /* Added from windowsx.h */
#define WM_WININICHANGE        0x001a
#define WM_SETTINGCHANGE       WM_WININICHANGE
#define WM_DEVMODECHANGE       0x001b
#define WM_ACTIVATEAPP         0x001c
#define WM_FONTCHANGE          0x001d
#define WM_TIMECHANGE          0x001e
#define WM_CANCELMODE          0x001f
#define WM_SETCURSOR           0x0020
#define WM_MOUSEACTIVATE       0x0021
#define WM_CHILDACTIVATE       0x0022
#define WM_QUEUESYNC           0x0023
#define WM_GETMINMAXINFO       0x0024

#define WM_PAINTICON           0x0026
#define WM_ICONERASEBKGND      0x0027
#define WM_NEXTDLGCTL          0x0028
#define WM_ALTTABACTIVE        0x0029 /* DDK / Win16 */
#define WM_SPOOLERSTATUS       0x002a
#define WM_DRAWITEM            0x002b
#define WM_MEASUREITEM         0x002c
#define WM_DELETEITEM          0x002d
#define WM_VKEYTOITEM          0x002e
#define WM_CHARTOITEM          0x002f
#define WM_SETFONT             0x0030
#define WM_GETFONT             0x0031
#define WM_SETHOTKEY           0x0032
#define WM_GETHOTKEY           0x0033
#define WM_FILESYSCHANGE       0x0034 /* DDK / Win16 */
#define WM_ISACTIVEICON        0x0035 /* DDK / Win16 */
#define WM_QUERYPARKICON       0x0036 /* Undocumented */
#define WM_QUERYDRAGICON       0x0037
#define WM_QUERYSAVESTATE      0x0038 /* Undocumented */
#define WM_COMPAREITEM         0x0039
#define WM_TESTING             0x003a /* DDK / Win16 */

#define WM_GETOBJECT           0x003d

#define WM_ACTIVATESHELLWINDOW 0x003e /* FIXME: Wine-only */

#define WM_COMPACTING          0x0041

#define WM_COMMNOTIFY          0x0044
#define WM_WINDOWPOSCHANGING   0x0046
#define WM_WINDOWPOSCHANGED    0x0047

#define WM_POWER               0x0048

/* For WM_POWER */
#define PWR_OK                 1
#define PWR_FAIL               (-1)
#define PWR_SUSPENDREQUEST     1
#define PWR_SUSPENDRESUME      2
#define PWR_CRITICALRESUME     3

/* Win32 4.0 messages */
#define WM_COPYDATA            0x004a
#define WM_CANCELJOURNAL       0x004b
#define WM_KEYF1               0x004d /* DDK / Win16 */
#define WM_NOTIFY              0x004e
#define WM_INPUTLANGCHANGEREQUEST 0x0050
#define WM_INPUTLANGCHANGE     0x0051
#define WM_TCARD               0x0052
#define WM_HELP                0x0053
#define WM_USERCHANGED         0x0054
#define WM_NOTIFYFORMAT        0x0055

/* WM_NOTIFYFORMAT commands and return values */
#define NFR_ANSI               1
#define NFR_UNICODE            2
#define NF_QUERY               3
#define NF_REQUERY             4

#define WM_CONTEXTMENU         0x007b
#define WM_STYLECHANGING       0x007c
#define WM_STYLECHANGED        0x007d
#define WM_DISPLAYCHANGE       0x007e
#define WM_GETICON             0x007f
#define WM_SETICON             0x0080

/* Non-client system messages */
#define WM_NCCREATE            0x0081
#define WM_NCDESTROY           0x0082
#define WM_NCCALCSIZE          0x0083
#define WM_NCHITTEST           0x0084
#define WM_NCPAINT             0x0085
#define WM_NCACTIVATE          0x0086

#define WM_GETDLGCODE          0x0087
#define WM_SYNCPAINT           0x0088
#define WM_SYNCTASK            0x0089 /* DDK / Win16 */

/* Non-client mouse messages */
#define WM_NCMOUSEMOVE         0x00a0
#define WM_NCLBUTTONDOWN       0x00a1
#define WM_NCLBUTTONUP         0x00a2
#define WM_NCLBUTTONDBLCLK     0x00a3
#define WM_NCRBUTTONDOWN       0x00a4
#define WM_NCRBUTTONUP         0x00a5
#define WM_NCRBUTTONDBLCLK     0x00a6
#define WM_NCMBUTTONDOWN       0x00a7
#define WM_NCMBUTTONUP         0x00a8
#define WM_NCMBUTTONDBLCLK     0x00a9

#define WM_NCXBUTTONDOWN       0x00ab
#define WM_NCXBUTTONUP         0x00ac
#define WM_NCXBUTTONDBLCLK     0x00ad

/* Raw input */
#define WM_INPUT_DEVICE_CHANGE 0x00fe
#define WM_INPUT               0x00ff

/* Keyboard messages */
#define WM_KEYFIRST            0x0100
#define WM_KEYDOWN             WM_KEYFIRST
#define WM_KEYUP               0x0101
#define WM_CHAR                0x0102
#define WM_DEADCHAR            0x0103
#define WM_SYSKEYDOWN          0x0104
#define WM_SYSKEYUP            0x0105
#define WM_SYSCHAR             0x0106
#define WM_SYSDEADCHAR         0x0107
#define WM_UNICHAR             0x0109
#define WM_KEYLAST             WM_UNICHAR

#define UNICODE_NOCHAR         0xffff

/* Win32 4.0 messages for IME */
#define WM_IME_STARTCOMPOSITION 0x010d
#define WM_IME_ENDCOMPOSITION  0x010e
#define WM_IME_COMPOSITION     0x010f
#define WM_IME_KEYLAST         0x010f

#define WM_INITDIALOG          0x0110
#define WM_COMMAND             0x0111
#define WM_SYSCOMMAND          0x0112
#define WM_TIMER               0x0113

/* Scroll messages */
#define WM_HSCROLL             0x0114
#define WM_VSCROLL             0x0115

/* Menu messages */
#define WM_INITMENU            0x0116
#define WM_INITMENUPOPUP       0x0117
#define WM_GESTURE             0x0119
#define WM_GESTURENOTIFY       0x011A

#define WM_MENUSELECT          0x011F
#define WM_MENUCHAR            0x0120
#define WM_ENTERIDLE           0x0121

#define WM_MENURBUTTONUP       0x0122
#define WM_MENUDRAG            0x0123
#define WM_MENUGETOBJECT       0x0124
#define WM_UNINITMENUPOPUP     0x0125
#define WM_MENUCOMMAND         0x0126

#define WM_CHANGEUISTATE       0x0127
#define WM_UPDATEUISTATE       0x0128
#define WM_QUERYUISTATE        0x0129

/* UI flags for WM_*UISTATE */
/* for low-order word of wparam */
#define UIS_SET                1
#define UIS_CLEAR              2
#define UIS_INITIALIZE         3
/* for hi-order word of wparam */
#define UISF_HIDEFOCUS         0x1
#define UISF_HIDEACCEL         0x2
#define UISF_ACTIVE            0x4

#define WM_LBTRACKPOINT        0x0131 /* DDK / Win16 */

/* Win32 CTLCOLOR messages */
#define WM_CTLCOLORMSGBOX      0x0132
#define WM_CTLCOLOREDIT        0x0133
#define WM_CTLCOLORLISTBOX     0x0134
#define WM_CTLCOLORBTN         0x0135
#define WM_CTLCOLORDLG         0x0136
#define WM_CTLCOLORSCROLLBAR   0x0137
#define WM_CTLCOLORSTATIC      0x0138

#define MN_GETHMENU            0x01E1

/* Mouse messages */
#define WM_MOUSEFIRST          0x0200
#define WM_MOUSEMOVE           WM_MOUSEFIRST
#define WM_LBUTTONDOWN         0x0201
#define WM_LBUTTONUP           0x0202
#define WM_LBUTTONDBLCLK       0x0203
#define WM_RBUTTONDOWN         0x0204
#define WM_RBUTTONUP           0x0205
#define WM_RBUTTONDBLCLK       0x0206
#define WM_MBUTTONDOWN         0x0207
#define WM_MBUTTONUP           0x0208
#define WM_MBUTTONDBLCLK       0x0209
#define WM_MOUSEWHEEL          0x020A
#define WM_XBUTTONDOWN         0x020B
#define WM_XBUTTONUP           0x020C
#define WM_XBUTTONDBLCLK       0x020D
#define WM_MOUSEHWHEEL         0x020E
#define WM_MOUSELAST           WM_MOUSEHWHEEL

/* Macros for the mouse messages */
#define WHEEL_DELTA            120
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#define WHEEL_PAGESCROLL       (UINT_MAX)

#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_NCHITTEST_WPARAM(wParam)    ((short)LOWORD(wParam))
#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
#define XBUTTON1               0x0001
#define XBUTTON2               0x0002

#define WM_PARENTNOTIFY        0x0210
#define WM_ENTERMENULOOP       0x0211
#define WM_EXITMENULOOP        0x0212
#define WM_NEXTMENU            0x0213

/* Win32 4.0 messages */
#define WM_SIZING              0x0214
#define WM_CAPTURECHANGED      0x0215
#define WM_MOVING              0x0216
#define WM_POWERBROADCAST      0x0218
#define WM_DEVICECHANGE        0x0219
/* MDI messages */
#define WM_MDICREATE           0x0220
#define WM_MDIDESTROY          0x0221
#define WM_MDIACTIVATE         0x0222
#define WM_MDIRESTORE          0x0223
#define WM_MDINEXT             0x0224
#define WM_MDIMAXIMIZE         0x0225
#define WM_MDITILE             0x0226
#define WM_MDICASCADE          0x0227
#define WM_MDIICONARRANGE      0x0228
#define WM_MDIGETACTIVE        0x0229
#define WM_MDISETMENU          0x0230
#define WM_MDIREFRESHMENU      0x0234

/* D&D messages */
#define WM_DROPOBJECT          0x022A /* DDK / Win16 */
#define WM_QUERYDROPOBJECT     0x022B /* DDK / Win16 */
#define WM_BEGINDRAG           0x022C /* DDK / Win16 */
#define WM_DRAGLOOP            0x022D /* DDK / Win16 */
#define WM_DRAGSELECT          0x022E /* DDK / Win16 */
#define WM_DRAGMOVE            0x022F /* DDK / Win16 */

#define WM_ENTERSIZEMOVE       0x0231
#define WM_EXITSIZEMOVE        0x0232
#define WM_DROPFILES           0x0233

#define WM_TOUCH               0x0240

/* Win32 4.0 messages for IME */
#define WM_IME_SETCONTEXT      0x0281
#define WM_IME_NOTIFY          0x0282
#define WM_IME_CONTROL         0x0283
#define WM_IME_COMPOSITIONFULL 0x0284
#define WM_IME_SELECT          0x0285
#define WM_IME_CHAR            0x0286
/* Win32 5.0 messages for IME */
#define WM_IME_REQUEST         0x0288

/* Win32 4.0 messages for IME */
#define WM_IME_KEYDOWN         0x0290
#define WM_IME_KEYUP           0x0291

#define WM_NCMOUSEHOVER        0x02A0
#define WM_MOUSEHOVER          0x02A1
#define WM_MOUSELEAVE          0x02A3
#define WM_NCMOUSELEAVE        0x02A2

#define WM_WTSSESSION_CHANGE   0x02B1

#define WM_TABLET_FIRST        0x02c0
#define WM_TABLET_LAST         0x02df

/* Clipboard command messages */
#define WM_CUT                 0x0300
#define WM_COPY                0x0301
#define WM_PASTE               0x0302
#define WM_CLEAR               0x0303
#define WM_UNDO                0x0304

/* Clipboard owner messages */
#define WM_RENDERFORMAT        0x0305
#define WM_RENDERALLFORMATS    0x0306
#define WM_DESTROYCLIPBOARD    0x0307

/* Clipboard viewer messages */
#define WM_DRAWCLIPBOARD       0x0308
#define WM_PAINTCLIPBOARD      0x0309
#define WM_VSCROLLCLIPBOARD    0x030A
#define WM_SIZECLIPBOARD       0x030B
#define WM_ASKCBFORMATNAME     0x030C
#define WM_CHANGECBCHAIN       0x030D
#define WM_HSCROLLCLIPBOARD    0x030E

#define WM_QUERYNEWPALETTE     0x030F
#define WM_PALETTEISCHANGING   0x0310
#define WM_PALETTECHANGED      0x0311
#define WM_HOTKEY              0x0312

#define WM_PRINT               0x0317
#define WM_PRINTCLIENT         0x0318
#define WM_APPCOMMAND          0x0319
#define WM_THEMECHANGED        0x031A
#define WM_CLIPBOARDUPDATE     0x031D

#define WM_DWMCOMPOSITIONCHANGED 0x031E
#define WM_DWMNCRENDERINGCHANGED 0x031F
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#define WM_DWMWINDOWMAXIMIZEDCHANGE 0x0321
#define WM_DWMSENDICONICTHUMBNAIL 0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP 0x0326

#define WM_GETTITLEBARINFOEX   0x033F

#define WM_HANDHELDFIRST       0x0358
#define WM_HANDHELDLAST        0x035F

#define WM_AFXFIRST            0x0360
#define WM_AFXLAST             0x037F

#define WM_PENWINFIRST         0x0380
#define WM_PENWINLAST          0x038F

#define WM_USER                0x0400

#define WM_APP                 0x8000

enum coords_relative
{
    COORDS_CLIENT,
    COORDS_WINDOW,
    COORDS_PARENT,
    COORDS_SCREEN
};

#define	STARTF_USESHOWWINDOW	0x00000001
#define	STARTF_USESIZE		0x00000002
#define	STARTF_USEPOSITION	0x00000004
#define	STARTF_USECOUNTCHARS	0x00000008
#define	STARTF_USEFILLATTRIBUTE	0x00000010
#define	STARTF_RUNFULLSCREEN	0x00000020
#define	STARTF_FORCEONFEEDBACK	0x00000040
#define	STARTF_FORCEOFFFEEDBACK	0x00000080
#define	STARTF_USESTDHANDLES	0x00000100
#define	STARTF_USEHOTKEY	0x00000200

typedef struct _STARTUPINFOA{
        DWORD cb;		/* 00: size of struct */
        LPSTR lpReserved;	/* 04: */
        LPSTR lpDesktop;	/* 08: */
        LPSTR lpTitle;		/* 0c: */
        DWORD dwX;		/* 10: */
        DWORD dwY;		/* 14: */
        DWORD dwXSize;		/* 18: */
        DWORD dwYSize;		/* 1c: */
        DWORD dwXCountChars;	/* 20: */
        DWORD dwYCountChars;	/* 24: */
        DWORD dwFillAttribute;	/* 28: */
        DWORD dwFlags;		/* 2c: */
        WORD wShowWindow;	/* 30: */
        WORD cbReserved2;	/* 32: */
        BYTE *lpReserved2;	/* 34: */
        HANDLE hStdInput;	/* 38: */
        HANDLE hStdOutput;	/* 3c: */
        HANDLE hStdError;	/* 40: */
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _STARTUPINFOW{
        DWORD cb;
        LPWSTR lpReserved;
        LPWSTR lpDesktop;
        LPWSTR lpTitle;
        DWORD dwX;
        DWORD dwY;
        DWORD dwXSize;
        DWORD dwYSize;
        DWORD dwXCountChars;
        DWORD dwYCountChars;
        DWORD dwFillAttribute;
        DWORD dwFlags;
        WORD wShowWindow;
        WORD cbReserved2;
        BYTE *lpReserved2;
        HANDLE hStdInput;
        HANDLE hStdOutput;
        HANDLE hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

#define SW_HIDE                0
#define SW_SHOWNORMAL          1
#define SW_NORMAL              SW_SHOWNORMAL
#define SW_SHOWMINIMIZED       2
#define SW_SHOWMAXIMIZED       3
#define SW_MAXIMIZE            SW_SHOWMAXIMIZED
#define SW_SHOWNOACTIVATE      4
#define SW_SHOW                5
#define SW_MINIMIZE            6
#define SW_SHOWMINNOACTIVE     7
#define SW_SHOWNA              8
#define SW_RESTORE             9
#define SW_SHOWDEFAULT         10
#define SW_FORCEMINIMIZE       11
#define SW_MAX                 11
#define SW_NORMALNA            0xCC /* Undocumented. Flag in MinMaximize */

/* Obsolete ShowWindow() codes for compatibility */
#define HIDE_WINDOW            SW_HIDE
#define SHOW_OPENWINDOW        SW_SHOWNORMAL
#define SHOW_ICONWINDOW        SW_SHOWMINIMIZED
#define SHOW_FULLSCREEN        SW_SHOWMAXIMIZED
#define SHOW_OPENNOACTIVATE    SW_SHOWNOACTIVATE

/* WM_SHOWWINDOW lParam codes */
#define SW_PARENTCLOSING       1
#define SW_OTHERZOOM           2
#define SW_PARENTOPENING       3
#define SW_OTHERUNZOOM         4

/* wParam for WM_SIZING message */
#define WMSZ_LEFT              1
#define WMSZ_RIGHT             2
#define WMSZ_TOP               3
#define WMSZ_TOPLEFT           4
#define WMSZ_TOPRIGHT          5
#define WMSZ_BOTTOM            6
#define WMSZ_BOTTOMLEFT        7
#define WMSZ_BOTTOMRIGHT       8

/* WM_NCHITTEST return codes */
#define HTERROR                (-2)
#define HTTRANSPARENT          (-1)
#define HTNOWHERE              0
#define HTCLIENT               1
#define HTCAPTION              2
#define HTSYSMENU              3
#define HTSIZE                 4
#define HTGROWBOX              HTSIZE
#define HTMENU                 5
#define HTHSCROLL              6
#define HTVSCROLL              7
#define HTMINBUTTON            8
#define HTREDUCE               HTMINBUTTON
#define HTMAXBUTTON            9
#define HTZOOM                 HTMAXBUTTON
#define HTLEFT                 10
#define HTSIZEFIRST            HTLEFT
#define HTRIGHT                11
#define HTTOP                  12
#define HTTOPLEFT              13
#define HTTOPRIGHT             14
#define HTBOTTOM               15
#define HTBOTTOMLEFT           16
#define HTBOTTOMRIGHT          17
#define HTSIZELAST             HTBOTTOMRIGHT
#define HTBORDER               18
#define HTOBJECT               19
#define HTCLOSE                20
#define HTHELP                 21

/* SendMessageTimeout flags */
#define SMTO_NORMAL            0x0000
#define SMTO_BLOCK             0x0001
#define SMTO_ABORTIFHUNG       0x0002
#define SMTO_NOTIMEOUTIFNOTHUNG 0x0008
#define SMTO_ERRORONEXIT       0x0020

/* WM_MOUSEACTIVATE return values */
#define MA_ACTIVATE            1
#define MA_ACTIVATEANDEAT      2
#define MA_NOACTIVATE          3
#define MA_NOACTIVATEANDEAT    4

/* WM_GETICON/WM_SETICON params values */
#define ICON_SMALL             0
#define ICON_BIG               1
#define ICON_SMALL2            2

/* WM_SIZE message wParam values */
#define SIZE_RESTORED          0
#define SIZE_MINIMIZED         1
#define SIZE_MAXIMIZED         2
#define SIZE_MAXSHOW           3
#define SIZE_MAXHIDE           4
#define SIZENORMAL             SIZE_RESTORED
#define SIZEICONIC             SIZE_MINIMIZED
#define SIZEFULLSCREEN         SIZE_MAXIMIZED
#define SIZEZOOMSHOW           SIZE_MAXSHOW
#define SIZEZOOMHIDE           SIZE_MAXHIDE

/* WM_NCCALCSIZE return flags */
#define WVR_ALIGNTOP           0x0010
#define WVR_ALIGNLEFT          0x0020
#define WVR_ALIGNBOTTOM        0x0040
#define WVR_ALIGNRIGHT         0x0080
#define WVR_HREDRAW            0x0100
#define WVR_VREDRAW            0x0200
#define WVR_REDRAW             (WVR_HREDRAW | WVR_VREDRAW)
#define WVR_VALIDRECTS         0x0400

/* Key status flags for mouse events */
#define MK_LBUTTON             0x0001
#define MK_RBUTTON             0x0002
#define MK_SHIFT               0x0004
#define MK_CONTROL             0x0008
#define MK_MBUTTON             0x0010
#define MK_XBUTTON1            0x0020
#define MK_XBUTTON2            0x0040

#define TME_HOVER              0x00000001
#define TME_LEAVE              0x00000002
#define TME_NONCLIENT          0x00000010
#define TME_QUERY              0x40000000
#define TME_CANCEL             0x80000000
#define HOVER_DEFAULT          0xFFFFFFFF

typedef struct _SYSTEM_INFO
{
    union {
	DWORD	dwOemId; /* Obsolete field - do not use */
	struct {
		WORD wProcessorArchitecture;
		WORD wReserved;
	} ;
    } ;
    DWORD	dwPageSize;
    LPVOID	lpMinimumApplicationAddress;
    LPVOID	lpMaximumApplicationAddress;
    DWORD_PTR	dwActiveProcessorMask;
    DWORD	dwNumberOfProcessors;
    DWORD	dwProcessorType;
    DWORD	dwAllocationGranularity;
    WORD	wProcessorLevel;
    WORD	wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

#define FILE_MAP_COPY                   0x00000001
#define FILE_MAP_WRITE                  0x00000002
#define FILE_MAP_READ                   0x00000004
#define FILE_MAP_ALL_ACCESS             0x000f001f
#define FILE_MAP_EXECUTE                0x00000020

#define	PAGE_NOACCESS		0x01
#define	PAGE_READONLY		0x02
#define	PAGE_READWRITE		0x04
#define	PAGE_WRITECOPY		0x08
#define	PAGE_EXECUTE		0x10
#define	PAGE_EXECUTE_READ	0x20
#define	PAGE_EXECUTE_READWRITE	0x40
#define	PAGE_EXECUTE_WRITECOPY	0x80
#define	PAGE_GUARD		0x100
#define	PAGE_NOCACHE		0x200
#define	PAGE_WRITECOMBINE	0x400

#define MEM_COMMIT              0x00001000
#define MEM_RESERVE             0x00002000
#define MEM_DECOMMIT            0x00004000
#define MEM_RELEASE             0x00008000
#define MEM_FREE                0x00010000
#define MEM_PRIVATE             0x00020000
#define MEM_MAPPED              0x00040000
#define MEM_RESET               0x00080000
#define MEM_TOP_DOWN            0x00100000
#define MEM_WRITE_WATCH         0x00200000
#define MEM_PHYSICAL            0x00400000
#define MEM_LARGE_PAGES         0x20000000
#define MEM_4MB_PAGES           0x80000000

#define WINE_GGO_GRAY16_BITMAP 0x10
#define WINE_GGO_HRGB_BITMAP   0x11
#define WINE_GGO_HBGR_BITMAP   0x12
#define WINE_GGO_VRGB_BITMAP   0x13
#define WINE_GGO_VBGR_BITMAP   0x14

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5

#define FILE_SHARE_READ                    0x00000001
#define FILE_SHARE_WRITE                   0x00000002
#define FILE_SHARE_DELETE                  0x00000004

#define FILE_ATTRIBUTE_READONLY            0x00000001
#define FILE_ATTRIBUTE_HIDDEN              0x00000002
#define FILE_ATTRIBUTE_SYSTEM              0x00000004
#define FILE_ATTRIBUTE_DIRECTORY           0x00000010
#define FILE_ATTRIBUTE_ARCHIVE             0x00000020
#define FILE_ATTRIBUTE_DEVICE              0x00000040
#define FILE_ATTRIBUTE_NORMAL              0x00000080
#define FILE_ATTRIBUTE_TEMPORARY           0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE         0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT       0x00000400
#define FILE_ATTRIBUTE_COMPRESSED          0x00000800
#define FILE_ATTRIBUTE_OFFLINE             0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED           0x00004000

#include <pshpack2.h>
typedef struct _IMAGE_DOS_HEADER {
    WORD  e_magic;      /* 00: MZ Header signature */
    WORD  e_cblp;       /* 02: Bytes on last page of file */
    WORD  e_cp;         /* 04: Pages in file */
    WORD  e_crlc;       /* 06: Relocations */
    WORD  e_cparhdr;    /* 08: Size of header in paragraphs */
    WORD  e_minalloc;   /* 0a: Minimum extra paragraphs needed */
    WORD  e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
    WORD  e_ss;         /* 0e: Initial (relative) SS value */
    WORD  e_sp;         /* 10: Initial SP value */
    WORD  e_csum;       /* 12: Checksum */
    WORD  e_ip;         /* 14: Initial IP value */
    WORD  e_cs;         /* 16: Initial (relative) CS value */
    WORD  e_lfarlc;     /* 18: File address of relocation table */
    WORD  e_ovno;       /* 1a: Overlay number */
    WORD  e_res[4];     /* 1c: Reserved words */
    WORD  e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
    WORD  e_oeminfo;    /* 26: OEM information; e_oemid specific */
    WORD  e_res2[10];   /* 28: Reserved words */
    DWORD e_lfanew;     /* 3c: Offset to extended header */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;
#include <poppack.h>

#include <pshpack2.h>
typedef struct
{
    WORD  ne_magic;             /* 00 NE signature 'NE' */
    BYTE  ne_ver;               /* 02 Linker version number */
    BYTE  ne_rev;               /* 03 Linker revision number */
    WORD  ne_enttab;            /* 04 Offset to entry table relative to NE */
    WORD  ne_cbenttab;          /* 06 Length of entry table in bytes */
    LONG  ne_crc;               /* 08 Checksum */
    WORD  ne_flags;             /* 0c Flags about segments in this file */
    WORD  ne_autodata;          /* 0e Automatic data segment number */
    WORD  ne_heap;              /* 10 Initial size of local heap */
    WORD  ne_stack;             /* 12 Initial size of stack */
    DWORD ne_csip;              /* 14 Initial CS:IP */
    DWORD ne_sssp;              /* 18 Initial SS:SP */
    WORD  ne_cseg;              /* 1c # of entries in segment table */
    WORD  ne_cmod;              /* 1e # of entries in module reference tab. */
    WORD  ne_cbnrestab;         /* 20 Length of nonresident-name table     */
    WORD  ne_segtab;            /* 22 Offset to segment table */
    WORD  ne_rsrctab;           /* 24 Offset to resource table */
    WORD  ne_restab;            /* 26 Offset to resident-name table */
    WORD  ne_modtab;            /* 28 Offset to module reference table */
    WORD  ne_imptab;            /* 2a Offset to imported name table */
    DWORD ne_nrestab;           /* 2c Offset to nonresident-name table */
    WORD  ne_cmovent;           /* 30 # of movable entry points */
    WORD  ne_align;             /* 32 Logical sector alignment shift count */
    WORD  ne_cres;              /* 34 # of resource segments */
    BYTE  ne_exetyp;            /* 36 Flags indicating target OS */
    BYTE  ne_flagsothers;       /* 37 Additional information flags */
    WORD  ne_pretthunks;        /* 38 Offset to return thunks */
    WORD  ne_psegrefbytes;      /* 3a Offset to segment ref. bytes */
    WORD  ne_swaparea;          /* 3c Reserved by Microsoft */
    WORD  ne_expver;            /* 3e Expected Windows version number */
} IMAGE_OS2_HEADER, *PIMAGE_OS2_HEADER;
#include <poppack.h>

#define IMAGE_DOS_SIGNATURE    0x5A4D     /* MZ   */
#define IMAGE_OS2_SIGNATURE    0x454E     /* NE   */
#define IMAGE_OS2_SIGNATURE_LE 0x454C     /* LE   */
#define IMAGE_OS2_SIGNATURE_LX 0x584C     /* LX */
#define IMAGE_VXD_SIGNATURE    0x454C     /* LE   */
#define IMAGE_NT_SIGNATURE     0x00004550 /* PE00 */

#define DONT_RESOLVE_DLL_REFERENCES         0x00000001
#define LOAD_LIBRARY_AS_DATAFILE            0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH       0x00000008
#define LOAD_IGNORE_CODE_AUTHZ_LEVEL        0x00000010
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE      0x00000020
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE  0x00000040
#define LOAD_LIBRARY_REQUIRE_SIGNED_TARGET  0x00000080

#define M_PI_2  1.57079632679489661923
#define M_PI    3.14159265358979323846
#define M_PI_4         0.7853981633974483096

#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif

/*
 * Resource directory stuff
 */
typedef struct _IMAGE_RESOURCE_DIRECTORY {
	DWORD	Characteristics;
	DWORD	TimeDateStamp;
	WORD	MajorVersion;
	WORD	MinorVersion;
	WORD	NumberOfNamedEntries;
	WORD	NumberOfIdEntries;
	/*  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[]; */
} IMAGE_RESOURCE_DIRECTORY,*PIMAGE_RESOURCE_DIRECTORY;

#define	IMAGE_RESOURCE_NAME_IS_STRING		0x80000000
#define	IMAGE_RESOURCE_DATA_IS_DIRECTORY	0x80000000

typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY {
	union {
		struct {
#ifdef BITFIELDS_BIGENDIAN
			unsigned NameIsString:1;
			unsigned NameOffset:31;
#else
			unsigned NameOffset:31;
			unsigned NameIsString:1;
#endif
		} ;
		DWORD   Name;
#ifdef WORDS_BIGENDIAN
		WORD    __pad;
		WORD    Id;
#else
		WORD    Id;
		WORD    __pad;
#endif
	} ;
	union {
		DWORD   OffsetToData;
		struct {
#ifdef BITFIELDS_BIGENDIAN
			unsigned DataIsDirectory:1;
			unsigned OffsetToDirectory:31;
#else
			unsigned OffsetToDirectory:31;
			unsigned DataIsDirectory:1;
#endif
		} ;
	} ;
} IMAGE_RESOURCE_DIRECTORY_ENTRY,*PIMAGE_RESOURCE_DIRECTORY_ENTRY;


typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING {
	WORD	Length;
	CHAR	NameString[ 1 ];
} IMAGE_RESOURCE_DIRECTORY_STRING,*PIMAGE_RESOURCE_DIRECTORY_STRING;

typedef struct _IMAGE_RESOURCE_DIR_STRING_U {
	WORD	Length;
	WCHAR	NameString[ 1 ];
} IMAGE_RESOURCE_DIR_STRING_U,*PIMAGE_RESOURCE_DIR_STRING_U;

typedef struct _IMAGE_RESOURCE_DATA_ENTRY {
	DWORD	OffsetToData;
	DWORD	Size;
	DWORD	CodePage;
	DWORD	Reserved;
} IMAGE_RESOURCE_DATA_ENTRY,*PIMAGE_RESOURCE_DATA_ENTRY;

typedef struct _LDR_RESOURCE_INFO
{
    ULONG_PTR Type;
    ULONG_PTR Name;
    ULONG Language;
} LDR_RESOURCE_INFO, *PLDR_RESOURCE_INFO;

/* These are indexes into the DataDirectory array */
#define IMAGE_FILE_EXPORT_DIRECTORY		0
#define IMAGE_FILE_IMPORT_DIRECTORY		1
#define IMAGE_FILE_RESOURCE_DIRECTORY		2
#define IMAGE_FILE_EXCEPTION_DIRECTORY		3
#define IMAGE_FILE_SECURITY_DIRECTORY		4
#define IMAGE_FILE_BASE_RELOCATION_TABLE	5
#define IMAGE_FILE_DEBUG_DIRECTORY		6
#define IMAGE_FILE_DESCRIPTION_STRING		7
#define IMAGE_FILE_MACHINE_VALUE		8  /* Mips */
#define IMAGE_FILE_THREAD_LOCAL_STORAGE		9
#define IMAGE_FILE_CALLBACK_DIRECTORY		10

/* Directory Entries, indices into the DataDirectory array */

#define	IMAGE_DIRECTORY_ENTRY_EXPORT		0
#define	IMAGE_DIRECTORY_ENTRY_IMPORT		1
#define	IMAGE_DIRECTORY_ENTRY_RESOURCE		2
#define	IMAGE_DIRECTORY_ENTRY_EXCEPTION		3
#define	IMAGE_DIRECTORY_ENTRY_SECURITY		4
#define	IMAGE_DIRECTORY_ENTRY_BASERELOC		5
#define	IMAGE_DIRECTORY_ENTRY_DEBUG		6
#define	IMAGE_DIRECTORY_ENTRY_COPYRIGHT		7
#define	IMAGE_DIRECTORY_ENTRY_GLOBALPTR		8   /* (MIPS GP) */
#define	IMAGE_DIRECTORY_ENTRY_TLS		9
#define	IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	10
#define	IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	11
#define	IMAGE_DIRECTORY_ENTRY_IAT		12  /* Import Address Table */
#define	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	13
#define	IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR	14

typedef struct tagMESSAGE_RESOURCE_BLOCK {
	DWORD	LowId;
	DWORD	HighId;
	DWORD	OffsetToEntries;
} MESSAGE_RESOURCE_BLOCK,*PMESSAGE_RESOURCE_BLOCK;

typedef struct tagMESSAGE_RESOURCE_DATA {
	DWORD			NumberOfBlocks;
	MESSAGE_RESOURCE_BLOCK	Blocks[ 1 ];
} MESSAGE_RESOURCE_DATA,*PMESSAGE_RESOURCE_DATA;

typedef struct tagMESSAGE_RESOURCE_ENTRY {
	WORD	Length;
	WORD	Flags;
	BYTE	Text[1];
} MESSAGE_RESOURCE_ENTRY,*PMESSAGE_RESOURCE_ENTRY;
#define	MESSAGE_RESOURCE_UNICODE	0x0001

typedef struct _LDR_MODULE
{
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
    void*               BaseAddress;
    void*               EntryPoint;
    ULONG               SizeOfImage;
    UNICODE_STRING      FullDllName;
    UNICODE_STRING      BaseDllName;
    ULONG               Flags;
    SHORT               LoadCount;
    SHORT               TlsIndex;
    HANDLE              SectionHandle;
    ULONG               CheckSum;
    ULONG               TimeDateStamp;
    HANDLE              ActivationContext;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _IMAGE_TLS_DIRECTORY32 {
    DWORD   StartAddressOfRawData;
    DWORD   EndAddressOfRawData;
    DWORD   AddressOfIndex;
    DWORD   AddressOfCallBacks;
    DWORD   SizeOfZeroFill;
    DWORD   Characteristics;
} IMAGE_TLS_DIRECTORY32, *PIMAGE_TLS_DIRECTORY32;

typedef IMAGE_TLS_DIRECTORY32           IMAGE_TLS_DIRECTORY;

typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD	Characteristics;
	DWORD	TimeDateStamp;
	WORD	MajorVersion;
	WORD	MinorVersion;
	DWORD	Name;
	DWORD	Base;
	DWORD	NumberOfFunctions;
	DWORD	NumberOfNames;
	DWORD	AddressOfFunctions;
	DWORD	AddressOfNames;
	DWORD	AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY,*PIMAGE_EXPORT_DIRECTORY;

/* Import name entry */
typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD	Hint;
	BYTE	Name[1];
} IMAGE_IMPORT_BY_NAME,*PIMAGE_IMPORT_BY_NAME;

#include <pshpack2.h>

typedef struct _IMAGE_THUNK_DATA32 {
	union {
		DWORD ForwarderString;
		DWORD Function;
		DWORD Ordinal;
		DWORD AddressOfData;
	} u1;
} IMAGE_THUNK_DATA32,*PIMAGE_THUNK_DATA32;

/* Import module directory */

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
	union {
		DWORD	Characteristics; /* 0 for terminating null import descriptor  */
		DWORD	OriginalFirstThunk;	/* RVA to original unbound IAT */
	} DUMMYUNIONNAME;
	DWORD	TimeDateStamp;	/* 0 if not bound,
				 * -1 if bound, and real date\time stamp
				 *    in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT
				 * (new BIND)
				 * otherwise date/time stamp of DLL bound to
				 * (Old BIND)
				 */
	DWORD	ForwarderChain;	/* -1 if no forwarders */
	DWORD	Name;
	/* RVA to IAT (if bound this IAT has actual addresses) */
	DWORD	FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR,*PIMAGE_IMPORT_DESCRIPTOR;

typedef IMAGE_THUNK_DATA32              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA32             PIMAGE_THUNK_DATA;
typedef struct _IMAGE_BOUND_IMPORT_DESCRIPTOR
{
    DWORD   TimeDateStamp;
    WORD    OffsetModuleName;
    WORD    NumberOfModuleForwarderRefs;
/* Array of zero or more IMAGE_BOUND_FORWARDER_REF follows */
} IMAGE_BOUND_IMPORT_DESCRIPTOR,  *PIMAGE_BOUND_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_BOUND_FORWARDER_REF
{
    DWORD   TimeDateStamp;
    WORD    OffsetModuleName;
    WORD    Reserved;
} IMAGE_BOUND_FORWARDER_REF, *PIMAGE_BOUND_FORWARDER_REF;

typedef struct _IMAGE_BASE_RELOCATION
{
	DWORD	VirtualAddress;
	DWORD	SizeOfBlock;
	/* WORD	TypeOffset[1]; */
} IMAGE_BASE_RELOCATION,*PIMAGE_BASE_RELOCATION;
#include <poppack.h>

typedef struct tagACTCTXA {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCSTR  lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCSTR  lpAssemblyDirectory;
    LPCSTR  lpResourceName;
    LPCSTR  lpApplicationName;
    HMODULE hModule;
} ACTCTXA, *PACTCTXA;

typedef struct tagACTCTXW {
    ULONG   cbSize;
    DWORD   dwFlags;
    LPCWSTR lpSource;
    USHORT  wProcessorArchitecture;
    LANGID  wLangId;
    LPCWSTR lpAssemblyDirectory;
    LPCWSTR lpResourceName;
    LPCWSTR lpApplicationName;
    HMODULE hModule;
} ACTCTXW, *PACTCTXW;

typedef const ACTCTXA *PCACTCTXA;
typedef const ACTCTXW *PCACTCTXW;

/* flags to ACTCTX[AW] */
#define ACTCTX_FLAG_PROCESSOR_ARCHITECTURE_VALID  (0x00000001)
#define ACTCTX_FLAG_LANGID_VALID                  (0x00000002)
#define ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID      (0x00000004)
#define ACTCTX_FLAG_RESOURCE_NAME_VALID           (0x00000008)
#define ACTCTX_FLAG_SET_PROCESS_DEFAULT           (0x00000010)
#define ACTCTX_FLAG_APPLICATION_NAME_VALID        (0x00000020)
#define ACTCTX_FLAG_SOURCE_IS_ASSEMBLYREF         (0x00000040)
#define ACTCTX_FLAG_HMODULE_VALID                 (0x00000080)

/* those defines are (some of the) regular LDR_MODULE.Flags values */
#define LDR_IMAGE_IS_DLL                0x00000004
#define LDR_LOAD_IN_PROGRESS            0x00001000
#define LDR_UNLOAD_IN_PROGRESS          0x00002000
#define LDR_NO_DLL_CALLS                0x00040000
#define LDR_PROCESS_ATTACHED            0x00080000
#define LDR_MODULE_REBASED              0x00200000

/* these ones is Wine specific */
#define LDR_DONT_RESOLVE_REFS           0x40000000
#define LDR_WINE_INTERNAL               0x80000000

typedef struct _IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD TimeDateStamp;
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  DWORD VirtualAddress;
  DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_OPTIONAL_HEADER {

  /* Standard fields */

  WORD  Magic; /* 0x10b or 0x107 */	/* 0x00 */
  BYTE  MajorLinkerVersion;
  BYTE  MinorLinkerVersion;
  DWORD SizeOfCode;
  DWORD SizeOfInitializedData;
  DWORD SizeOfUninitializedData;
  DWORD AddressOfEntryPoint;		/* 0x10 */
  DWORD BaseOfCode;
  DWORD BaseOfData;

  /* NT additional fields */

  DWORD ImageBase;
  DWORD SectionAlignment;		/* 0x20 */
  DWORD FileAlignment;
  WORD  MajorOperatingSystemVersion;
  WORD  MinorOperatingSystemVersion;
  WORD  MajorImageVersion;
  WORD  MinorImageVersion;
  WORD  MajorSubsystemVersion;		/* 0x30 */
  WORD  MinorSubsystemVersion;
  DWORD Win32VersionValue;
  DWORD SizeOfImage;
  DWORD SizeOfHeaders;
  DWORD CheckSum;			/* 0x40 */
  WORD  Subsystem;
  WORD  DllCharacteristics;
  DWORD SizeOfStackReserve;
  DWORD SizeOfStackCommit;
  DWORD SizeOfHeapReserve;		/* 0x50 */
  DWORD SizeOfHeapCommit;
  DWORD LoaderFlags;
  DWORD NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]; /* 0x60 */
  /* 0xE0 */
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_NT_HEADERS {
  DWORD Signature; /* "PE"\0\0 */	/* 0x00 */
  IMAGE_FILE_HEADER FileHeader;		/* 0x04 */
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;	/* 0x18 */
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef IMAGE_NT_HEADERS32  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32 PIMAGE_NT_HEADERS;

#define	IMAGE_SUBSYSTEM_UNKNOWN			0
#define	IMAGE_SUBSYSTEM_NATIVE			1
#define	IMAGE_SUBSYSTEM_WINDOWS_GUI		2	/* Windows GUI subsystem */
#define	IMAGE_SUBSYSTEM_WINDOWS_CUI		3	/* Windows character subsystem */
#define	IMAGE_SUBSYSTEM_OS2_CUI			5
#define	IMAGE_SUBSYSTEM_POSIX_CUI		7
#define	IMAGE_SUBSYSTEM_NATIVE_WINDOWS		8	/* native Win9x driver */
#define	IMAGE_SUBSYSTEM_WINDOWS_CE_GUI		9	/* Windows CE subsystem */
#define	IMAGE_SUBSYSTEM_EFI_APPLICATION		10
#define	IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER	11
#define	IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER	12
#define	IMAGE_SUBSYSTEM_EFI_ROM			13
#define	IMAGE_SUBSYSTEM_XBOX			14
#define	IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION	16

#define IMAGE_FILE_RELOCS_STRIPPED	0x0001 /* No relocation info */
#define IMAGE_FILE_EXECUTABLE_IMAGE	0x0002
#define IMAGE_FILE_LINE_NUMS_STRIPPED   0x0004
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED  0x0008
#define IMAGE_FILE_AGGRESIVE_WS_TRIM	0x0010
#define IMAGE_FILE_LARGE_ADDRESS_AWARE	0x0020
#define IMAGE_FILE_16BIT_MACHINE	0x0040
#define IMAGE_FILE_BYTES_REVERSED_LO	0x0080
#define IMAGE_FILE_32BIT_MACHINE	0x0100
#define IMAGE_FILE_DEBUG_STRIPPED	0x0200
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP	0x0400
#define IMAGE_FILE_NET_RUN_FROM_SWAP	0x0800
#define IMAGE_FILE_SYSTEM		0x1000
#define IMAGE_FILE_DLL			0x2000
#define IMAGE_FILE_UP_SYSTEM_ONLY	0x4000
#define IMAGE_FILE_BYTES_REVERSED_HI	0x8000

/* DLL Characteristics */
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE          0x0040
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY       0x0080
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT             0x0100
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION          0x0200
#define IMAGE_DLLCHARACTERISTICS_NO_SEH                0x0400
#define IMAGE_DLLCHARACTERISTICS_NO_BIND               0x0800
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER            0x2000
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE 0x8000

#define SECTION_QUERY              0x0001
#define SECTION_MAP_WRITE          0x0002
#define SECTION_MAP_READ           0x0004
#define SECTION_MAP_EXECUTE        0x0008
#define SECTION_EXTEND_SIZE        0x0010
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020
#define SECTION_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED|0x01f)

#define SEC_FILE                0x00800000
#define SEC_IMAGE               0x01000000
#define SEC_RESERVE             0x04000000
#define SEC_COMMIT              0x08000000
#define SEC_NOCACHE             0x10000000
#define SEC_LARGE_PAGES         0x80000000
#define MEM_IMAGE               SEC_IMAGE

typedef unsigned short RTL_ATOM, *PRTL_ATOM;

/* Wine doesn't implement atom table as NT does:
 * - in NT, atom tables are user space tables, which ntdll directly accesses
 * - on Wine, (even local) atom tables are wineserver objects, hence a HANDLE
 */
typedef struct PblMap *RTL_ATOM_TABLE, **PRTL_ATOM_TABLE;

typedef enum _ATOM_INFORMATION_CLASS {
   AtomBasicInformation         = 0,
   AtomTableInformation         = 1,
} ATOM_INFORMATION_CLASS;

typedef struct _ATOM_BASIC_INFORMATION {
   USHORT       ReferenceCount;
   USHORT       Pinned;
   USHORT       NameLength;
   WCHAR        Name[1];
} ATOM_BASIC_INFORMATION, *PATOM_BASIC_INFORMATION;

#define INVALID_ATOM        ((ATOM)0)
#define MAXINTATOM          0xc000
#define MAKEINTATOM(atom)   ((char*)((ULONG_PTR)((WORD)(atom))))

#define BM_GETCHECK            0x00f0
#define BM_SETCHECK            0x00f1
#define BM_GETSTATE            0x00f2
#define BM_SETSTATE            0x00f3
#define BM_SETSTYLE            0x00f4
#define BM_CLICK               0x00f5
#define BM_GETIMAGE            0x00f6
#define BM_SETIMAGE            0x00f7
#define BM_SETDONTCLICK        0x00f8

/* Button states */
#define BST_UNCHECKED          0x0000
#define BST_CHECKED            0x0001
#define BST_INDETERMINATE      0x0002
#define BST_PUSHED             0x0004
#define BST_FOCUS              0x0008

/*** Button control styles ***/
#define BS_PUSHBUTTON          __MSABI_LONG(0x00000000)
#define BS_DEFPUSHBUTTON       __MSABI_LONG(0x00000001)
#define BS_CHECKBOX            __MSABI_LONG(0x00000002)
#define BS_AUTOCHECKBOX        __MSABI_LONG(0x00000003)
#define BS_RADIOBUTTON         __MSABI_LONG(0x00000004)
#define BS_3STATE              __MSABI_LONG(0x00000005)
#define BS_AUTO3STATE          __MSABI_LONG(0x00000006)
#define BS_GROUPBOX            __MSABI_LONG(0x00000007)
#define BS_USERBUTTON          __MSABI_LONG(0x00000008)
#define BS_AUTORADIOBUTTON     __MSABI_LONG(0x00000009)
#define BS_PUSHBOX             __MSABI_LONG(0x0000000A)
#define BS_OWNERDRAW           __MSABI_LONG(0x0000000B)
#define BS_TYPEMASK            __MSABI_LONG(0x0000000F)
#define BS_LEFTTEXT            __MSABI_LONG(0x00000020)
#define BS_RIGHTBUTTON         BS_LEFTTEXT

#define BS_TEXT                __MSABI_LONG(0x00000000)
#define BS_ICON                __MSABI_LONG(0x00000040)
#define BS_BITMAP              __MSABI_LONG(0x00000080)
#define BS_LEFT                __MSABI_LONG(0x00000100)
#define BS_RIGHT               __MSABI_LONG(0x00000200)
#define BS_CENTER              __MSABI_LONG(0x00000300)
#define BS_TOP                 __MSABI_LONG(0x00000400)
#define BS_BOTTOM              __MSABI_LONG(0x00000800)
#define BS_VCENTER             __MSABI_LONG(0x00000C00)
#define BS_PUSHLIKE            __MSABI_LONG(0x00001000)
#define BS_MULTILINE           __MSABI_LONG(0x00002000)
#define BS_NOTIFY              __MSABI_LONG(0x00004000)
#define BS_FLAT                __MSABI_LONG(0x00008000)


/*** Button notification codes ***/
#define BN_CLICKED             0
#define BN_PAINT               1
#define BN_HILITE              2
#define BN_PUSHED              BN_HILITE
#define BN_UNHILITE            3
#define BN_UNPUSHED            BN_UNHILITE
#define BN_DISABLE             4
#define BN_DOUBLECLICKED       5
#define BN_DBLCLK              BN_DOUBLECLICKED
#define BN_SETFOCUS            6
#define BN_KILLFOCUS           7

#define OBM_RDRVERT            32559
#define OBM_RDRHORZ            32660
#define OBM_RDR2DIM            32661
#define OBM_TRTYPE             32732 /* FIXME: Wine-only */
#define OBM_LFARROWI           32734
#define OBM_RGARROWI           32735
#define OBM_DNARROWI           32736
#define OBM_UPARROWI           32737
#define OBM_COMBO              32738
#define OBM_MNARROW            32739
#define OBM_LFARROWD           32740
#define OBM_RGARROWD           32741
#define OBM_DNARROWD           32742
#define OBM_UPARROWD           32743
#define OBM_RESTORED           32744
#define OBM_ZOOMD              32745
#define OBM_REDUCED            32746
#define OBM_RESTORE            32747
#define OBM_ZOOM               32748
#define OBM_REDUCE             32749
#define OBM_LFARROW            32750
#define OBM_RGARROW            32751
#define OBM_DNARROW            32752
#define OBM_UPARROW            32753
#define OBM_CLOSE              32754
#define OBM_OLD_RESTORE        32755
#define OBM_OLD_ZOOM           32756
#define OBM_OLD_REDUCE         32757
#define OBM_BTNCORNERS         32758
#define OBM_CHECKBOXES         32759
#define OBM_CHECK              32760
#define OBM_BTSIZE             32761
#define OBM_OLD_LFARROW        32762
#define OBM_OLD_RGARROW        32763
#define OBM_OLD_DNARROW        32764
#define OBM_OLD_UPARROW        32765
#define OBM_SIZE               32766
#define OBM_OLD_CLOSE          32767

/*** Virtual key codes ***/
#define VK_LBUTTON             0x01
#define VK_RBUTTON             0x02
#define VK_CANCEL              0x03
#define VK_MBUTTON             0x04
#define VK_XBUTTON1            0x05
#define VK_XBUTTON2            0x06
/*                             0x07  Undefined */
#define VK_BACK                0x08
#define VK_TAB                 0x09
/*                             0x0A-0x0B  Undefined */
#define VK_CLEAR               0x0C
#define VK_RETURN              0x0D
/*                             0x0E-0x0F  Undefined */
#define VK_SHIFT               0x10
#define VK_CONTROL             0x11
#define VK_MENU                0x12
#define VK_PAUSE               0x13
#define VK_CAPITAL             0x14

#define VK_KANA                0x15
#define VK_HANGEUL             VK_KANA
#define VK_HANGUL              VK_KANA
#define VK_JUNJA               0x17
#define VK_FINAL               0x18
#define VK_HANJA               0x19
#define VK_KANJI               VK_HANJA

/*                             0x1A       Undefined */
#define VK_ESCAPE              0x1B

#define VK_CONVERT             0x1C
#define VK_NONCONVERT          0x1D
#define VK_ACCEPT              0x1E
#define VK_MODECHANGE          0x1F

#define VK_SPACE               0x20
#define VK_PRIOR               0x21
#define VK_NEXT                0x22
#define VK_END                 0x23
#define VK_HOME                0x24
#define VK_LEFT                0x25
#define VK_UP                  0x26
#define VK_RIGHT               0x27
#define VK_DOWN                0x28
#define VK_SELECT              0x29
#define VK_PRINT               0x2A /* OEM specific in Windows 3.1 SDK */
#define VK_EXECUTE             0x2B
#define VK_SNAPSHOT            0x2C
#define VK_INSERT              0x2D
#define VK_DELETE              0x2E
#define VK_HELP                0x2F
/* VK_0 - VK-9                 0x30-0x39  Use ASCII instead */
/*                             0x3A-0x40  Undefined */
/* VK_A - VK_Z                 0x41-0x5A  Use ASCII instead */
#define VK_LWIN                0x5B
#define VK_RWIN                0x5C
#define VK_APPS                0x5D
/*                             0x5E Unassigned */
#define VK_SLEEP               0x5F
#define VK_NUMPAD0             0x60
#define VK_NUMPAD1             0x61
#define VK_NUMPAD2             0x62
#define VK_NUMPAD3             0x63
#define VK_NUMPAD4             0x64
#define VK_NUMPAD5             0x65
#define VK_NUMPAD6             0x66
#define VK_NUMPAD7             0x67
#define VK_NUMPAD8             0x68
#define VK_NUMPAD9             0x69
#define VK_MULTIPLY            0x6A
#define VK_ADD                 0x6B
#define VK_SEPARATOR           0x6C
#define VK_SUBTRACT            0x6D
#define VK_DECIMAL             0x6E
#define VK_DIVIDE              0x6F
#define VK_F1                  0x70
#define VK_F2                  0x71
#define VK_F3                  0x72
#define VK_F4                  0x73
#define VK_F5                  0x74
#define VK_F6                  0x75
#define VK_F7                  0x76
#define VK_F8                  0x77
#define VK_F9                  0x78
#define VK_F10                 0x79
#define VK_F11                 0x7A
#define VK_F12                 0x7B
#define VK_F13                 0x7C
#define VK_F14                 0x7D
#define VK_F15                 0x7E
#define VK_F16                 0x7F
#define VK_F17                 0x80
#define VK_F18                 0x81
#define VK_F19                 0x82
#define VK_F20                 0x83
#define VK_F21                 0x84
#define VK_F22                 0x85
#define VK_F23                 0x86
#define VK_F24                 0x87
/*                             0x88-0x8F  Unassigned */
#define VK_NUMLOCK             0x90
#define VK_SCROLL              0x91
#define VK_OEM_NEC_EQUAL       0x92
#define VK_OEM_FJ_JISHO        0x92
#define VK_OEM_FJ_MASSHOU      0x93
#define VK_OEM_FJ_TOUROKU      0x94
#define VK_OEM_FJ_LOYA         0x95
#define VK_OEM_FJ_ROYA         0x96
/*                             0x97-0x9F  Unassigned */
/*
 * differencing between right and left shift/control/alt key.
 * Used only by GetAsyncKeyState() and GetKeyState().
 */
#define VK_LSHIFT              0xA0
#define VK_RSHIFT              0xA1
#define VK_LCONTROL            0xA2
#define VK_RCONTROL            0xA3
#define VK_LMENU               0xA4
#define VK_RMENU               0xA5

#define VK_BROWSER_BACK        0xA6
#define VK_BROWSER_FORWARD     0xA7
#define VK_BROWSER_REFRESH     0xA8
#define VK_BROWSER_STOP        0xA9
#define VK_BROWSER_SEARCH      0xAA
#define VK_BROWSER_FAVORITES   0xAB
#define VK_BROWSER_HOME        0xAC
#define VK_VOLUME_MUTE         0xAD
#define VK_VOLUME_DOWN         0xAE
#define VK_VOLUME_UP           0xAF
#define VK_MEDIA_NEXT_TRACK    0xB0
#define VK_MEDIA_PREV_TRACK    0xB1
#define VK_MEDIA_STOP          0xB2
#define VK_MEDIA_PLAY_PAUSE    0xB3
#define VK_LAUNCH_MAIL         0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1         0xB6
#define VK_LAUNCH_APP2         0xB7

/*                             0xB8-0xB9  Unassigned */
#define VK_OEM_1               0xBA
#define VK_OEM_PLUS            0xBB
#define VK_OEM_COMMA           0xBC
#define VK_OEM_MINUS           0xBD
#define VK_OEM_PERIOD          0xBE
#define VK_OEM_2               0xBF
#define VK_OEM_3               0xC0
/*                             0xC1-0xDA  Unassigned */
#define VK_OEM_4               0xDB
#define VK_OEM_5               0xDC
#define VK_OEM_6               0xDD
#define VK_OEM_7               0xDE
#define VK_OEM_8               0xDF
/*                             0xE0       OEM specific */
#define VK_OEM_AX              0xE1  /* "AX" key on Japanese AX keyboard */
#define VK_OEM_102             0xE2  /* "<>" or "\|" on RT 102-key keyboard */
#define VK_ICO_HELP            0xE3  /* Help key on ICO */
#define VK_ICO_00              0xE4  /* 00 key on ICO */
#define VK_PROCESSKEY          0xE5
#define VK_ICO_CLEAR           0xE6

#define VK_PACKET              0xE7
/*                             0xE8       Unassigned */

#define VK_OEM_RESET           0xE9
#define VK_OEM_JUMP            0xEA
#define VK_OEM_PA1             0xEB
#define VK_OEM_PA2             0xEC
#define VK_OEM_PA3             0xED
#define VK_OEM_WSCTRL          0xEE
#define VK_OEM_CUSEL           0xEF
#define VK_OEM_ATTN            0xF0
#define VK_OEM_FINISH          0xF1
#define VK_OEM_COPY            0xF2
#define VK_OEM_AUTO            0xF3
#define VK_OEM_ENLW            0xF4
#define VK_OEM_BACKTAB         0xF5
#define VK_ATTN                0xF6
#define VK_CRSEL               0xF7
#define VK_EXSEL               0xF8
#define VK_EREOF               0xF9
#define VK_PLAY                0xFA
#define VK_ZOOM                0xFB
#define VK_NONAME              0xFC
#define VK_PA1                 0xFD
#define VK_OEM_CLEAR           0xFE
/*                             0xFF       Unassigned */

#define ES_LEFT                __MSABI_LONG(0x00000000)
#define ES_CENTER              __MSABI_LONG(0x00000001)
#define ES_RIGHT               __MSABI_LONG(0x00000002)
#define ES_MULTILINE           __MSABI_LONG(0x00000004)
#define ES_UPPERCASE           __MSABI_LONG(0x00000008)
#define ES_LOWERCASE           __MSABI_LONG(0x00000010)
#define ES_PASSWORD            __MSABI_LONG(0x00000020)
#define ES_AUTOVSCROLL         __MSABI_LONG(0x00000040)
#define ES_AUTOHSCROLL         __MSABI_LONG(0x00000080)
#define ES_NOHIDESEL           __MSABI_LONG(0x00000100)
#define ES_COMBO               __MSABI_LONG(0x00000200) /* Undocumented. Parent is a combobox */
#define ES_OEMCONVERT          __MSABI_LONG(0x00000400)
#define ES_READONLY            __MSABI_LONG(0x00000800)
#define ES_WANTRETURN          __MSABI_LONG(0x00001000)
#define ES_NUMBER              __MSABI_LONG(0x00002000)

#define LBS_NOTIFY             __MSABI_LONG(0x00000001)
#define LBS_SORT               __MSABI_LONG(0x00000002)
#define LBS_NOREDRAW           __MSABI_LONG(0x00000004)
#define LBS_MULTIPLESEL        __MSABI_LONG(0x00000008)
#define LBS_OWNERDRAWFIXED     __MSABI_LONG(0x00000010)
#define LBS_OWNERDRAWVARIABLE  __MSABI_LONG(0x00000020)
#define LBS_HASSTRINGS         __MSABI_LONG(0x00000040)
#define LBS_USETABSTOPS        __MSABI_LONG(0x00000080)
#define LBS_NOINTEGRALHEIGHT   __MSABI_LONG(0x00000100)
#define LBS_MULTICOLUMN        __MSABI_LONG(0x00000200)
#define LBS_WANTKEYBOARDINPUT  __MSABI_LONG(0x00000400)
#define LBS_EXTENDEDSEL        __MSABI_LONG(0x00000800)
#define LBS_DISABLENOSCROLL    __MSABI_LONG(0x00001000)
#define LBS_NODATA             __MSABI_LONG(0x00002000)
#define LBS_NOSEL              __MSABI_LONG(0x00004000)
#define LBS_COMBOBOX           __MSABI_LONG(0x00008000)
#define LBS_STANDARD           (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

#define CBS_SIMPLE             __MSABI_LONG(0x00000001)
#define CBS_DROPDOWN           __MSABI_LONG(0x00000002)
#define CBS_DROPDOWNLIST       __MSABI_LONG(0x00000003)
#define CBS_OWNERDRAWFIXED     __MSABI_LONG(0x00000010)
#define CBS_OWNERDRAWVARIABLE  __MSABI_LONG(0x00000020)
#define CBS_AUTOHSCROLL        __MSABI_LONG(0x00000040)
#define CBS_OEMCONVERT         __MSABI_LONG(0x00000080)
#define CBS_SORT               __MSABI_LONG(0x00000100)
#define CBS_HASSTRINGS         __MSABI_LONG(0x00000200)
#define CBS_NOINTEGRALHEIGHT   __MSABI_LONG(0x00000400)
#define CBS_DISABLENOSCROLL    __MSABI_LONG(0x00000800)

#define CBS_UPPERCASE          __MSABI_LONG(0x00002000)
#define CBS_LOWERCASE          __MSABI_LONG(0x00004000)

#define EM_GETSEL              0x00b0
#define EM_SETSEL              0x00b1
#define EM_GETRECT             0x00b2
#define EM_SETRECT             0x00b3
#define EM_SETRECTNP           0x00b4
#define EM_SCROLL              0x00b5
#define EM_LINESCROLL          0x00b6
#define EM_SCROLLCARET         0x00b7
#define EM_GETMODIFY           0x00b8
#define EM_SETMODIFY           0x00b9
#define EM_GETLINECOUNT        0x00ba
#define EM_LINEINDEX           0x00bb
#define EM_SETHANDLE           0x00bc
#define EM_GETHANDLE           0x00bd
#define EM_GETTHUMB            0x00be
/* Unassigned 0x00bf and 0x00c0 */
#define EM_LINELENGTH          0x00c1
#define EM_REPLACESEL          0x00c2
#define EM_SETFONT             0x00c3 /* DDK / Win16 */
#define EM_GETLINE             0x00c4
#define EM_LIMITTEXT           0x00c5
#define EM_SETLIMITTEXT        EM_LIMITTEXT
#define EM_CANUNDO             0x00c6
#define EM_UNDO                0x00c7
#define EM_FMTLINES            0x00c8
#define EM_LINEFROMCHAR        0x00c9
#define EM_SETWORDBREAK        0x00ca /* DDK / Win16 */
#define EM_SETTABSTOPS         0x00cb
#define EM_SETPASSWORDCHAR     0x00cc
#define EM_EMPTYUNDOBUFFER     0x00cd
#define EM_GETFIRSTVISIBLELINE 0x00ce
#define EM_SETREADONLY         0x00cf
#define EM_SETWORDBREAKPROC    0x00d0
#define EM_GETWORDBREAKPROC    0x00d1
#define EM_GETPASSWORDCHAR     0x00d2
#define EM_SETMARGINS          0x00d3
#define EM_GETMARGINS          0x00d4
#define EM_GETLIMITTEXT        0x00d5
#define EM_POSFROMCHAR         0x00d6
#define EM_CHARFROMPOS         0x00d7
#define EM_SETIMESTATUS        0x00d8
#define EM_GETIMESTATUS        0x00d9

#include "list.h"

struct thread_input
{
    struct desktop*         desktop;    
    HWND          focus;         /* focus window */
    HWND          capture;       /* capture window */
    HWND          active;        /* active window */
    HWND          menu_owner;    /* current menu owner window */
    HWND          move_size;     /* current moving/resizing window */
    HWND          caret;         /* caret window */
    RECT            caret_rect;    /* caret rectangle */
    int                    caret_hide;    /* caret hide count */
    int                    caret_state;   /* caret on/off state */
    unsigned int          cursor;        /* current cursor */
    int                    cursor_count;  /* cursor show count */
    struct list            msg_list;      /* list of hardware messages */
    unsigned char          keystate[256]; /* state of each key */    
    struct hook_table*     hooks;
};

#define IDOK                   1
#define IDCANCEL               2
#define IDABORT                3
#define IDRETRY                4
#define IDIGNORE               5
#define IDYES                  6
#define IDNO                   7
#define IDCLOSE                8
#define IDHELP                 9
#define IDTRYAGAIN             10
#define IDCONTINUE             11
#define IDTIMEOUT              32000

#define SC_SIZE                0xf000
#define SC_MOVE                0xf010
#define SC_MINIMIZE            0xf020
#define SC_MAXIMIZE            0xf030
#define SC_NEXTWINDOW          0xf040
#define SC_PREVWINDOW          0xf050
#define SC_CLOSE               0xf060
#define SC_VSCROLL             0xf070
#define SC_HSCROLL             0xf080
#define SC_MOUSEMENU           0xf090
#define SC_KEYMENU             0xf100
#define SC_ARRANGE             0xf110
#define SC_RESTORE             0xf120
#define SC_TASKLIST            0xf130
#define SC_SCREENSAVE          0xf140
#define SC_HOTKEY              0xf150

/* Win32 4.0 */
#define SC_DEFAULT             0xf160
#define SC_MONITORPOWER        0xf170
#define SC_CONTEXTHELP         0xf180
#define SC_SEPARATOR           0xf00f

#define DS_ABSALIGN            __MSABI_LONG(0x00000001)
#define DS_SYSMODAL            __MSABI_LONG(0x00000002)
#define DS_3DLOOK              __MSABI_LONG(0x00000004) /* win95 */
#define DS_FIXEDSYS            __MSABI_LONG(0x00000008) /* win95 */
#define DS_NOFAILCREATE        __MSABI_LONG(0x00000010) /* win95 */
#define DS_LOCALEDIT           __MSABI_LONG(0x00000020)
#define DS_SETFONT             __MSABI_LONG(0x00000040)
#define DS_MODALFRAME          __MSABI_LONG(0x00000080)
#define DS_NOIDLEMSG           __MSABI_LONG(0x00000100)
#define DS_SETFOREGROUND       __MSABI_LONG(0x00000200) /* win95 */
#define DS_CONTROL             __MSABI_LONG(0x00000400) /* win95 */
#define DS_CENTER              __MSABI_LONG(0x00000800) /* win95 */
#define DS_CENTERMOUSE         __MSABI_LONG(0x00001000) /* win95 */
#define DS_CONTEXTHELP         __MSABI_LONG(0x00002000) /* win95 */
#define DS_USEPIXELS           __MSABI_LONG(0x00008000)
#define DS_SHELLFONT           (DS_SETFONT | DS_FIXEDSYS)

typedef struct {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	CHAR szCSDVersion[128];
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef struct {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WCHAR szCSDVersion[128];
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW, RTL_OSVERSIONINFOW, *PRTL_OSVERSIONINFOW;

#define LMEM_FIXED          0
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_DISCARDED      0x4000
#define LMEM_INVALID_HANDLE 0x8000
#define LMEM_LOCKCOUNT      0x00FF

#define LPTR (LMEM_FIXED | LMEM_ZEROINIT)
#define LHND (LMEM_MOVEABLE | LMEM_ZEROINIT)

#define CF_TEXT                1
#define CF_BITMAP              2
#define CF_METAFILEPICT        3
#define CF_SYLK                4
#define CF_DIF                 5
#define CF_TIFF                6
#define CF_OEMTEXT             7
#define CF_DIB                 8
#define CF_PALETTE             9
#define CF_PENDATA             10
#define CF_RIFF                11
#define CF_WAVE                12
#define CF_UNICODETEXT         13
#define CF_ENHMETAFILE         14
#define CF_HDROP               15
#define CF_LOCALE              16
#define CF_DIBV5               17
#define CF_MAX                 18

#define CF_OWNERDISPLAY        0x0080
#define CF_DSPTEXT             0x0081
#define CF_DSPBITMAP           0x0082
#define CF_DSPMETAFILEPICT     0x0083
#define CF_DSPENHMETAFILE      0x008E

/* "Private" formats don't get GlobalFree()'d */
#define CF_PRIVATEFIRST        0x0200
#define CF_PRIVATELAST         0x02FF

/* "GDIOBJ" formats do get DeleteObject()'d */
#define CF_GDIOBJFIRST         0x0300
#define CF_GDIOBJLAST          0x03FF

#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_DISCARDED      0x4000
#define GMEM_LOCKCOUNT      0x00ff
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define MF_INSERT              __MSABI_LONG(0x00000000)
#define MF_CHANGE              __MSABI_LONG(0x00000080)
#define MF_APPEND              __MSABI_LONG(0x00000100)
#define MF_DELETE              __MSABI_LONG(0x00000200)
#define MF_REMOVE              __MSABI_LONG(0x00001000)
#define MF_END                 __MSABI_LONG(0x00000080)

#define MF_ENABLED             __MSABI_LONG(0x00000000)
#define MF_GRAYED              __MSABI_LONG(0x00000001)
#define MF_DISABLED            __MSABI_LONG(0x00000002)
#define MF_STRING              __MSABI_LONG(0x00000000)
#define MF_BITMAP              __MSABI_LONG(0x00000004)
#define MF_UNCHECKED           __MSABI_LONG(0x00000000)
#define MF_CHECKED             __MSABI_LONG(0x00000008)
#define MF_POPUP               __MSABI_LONG(0x00000010)
#define MF_MENUBARBREAK        __MSABI_LONG(0x00000020)
#define MF_MENUBREAK           __MSABI_LONG(0x00000040)
#define MF_UNHILITE            __MSABI_LONG(0x00000000)
#define MF_HILITE              __MSABI_LONG(0x00000080)
#define MF_OWNERDRAW           __MSABI_LONG(0x00000100)
#define MF_USECHECKBITMAPS     __MSABI_LONG(0x00000200)
#define MF_BYCOMMAND           __MSABI_LONG(0x00000000)
#define MF_BYPOSITION          __MSABI_LONG(0x00000400)
#define MF_SEPARATOR           __MSABI_LONG(0x00000800)
#define MF_DEFAULT             __MSABI_LONG(0x00001000)
#define MF_SYSMENU             __MSABI_LONG(0x00002000)
#define MF_HELP                __MSABI_LONG(0x00004000)
#define MF_RIGHTJUSTIFY        __MSABI_LONG(0x00004000)
#define MF_MOUSESELECT         __MSABI_LONG(0x00008000)

/* Flags for extended menu item types */
#define MFT_STRING             MF_STRING
#define MFT_BITMAP             MF_BITMAP
#define MFT_MENUBARBREAK       MF_MENUBARBREAK
#define MFT_MENUBREAK          MF_MENUBREAK
#define MFT_OWNERDRAW          MF_OWNERDRAW
#define MFT_RADIOCHECK         __MSABI_LONG(0x00000200)
#define MFT_SEPARATOR          MF_SEPARATOR
#define MFT_RIGHTORDER         __MSABI_LONG(0x00002000)
#define MFT_RIGHTJUSTIFY       MF_RIGHTJUSTIFY

/* Flags for extended menu item states */
#define MFS_GRAYED             __MSABI_LONG(0x00000003)
#define MFS_DISABLED           MFS_GRAYED
#define MFS_CHECKED            MF_CHECKED
#define MFS_HILITE             MF_HILITE
#define MFS_ENABLED            MF_ENABLED
#define MFS_UNCHECKED          MF_UNCHECKED
#define MFS_UNHILITE           MF_UNHILITE
#define MFS_DEFAULT            MF_DEFAULT

/* DDK / Win16 defines */
#define MFS_MASK               __MSABI_LONG(0x0000108B)
#define MFS_HOTTRACKDRAWN      __MSABI_LONG(0x10000000)
#define MFS_CACHEDBMP          __MSABI_LONG(0x20000000)
#define MFS_BOTTOMGAPDROP      __MSABI_LONG(0x40000000)
#define MFS_TOPGAPDROP         __MSABI_LONG(0x80000000)
#define MFS_GAPDROP            (MFS_BOTTOMGAPDROP | MFS_TOPGAPDROP)

#define IMAGE_SIZEOF_SHORT_NAME 8

typedef struct _IMAGE_SECTION_HEADER {
  BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD  NumberOfRelocations;
  WORD  NumberOfLinenumbers;
  DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define SEND_HWMSG_INJECTED    0x01

enum message_type
{
    MSG_ASCII,
    MSG_UNICODE,
    MSG_NOTIFY,
    MSG_CALLBACK,
    MSG_CALLBACK_RESULT,
    MSG_POSTED,
    MSG_HARDWARE,
    MSG_WINEVENT,
    MSG_HOOK_LL
};

typedef struct _DEV_BROADCAST_HDR
{
    DWORD dbch_size;
    DWORD dbch_devicetype;
    DWORD dbch_reserved;
} DEV_BROADCAST_HDR, *PDEV_BROADCAST_HDR;

#define WM_DDE_INITIATE   0x3E0
#define WM_DDE_TERMINATE  0x3E1
#define WM_DDE_ADVISE	  0x3E2
#define WM_DDE_UNADVISE   0x3E3
#define WM_DDE_ACK	  0x3E4
#define WM_DDE_DATA	  0x3E5
#define WM_DDE_REQUEST	  0x3E6
#define WM_DDE_POKE	  0x3E7
#define WM_DDE_EXECUTE	  0x3E8
#define WM_DDE_LAST	  WM_DDE_EXECUTE
#define WM_DDE_FIRST	  WM_DDE_INITIATE

struct hardware_msg_data
{
    LPARAM        info;
    int             x;
    int             y;
    unsigned int    hw_id;
    unsigned int    flags;
    union
    {
        int type;
        struct
        {
            int            type;
            unsigned int   message;
            unsigned short vkey;
            unsigned short scan;
        } kbd;
        struct
        {
            int            type;
            int            x;
            int            y;
            unsigned int   data;
        } mouse;
    } rawinput;
};

struct callback_msg_data
{
    void*    callback;
    LPARAM        data;
    LPARAM        result;
};

struct winevent_msg_data
{
    HANDLE   hook;
    DWORD     tid;
    void*    hook_proc;
    WCHAR    module[MAX_PATH];
};

typedef struct
{
    unsigned short unused:12, fResponse:1, fRelease:1, reserved:1, fAckReq:1;
    short cfFormat;
    BYTE Value[1];		/* undetermined array */
} DDEDATA;

/* DDEADVISE: hOptions in WM_DDE_ADVISE message */
typedef struct
{
    unsigned short reserved:14, fDeferUpd:1, fAckReq:1;
    short cfFormat;
} DDEADVISE;

/* DDEPOKE: hData in WM_DDE_POKE message. */
typedef struct
{
    unsigned short unused:13, fRelease:1, fReserved:2;
    short cfFormat;
    BYTE Value[1];   	/* undetermined array */
} DDEPOKE;

#define SEND_MSG_ABORT_IF_HUNG  0x01

#define MAXIMUM_WAIT_OBJECTS 64

#define WAIT_FAILED		0xffffffff
#define WAIT_OBJECT_0		0
#define WAIT_ABANDONED		STATUS_ABANDONED_WAIT_0
#define WAIT_ABANDONED_0	STATUS_ABANDONED_WAIT_0
#define WAIT_IO_COMPLETION	STATUS_USER_APC
#define WAIT_TIMEOUT		STATUS_TIMEOUT

struct global_cursor
{
    int                  x;                /* cursor position */
    int                  y;
    RECT                 clip;             /* cursor clip rectangle */
    unsigned int         clip_msg;         /* message to post for cursor clip changes */
    unsigned int         last_change;      /* time of last position change */
    HWND                 win;              /* window that contains the cursor */
};


struct desktop
{
    //struct object        obj;              /* object header */
    unsigned int         flags;            /* desktop flags */
    struct winstation   *winstation;       /* winstation this desktop belongs to */
    HWND                 top_window;       /* desktop window for this desktop */
    HWND                  msg_window;       /* HWND_MESSAGE top window */
    struct hook_table   *global_hooks;     /* table of global hooks on this desktop */
    struct list          hotkeys;          /* list of registered hotkeys */
    struct timeout_user *close_timeout;    /* timeout before closing the desktop */
    struct thread_input *foreground_input; /* thread input of foreground thread */
    unsigned int         users;            /* processes and threads using this desktop */
    struct global_cursor cursor;           /* global cursor information */
    unsigned char        keystate[256];    /* asynchronous key state */
    void*                 wnd;
};

/* Find* structures */
typedef struct _WIN32_FIND_DATAA
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    CHAR      cFileName[260];
    CHAR      cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    WCHAR     cFileName[260];
    WCHAR     cAlternateFileName[14];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

#define DRIVE_UNKNOWN              0
#define DRIVE_NO_ROOT_DIR          1
#define DRIVE_REMOVABLE            2
#define DRIVE_FIXED                3
#define DRIVE_REMOTE               4
/* Win32 additions */
#define DRIVE_CDROM                5
#define DRIVE_RAMDISK              6

#define HWND_16(h32)      (LOWORD(h32))
#define HMENU_16(h32)     (LOWORD(h32))
#define HDWP_16(h32)      (LOWORD(h32))
#define HDROP_16(h32)     (LOWORD(h32))
#define HDC_16(h32)       (LOWORD(h32))
#define HFONT_16(h32)     (LOWORD(h32))
#define HRGN_16(h32)      (LOWORD(h32))
#define HBITMAP_16(h32)   (LOWORD(h32))
#define HBRUSH_16(h32)    (LOWORD(h32))
#define HPALETTE_16(h32)  (LOWORD(h32))
#define HPEN_16(h32)      (LOWORD(h32))
#define HACCEL_16(h32)    (LOWORD(h32))

#define HWND_32(h16)      ((HWND)      (ULONG_PTR)(h16))
#define HMENU_32(h16)     ((HMENU)     (ULONG_PTR)(h16))
#define HDWP_32(h16)      ((HDWP)      (ULONG_PTR)(h16))
#define HDROP_32(h16)     ((HDROP)     (ULONG_PTR)(h16))
#define HDC_32(h16)       ((HDC)       (ULONG_PTR)(h16))
#define HFONT_32(h16)     ((HFONT)     (ULONG_PTR)(h16))
#define HRGN_32(h16)      ((HRGN)      (ULONG_PTR)(h16))
#define HBITMAP_32(h16)   ((HBITMAP)   (ULONG_PTR)(h16))
#define HBRUSH_32(h16)    ((HBRUSH)    (ULONG_PTR)(h16))
#define HPALETTE_32(h16)  ((HPALETTE)  (ULONG_PTR)(h16))
#define HPEN_32(h16)      ((HPEN)      (ULONG_PTR)(h16))
#define HACCEL_32(h16)    ((HACCEL)    (ULONG_PTR)(h16))

#define stc1        0x0440

#define SBS_HORZ               __MSABI_LONG(0x00000000)
#define SBS_VERT               __MSABI_LONG(0x00000001)
#define SBS_TOPALIGN           __MSABI_LONG(0x00000002)
#define SBS_LEFTALIGN          __MSABI_LONG(0x00000002)
#define SBS_BOTTOMALIGN        __MSABI_LONG(0x00000004)
#define SBS_RIGHTALIGN         __MSABI_LONG(0x00000004)
#define SBS_SIZEBOXTOPLEFTALIGN __MSABI_LONG(0x00000002)
#define SBS_SIZEBOXBOTTOMRIGHTALIGN __MSABI_LONG(0x00000004)
#define SBS_SIZEBOX            __MSABI_LONG(0x00000008)
#define SBS_SIZEGRIP           __MSABI_LONG(0x00000010)

#define SS_LEFT                __MSABI_LONG(0x00000000)
#define SS_CENTER              __MSABI_LONG(0x00000001)
#define SS_RIGHT               __MSABI_LONG(0x00000002)
#define SS_ICON                __MSABI_LONG(0x00000003)
#define SS_BLACKRECT           __MSABI_LONG(0x00000004)
#define SS_GRAYRECT            __MSABI_LONG(0x00000005)
#define SS_WHITERECT           __MSABI_LONG(0x00000006)
#define SS_BLACKFRAME          __MSABI_LONG(0x00000007)
#define SS_GRAYFRAME           __MSABI_LONG(0x00000008)
#define SS_WHITEFRAME          __MSABI_LONG(0x00000009)
#define SS_USERITEM            __MSABI_LONG(0x0000000A)
#define SS_SIMPLE              __MSABI_LONG(0x0000000B)
#define SS_LEFTNOWORDWRAP      __MSABI_LONG(0x0000000C)
#define SS_OWNERDRAW           __MSABI_LONG(0x0000000D)
#define SS_BITMAP              __MSABI_LONG(0x0000000E)
#define SS_ENHMETAFILE         __MSABI_LONG(0x0000000F)
#define SS_ETCHEDHORZ          __MSABI_LONG(0x00000010)
#define SS_ETCHEDVERT          __MSABI_LONG(0x00000011)
#define SS_ETCHEDFRAME         __MSABI_LONG(0x00000012)
#define SS_TYPEMASK            __MSABI_LONG(0x0000001F)

#define SS_REALSIZECONTROL     __MSABI_LONG(0x00000040)
#define SS_NOPREFIX            __MSABI_LONG(0x00000080)
#define SS_NOTIFY              __MSABI_LONG(0x00000100)
#define SS_CENTERIMAGE         __MSABI_LONG(0x00000200)
#define SS_RIGHTJUST           __MSABI_LONG(0x00000400)
#define SS_REALSIZEIMAGE       __MSABI_LONG(0x00000800)
#define SS_SUNKEN              __MSABI_LONG(0x00001000)
#define SS_EDITCONTROL         __MSABI_LONG(0x00002000)
#define SS_ENDELLIPSIS         __MSABI_LONG(0x00004000)
#define SS_PATHELLIPSIS        __MSABI_LONG(0x00008000)
#define SS_WORDELLIPSIS        __MSABI_LONG(0x0000C000)
#define SS_ELLIPSISMASK        SS_WORDELLIPSIS

#define UPDATE_NONCLIENT       0x01
#define UPDATE_ERASE           0x02
#define UPDATE_PAINT           0x04
#define UPDATE_INTERNALPAINT   0x08
#define UPDATE_ALLCHILDREN     0x10
#define UPDATE_NOCHILDREN      0x20
#define UPDATE_NOREGION        0x40
#define UPDATE_DELAYED_ERASE   0x80

#define SET_WINPOS_PAINT_SURFACE 0x01
#define SET_WINPOS_PIXEL_FORMAT  0x02

void USER_Lock(void);
void USER_Unlock(void);

struct winstation
{
    unsigned int       flags;              /* winstation flags */
    PblMap *           desktops;           /* list of desktops of this winstation */
    struct clipboard  *clipboard;          /* clipboard information */
    struct atom_table *atom_table;         /* global atom table */
    WCHAR              name[MAX_PATH];
};

#define	SECURITY_DESCRIPTOR_REVISION	1
#define	SECURITY_DESCRIPTOR_REVISION1	1

/* Heap flags */

#define HEAP_NO_SERIALIZE               0x00000001
#define HEAP_GROWABLE                   0x00000002
#define HEAP_GENERATE_EXCEPTIONS        0x00000004
#define HEAP_ZERO_MEMORY                0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020
#define HEAP_FREE_CHECKING_ENABLED      0x00000040
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080
#define HEAP_CREATE_ALIGN_16            0x00010000
#define HEAP_CREATE_ENABLE_TRACING      0x00020000
#define HEAP_CREATE_ENABLE_EXECUTE      0x00040000

/* This flag allows it to create heaps shared by all processes under win95,
   FIXME: correct name */
#define HEAP_SHARED                     0x04000000

typedef struct _PROCESS_HEAP_ENTRY
{
    LPVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[3];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    } ;
} PROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION                   0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE        0x0002
#define PROCESS_HEAP_ENTRY_BUSY               0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE           0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE           0x0020

#define DUPLICATE_CLOSE_SOURCE     0x00000001
#define DUPLICATE_SAME_ACCESS      0x00000002

#define DUP_HANDLE_CLOSE_SOURCE  DUPLICATE_CLOSE_SOURCE
#define DUP_HANDLE_SAME_ACCESS   DUPLICATE_SAME_ACCESS
#define DUP_HANDLE_MAKE_GLOBAL   0x80000000  /* Not a Windows flag */

/* NtGlobalFlag bits */
#define FLG_STOP_ON_EXCEPTION            0x00000001
#define FLG_SHOW_LDR_SNAPS               0x00000002
#define FLG_DEBUG_INITIAL_COMMAND        0x00000004
#define FLG_STOP_ON_HUNG_GUI             0x00000008
#define FLG_HEAP_ENABLE_TAIL_CHECK       0x00000010
#define FLG_HEAP_ENABLE_FREE_CHECK       0x00000020
#define FLG_HEAP_VALIDATE_PARAMETERS     0x00000040
#define FLG_HEAP_VALIDATE_ALL            0x00000080
#define FLG_APPLICATION_VERIFIER         0x00000100
#define FLG_POOL_ENABLE_TAGGING          0x00000400
#define FLG_HEAP_ENABLE_TAGGING          0x00000800
#define FLG_USER_STACK_TRACE_DB          0x00001000
#define FLG_KERNEL_STACK_TRACE_DB        0x00002000
#define FLG_MAINTAIN_OBJECT_TYPELIST     0x00004000
#define FLG_HEAP_ENABLE_TAG_BY_DLL       0x00008000
#define FLG_DISABLE_STACK_EXTENSION      0x00010000
#define FLG_ENABLE_CSRDEBUG              0x00020000
#define FLG_ENABLE_KDEBUG_SYMBOL_LOAD    0x00040000
#define FLG_DISABLE_PAGE_KERNEL_STACKS   0x00080000
#define FLG_ENABLE_SYSTEM_CRIT_BREAKS    0x00100000
#define FLG_HEAP_DISABLE_COALESCING      0x00200000
#define FLG_ENABLE_CLOSE_EXCEPTIONS      0x00400000
#define FLG_ENABLE_EXCEPTION_LOGGING     0x00800000
#define FLG_ENABLE_HANDLE_TYPE_TAGGING   0x01000000
#define FLG_HEAP_PAGE_ALLOCS             0x02000000
#define FLG_DEBUG_INITIAL_COMMAND_EX     0x04000000
#define FLG_DISABLE_DBGPRINT             0x08000000
#define FLG_CRITSEC_EVENT_CREATION       0x10000000
#define FLG_LDR_TOP_DOWN                 0x20000000
#define FLG_ENABLE_HANDLE_EXCEPTIONS     0x40000000
#define FLG_DISABLE_PROTDLLS             0x80000000

typedef struct _RTL_HEAP_DEFINITION {
    ULONG Length; /* = sizeof(RTL_HEAP_DEFINITION) */

    ULONG Unknown[11];
} RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

typedef enum _HEAP_INFORMATION_CLASS {
    HeapCompatibilityInformation,
} HEAP_INFORMATION_CLASS;

typedef struct tagMEMORYSTATUS
{
    DWORD    dwLength;
    DWORD    dwMemoryLoad;
    SIZE_T   dwTotalPhys;
    SIZE_T   dwAvailPhys;
    SIZE_T   dwTotalPageFile;
    SIZE_T   dwAvailPageFile;
    SIZE_T   dwTotalVirtual;
    SIZE_T   dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

#include <pshpack8.h>
typedef struct tagMEMORYSTATUSEX {
  DWORD dwLength;
  DWORD dwMemoryLoad;
  DWORDLONG ullTotalPhys;
  DWORDLONG ullAvailPhys;
  DWORDLONG ullTotalPageFile;
  DWORDLONG ullAvailPageFile;
  DWORDLONG ullTotalVirtual;
  DWORDLONG ullAvailVirtual;
  DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
#include <poppack.h>

#define VER_PLATFORM_WIN32s                     0
#define VER_PLATFORM_WIN32_WINDOWS              1
#define VER_PLATFORM_WIN32_NT                   2

#define MINCHAR       0x80
#define MAXCHAR       0x7f
#define MINSHORT      0x8000
#define MAXSHORT      0x7fff
#define MINLONG       0x80000000
#define MAXLONG       0x7fffffff
#define MAXBYTE       0xff
#define MAXWORD       0xffff
#define MAXDWORD      0xffffffff

typedef BOOL (CALLBACK *ENUMRESTYPEPROCA)(HMODULE,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESTYPEPROCW)(HMODULE,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCA)(HMODULE,LPCSTR,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCW)(HMODULE,LPCWSTR,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCA)(HMODULE,LPCSTR,LPCSTR,WORD,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCW)(HMODULE,LPCWSTR,LPCWSTR,WORD,LONG_PTR);

void post_win_event( struct KThread *thread, unsigned int event, HWND win, unsigned int object_id, unsigned int child_id, void* hook_proc, const WCHAR *module, HHOOK hook);

#define DIRECTORY_QUERY                 0x0001
#define DIRECTORY_TRAVERSE              0x0002
#define DIRECTORY_CREATE_OBJECT         0x0004
#define DIRECTORY_CREATE_SUBDIRECTORY   0x0008
#define DIRECTORY_ALL_ACCESS            (STANDARD_RIGHTS_REQUIRED | 0xF)

#define STD_INPUT_HANDLE        ((DWORD) -10)
#define STD_OUTPUT_HANDLE       ((DWORD) -11)
#define STD_ERROR_HANDLE        ((DWORD) -12)

#define NTAPI __stdcall

typedef VOID (NTAPI * WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN );
typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK;

typedef enum _EVENT_TYPE {
  NotificationEvent,
  SynchronizationEvent
} EVENT_TYPE, *PEVENT_TYPE;

#define CREATE_EVENT_MANUAL_RESET 1
#define CREATE_EVENT_INITIAL_SET  2

#define CREATE_MUTEX_INITIAL_OWNER 1

#define CREATE_WAITABLE_TIMER_MANUAL_RESET 1

#define OBJ_INHERIT          0x00000002
#define OBJ_PERMANENT        0x00000010
#define OBJ_EXCLUSIVE        0x00000020
#define OBJ_CASE_INSENSITIVE 0x00000040
#define OBJ_OPENIF           0x00000080
#define OBJ_OPENLINK         0x00000100
#define OBJ_KERNEL_HANDLE    0x00000200
#define OBJ_VALID_ATTRIBUTES 0x000003F2

#define DELETE                     0x00010000
#define READ_CONTROL               0x00020000
#define WRITE_DAC                  0x00040000
#define WRITE_OWNER                0x00080000
#define SYNCHRONIZE                0x00100000
#define STANDARD_RIGHTS_REQUIRED   0x000f0000

#define STANDARD_RIGHTS_READ       READ_CONTROL
#define STANDARD_RIGHTS_WRITE      READ_CONTROL
#define STANDARD_RIGHTS_EXECUTE    READ_CONTROL

#define STANDARD_RIGHTS_ALL        0x001f0000

#define SPECIFIC_RIGHTS_ALL        0x0000ffff

#define GENERIC_READ               0x80000000
#define GENERIC_WRITE              0x40000000
#define GENERIC_EXECUTE            0x20000000
#define GENERIC_ALL                0x10000000

#define MAXIMUM_ALLOWED            0x02000000
#define ACCESS_SYSTEM_SECURITY     0x01000000

#define EVENT_QUERY_STATE          0x0001
#define EVENT_MODIFY_STATE         0x0002
#define EVENT_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define SEMAPHORE_MODIFY_STATE     0x0002
#define SEMAPHORE_ALL_ACCESS       (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define MUTANT_QUERY_STATE         0x0001
#define MUTANT_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|MUTANT_QUERY_STATE)

#define JOB_OBJECT_ASSIGN_PROCESS           0x0001
#define JOB_OBJECT_SET_ATTRIBUTES           0x0002
#define JOB_OBJECT_QUERY                    0x0004
#define JOB_OBJECT_TERMINATE                0x0008
#define JOB_OBJECT_SET_SECURITY_ATTRIBUTES  0x0010
#define JOB_OBJECT_ALL_ACCESS               (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x1f)

#define TIMER_QUERY_STATE          0x0001
#define TIMER_MODIFY_STATE         0x0002
#define TIMER_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

#define MUTEX_MODIFY_STATE              MUTANT_QUERY_STATE
#define MUTEX_ALL_ACCESS                MUTANT_ALL_ACCESS

typedef enum _JOBOBJECTINFOCLASS
{
    JobObjectBasicAccountingInformation = 1,
    JobObjectBasicLimitInformation,
    JobObjectBasicProcessIdList,
    JobObjectBasicUIRestrictions,
    JobObjectSecurityLimitInformation,
    JobObjectEndOfJobTimeInformation,
    JobObjectAssociateCompletionPortInformation,
    JobObjectBasicAndIoAccountingInformation,
    JobObjectExtendedLimitInformation,
    JobObjectJobSetInformation,
    MaxJobObjectInfoClass
} JOBOBJECTINFOCLASS;

typedef enum _TIMER_TYPE {
    NotificationTimer,
    SynchronizationTimer
} TIMER_TYPE;

typedef void (CALLBACK *PTIMERAPCROUTINE)(LPVOID,DWORD,DWORD);
typedef VOID (CALLBACK *PTIMER_APC_ROUTINE) ( PVOID, ULONG, LONG );

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  } DUMMYUNIONNAME;

  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

#define PIPE_ACCESS_INBOUND  1
#define PIPE_ACCESS_OUTBOUND 2
#define PIPE_ACCESS_DUPLEX   3

#define PIPE_CLIENT_END       0
#define PIPE_SERVER_END       1
#define PIPE_READMODE_BYTE    0
#define PIPE_READMODE_MESSAGE 2
#define PIPE_TYPE_BYTE        0
#define PIPE_TYPE_MESSAGE     4

#define PIPE_WAIT   0
#define PIPE_NOWAIT 1

#define PIPE_UNLIMITED_INSTANCES 255

#define FILE_TYPE_UNKNOWN       0
#define FILE_TYPE_DISK          1
#define FILE_TYPE_CHAR          2
#define FILE_TYPE_PIPE          3
#define FILE_TYPE_REMOTE        32768

/* File encryption status
 */
#define FILE_ENCRYPTABLE         0
#define FILE_IS_ENCRYPTED        1
#define FILE_SYSTEM_ATTR         2
#define FILE_ROOT_DIR            3
#define FILE_SYSTEM_DIR          4
#define FILE_UNKNOWN             5
#define FILE_SYSTEM_NOT_SUPPORT  6
#define FILE_USER_DISALLOWED     7
#define FILE_READ_ONLY           8
#define FILE_DIR_DISALOWED       9

/* File creation flags
 */
#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define CREATE_NEW              1
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define TRUNCATE_EXISTING       5

/* flags for NtCreateFile and NtOpenFile */
#define FILE_DIRECTORY_FILE             0x00000001
#define FILE_WRITE_THROUGH              0x00000002
#define FILE_SEQUENTIAL_ONLY            0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING  0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT       0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT    0x00000020
#define FILE_NON_DIRECTORY_FILE         0x00000040
#define FILE_CREATE_TREE_CONNECTION     0x00000080
#define FILE_COMPLETE_IF_OPLOCKED       0x00000100
#define FILE_NO_EA_KNOWLEDGE            0x00000200
#define FILE_OPEN_FOR_RECOVERY          0x00000400
#define FILE_RANDOM_ACCESS              0x00000800
#define FILE_DELETE_ON_CLOSE            0x00001000
#define FILE_OPEN_BY_FILE_ID            0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT     0x00004000
#define FILE_NO_COMPRESSION             0x00008000
#define FILE_RESERVE_OPFILTER           0x00100000
#define FILE_TRANSACTED_MODE            0x00200000
#define FILE_OPEN_OFFLINE_FILE          0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY  0x00800000

#define FILE_ATTRIBUTE_VALID_FLAGS      0x00007fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS  0x000031a7

/* status for NtCreateFile or NtOpenFile */
#define FILE_SUPERSEDED                 0
#define FILE_OPENED                     1
#define FILE_CREATED                    2
#define FILE_OVERWRITTEN                3
#define FILE_EXISTS                     4
#define FILE_DOES_NOT_EXIST             5

/* disposition for NtCreateFile */
#define FILE_SUPERSEDE                  0
#define FILE_OPEN                       1
#define FILE_CREATE                     2
#define FILE_OPEN_IF                    3
#define FILE_OVERWRITE                  4
#define FILE_OVERWRITE_IF               5
#define FILE_MAXIMUM_DISPOSITION        5

/* Characteristics of a File System */
#define FILE_REMOVABLE_MEDIA            0x00000001
#define FILE_READ_ONLY_DEVICE           0x00000002
#define FILE_FLOPPY_DISKETTE            0x00000004
#define FILE_WRITE_ONE_MEDIA            0x00000008
#define FILE_REMOTE_DEVICE              0x00000010
#define FILE_DEVICE_IS_MOUNTED          0x00000020
#define FILE_VIRTUAL_VOLUME             0x00000040
#define FILE_AUTOGENERATED_DEVICE_NAME  0x00000080
#define FILE_DEVICE_SECURE_OPEN         0x00000100

/* options for NtCreateNamedPipeFile */
#define FILE_PIPE_INBOUND               0x00000000
#define FILE_PIPE_OUTBOUND              0x00000001
#define FILE_PIPE_FULL_DUPLEX           0x00000002

/* options for pipe's type */
#define FILE_PIPE_TYPE_MESSAGE          0x00000001
#define FILE_PIPE_TYPE_BYTE             0x00000000
/* and client / server end */
#define FILE_PIPE_SERVER_END            0x00000001
#define FILE_PIPE_CLIENT_END            0x00000000

#define FILE_READ_DATA            0x0001    /* file & pipe */
#define FILE_LIST_DIRECTORY       0x0001    /* directory */
#define FILE_WRITE_DATA           0x0002    /* file & pipe */
#define FILE_ADD_FILE             0x0002    /* directory */
#define FILE_APPEND_DATA          0x0004    /* file */
#define FILE_ADD_SUBDIRECTORY     0x0004    /* directory */
#define FILE_CREATE_PIPE_INSTANCE 0x0004    /* named pipe */
#define FILE_READ_EA              0x0008    /* file & directory */
#define FILE_READ_PROPERTIES      FILE_READ_EA
#define FILE_WRITE_EA             0x0010    /* file & directory */
#define FILE_WRITE_PROPERTIES     FILE_WRITE_EA
#define FILE_EXECUTE              0x0020    /* file */
#define FILE_TRAVERSE             0x0020    /* directory */
#define FILE_DELETE_CHILD         0x0040    /* directory */
#define FILE_READ_ATTRIBUTES      0x0080    /* all */
#define FILE_WRITE_ATTRIBUTES     0x0100    /* all */
#define FILE_ALL_ACCESS           (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x1ff)

#define NMPWAIT_WAIT_FOREVER		0xffffffff
#define NMPWAIT_NOWAIT			0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT	0x00000000

typedef struct _OVERLAPPED {
#ifdef WORDS_BIGENDIAN
        ULONG_PTR InternalHigh;
        ULONG_PTR Internal;
#else
        ULONG_PTR Internal;
        ULONG_PTR InternalHigh;
#endif
        union {
            struct {
#ifdef WORDS_BIGENDIAN
                DWORD OffsetHigh;
                DWORD Offset;
#else
                DWORD Offset;
                DWORD OffsetHigh;
#endif
            } DUMMYSTRUCTNAME;
            PVOID Pointer;
        } DUMMYUNIONNAME;
        HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _FILE_PIPE_LOCAL_INFORMATION {
    ULONG NamedPipeType;
    ULONG NamedPipeConfiguration;
    ULONG MaximumInstances;
    ULONG CurrentInstances;
    ULONG InboundQuota;
    ULONG ReadDataAvailable;
    ULONG OutboundQuota;
    ULONG WriteQuotaAvailable;
    ULONG NamedPipeState;
    ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileObjectIdInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileAttributeTagInformation,
    FileTrackingInformation,
    FileIdBothDirectoryInformation,
    FileIdFullDirectoryInformation,
    FileValidDataLengthInformation,
    FileShortNameInformation = 40,
    /* 41, 42, 43 undocumented */
    FileSfioReserveInformation = 44,
    FileSfioVolumeInformation = 45,
    FileHardLinkInformation = 46,
    /* 47 undocumented */
    FileNormalizedNameInformation = 48,
    /* 49 undocumented */
    FileIdGlobalTxDirectoryInformation = 50,
    /* 51, 52, 53 undocumented */
    FileStandardLinkInformation = 54,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

#define MAILSLOT_NO_MESSAGE             ((DWORD)-1)
#define MAILSLOT_WAIT_FOREVER           ((DWORD)-1)

typedef struct _FILE_MAILSLOT_QUERY_INFORMATION {
    ULONG MaximumMessageSize;
    ULONG MailslotQuota;
    ULONG NextMessageSize;
    ULONG MessagesAvailable;
    LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION, *PFILE_MAILSLOT_QUERY_INFORMATION;

typedef struct _FILE_MAILSLOT_SET_INFORMATION {
    LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION, *PFILE_MAILSLOT_SET_INFORMATION;

#define IO_COMPLETION_QUERY_STATE  0x0001
#define IO_COMPLETION_MODIFY_STATE 0x0002
#define IO_COMPLETION_ALL_ACCESS   (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

typedef struct _FILE_COMPLETION_INFORMATION {
    HANDLE CompletionPort;
    ULONG_PTR CompletionKey;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

typedef VOID (CALLBACK *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPOVERLAPPED);
typedef VOID (CALLBACK *PRTL_OVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPVOID);

typedef enum _MEMORY_RESOURCE_NOTIFICATION_TYPE {
    LowMemoryResourceNotification,
    HighMemoryResourceNotification
} MEMORY_RESOURCE_NOTIFICATION_TYPE;

typedef union _RTL_RUN_ONCE {
    PVOID Ptr;
} RTL_RUN_ONCE, *PRTL_RUN_ONCE;
typedef RTL_RUN_ONCE  INIT_ONCE;
typedef PRTL_RUN_ONCE PINIT_ONCE;
typedef PRTL_RUN_ONCE LPINIT_ONCE;
#define INIT_ONCE_STATIC_INIT       RTL_RUN_ONCE_INIT
#define INIT_ONCE_CHECK_ONLY        RTL_RUN_ONCE_CHECK_ONLY
#define INIT_ONCE_ASYNC             RTL_RUN_ONCE_ASYNC
#define INIT_ONCE_INIT_FAILED       RTL_RUN_ONCE_INIT_FAILED
/* initialization callback prototype */
typedef BOOL (WINAPI *PINIT_ONCE_FN)(PINIT_ONCE,PVOID,PVOID*);

#define RTL_RUN_ONCE_CHECK_ONLY     0x00000001
#define RTL_RUN_ONCE_ASYNC          0x00000002
#define RTL_RUN_ONCE_INIT_FAILED    0x00000004

typedef DWORD WINAPI RTL_RUN_ONCE_INIT_FN(PRTL_RUN_ONCE, PVOID, PVOID*);
typedef RTL_RUN_ONCE_INIT_FN *PRTL_RUN_ONCE_INIT_FN;
#endif