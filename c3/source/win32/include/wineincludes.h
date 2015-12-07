#ifndef __WINEINCLUDES_H__
#define __WINEINCLUDES_H__

#undef BYTE
#undef WORD
#undef DWORD
#undef LONG

#include "kalloc.h"

#define CALLBACK __stdcall

#undef strcasecmp
int strcasecmp( const char *str1, const char *str2 );
void *interlocked_cmpxchg_ptr( void **dest, void *xchg, void *compare );
int interlocked_xchg_add( int *dest, int incr );
int interlocked_cmpxchg( int *dest, int xchg, int compare );

#define HeapAlloc(x, y, z) kalloc(z)
#define HeapFree(x, y, z) kfree(z)

#define debugstr_w(x) ""
#define debugstr_a(x) x

#define WARN printf
#define TRACE printf

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

typedef unsigned short  WCHAR;
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

#define WINAPI

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

typedef short CSHORT;
typedef CSHORT *PCSHORT;

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

typedef struct _UNICODE_STRING {
  USHORT Length;        /* bytes */
  USHORT MaximumLength; /* bytes */
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef const UNICODE_STRING *PCUNICODE_STRING;

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

#define MAKEINTRESOURCE(i) ((ULONG_PTR)((WORD)(i)))

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

static __inline void *ULongToHandle(ULONG ul)
{
    return (void *)(ULONG_PTR)ul;
}
static __inline ULONG HandleToULong(const void *h)
{
    return (ULONG)(ULONG_PTR)h;
}

#define IS_INTRESOURCE(x)   (((ULONG_PTR)(x) >> 16) == 0)

#endif