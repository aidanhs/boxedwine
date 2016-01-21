#ifndef __WINBASE_H__
#define __WINBASE_H__

#include "windef.h"
#include "winerror.h"
#include "pbl.h"

#define DUP_HANDLE_CLOSE_SOURCE  DUPLICATE_CLOSE_SOURCE
#define DUP_HANDLE_SAME_ACCESS   DUPLICATE_SAME_ACCESS
#define DUP_HANDLE_MAKE_GLOBAL   0x80000000

# define strcasecmp _stricmp

#define __TRY __try
#define __EXCEPT(func) __except((func)(GetExceptionInformation()))
#define __FINALLY(func) __finally { (func)(!AbnormalTermination()); }
#define __ENDTRY /*nothing*/
#define __EXCEPT_PAGE_FAULT __except(GetExceptionCode_k() == EXCEPTION_ACCESS_VIOLATION)
#define __EXCEPT_ALL __except(EXCEPTION_EXECUTE_HANDLER)
#define GetExceptionCode_k    _exception_code
#define GetExceptionInformation (struct _EXCEPTION_POINTERS *)_exception_info
#define AbnormalTermination _abnormal_termination

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.570796326794896619
#endif

#define M_PI_4      0.785398163397448309616

#define __WINE_ALLOC_SIZE_k(x)
#define TRACE printf
#define ERR printf
#define FIXME printf
#define WARN printf
#define FIXME_(x) printf
#define ERR_(x) printf
#define TRACE_(x) printf
#define WARN_(x) printf

void USER_Lock();
void USER_Unlock();

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _KERNEL32_
#define WINBASEAPI
#else
#define WINBASEAPI DECLSPEC_IMPORT
#endif

#ifdef _ADVAPI32_
#define WINADVAPI
#else
#define WINADVAPI DECLSPEC_IMPORT
#endif

  /* Windows Exit Procedure flag values */
#define	WEP_FREE_DLL        0
#define	WEP_SYSTEM_EXIT     1

typedef DWORD (CALLBACK *LPTHREAD_START_ROUTINE)(LPVOID);

typedef VOID (WINAPI *PFIBER_START_ROUTINE)( LPVOID lpFiberParameter );
typedef PFIBER_START_ROUTINE LPFIBER_START_ROUTINE;

#define FIBER_FLAG_FLOAT_SWITCH     1

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

#define CRITICAL_SECTION_NO_DEBUG_INFO RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO

typedef RTL_SRWLOCK SRWLOCK;
typedef PRTL_SRWLOCK PSRWLOCK;

typedef WAITORTIMERCALLBACKFUNC WAITORTIMERCALLBACK;

#define CONDITION_VARIABLE_INIT RTL_CONDITION_VARIABLE_INIT
#define CONDITION_VARIABLE_LOCKMODE_SHARED RTL_CONDITION_VARIABLE_LOCKMODE_SHARED
typedef RTL_CONDITION_VARIABLE CONDITION_VARIABLE, *PCONDITION_VARIABLE;

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID   lpBaseOfDll;
    DWORD    dwDebugInfoFileOffset;
    DWORD    nDebugInfoSize;
    LPVOID   lpImageName;
    WORD     fUnicode;
} LOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD  fUnicode;
    WORD  nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO;

typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO      Exception;
        CREATE_THREAD_DEBUG_INFO  CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO    ExitThread;
        EXIT_PROCESS_DEBUG_INFO   ExitProcess;
        LOAD_DLL_DEBUG_INFO       LoadDll;
        UNLOAD_DLL_DEBUG_INFO     UnloadDll;
        OUTPUT_DEBUG_STRING_INFO  DebugString;
        RIP_INFO                  RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

typedef PCONTEXT LPCONTEXT;
typedef PEXCEPTION_RECORD LPEXCEPTION_RECORD;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;

typedef LONG (CALLBACK *PTOP_LEVEL_EXCEPTION_FILTER)(PEXCEPTION_POINTERS);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

#define OFS_MAXPATHNAME 128
typedef struct _OFSTRUCT
{
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    WORD Reserved1;
    WORD Reserved2;
    CHAR szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *POFSTRUCT, *LPOFSTRUCT;

#define OF_READ               0x0000
#define OF_WRITE              0x0001
#define OF_READWRITE          0x0002
#define OF_SHARE_COMPAT       0x0000
#define OF_SHARE_EXCLUSIVE    0x0010
#define OF_SHARE_DENY_WRITE   0x0020
#define OF_SHARE_DENY_READ    0x0030
#define OF_SHARE_DENY_NONE    0x0040
#define OF_PARSE              0x0100
#define OF_DELETE             0x0200
#define OF_VERIFY             0x0400   /* Used with OF_REOPEN */
#define OF_SEARCH             0x0400   /* Used without OF_REOPEN */
#define OF_CANCEL             0x0800
#define OF_CREATE             0x1000
#define OF_PROMPT             0x2000
#define OF_EXIST              0x4000
#define OF_REOPEN             0x8000

/* SetErrorMode values */
#define SEM_FAILCRITICALERRORS      0x0001
#define SEM_NOGPFAULTERRORBOX       0x0002
#define SEM_NOALIGNMENTFAULTEXCEPT  0x0004
#define SEM_NOOPENFILEERRORBOX      0x8000

/* CopyFileEx flags */
#define COPY_FILE_FAIL_IF_EXISTS        0x00000001
#define COPY_FILE_RESTARTABLE           0x00000002
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE 0x00000004
#define COPY_FILE_ALLOW_DECRYPTED_DESTINATION  0x00000008
#define COPY_FILE_COPY_SYMLINK          0x00000800
#define COPY_FILE_NO_BUFFERING          0x00001000

/* return values for CopyProgressRoutine */
#define PROGRESS_CONTINUE   0
#define PROGRESS_CANCEL     1
#define PROGRESS_STOP       2
#define PROGRESS_QUIET      3

/* reason codes for CopyProgressRoutine */
#define CALLBACK_CHUNK_FINISHED 0
#define CALLBACK_STREAM_SWITCH  1

/* GetTempFileName() Flags */
#define TF_FORCEDRIVE	        0x80

#define DRIVE_UNKNOWN              0
#define DRIVE_NO_ROOT_DIR          1
#define DRIVE_REMOVABLE            2
#define DRIVE_FIXED                3
#define DRIVE_REMOTE               4
/* Win32 additions */
#define DRIVE_CDROM                5
#define DRIVE_RAMDISK              6

#define MAX_COMPUTERNAME_LENGTH    15

/* The security attributes structure */
typedef struct _SECURITY_ATTRIBUTES
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
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
#endif /* _FILETIME_ */

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

DECL_WINELIB_TYPE_AW(WIN32_FIND_DATA)
DECL_WINELIB_TYPE_AW(PWIN32_FIND_DATA)
DECL_WINELIB_TYPE_AW(LPWIN32_FIND_DATA)

typedef enum _FINDEX_INFO_LEVELS
{
	FindExInfoStandard,
	FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS
{
	FindExSearchNameMatch,
	FindExSearchLimitToDirectories,
	FindExSearchLimitToDevices,
	FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

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
    } DUMMYUNIONNAME;
} PROCESS_HEAP_ENTRY, *PPROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY;

#define PROCESS_HEAP_REGION                   0x0001
#define PROCESS_HEAP_UNCOMMITTED_RANGE        0x0002
#define PROCESS_HEAP_ENTRY_BUSY               0x0004
#define PROCESS_HEAP_ENTRY_MOVEABLE           0x0010
#define PROCESS_HEAP_ENTRY_DDESHARE           0x0020

#define INVALID_HANDLE_VALUE     ((HANDLE)~(ULONG_PTR)0)
#define INVALID_FILE_SIZE        (~0u)
#define INVALID_SET_FILE_POINTER (~0u)
#define INVALID_FILE_ATTRIBUTES  (~0u)

#define LOCKFILE_FAIL_IMMEDIATELY   1
#define LOCKFILE_EXCLUSIVE_LOCK     2

#define FLS_OUT_OF_INDEXES (~0u)
#define TLS_OUT_OF_INDEXES (~0u)

#define SHUTDOWN_NORETRY 1

/* comm */

#define CBR_110	0xFF10
#define CBR_300	0xFF11
#define CBR_600	0xFF12
#define CBR_1200	0xFF13
#define CBR_2400	0xFF14
#define CBR_4800	0xFF15
#define CBR_9600	0xFF16
#define CBR_14400	0xFF17
#define CBR_19200	0xFF18
#define CBR_38400	0xFF1B
#define CBR_56000	0xFF1F
#define CBR_57600       0xFF20
#define CBR_115200      0xFF21
#define CBR_128000	0xFF23
#define CBR_256000	0xFF27

#define NOPARITY	0
#define ODDPARITY	1
#define EVENPARITY	2
#define MARKPARITY	3
#define SPACEPARITY	4
#define ONESTOPBIT	0
#define ONE5STOPBITS	1
#define TWOSTOPBITS	2

#define IGNORE		0
#define INFINITE      0xFFFFFFFF

#define CE_RXOVER	0x0001
#define CE_OVERRUN	0x0002
#define CE_RXPARITY	0x0004
#define CE_FRAME	0x0008
#define CE_BREAK	0x0010
#define CE_CTSTO	0x0020
#define CE_DSRTO	0x0040
#define CE_RLSDTO	0x0080
#define CE_TXFULL	0x0100
#define CE_PTO		0x0200
#define CE_IOE		0x0400
#define CE_DNS		0x0800
#define CE_OOP		0x1000
#define CE_MODE	0x8000

#define IE_BADID	-1
#define IE_OPEN	-2
#define IE_NOPEN	-3
#define IE_MEMORY	-4
#define IE_DEFAULT	-5
#define IE_HARDWARE	-10
#define IE_BYTESIZE	-11
#define IE_BAUDRATE	-12

#define EV_RXCHAR    0x0001
#define EV_RXFLAG    0x0002
#define EV_TXEMPTY   0x0004
#define EV_CTS       0x0008
#define EV_DSR       0x0010
#define EV_RLSD      0x0020
#define EV_BREAK     0x0040
#define EV_ERR       0x0080
#define EV_RING      0x0100
#define EV_PERR      0x0200
#define EV_RX80FULL  0x0400
#define EV_EVENT1    0x0800
#define EV_EVENT2    0x1000

#define SETXOFF	1
#define SETXON		2
#define SETRTS		3
#define CLRRTS		4
#define SETDTR		5
#define CLRDTR		6
#define RESETDEV	7
#define SETBREAK	8
#define CLRBREAK	9

/* Purge functions for Comm Port */
#define PURGE_TXABORT       0x0001  /* Kill the pending/current writes to the
				       comm port */
#define PURGE_RXABORT       0x0002  /*Kill the pending/current reads to
				     the comm port */
#define PURGE_TXCLEAR       0x0004  /* Kill the transmit queue if there*/
#define PURGE_RXCLEAR       0x0008  /* Kill the typeahead buffer if there*/


/* Modem Status Flags */
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

#define	RTS_CONTROL_DISABLE	0
#define	RTS_CONTROL_ENABLE	1
#define	RTS_CONTROL_HANDSHAKE	2
#define	RTS_CONTROL_TOGGLE	3

#define	DTR_CONTROL_DISABLE	0
#define	DTR_CONTROL_ENABLE	1
#define	DTR_CONTROL_HANDSHAKE	2


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

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

#define LocalDiscard(h)     (LocalReAlloc((h),0,LMEM_MOVEABLE))

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

#define GlobalLRUNewest(h)  ((HANDLE)(h))
#define GlobalLRUOldest(h)  ((HANDLE)(h))
#define GlobalDiscard(h)    (GlobalReAlloc((h),0,GMEM_MOVEABLE))

#define INVALID_ATOM        ((ATOM)0)
#define MAXINTATOM          0xc000
#ifdef WINE_NO_UNICODE_MACROS /* force using a cast */
#define MAKEINTATOM(atom)   ((ULONG_PTR)((WORD)(atom)))
#else
#define MAKEINTATOM(atom)   ((LPTSTR)((ULONG_PTR)((WORD)(atom))))
#endif

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
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalPhys;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailPhys;
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalPageFile;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailPageFile;
  DWORDLONG DECLSPEC_ALIGN(8) ullTotalVirtual;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailVirtual;
  DWORDLONG DECLSPEC_ALIGN(8) ullAvailExtendedVirtual;
} MEMORYSTATUSEX, *LPMEMORYSTATUSEX;
#include <poppack.h>

typedef enum _MEMORY_RESOURCE_NOTIFICATION_TYPE {
    LowMemoryResourceNotification,
    HighMemoryResourceNotification
} MEMORY_RESOURCE_NOTIFICATION_TYPE;

#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
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
#endif /* _SYSTEMTIME_ */

/* The 'overlapped' data structure used by async I/O functions.
 */
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

typedef VOID (CALLBACK *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPOVERLAPPED);

/* Process startup information.
 */

/* STARTUPINFO.dwFlags */
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

DECL_WINELIB_TYPE_AW(STARTUPINFO)
DECL_WINELIB_TYPE_AW(LPSTARTUPINFO)

typedef struct _PROCESS_INFORMATION{
	HANDLE	hProcess;
	HANDLE	hThread;
	DWORD		dwProcessId;
	DWORD		dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

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

typedef struct _TIME_ZONE_INFORMATION{
        LONG Bias;
        WCHAR StandardName[32];
        SYSTEMTIME StandardDate;
        LONG StandardBias;
        WCHAR DaylightName[32];
        SYSTEMTIME DaylightDate;
        LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

#define TIME_ZONE_ID_INVALID	((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN    0
#define TIME_ZONE_ID_STANDARD   1
#define TIME_ZONE_ID_DAYLIGHT   2

/* CreateProcess: dwCreationFlag values
 */
#define DEBUG_PROCESS               0x00000001
#define DEBUG_ONLY_THIS_PROCESS     0x00000002
#define CREATE_SUSPENDED            0x00000004
#define DETACHED_PROCESS            0x00000008
#define CREATE_NEW_CONSOLE          0x00000010
#define NORMAL_PRIORITY_CLASS       0x00000020
#define IDLE_PRIORITY_CLASS         0x00000040
#define HIGH_PRIORITY_CLASS         0x00000080
#define REALTIME_PRIORITY_CLASS     0x00000100
#define BELOW_NORMAL_PRIORITY_CLASS 0x00004000
#define ABOVE_NORMAL_PRIORITY_CLASS 0x00008000
#define CREATE_NEW_PROCESS_GROUP    0x00000200
#define CREATE_UNICODE_ENVIRONMENT  0x00000400
#define CREATE_SEPARATE_WOW_VDM     0x00000800
#define CREATE_SHARED_WOW_VDM       0x00001000
#define STACK_SIZE_PARAM_IS_A_RESERVATION 0x00010000
#define CREATE_DEFAULT_ERROR_MODE   0x04000000
#define CREATE_NO_WINDOW            0x08000000
#define PROFILE_USER                0x10000000
#define PROFILE_KERNEL              0x20000000
#define PROFILE_SERVER              0x40000000


/* File object type definitions
 */
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

/* Standard handle identifiers
 */
#define STD_INPUT_HANDLE        ((DWORD) -10)
#define STD_OUTPUT_HANDLE       ((DWORD) -11)
#define STD_ERROR_HANDLE        ((DWORD) -12)

typedef struct _BY_HANDLE_FILE_INFORMATION
{
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD dwVolumeSerialNumber;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  DWORD nNumberOfLinks;
  DWORD nFileIndexHigh;
  DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION ;

typedef enum _FILE_ID_TYPE {
    FileIdType,
    ObjectIdType,
    ExtendedFileIdType,
    MaximumFileIdType
} FILE_ID_TYPE, *PFILE_ID_TYPE;

typedef struct _FILE_ID_DESCRIPTOR {
    DWORD        dwSize;
    FILE_ID_TYPE Type;
    union {
        LARGE_INTEGER FileId;
        GUID          ObjectId;
    } DUMMYUNIONNAME;
} FILE_ID_DESCRIPTOR, *LPFILE_ID_DESCRIPTOR;

typedef enum _FILE_INFO_BY_HANDLE_CLASS {
    FileBasicInfo,
    FileStandardInfo,
    FileNameInfo,
    FileRenameInfo,
    FileDispositionInfo,
    FileAllocationInfo,
    FileEndOfFileInfo,
    FileStreamInfo,
    FileCompressionInfo,
    FileAttributeTagInfo,
    FileIdBothDirectoryInfo,
    FileIdBothDirectoryRestartInfo,
    FileIoPriorityHintInfo,
    FileRemoteProtocolInfo,
    FileFullDirectoryInfo,
    FileFullDirectoryRestartInfo,
    FileStorageInfo,
    FileAlignmentInfo,
    FileIdInfo,
    FileIdExtdDirectoryInfo,
    FileIdExtdDirectoryRestartInfo,
    MaximumFileInfoByHandlesClass
} FILE_INFO_BY_HANDLE_CLASS, *PFILE_INFO_BY_HANDLE_CLASS;

typedef struct _FILE_ID_BOTH_DIR_INFO {
    DWORD         NextEntryOffset;
    DWORD         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    DWORD         FileAttributes;
    DWORD         FileNameLength;
    DWORD         EaSize;
    CCHAR         ShortNameLength;
    WCHAR         ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR         FileName[1];
} FILE_ID_BOTH_DIR_INFO, *PFILE_ID_BOTH_DIR_INFO;

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

#define NMPWAIT_WAIT_FOREVER		0xffffffff
#define NMPWAIT_NOWAIT			0x00000001
#define NMPWAIT_USE_DEFAULT_WAIT	0x00000000

/* Security flags for dwFlagsAndAttributes of CreateFile */
#define SECURITY_ANONYMOUS          (SecurityAnonymous << 16)
#define SECURITY_IDENTIFICATION     (SecurityIdentification << 16)
#define SECURITY_IMPERSONATION      (SecurityImpersonation << 16)
#define SECURITY_DELEGATION         (SecurityDelegation << 16)

#define SECURITY_CONTEXT_TRACKING   0x00040000
#define SECURITY_EFFECTIVE_ONLY     0x00080000

#define SECURITY_SQOS_PRESENT       0x00100000
#define SECURITY_VALID_SQOS_FLAGS   0x001f0000

typedef struct _SYSTEM_POWER_STATUS
{
  BYTE    ACLineStatus;
  BYTE    BatteryFlag;
  BYTE    BatteryLifePercent;
  BYTE    Reserved1;
  DWORD   BatteryLifeTime;
  DWORD   BatteryFullLifeTime;
} SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;


typedef struct _SYSTEM_INFO
{
    union {
	DWORD	dwOemId; /* Obsolete field - do not use */
	struct {
		WORD wProcessorArchitecture;
		WORD wReserved;
	} DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
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

typedef BOOL (CALLBACK *ENUMRESTYPEPROCA)(HMODULE,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESTYPEPROCW)(HMODULE,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCA)(HMODULE,LPCSTR,LPSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESNAMEPROCW)(HMODULE,LPCWSTR,LPWSTR,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCA)(HMODULE,LPCSTR,LPCSTR,WORD,LONG_PTR);
typedef BOOL (CALLBACK *ENUMRESLANGPROCW)(HMODULE,LPCWSTR,LPCWSTR,WORD,LONG_PTR);

DECL_WINELIB_TYPE_AW(ENUMRESTYPEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESNAMEPROC)
DECL_WINELIB_TYPE_AW(ENUMRESLANGPROC)

/* flags that can be passed to LoadLibraryEx */
#define DONT_RESOLVE_DLL_REFERENCES         0x00000001
#define LOAD_LIBRARY_AS_DATAFILE            0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH       0x00000008
#define LOAD_IGNORE_CODE_AUTHZ_LEVEL        0x00000010
#define LOAD_LIBRARY_AS_IMAGE_RESOURCE      0x00000020
#define LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE  0x00000040
#define LOAD_LIBRARY_REQUIRE_SIGNED_TARGET  0x00000080

#define GET_MODULE_HANDLE_EX_FLAG_PIN                 1
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  2
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS        4

typedef PLDT_ENTRY LPLDT_ENTRY;

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTES_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

/*
 * This one seems to be a Win32 only definition. It also is defined with
 * WINAPI instead of CALLBACK in the windows headers.
 */
typedef DWORD (CALLBACK *LPPROGRESS_ROUTINE)(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER,
                                           LARGE_INTEGER, DWORD, DWORD, HANDLE,
                                           HANDLE, LPVOID);

typedef DWORD (WINAPI *APPLICATION_RECOVERY_CALLBACK)(PVOID);

#define CREATE_EVENT_MANUAL_RESET 1
#define CREATE_EVENT_INITIAL_SET  2

#define CREATE_MUTEX_INITIAL_OWNER 1

#define CREATE_WAITABLE_TIMER_MANUAL_RESET 1

#define WAIT_FAILED		0xffffffff
#define WAIT_OBJECT_0		0
#define WAIT_ABANDONED		STATUS_ABANDONED_WAIT_0
#define WAIT_ABANDONED_0	STATUS_ABANDONED_WAIT_0
#define WAIT_IO_COMPLETION	STATUS_USER_APC
#define WAIT_TIMEOUT		STATUS_TIMEOUT
#define STILL_ACTIVE            STATUS_PENDING

#define FILE_BEGIN              0
#define FILE_CURRENT            1
#define FILE_END                2

#define FILE_MAP_COPY                   0x00000001
#define FILE_MAP_WRITE                  0x00000002
#define FILE_MAP_READ                   0x00000004
#define FILE_MAP_ALL_ACCESS             0x000f001f
#define FILE_MAP_EXECUTE                0x00000020

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004
#define MOVEFILE_WRITE_THROUGH          0x00000008

#define REPLACEFILE_WRITE_THROUGH       0x00000001
#define REPLACEFILE_IGNORE_MERGE_ERRORS 0x00000002

#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK
#define FS_PERSISTENT_ACLS              FILE_PERSISTENT_ACLS
#define FS_VOL_IS_COMPRESSED            FILE_VOLUME_IS_COMPRESSED
#define FS_FILE_COMPRESSION             FILE_FILE_COMPRESSION

#define MUTEX_MODIFY_STATE              MUTANT_QUERY_STATE
#define MUTEX_ALL_ACCESS                MUTANT_ALL_ACCESS

#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT

#define HANDLE_FLAG_INHERIT             0x00000001
#define HANDLE_FLAG_PROTECT_FROM_CLOSE  0x00000002

#define HINSTANCE_ERROR 32

#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
#define THREAD_PRIORITY_NORMAL          0
#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
#define THREAD_PRIORITY_ERROR_RETURN    (0x7fffffff)
#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDLE

/* flags to FormatMessage */
#define	FORMAT_MESSAGE_ALLOCATE_BUFFER	0x00000100
#define	FORMAT_MESSAGE_IGNORE_INSERTS	0x00000200
#define	FORMAT_MESSAGE_FROM_STRING	0x00000400
#define	FORMAT_MESSAGE_FROM_HMODULE	0x00000800
#define	FORMAT_MESSAGE_FROM_SYSTEM	0x00001000
#define	FORMAT_MESSAGE_ARGUMENT_ARRAY	0x00002000
#define	FORMAT_MESSAGE_MAX_WIDTH_MASK	0x000000FF

/* flags to ACTCTX[AW] */
#define ACTCTX_FLAG_PROCESSOR_ARCHITECTURE_VALID  (0x00000001)
#define ACTCTX_FLAG_LANGID_VALID                  (0x00000002)
#define ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID      (0x00000004)
#define ACTCTX_FLAG_RESOURCE_NAME_VALID           (0x00000008)
#define ACTCTX_FLAG_SET_PROCESS_DEFAULT           (0x00000010)
#define ACTCTX_FLAG_APPLICATION_NAME_VALID        (0x00000020)
#define ACTCTX_FLAG_SOURCE_IS_ASSEMBLYREF         (0x00000040)
#define ACTCTX_FLAG_HMODULE_VALID                 (0x00000080)

/* flags to DeactiveActCtx */
#define DEACTIVATE_ACTCTX_FLAG_FORCE_EARLY_DEACTIVATION  (0x00000001)

/* flags to FindActCtxSection{Guid,String[AW]} */
#define FIND_ACTCTX_SECTION_KEY_RETURN_HACTCTX            (0x00000001)
#define FIND_ACTCTX_SECTION_KEY_RETURN_FLAGS              (0x00000002)
#define FIND_ACTCTX_SECTION_KEY_RETURN_ASSEMBLY_METADATA  (0x00000004)

/* flags to QueryActCtxW */
#define QUERY_ACTCTX_FLAG_USE_ACTIVE_ACTCTX  (0x00000004)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_HMODULE  (0x00000008)
#define QUERY_ACTCTX_FLAG_ACTCTX_IS_ADDRESS  (0x00000010)
#define QUERY_ACTCTX_FLAG_NO_ADDREF          (0x80000000)

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

DECL_WINELIB_TYPE_AW(ACTCTX)
DECL_WINELIB_TYPE_AW(PACTCTX)

typedef const ACTCTXA *PCACTCTXA;
typedef const ACTCTXW *PCACTCTXW;
DECL_WINELIB_TYPE_AW(PCACTCTX)

typedef struct tagACTCTX_SECTION_KEYED_DATA_2600 {
    ULONG  cbSize;
    ULONG  ulDataFormatVersion;
    PVOID  lpData;
    ULONG  ulLength;
    PVOID  lpSectionGlobalData;
    ULONG  ulSectionGlobalDataLength;
    PVOID  lpSectionBase;
    ULONG  ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG  ulAssemblyRosterIndex;
} ACTCTX_SECTION_KEYED_DATA_2600, *PACTCTX_SECTION_KEYED_DATA_2600;
typedef const ACTCTX_SECTION_KEYED_DATA_2600 *PCACTCTX_SECTION_KEYED_DATA_2600;

typedef struct tagACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA {
    PVOID lpInformation;
    PVOID lpSectionBase;
    ULONG ulSectionLength;
    PVOID lpSectionGlobalDataBase;
    ULONG ulSectionGlobalDataLength;
} ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA, *PACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef const ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA *PCACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;

typedef struct tagACTCTX_SECTION_KEYED_DATA {
    ULONG  cbSize;
    ULONG  ulDataFormatVersion;
    PVOID  lpData;
    ULONG  ulLength;
    PVOID  lpSectionGlobalData;
    ULONG  ulSectionGlobalDataLength;
    PVOID  lpSectionBase;
    ULONG  ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG  ulAssemblyRosterIndex;

    /* Non 2600 extra fields */
    ULONG ulFlags;
    ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA AssemblyMetadata;
} ACTCTX_SECTION_KEYED_DATA, *PACTCTX_SECTION_KEYED_DATA;
typedef const ACTCTX_SECTION_KEYED_DATA *PCACTCTX_SECTION_KEYED_DATA;

typedef struct _ACTIVATION_CONTEXT_BASIC_INFORMATION {
    HANDLE hActCtx;
    DWORD  dwFlags;
} ACTIVATION_CONTEXT_BASIC_INFORMATION, *PACTIVATION_CONTEXT_BASIC_INFORMATION;

typedef BOOL (WINAPI *PQUERYACTCTXW_FUNC)(DWORD,HANDLE,PVOID,ULONG,PVOID,SIZE_T,SIZE_T *);

typedef struct tagCOMSTAT
{
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT, *LPCOMSTAT;

typedef struct tagDCB
{
    DWORD DCBlength;
    DWORD BaudRate;
    unsigned fBinary               :1;
    unsigned fParity               :1;
    unsigned fOutxCtsFlow          :1;
    unsigned fOutxDsrFlow          :1;
    unsigned fDtrControl           :2;
    unsigned fDsrSensitivity       :1;
    unsigned fTXContinueOnXoff     :1;
    unsigned fOutX                 :1;
    unsigned fInX                  :1;
    unsigned fErrorChar            :1;
    unsigned fNull                 :1;
    unsigned fRtsControl           :2;
    unsigned fAbortOnError         :1;
    unsigned fDummy2               :17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
    WORD wReserved1;
} DCB, *LPDCB;

typedef struct tagCOMMCONFIG {
	DWORD dwSize;
	WORD  wVersion;
	WORD  wReserved;
	DCB   dcb;
	DWORD dwProviderSubType;
	DWORD dwProviderOffset;
	DWORD dwProviderSize;
	DWORD wcProviderData[1];
} COMMCONFIG, *LPCOMMCONFIG;

typedef struct tagCOMMPROP {
	WORD  wPacketLength;
	WORD  wPacketVersion;
	DWORD dwServiceMask;
	DWORD dwReserved1;
	DWORD dwMaxTxQueue;
	DWORD dwMaxRxQueue;
	DWORD dwMaxBaud;
	DWORD dwProvSubType;
	DWORD dwProvCapabilities;
	DWORD dwSettableParams;
	DWORD dwSettableBaud;
	WORD  wSettableData;
	WORD  wSettableStopParity;
	DWORD dwCurrentTxQueue;
	DWORD dwCurrentRxQueue;
	DWORD dwProvSpec1;
	DWORD dwProvSpec2;
	WCHAR wcProvChar[1];
} COMMPROP, *LPCOMMPROP;

#define SP_SERIALCOMM ((DWORD)1)

#define BAUD_075     ((DWORD)0x01)
#define BAUD_110     ((DWORD)0x02)
#define BAUD_134_5   ((DWORD)0x04)
#define BAUD_150     ((DWORD)0x08)
#define BAUD_300     ((DWORD)0x10)
#define BAUD_600     ((DWORD)0x20)
#define BAUD_1200    ((DWORD)0x40)
#define BAUD_1800    ((DWORD)0x80)
#define BAUD_2400    ((DWORD)0x100)
#define BAUD_4800    ((DWORD)0x200)
#define BAUD_7200    ((DWORD)0x400)
#define BAUD_9600    ((DWORD)0x800)
#define BAUD_14400   ((DWORD)0x1000)
#define BAUD_19200   ((DWORD)0x2000)
#define BAUD_38400   ((DWORD)0x4000)
#define BAUD_56K     ((DWORD)0x8000)
#define BAUD_57600   ((DWORD)0x40000)
#define BAUD_115200  ((DWORD)0x20000)
#define BAUD_128K    ((DWORD)0x10000)
#define BAUD_USER    ((DWORD)0x10000000)

#define PST_FAX            ((DWORD)0x21)
#define PST_LAT            ((DWORD)0x101)
#define PST_MODEM          ((DWORD)0x06)
#define PST_NETWORK_BRIDGE ((DWORD)0x100)
#define PST_PARALLELPORT   ((DWORD)0x02)
#define PST_RS232          ((DWORD)0x01)
#define PST_RS442          ((DWORD)0x03)
#define PST_RS423          ((DWORD)0x04)
#define PST_RS449          ((DWORD)0x06)
#define PST_SCANNER        ((DWORD)0x22)
#define PST_TCPIP_TELNET   ((DWORD)0x102)
#define PST_UNSPECIFIED    ((DWORD)0x00)
#define PST_X25            ((DWORD)0x103)

#define PCF_16BITMODE     ((DWORD)0x200)
#define PCF_DTRDSR        ((DWORD)0x01)
#define PCF_INTTIMEOUTS   ((DWORD)0x80)
#define PCF_PARITY_CHECK  ((DWORD)0x08)
#define PCF_RLSD          ((DWORD)0x04)
#define PCF_RTSCTS        ((DWORD)0x02)
#define PCF_SETXCHAR      ((DWORD)0x20)
#define PCF_SPECIALCHARS  ((DWORD)0x100)
#define PCF_TOTALTIMEOUTS ((DWORD)0x40)
#define PCF_XONXOFF       ((DWORD)0x10)

#define SP_BAUD         ((DWORD)0x02)
#define SP_DATABITS     ((DWORD)0x04)
#define SP_HANDSHAKING  ((DWORD)0x10)
#define SP_PARITY       ((DWORD)0x01)
#define SP_PARITY_CHECK ((DWORD)0x20)
#define SP_RLSD         ((DWORD)0x40)
#define SP_STOPBITS     ((DWORD)0x08)

#define DATABITS_5   ((DWORD)0x01)
#define DATABITS_6   ((DWORD)0x02)
#define DATABITS_7   ((DWORD)0x04)
#define DATABITS_8   ((DWORD)0x08)
#define DATABITS_16  ((DWORD)0x10)
#define DATABITS_16X ((DWORD)0x20)

#define STOPBITS_10 ((DWORD)1)
#define STOPBITS_15 ((DWORD)2)
#define STOPBITS_20 ((DWORD)4)

#define PARITY_NONE  ((DWORD)0x100)
#define PARITY_ODD   ((DWORD)0x200)
#define PARITY_EVEN  ((DWORD)0x400)
#define PARITY_MARK  ((DWORD)0x800)
#define PARITY_SPACE ((DWORD)0x1000)

typedef struct tagCOMMTIMEOUTS {
	DWORD	ReadIntervalTimeout;
	DWORD	ReadTotalTimeoutMultiplier;
	DWORD	ReadTotalTimeoutConstant;
	DWORD	WriteTotalTimeoutMultiplier;
	DWORD	WriteTotalTimeoutConstant;
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

#define GET_TAPE_MEDIA_INFORMATION 0
#define GET_TAPE_DRIVE_INFORMATION 1
#define SET_TAPE_MEDIA_INFORMATION 0
#define SET_TAPE_DRIVE_INFORMATION 1

#define PROCESS_NAME_NATIVE        1

typedef void (CALLBACK *PAPCFUNC)(ULONG_PTR);
typedef void (CALLBACK *PTIMERAPCROUTINE)(LPVOID,DWORD,DWORD);

typedef enum _COMPUTER_NAME_FORMAT
{
	ComputerNameNetBIOS,
	ComputerNameDnsHostname,
	ComputerNameDnsDomain,
	ComputerNameDnsFullyQualified,
	ComputerNamePhysicalNetBIOS,
	ComputerNamePhysicalDnsHostname,
	ComputerNamePhysicalDnsDomain,
	ComputerNamePhysicalDnsFullyQualified,
	ComputerNameMax
} COMPUTER_NAME_FORMAT;

#define HW_PROFILE_GUIDLEN	39
#define MAX_PROFILE_LEN		80

#define DOCKINFO_UNDOCKED	0x1
#define DOCKINFO_DOCKED		0x2
#define DOCKINFO_USER_SUPPLIED	0x4
#define DOCKINFO_USER_UNDOCKED	(DOCKINFO_USER_SUPPLIED | DOCKINFO_UNDOCKED)
#define DOCKINFO_USER_DOCKED	(DOCKINFO_USER_SUPPLIED | DOCKINFO_DOCKED)

typedef struct tagHW_PROFILE_INFOA {
    DWORD dwDockInfo;
    CHAR  szHwProfileGuid[HW_PROFILE_GUIDLEN];
    CHAR  szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOA, *LPHW_PROFILE_INFOA;

typedef struct tagHW_PROFILE_INFOW {
    DWORD dwDockInfo;
    WCHAR szHwProfileGuid[HW_PROFILE_GUIDLEN];
    WCHAR szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOW, *LPHW_PROFILE_INFOW;

DECL_WINELIB_TYPE_AW(HW_PROFILE_INFO)
DECL_WINELIB_TYPE_AW(LPHW_PROFILE_INFO)

typedef enum _DEP_SYSTEM_POLICY_TYPE {
    AlwaysOff = 0,
    AlwaysOn = 1,
    OptIn = 2,
    OptOut = 3
} DEP_SYSTEM_POLICY_TYPE;

/* Event Logging */

#define EVENTLOG_FULL_INFO          0

typedef struct _EVENTLOG_FULL_INFORMATION {
    DWORD dwFull;
} EVENTLOG_FULL_INFORMATION, *LPEVENTLOG_FULL_INFORMATION;


/* Stream data structures and defines */
/*the types of backup data -- WIN32_STREAM_ID.dwStreamId below*/
#define BACKUP_INVALID        0
#define BACKUP_DATA           1
#define BACKUP_EA_DATA        2
#define BACKUP_SECURITY_DATA  3
#define BACKUP_ALTERNATE_DATA 4
#define BACKUP_LINK           5
#define BACKUP_PROPERTY_DATA  6
#define BACKUP_OBJECT_ID      7
#define BACKUP_REPARSE_DATA   8
#define BACKUP_SPARSE_BLOCK   9

/*flags for WIN32_STREAM_ID.dwStreamAttributes below*/
#define STREAM_NORMAL_ATTRIBUTE    0
#define STREAM_MODIFIED_WHEN_READ  1
#define STREAM_CONTAINS_SECURITY   2
#define STREAM_CONTAINS_PROPERTIES 4
#define STREAM_SPARSE_ATTRIBUTE    8

#include <pshpack8.h>
typedef struct _WIN32_STREAM_ID {
	DWORD   dwStreamId;
	DWORD   dwStreamAttributes;
	LARGE_INTEGER DECLSPEC_ALIGN(8) Size;
	DWORD   dwStreamNameSize;
	WCHAR   cStreamName[ANYSIZE_ARRAY];
} WIN32_STREAM_ID, *LPWIN32_STREAM_ID;
#include <poppack.h>


/* GetBinaryType return values.
 */

#define SCS_32BIT_BINARY    0
#define SCS_DOS_BINARY      1
#define SCS_WOW_BINARY      2
#define SCS_PIF_BINARY      3
#define SCS_POSIX_BINARY    4
#define SCS_OS216_BINARY    5
#define SCS_64BIT_BINARY    6

/* flags for DefineDosDevice */
#define DDD_RAW_TARGET_PATH         0x00000001
#define DDD_REMOVE_DEFINITION       0x00000002
#define DDD_EXACT_MATCH_ON_REMOVE   0x00000004
#define DDD_NO_BROADCAST_SYSTEM     0x00000008
#define DDD_LUID_BROADCAST_DRIVE    0x00000010

#define LOGON_WITH_PROFILE          0x00000001
#define LOGON_NETCREDENTIALS_ONLY   0x00000002
#define LOGON_ZERO_PASSWORD_BUFFER  0x80000000

/* one-time initialisation API */
typedef RTL_RUN_ONCE  INIT_ONCE;
typedef PRTL_RUN_ONCE PINIT_ONCE;
typedef PRTL_RUN_ONCE LPINIT_ONCE;
#define INIT_ONCE_STATIC_INIT       RTL_RUN_ONCE_INIT
#define INIT_ONCE_CHECK_ONLY        RTL_RUN_ONCE_CHECK_ONLY
#define INIT_ONCE_ASYNC             RTL_RUN_ONCE_ASYNC
#define INIT_ONCE_INIT_FAILED       RTL_RUN_ONCE_INIT_FAILED
/* initialization callback prototype */
typedef BOOL (WINAPI *PINIT_ONCE_FN)(PINIT_ONCE,PVOID,PVOID*);

BOOL        WINAPI ActivateActCtx_k(HANDLE,ULONG_PTR *);
BOOL        WINAPI AddAccessAllowedAce_k(PACL,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessAllowedAceEx_k(PACL,DWORD,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessDeniedAce_k(PACL,DWORD,DWORD,PSID);
BOOL        WINAPI AddAccessDeniedAceEx_k(PACL,DWORD,DWORD,DWORD,PSID);
BOOL        WINAPI AddAce_k(PACL,DWORD,DWORD,LPVOID,DWORD);
ATOM        WINAPI AddAtomA_k(LPCSTR);
ATOM        WINAPI AddAtomW_k(LPCWSTR);
#define                       AddAtom WINELIB_NAME_AW(AddAtom)
BOOL        WINAPI AddAuditAccessAce_k(PACL,DWORD,DWORD,PSID,BOOL,BOOL);
BOOL        WINAPI AddAuditAccessAceEx_k(PACL,DWORD,DWORD,DWORD,PSID,BOOL,BOOL);
VOID        WINAPI AddRefActCtx_k(HANDLE);
PVOID       WINAPI AddVectoredExceptionHandler_k(ULONG,PVECTORED_EXCEPTION_HANDLER);
BOOL        WINAPI AdjustTokenGroups_k(HANDLE,BOOL,PTOKEN_GROUPS,DWORD,PTOKEN_GROUPS,PDWORD);
BOOL        WINAPI AccessCheck_k(PSECURITY_DESCRIPTOR,HANDLE,DWORD,PGENERIC_MAPPING,PPRIVILEGE_SET,LPDWORD,LPDWORD,LPBOOL);
BOOL        WINAPI AccessCheckAndAuditAlarmA_k(LPCSTR,LPVOID,LPSTR,LPSTR,PSECURITY_DESCRIPTOR,DWORD,PGENERIC_MAPPING,BOOL,LPDWORD,LPBOOL,LPBOOL);
BOOL        WINAPI AccessCheckAndAuditAlarmW_k(LPCWSTR,LPVOID,LPWSTR,LPWSTR,PSECURITY_DESCRIPTOR,DWORD,PGENERIC_MAPPING,BOOL,LPDWORD,LPBOOL,LPBOOL);
#define                       AccessCheckAndAuditAlarm WINELIB_NAME_AW(AccessCheckAndAuditAlarm)
BOOL        WINAPI AccessCheckByType_k(PSECURITY_DESCRIPTOR,PSID,HANDLE,DWORD,POBJECT_TYPE_LIST,DWORD,PGENERIC_MAPPING,PPRIVILEGE_SET,LPDWORD,LPDWORD,LPBOOL);
VOID        WINAPI AcquireSRWLockExclusive_k(PSRWLOCK);
VOID        WINAPI AcquireSRWLockShared_k(PSRWLOCK);
BOOL        WINAPI AdjustTokenPrivileges_k(HANDLE,BOOL,PTOKEN_PRIVILEGES,DWORD,PTOKEN_PRIVILEGES,PDWORD);
BOOL        WINAPI AllocateAndInitializeSid_k(PSID_IDENTIFIER_AUTHORITY,BYTE,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,PSID *);
BOOL        WINAPI AllocateLocallyUniqueId_k(PLUID);
BOOL        WINAPI AreAllAccessesGranted_k(DWORD,DWORD);
BOOL        WINAPI AreAnyAccessesGranted_k(DWORD,DWORD);
BOOL        WINAPI AreFileApisANSI_k(void);
BOOL        WINAPI AssignProcessToJobObject_k(HANDLE,HANDLE);
BOOL        WINAPI BackupEventLogA_k(HANDLE,LPCSTR);
BOOL        WINAPI BackupEventLogW_k(HANDLE,LPCWSTR);
#define                       BackupEventLog WINELIB_NAME_AW(BackupEventLog)
BOOL        WINAPI BackupRead_k(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL        WINAPI BackupSeek_k(HANDLE,DWORD,DWORD,LPDWORD,LPDWORD,LPVOID*);
BOOL        WINAPI BackupWrite_k(HANDLE,LPBYTE,DWORD,LPDWORD,BOOL,BOOL,LPVOID*);
BOOL        WINAPI Beep_k(DWORD,DWORD);
HANDLE      WINAPI BeginUpdateResourceA_k(LPCSTR,BOOL);
HANDLE      WINAPI BeginUpdateResourceW_k(LPCWSTR,BOOL);
#define                       BeginUpdateResource WINELIB_NAME_AW(BeginUpdateResource)
BOOL        WINAPI BindIoCompletionCallback_k(HANDLE,LPOVERLAPPED_COMPLETION_ROUTINE,ULONG);
BOOL        WINAPI BuildCommDCBA_k(LPCSTR,LPDCB);
BOOL        WINAPI BuildCommDCBW_k(LPCWSTR,LPDCB);
#define                       BuildCommDCB WINELIB_NAME_AW(BuildCommDCB)
BOOL        WINAPI BuildCommDCBAndTimeoutsA_k(LPCSTR,LPDCB,LPCOMMTIMEOUTS);
BOOL        WINAPI BuildCommDCBAndTimeoutsW_k(LPCWSTR,LPDCB,LPCOMMTIMEOUTS);
#define                       BuildCommDCBAndTimeouts WINELIB_NAME_AW(BuildCommDCBAndTimeouts)
BOOL        WINAPI CallNamedPipeA_k(LPCSTR,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,DWORD);
BOOL        WINAPI CallNamedPipeW_k(LPCWSTR,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,DWORD);
#define                       CallNamedPipe WINELIB_NAME_AW(CallNamedPipe)
BOOL        WINAPI CancelIo_k(HANDLE);
BOOL        WINAPI CancelIoEx_k(HANDLE,LPOVERLAPPED);
BOOL        WINAPI CancelTimerQueueTimer_k(HANDLE,HANDLE);
BOOL        WINAPI CancelWaitableTimer_k(HANDLE);
BOOL        WINAPI ChangeTimerQueueTimer_k(HANDLE,HANDLE,ULONG,ULONG);
BOOL        WINAPI CheckTokenMembership_k(HANDLE,PSID,PBOOL);
BOOL        WINAPI ClearCommBreak_k(HANDLE);
BOOL        WINAPI ClearCommError_k(HANDLE,LPDWORD,LPCOMSTAT);
BOOL        WINAPI ClearEventLogA_k(HANDLE,LPCSTR);
BOOL        WINAPI ClearEventLogW_k(HANDLE,LPCWSTR);
#define                       ClearEventLog WINELIB_NAME_AW(ClearEventLog)
BOOL        WINAPI CloseEventLog_k(HANDLE);
BOOL        WINAPI CloseHandle_k(HANDLE);
BOOL        WINAPI CommConfigDialogA_k(LPCSTR,HWND,LPCOMMCONFIG);
BOOL        WINAPI CommConfigDialogW_k(LPCWSTR,HWND,LPCOMMCONFIG);
#define                       CommConfigDialog WINELIB_NAME_AW(CommConfigDialog)
BOOL        WINAPI ConnectNamedPipe_k(HANDLE,LPOVERLAPPED);
BOOL        WINAPI ContinueDebugEvent_k(DWORD,DWORD,DWORD);
HANDLE      WINAPI ConvertToGlobalHandle_k(HANDLE hSrc);
BOOL        WINAPI CopyFileA_k(LPCSTR,LPCSTR,BOOL);
BOOL        WINAPI CopyFileW_k(LPCWSTR,LPCWSTR,BOOL);
#define                       CopyFile WINELIB_NAME_AW(CopyFile)
BOOL        WINAPI CopyFileExA_k(LPCSTR, LPCSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
BOOL        WINAPI CopyFileExW_k(LPCWSTR, LPCWSTR, LPPROGRESS_ROUTINE, LPVOID, LPBOOL, DWORD);
#define                       CopyFileEx WINELIB_NAME_AW(CopyFileEx)
BOOL        WINAPI CopySid_k(DWORD,PSID,PSID);
INT         WINAPI CompareFileTime_k(const FILETIME*,const FILETIME*);
BOOL        WINAPI ConvertFiberToThread_k(void);
LPVOID      WINAPI ConvertThreadToFiber_k(LPVOID);
LPVOID      WINAPI ConvertThreadToFiberEx_k(LPVOID,DWORD);
BOOL        WINAPI ConvertToAutoInheritPrivateObjectSecurity_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR*,GUID*,BOOL,PGENERIC_MAPPING);
HANDLE      WINAPI CreateActCtxA_k(PCACTCTXA);
HANDLE      WINAPI CreateActCtxW_k(PCACTCTXW);
#define                       CreateActCtx WINELIB_NAME_AW(CreateActCtx)
BOOL        WINAPI CreateDirectoryA_k(LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL        WINAPI CreateDirectoryW_k(LPCWSTR,LPSECURITY_ATTRIBUTES);
#define                       CreateDirectory WINELIB_NAME_AW(CreateDirectory)
BOOL        WINAPI CreateDirectoryExA_k(LPCSTR,LPCSTR,LPSECURITY_ATTRIBUTES);
BOOL        WINAPI CreateDirectoryExW_k(LPCWSTR,LPCWSTR,LPSECURITY_ATTRIBUTES);
#define                       CreateDirectoryEx WINELIB_NAME_AW(CreateDirectoryEx)
HANDLE      WINAPI CreateEventA_k(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCSTR);
HANDLE      WINAPI CreateEventW_k(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCWSTR);
#define                       CreateEvent WINELIB_NAME_AW(CreateEvent)
HANDLE      WINAPI CreateEventExA_k(SECURITY_ATTRIBUTES*,LPCSTR,DWORD,DWORD);
HANDLE      WINAPI CreateEventExW_k(SECURITY_ATTRIBUTES*,LPCWSTR,DWORD,DWORD);
#define                       CreateEventEx WINELIB_NAME_AW(CreateEventEx)
LPVOID      WINAPI CreateFiber_k(SIZE_T,LPFIBER_START_ROUTINE,LPVOID);
LPVOID      WINAPI CreateFiberEx_k(SIZE_T,SIZE_T,DWORD,LPFIBER_START_ROUTINE,LPVOID);
HANDLE      WINAPI CreateFileA_k(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
HANDLE      WINAPI CreateFileW_k(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
#define                       CreateFile WINELIB_NAME_AW(CreateFile)
HANDLE      WINAPI CreateFileMappingA_k(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCSTR);
HANDLE      WINAPI CreateFileMappingW_k(HANDLE,LPSECURITY_ATTRIBUTES,DWORD,DWORD,DWORD,LPCWSTR);
#define                       CreateFileMapping WINELIB_NAME_AW(CreateFileMapping)
HANDLE      WINAPI CreateIoCompletionPort_k(HANDLE,HANDLE,ULONG_PTR,DWORD);
HANDLE      WINAPI CreateJobObjectA_k(LPSECURITY_ATTRIBUTES,LPCSTR);
HANDLE      WINAPI CreateJobObjectW_k(LPSECURITY_ATTRIBUTES,LPCWSTR);
#define                       CreateJobObject WINELIB_NAME_AW(CreateJobObject)
HANDLE      WINAPI CreateMailslotA_k(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
HANDLE      WINAPI CreateMailslotW_k(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
#define                       CreateMailslot WINELIB_NAME_AW(CreateMailslot)
HANDLE      WINAPI CreateMemoryResourceNotification_k(MEMORY_RESOURCE_NOTIFICATION_TYPE);
HANDLE      WINAPI CreateMutexA_k(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE      WINAPI CreateMutexW_k(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define                       CreateMutex WINELIB_NAME_AW(CreateMutex)
HANDLE      WINAPI CreateMutexExA_k(SECURITY_ATTRIBUTES*,LPCSTR,DWORD,DWORD);
HANDLE      WINAPI CreateMutexExW_k(SECURITY_ATTRIBUTES*,LPCWSTR,DWORD,DWORD);
#define                       CreateMutexEx WINELIB_NAME_AW(CreateMutexEx)
HANDLE      WINAPI CreateNamedPipeA_k(LPCSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
HANDLE      WINAPI CreateNamedPipeW_k(LPCWSTR,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,LPSECURITY_ATTRIBUTES);
#define                       CreateNamedPipe WINELIB_NAME_AW(CreateNamedPipe)
BOOL        WINAPI CreatePipe_k(PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
BOOL        WINAPI CreatePrivateObjectSecurity_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR*,BOOL,HANDLE,PGENERIC_MAPPING);
BOOL        WINAPI CreateProcessA_k(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL        WINAPI CreateProcessW_k(LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
#define                       CreateProcess WINELIB_NAME_AW(CreateProcess)
BOOL        WINAPI CreateProcessAsUserA_k(HANDLE,LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
BOOL        WINAPI CreateProcessAsUserW_k(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
#define                       CreateProcessAsUser WINELIB_NAME_AW(CreateProcessAsUser)
BOOL        WINAPI CreateProcessWithLogonW_k(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPCWSTR,LPWSTR,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION);
HANDLE      WINAPI CreateRemoteThread_k(HANDLE,LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
BOOL        WINAPI CreateRestrictedToken_k(HANDLE,DWORD,DWORD,PSID_AND_ATTRIBUTES,DWORD,PLUID_AND_ATTRIBUTES,DWORD,PSID_AND_ATTRIBUTES,PHANDLE);
HANDLE      WINAPI CreateSemaphoreA_k(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCSTR);
HANDLE      WINAPI CreateSemaphoreW_k(LPSECURITY_ATTRIBUTES,LONG,LONG,LPCWSTR);
#define                       CreateSemaphore WINELIB_NAME_AW(CreateSemaphore)
HANDLE      WINAPI CreateSemaphoreExA_k(SECURITY_ATTRIBUTES*,LONG,LONG,LPCSTR,DWORD,DWORD);
HANDLE      WINAPI CreateSemaphoreExW_k(SECURITY_ATTRIBUTES*,LONG,LONG,LPCWSTR,DWORD,DWORD);
#define                       CreateSemaphoreEx WINELIB_NAME_AW(CreateSemaphoreEx)
DWORD       WINAPI CreateTapePartition_k(HANDLE,DWORD,DWORD,DWORD);
HANDLE      WINAPI CreateThread_k(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
HANDLE      WINAPI CreateTimerQueue_k(void);
BOOL        WINAPI CreateTimerQueueTimer_k(PHANDLE,HANDLE,WAITORTIMERCALLBACK,PVOID,DWORD,DWORD,ULONG);
HANDLE      WINAPI CreateWaitableTimerA_k(LPSECURITY_ATTRIBUTES,BOOL,LPCSTR);
HANDLE      WINAPI CreateWaitableTimerW_k(LPSECURITY_ATTRIBUTES,BOOL,LPCWSTR);
#define                       CreateWaitableTimer WINELIB_NAME_AW(CreateWaitableTimer)
HANDLE      WINAPI CreateWaitableTimerExA_k(SECURITY_ATTRIBUTES*,LPCSTR,DWORD,DWORD);
HANDLE      WINAPI CreateWaitableTimerExW_k(SECURITY_ATTRIBUTES*,LPCWSTR,DWORD,DWORD);
#define                       CreateWaitableTimerEx WINELIB_NAME_AW(CreateWaitableTimerEx)
BOOL        WINAPI CreateWellKnownSid_k(WELL_KNOWN_SID_TYPE,PSID,PSID,DWORD*);
BOOL        WINAPI DeactivateActCtx_k(DWORD,ULONG_PTR);
BOOL        WINAPI DebugActiveProcess_k(DWORD);
BOOL        WINAPI DebugActiveProcessStop_k(DWORD);
void        WINAPI DebugBreak_k(void);
BOOL        WINAPI DebugBreakProcess_k(HANDLE);
BOOL        WINAPI DebugSetProcessKillOnExit_k(BOOL);
PVOID       WINAPI DecodePointer_k(PVOID);
BOOL        WINAPI DecryptFileA_k(LPCSTR,DWORD);
BOOL        WINAPI DecryptFileW_k(LPCWSTR,DWORD);
#define                       DecryptFile WINELIB_NAME_AW(DecryptFile)
BOOL        WINAPI DefineDosDeviceA_k(DWORD,LPCSTR,LPCSTR);
BOOL        WINAPI DefineDosDeviceW_k(DWORD,LPCWSTR,LPCWSTR);
#define                       DefineDosDevice WINELIB_NAME_AW(DefineDosDevice)
#define                       DefineHandleTable_k(w) _k(_k(w),TRUE)
BOOL        WINAPI DeleteAce_k(PACL,DWORD);
ATOM        WINAPI DeleteAtom_k(ATOM);
void        WINAPI DeleteCriticalSection_k(CRITICAL_SECTION *lpCrit);
void        WINAPI DeleteFiber_k(LPVOID);
BOOL        WINAPI DeleteFileA_k(LPCSTR);
BOOL        WINAPI DeleteFileW_k(LPCWSTR);
#define                       DeleteFile WINELIB_NAME_AW(DeleteFile)
BOOL        WINAPI DeleteTimerQueue_k(HANDLE);
BOOL        WINAPI DeleteTimerQueueEx_k(HANDLE,HANDLE);
BOOL        WINAPI DeleteTimerQueueTimer_k(HANDLE,HANDLE,HANDLE);
BOOL        WINAPI DeleteVolumeMountPointA_k(LPCSTR);
BOOL        WINAPI DeleteVolumeMountPointW_k(LPCWSTR);
#define                       DeleteVolumeMountPoint WINELIB_NAME_AW(DeleteVolumeMountPoint)
BOOL        WINAPI DeregisterEventSource_k(HANDLE);
BOOL        WINAPI DestroyPrivateObjectSecurity_k(PSECURITY_DESCRIPTOR*);
BOOL        WINAPI DeviceIoControl_k(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI DisableThreadLibraryCalls_k(HMODULE);
BOOL        WINAPI DisconnectNamedPipe_k(HANDLE);
BOOL        WINAPI DnsHostnameToComputerNameA_k(LPCSTR,LPSTR,LPDWORD);
BOOL        WINAPI DnsHostnameToComputerNameW_k(LPCWSTR,LPWSTR,LPDWORD);
#define                       DnsHostnameToComputerName WINELIB_NAME_AW(DnsHostnameToComputerName)
BOOL        WINAPI DosDateTimeToFileTime_k(WORD,WORD,LPFILETIME);
BOOL        WINAPI DuplicateHandle_k(HANDLE,HANDLE,HANDLE,HANDLE*,DWORD,BOOL,DWORD);
BOOL        WINAPI DuplicateToken_k(HANDLE,SECURITY_IMPERSONATION_LEVEL,PHANDLE);
BOOL        WINAPI DuplicateTokenEx_k(HANDLE,DWORD,LPSECURITY_ATTRIBUTES,SECURITY_IMPERSONATION_LEVEL,TOKEN_TYPE,PHANDLE);
BOOL        WINAPI EscapeCommFunction_k(HANDLE,UINT);
PVOID       WINAPI EncodePointer_k(PVOID);
BOOL        WINAPI EncryptFileA_k(LPCSTR);
BOOL        WINAPI EncryptFileW_k(LPCWSTR);
#define                       EncryptFile WINELIB_NAME_AW(EncryptFile)
BOOL        WINAPI EndUpdateResourceA_k(HANDLE,BOOL);
BOOL        WINAPI EndUpdateResourceW_k(HANDLE,BOOL);
#define                       EndUpdateResource WINELIB_NAME_AW(EndUpdateResource)
#define EnterCriticalSection_k  RtlEnterCriticalSection_k
BOOL        WINAPI EnumResourceLanguagesA_k(HMODULE,LPCSTR,LPCSTR,ENUMRESLANGPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceLanguagesW_k(HMODULE,LPCWSTR,LPCWSTR,ENUMRESLANGPROCW,LONG_PTR);
#define                       EnumResourceLanguages WINELIB_NAME_AW(EnumResourceLanguages)
BOOL        WINAPI EnumResourceNamesA_k(HMODULE,LPCSTR,ENUMRESNAMEPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceNamesW_k(HMODULE,LPCWSTR,ENUMRESNAMEPROCW,LONG_PTR);
#define                       EnumResourceNames WINELIB_NAME_AW(EnumResourceNames)
BOOL        WINAPI EnumResourceTypesA_k(HMODULE,ENUMRESTYPEPROCA,LONG_PTR);
BOOL        WINAPI EnumResourceTypesW_k(HMODULE,ENUMRESTYPEPROCW,LONG_PTR);
#define                       EnumResourceTypes WINELIB_NAME_AW(EnumResourceTypes)
BOOL        WINAPI EqualSid_k(PSID, PSID);
BOOL        WINAPI EqualPrefixSid_k(PSID,PSID);
DWORD       WINAPI EraseTape_k(HANDLE,DWORD,BOOL);
VOID WINAPI ExitProcess_k(DWORD);
VOID WINAPI ExitThread_k(DWORD);
DWORD       WINAPI ExpandEnvironmentStringsA_k(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI ExpandEnvironmentStringsW_k(LPCWSTR,LPWSTR,DWORD);
#define                       ExpandEnvironmentStrings WINELIB_NAME_AW(ExpandEnvironmentStrings)
void        WINAPI FatalAppExitA_k(UINT,LPCSTR);
void        WINAPI FatalAppExitW_k(UINT,LPCWSTR);
void        WINAPI FatalExit_k(int);
#define                       FatalAppExit WINELIB_NAME_AW(FatalAppExit)
BOOL        WINAPI FileEncryptionStatusA_k(LPCSTR,LPDWORD);
BOOL        WINAPI FileEncryptionStatusW_k(LPCWSTR,LPDWORD);
#define                       FileEncryptionStatus WINELIB_NAME_AW(FileEncryptionStatus)
BOOL        WINAPI FileTimeToDosDateTime_k(const FILETIME*,LPWORD,LPWORD);
BOOL        WINAPI FileTimeToLocalFileTime_k(const FILETIME*,LPFILETIME);
BOOL        WINAPI FileTimeToSystemTime_k(const FILETIME*,LPSYSTEMTIME);
BOOL        WINAPI FindActCtxSectionStringA_k(DWORD,const GUID *,ULONG,LPCSTR,PACTCTX_SECTION_KEYED_DATA);
BOOL        WINAPI FindActCtxSectionStringW_k(DWORD,const GUID *,ULONG,LPCWSTR,PACTCTX_SECTION_KEYED_DATA);
#define                       FindActCtxSectionString WINELIB_NAME_AW(FindActCtxSectionString)
BOOL        WINAPI FindActCtxSectionGuid_k(DWORD,const GUID *,ULONG,const GUID *,PACTCTX_SECTION_KEYED_DATA);
ATOM        WINAPI FindAtomA_k(LPCSTR);
ATOM        WINAPI FindAtomW_k(LPCWSTR);
#define                       FindAtom WINELIB_NAME_AW(FindAtom)
BOOL        WINAPI FindClose_k(HANDLE);
HANDLE      WINAPI FindFirstChangeNotificationA_k(LPCSTR,BOOL,DWORD);
HANDLE      WINAPI FindFirstChangeNotificationW_k(LPCWSTR,BOOL,DWORD);
#define                       FindFirstChangeNotification WINELIB_NAME_AW(FindFirstChangeNotification)
HANDLE      WINAPI FindFirstFileA_k(LPCSTR,LPWIN32_FIND_DATAA);
HANDLE      WINAPI FindFirstFileW_k(LPCWSTR,LPWIN32_FIND_DATAW);
#define                       FindFirstFile WINELIB_NAME_AW(FindFirstFile)
HANDLE      WINAPI FindFirstFileExA_k(LPCSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
HANDLE      WINAPI FindFirstFileExW_k(LPCWSTR,FINDEX_INFO_LEVELS,LPVOID,FINDEX_SEARCH_OPS,LPVOID,DWORD);
#define                       FindFirstFileEx WINELIB_NAME_AW(FindFirstFileEx)
BOOL        WINAPI FindFirstFreeAce_k(PACL,LPVOID*);
BOOL        WINAPI FindNextChangeNotification_k(HANDLE);
BOOL        WINAPI FindNextFileA_k(HANDLE,LPWIN32_FIND_DATAA);
BOOL        WINAPI FindNextFileW_k(HANDLE,LPWIN32_FIND_DATAW);
#define                       FindNextFile WINELIB_NAME_AW(FindNextFile)
BOOL        WINAPI FindCloseChangeNotification_k(HANDLE);
HRSRC       WINAPI FindResourceA_k(HMODULE,LPCSTR,LPCSTR);
HRSRC       WINAPI FindResourceW_k(HMODULE,LPCWSTR,LPCWSTR);
#define                       FindResource WINELIB_NAME_AW(FindResource)
HRSRC       WINAPI FindResourceExA_k(HMODULE,LPCSTR,LPCSTR,WORD);
HRSRC       WINAPI FindResourceExW_k(HMODULE,LPCWSTR,LPCWSTR,WORD);
#define                       FindResourceEx WINELIB_NAME_AW(FindResourceEx)
HANDLE      WINAPI FindFirstVolumeA_k(LPSTR,DWORD);
HANDLE      WINAPI FindFirstVolumeW_k(LPWSTR,DWORD);
#define                       FindFirstVolume WINELIB_NAME_AW(FindFirstVolume)
HANDLE      WINAPI FindFirstVolumeMountPointA_k(LPCSTR,LPSTR,DWORD);
HANDLE      WINAPI FindFirstVolumeMountPointW_k(LPCWSTR,LPWSTR,DWORD);
#define                       FindFirstVolumeMountPoint WINELIB_NAME_AW(FindFirstVolumeMountPoint)
BOOL        WINAPI FindNextVolumeA_k(HANDLE,LPSTR,DWORD);
BOOL        WINAPI FindNextVolumeW_k(HANDLE,LPWSTR,DWORD);
#define                       FindNextVolume WINELIB_NAME_AW(FindNextVolume)
BOOL        WINAPI FindNextVolumeMountPointA_k(HANDLE,LPSTR,DWORD);
BOOL        WINAPI FindNextVolumeMountPointW_k(HANDLE,LPWSTR,DWORD);
#define                       FindNextVolumeMountPoint WINELIB_NAME_AW(FindNextVolumeMountPoint)
BOOL        WINAPI FindVolumeClose_k(HANDLE);
BOOL        WINAPI FindVolumeMountPointClose_k(HANDLE);
DWORD       WINAPI FlsAlloc_k(PFLS_CALLBACK_FUNCTION);
BOOL        WINAPI FlsFree_k(DWORD);
PVOID       WINAPI FlsGetValue_k(DWORD);
BOOL        WINAPI FlsSetValue_k(DWORD,PVOID);
BOOL        WINAPI FlushFileBuffers_k(HANDLE);
BOOL        WINAPI FlushInstructionCache_k(HANDLE,LPCVOID,SIZE_T);
BOOL        WINAPI FlushViewOfFile_k(LPCVOID,SIZE_T);
DWORD       WINAPI FormatMessageA_k(DWORD,LPCVOID,DWORD,DWORD,LPSTR,DWORD,__ms_va_list*);
DWORD       WINAPI FormatMessageW_k(DWORD,LPCVOID,DWORD,DWORD,LPWSTR,DWORD,__ms_va_list*);
#define                       FormatMessage WINELIB_NAME_AW(FormatMessage)
BOOL        WINAPI FreeEnvironmentStringsA_k(LPSTR);
BOOL        WINAPI FreeEnvironmentStringsW_k(LPWSTR);
#define                       FreeEnvironmentStrings WINELIB_NAME_AW(FreeEnvironmentStrings)
BOOL        WINAPI FreeLibrary_k(HMODULE);
VOID WINAPI FreeLibraryAndExitThread_k(HINSTANCE,DWORD);
#define                       FreeModule_k(handle) FreeLibrary_k(handle)
#define                       FreeProcInstance_k(proc) /*nothing*/
BOOL        WINAPI FreeResource_k(HGLOBAL);
PVOID       WINAPI FreeSid_k(PSID);
BOOL        WINAPI GetAce_k(PACL,DWORD,LPVOID*);
BOOL        WINAPI GetAclInformation_k(PACL,LPVOID,DWORD,ACL_INFORMATION_CLASS);
UINT        WINAPI GetAtomNameA_k(ATOM,LPSTR,INT);
UINT        WINAPI GetAtomNameW_k(ATOM,LPWSTR,INT);
#define                       GetAtomName WINELIB_NAME_AW(GetAtomName)
BOOL        WINAPI GetBinaryTypeA_k( LPCSTR lpApplicationName, LPDWORD lpBinaryType );
BOOL        WINAPI GetBinaryTypeW_k( LPCWSTR lpApplicationName, LPDWORD lpBinaryType );
#define                       GetBinaryType WINELIB_NAME_AW(GetBinaryType)
BOOL        WINAPI GetCurrentActCtx_k(HANDLE *);
BOOL        WINAPI GetCommConfig_k(HANDLE,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetCommMask_k(HANDLE,LPDWORD);
BOOL        WINAPI GetCommModemStatus_k(HANDLE,LPDWORD);
BOOL        WINAPI GetCommProperties_k(HANDLE,LPCOMMPROP);
BOOL        WINAPI GetCommState_k(HANDLE,LPDCB);
BOOL        WINAPI GetCommTimeouts_k(HANDLE,LPCOMMTIMEOUTS);
LPSTR       WINAPI GetCommandLineA_k(void);
LPWSTR      WINAPI GetCommandLineW_k(void);
#define                       GetCommandLine WINELIB_NAME_AW(GetCommandLine)
DWORD       WINAPI GetCompressedFileSizeA_k(LPCSTR,LPDWORD);
DWORD       WINAPI GetCompressedFileSizeW_k(LPCWSTR,LPDWORD);
#define                       GetCompressedFileSize WINELIB_NAME_AW(GetCompressedFileSize)
BOOL        WINAPI GetComputerNameA_k(LPSTR,LPDWORD);
BOOL        WINAPI GetComputerNameW_k(LPWSTR,LPDWORD);
#define                       GetComputerName WINELIB_NAME_AW(GetComputerName)
BOOL        WINAPI GetComputerNameExA_k(COMPUTER_NAME_FORMAT,LPSTR,LPDWORD);
BOOL        WINAPI GetComputerNameExW_k(COMPUTER_NAME_FORMAT,LPWSTR,LPDWORD);
#define                       GetComputerNameEx WINELIB_NAME_AW(GetComputerNameEx)
UINT        WINAPI GetCurrentDirectoryA_k(UINT,LPSTR);
UINT        WINAPI GetCurrentDirectoryW_k(UINT,LPWSTR);
#define                       GetCurrentDirectory WINELIB_NAME_AW(GetCurrentDirectory)
BOOL        WINAPI GetCurrentHwProfileA_k(LPHW_PROFILE_INFOA);
BOOL        WINAPI GetCurrentHwProfileW_k(LPHW_PROFILE_INFOW);
#define                       GetCurrentHwProfile WINELIB_NAME_AW(GetCurrentHwProfile)
HANDLE      WINAPI GetCurrentProcess_k(void);
DWORD       WINAPI GetCurrentProcessorNumber_k(void);
VOID        WINAPI GetCurrentProcessorNumberEx_k(PPROCESSOR_NUMBER);
HANDLE      WINAPI GetCurrentThread_k(void);
#define                       GetCurrentTime_k() GetTickCount_k()
BOOL        WINAPI GetDefaultCommConfigA_k(LPCSTR,LPCOMMCONFIG,LPDWORD);
BOOL        WINAPI GetDefaultCommConfigW_k(LPCWSTR,LPCOMMCONFIG,LPDWORD);
#define                       GetDefaultCommConfig WINELIB_NAME_AW(GetDefaultCommConfig)
BOOL        WINAPI GetDevicePowerState_k(HANDLE,BOOL*);
BOOL        WINAPI GetDiskFreeSpaceA_k(LPCSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
BOOL        WINAPI GetDiskFreeSpaceW_k(LPCWSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
#define                       GetDiskFreeSpace WINELIB_NAME_AW(GetDiskFreeSpace)
BOOL        WINAPI GetDiskFreeSpaceExA_k(LPCSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
BOOL        WINAPI GetDiskFreeSpaceExW_k(LPCWSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
#define                       GetDiskFreeSpaceEx WINELIB_NAME_AW(GetDiskFreeSpaceEx)
DWORD       WINAPI GetDllDirectoryA_k(DWORD,LPSTR);
DWORD       WINAPI GetDllDirectoryW_k(DWORD,LPWSTR);
#define                       GetDllDirectory WINELIB_NAME_AW(GetDllDirectory)
UINT        WINAPI GetDriveTypeA_k(LPCSTR);
UINT        WINAPI GetDriveTypeW_k(LPCWSTR);
#define                       GetDriveType WINELIB_NAME_AW(GetDriveType)
DWORD       WINAPI GetDynamicTimeZoneInformation_k(PDYNAMIC_TIME_ZONE_INFORMATION);
LPSTR       WINAPI GetEnvironmentStringsA_k(void);
LPWSTR      WINAPI GetEnvironmentStringsW_k(void);
#define                       GetEnvironmentStrings WINELIB_NAME_AW(GetEnvironmentStrings)
DWORD       WINAPI GetEnvironmentVariableA_k(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetEnvironmentVariableW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetEnvironmentVariable WINELIB_NAME_AW(GetEnvironmentVariable)
UINT        WINAPI GetErrorMode_k(void);
BOOL        WINAPI GetEventLogInformation_k(HANDLE,DWORD,LPVOID,DWORD,LPDWORD);
BOOL        WINAPI GetExitCodeProcess_k(HANDLE,LPDWORD);
BOOL        WINAPI GetExitCodeThread_k(HANDLE,LPDWORD);
DWORD       WINAPI GetFileAttributesA_k(LPCSTR);
DWORD       WINAPI GetFileAttributesW_k(LPCWSTR);
#define                       GetFileAttributes WINELIB_NAME_AW(GetFileAttributes)
BOOL        WINAPI GetFileAttributesExA_k(LPCSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
BOOL        WINAPI GetFileAttributesExW_k(LPCWSTR,GET_FILEEX_INFO_LEVELS,LPVOID);
#define                       GetFileAttributesEx WINELIB_NAME_AW(GetFileAttributesEx)
BOOL        WINAPI GetFileInformationByHandle_k(HANDLE,BY_HANDLE_FILE_INFORMATION*);
BOOL        WINAPI GetFileInformationByHandleEx_k(HANDLE,FILE_INFO_BY_HANDLE_CLASS,LPVOID,DWORD);
BOOL        WINAPI GetFileSecurityA_k(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
BOOL        WINAPI GetFileSecurityW_k(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
#define                       GetFileSecurity WINELIB_NAME_AW(GetFileSecurity)
DWORD       WINAPI GetFileSize_k(HANDLE,LPDWORD);
BOOL        WINAPI GetFileSizeEx_k(HANDLE,PLARGE_INTEGER);
BOOL        WINAPI GetFileTime_k(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME);
DWORD       WINAPI GetFileType_k(HANDLE);
#define                       GetFreeSpace_k(w) _k(__MSABI_LONG_k(0x100000))
DWORD       WINAPI GetFullPathNameA_k(LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI GetFullPathNameW_k(LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define                       GetFullPathName WINELIB_NAME_AW(GetFullPathName)
BOOL        WINAPI GetHandleInformation_k(HANDLE,LPDWORD);
BOOL        WINAPI GetKernelObjectSecurity_k(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
DWORD       WINAPI GetLengthSid_k(PSID);
VOID        WINAPI GetLocalTime_k(LPSYSTEMTIME);
DWORD       WINAPI GetLogicalDrives_k(void);
UINT        WINAPI GetLogicalDriveStringsA_k(UINT,LPSTR);
UINT        WINAPI GetLogicalDriveStringsW_k(UINT,LPWSTR);
#define                       GetLogicalDriveStrings WINELIB_NAME_AW(GetLogicalDriveStrings)
DWORD       WINAPI GetLongPathNameA_k(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetLongPathNameW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetLongPathName WINELIB_NAME_AW(GetLongPathName)
BOOL        WINAPI GetMailslotInfo_k(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
DWORD       WINAPI GetModuleFileNameA_k(HMODULE,LPSTR,DWORD);
DWORD       WINAPI GetModuleFileNameW_k(HMODULE,LPWSTR,DWORD);
#define                       GetModuleFileName WINELIB_NAME_AW(GetModuleFileName)
HMODULE     WINAPI GetModuleHandleA_k(LPCSTR);
HMODULE     WINAPI GetModuleHandleW_k(LPCWSTR);
#define                       GetModuleHandle WINELIB_NAME_AW(GetModuleHandle)
BOOL        WINAPI GetModuleHandleExA_k(DWORD,LPCSTR,HMODULE*);
BOOL        WINAPI GetModuleHandleExW_k(DWORD,LPCWSTR,HMODULE*);
#define                       GetModuleHandleEx WINELIB_NAME_AW(GetModuleHandleEx)
BOOL        WINAPI GetNamedPipeHandleStateA_k(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPSTR,DWORD);
BOOL        WINAPI GetNamedPipeHandleStateW_k(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPWSTR,DWORD);
#define                       GetNamedPipeHandleState WINELIB_NAME_AW(GetNamedPipeHandleState)
BOOL        WINAPI GetNamedPipeInfo_k(HANDLE,LPDWORD,LPDWORD,LPDWORD,LPDWORD);
VOID        WINAPI GetNativeSystemInfo_k(LPSYSTEM_INFO);
BOOL        WINAPI GetNumberOfEventLogRecords_k(HANDLE,PDWORD);
BOOL        WINAPI GetOldestEventLogRecord_k(HANDLE,PDWORD);
BOOL        WINAPI GetOverlappedResult_k(HANDLE,LPOVERLAPPED,LPDWORD,BOOL);
DWORD       WINAPI GetPriorityClass_k(HANDLE);
BOOL        WINAPI GetPrivateObjectSecurity_k(PSECURITY_DESCRIPTOR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,PDWORD);
UINT        WINAPI GetPrivateProfileIntA_k(LPCSTR,LPCSTR,INT,LPCSTR);
UINT        WINAPI GetPrivateProfileIntW_k(LPCWSTR,LPCWSTR,INT,LPCWSTR);
#define                       GetPrivateProfileInt WINELIB_NAME_AW(GetPrivateProfileInt)
INT         WINAPI GetPrivateProfileSectionA_k(LPCSTR,LPSTR,DWORD,LPCSTR);
INT         WINAPI GetPrivateProfileSectionW_k(LPCWSTR,LPWSTR,DWORD,LPCWSTR);
#define                       GetPrivateProfileSection WINELIB_NAME_AW(GetPrivateProfileSection)
DWORD       WINAPI GetPrivateProfileSectionNamesA_k(LPSTR,DWORD,LPCSTR);
DWORD       WINAPI GetPrivateProfileSectionNamesW_k(LPWSTR,DWORD,LPCWSTR);
#define                       GetPrivateProfileSectionNames WINELIB_NAME_AW(GetPrivateProfileSectionNames)
INT         WINAPI GetPrivateProfileStringA_k(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT,LPCSTR);
INT         WINAPI GetPrivateProfileStringW_k(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT,LPCWSTR);
#define                       GetPrivateProfileString WINELIB_NAME_AW(GetPrivateProfileString)
BOOL        WINAPI GetPrivateProfileStructA_k(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL        WINAPI GetPrivateProfileStructW_k(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define                       GetPrivateProfileStruct WINELIB_NAME_AW(GetPrivateProfileStruct)
FARPROC     WINAPI GetProcAddress_k(HMODULE,LPCSTR);
BOOL        WINAPI GetProcessAffinityMask_k(HANDLE,PDWORD_PTR,PDWORD_PTR);
BOOL        WINAPI GetLogicalProcessorInformation_k(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,PDWORD);
BOOL        WINAPI GetLogicalProcessorInformationEx_k(LOGICAL_PROCESSOR_RELATIONSHIP,PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX,PDWORD);
DWORD       WINAPI GetProcessHeaps_k(DWORD,PHANDLE);
DWORD       WINAPI GetProcessId_k(HANDLE);
BOOL        WINAPI GetProcessIoCounters_k(HANDLE,PIO_COUNTERS);
BOOL        WINAPI GetProcessPriorityBoost_k(HANDLE,PBOOL);
BOOL        WINAPI GetProcessShutdownParameters_k(LPDWORD,LPDWORD);
BOOL        WINAPI GetProcessTimes_k(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
DWORD       WINAPI GetProcessVersion_k(DWORD);
BOOL        WINAPI GetProcessWorkingSetSize_k(HANDLE,PSIZE_T,PSIZE_T);
BOOL        WINAPI GetProductInfo_k(DWORD,DWORD,DWORD,DWORD,PDWORD);
UINT        WINAPI GetProfileIntA_k(LPCSTR,LPCSTR,INT);
UINT        WINAPI GetProfileIntW_k(LPCWSTR,LPCWSTR,INT);
#define                       GetProfileInt WINELIB_NAME_AW(GetProfileInt)
INT         WINAPI GetProfileSectionA_k(LPCSTR,LPSTR,DWORD);
INT         WINAPI GetProfileSectionW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetProfileSection WINELIB_NAME_AW(GetProfileSection)
INT         WINAPI GetProfileStringA_k(LPCSTR,LPCSTR,LPCSTR,LPSTR,UINT);
INT         WINAPI GetProfileStringW_k(LPCWSTR,LPCWSTR,LPCWSTR,LPWSTR,UINT);
#define                       GetProfileString WINELIB_NAME_AW(GetProfileString)
BOOL        WINAPI GetQueuedCompletionStatus_k(HANDLE,LPDWORD,PULONG_PTR,LPOVERLAPPED*,DWORD);
BOOL        WINAPI GetSecurityDescriptorControl_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR_CONTROL,LPDWORD);
BOOL        WINAPI GetSecurityDescriptorDacl_k(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorGroup_k(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
DWORD       WINAPI GetSecurityDescriptorLength_k(PSECURITY_DESCRIPTOR);
BOOL        WINAPI GetSecurityDescriptorOwner_k(PSECURITY_DESCRIPTOR,PSID *,LPBOOL);
BOOL        WINAPI GetSecurityDescriptorSacl_k(PSECURITY_DESCRIPTOR,LPBOOL,PACL *,LPBOOL);
PSID_IDENTIFIER_AUTHORITY WINAPI GetSidIdentifierAuthority_k(PSID);
DWORD       WINAPI GetSidLengthRequired_k(BYTE);
PDWORD      WINAPI GetSidSubAuthority_k(PSID,DWORD);
PUCHAR      WINAPI GetSidSubAuthorityCount_k(PSID);
DWORD       WINAPI GetShortPathNameA_k(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI GetShortPathNameW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetShortPathName WINELIB_NAME_AW(GetShortPathName)
VOID        WINAPI GetStartupInfoA_k(LPSTARTUPINFOA);
VOID        WINAPI GetStartupInfoW_k(LPSTARTUPINFOW);
#define                       GetStartupInfo WINELIB_NAME_AW(GetStartupInfo)
HANDLE      WINAPI GetStdHandle_k(DWORD);
UINT        WINAPI GetSystemDirectoryA_k(LPSTR,UINT);
UINT        WINAPI GetSystemDirectoryW_k(LPWSTR,UINT);
#define                       GetSystemDirectory WINELIB_NAME_AW(GetSystemDirectory)
VOID        WINAPI GetSystemInfo_k(LPSYSTEM_INFO);
BOOL        WINAPI GetSystemPowerStatus_k(LPSYSTEM_POWER_STATUS);
BOOL        WINAPI GetSystemRegistryQuota_k(PDWORD,PDWORD);
VOID        WINAPI GetSystemTime_k(LPSYSTEMTIME);
BOOL        WINAPI GetSystemTimeAdjustment_k(PDWORD,PDWORD,PBOOL);
VOID        WINAPI GetSystemTimeAsFileTime_k(LPFILETIME);
UINT        WINAPI GetSystemWindowsDirectoryA_k(LPSTR,UINT);
UINT        WINAPI GetSystemWindowsDirectoryW_k(LPWSTR,UINT);
#define                       GetSystemWindowsDirectory WINELIB_NAME_AW(GetSystemWindowsDirectory)
UINT        WINAPI GetSystemWow64DirectoryA_k(LPSTR,UINT);
UINT        WINAPI GetSystemWow64DirectoryW_k(LPWSTR,UINT);
#define                       GetSystemWow64Directory WINELIB_NAME_AW(GetSystemWow64Directory)
DWORD       WINAPI GetTapeParameters_k(HANDLE,DWORD,LPDWORD,LPVOID);
DWORD       WINAPI GetTapePosition_k(HANDLE,DWORD,LPDWORD,LPDWORD,LPDWORD);
DWORD       WINAPI GetTapeStatus_k(HANDLE);
UINT        WINAPI GetTempFileNameA_k(LPCSTR,LPCSTR,UINT,LPSTR);
UINT        WINAPI GetTempFileNameW_k(LPCWSTR,LPCWSTR,UINT,LPWSTR);
#define                       GetTempFileName WINELIB_NAME_AW(GetTempFileName)
DWORD       WINAPI GetTempPathA_k(DWORD,LPSTR);
DWORD       WINAPI GetTempPathW_k(DWORD,LPWSTR);
#define                       GetTempPath WINELIB_NAME_AW(GetTempPath)
DWORD       WINAPI GetThreadId_k(HANDLE);
BOOL        WINAPI GetThreadIOPendingFlag_k(HANDLE,PBOOL);
DWORD       WINAPI GetTickCount_k(void);
ULONGLONG   WINAPI GetTickCount64_k(void);
DWORD       WINAPI GetTimeZoneInformation_k(LPTIME_ZONE_INFORMATION);
BOOL        WINAPI GetThreadContext_k(HANDLE,CONTEXT *);
DWORD       WINAPI GetThreadErrorMode_k(void);
INT         WINAPI GetThreadPriority_k(HANDLE);
BOOL        WINAPI GetThreadPriorityBoost_k(HANDLE,PBOOL);
BOOL        WINAPI GetThreadSelectorEntry_k(HANDLE,DWORD,LPLDT_ENTRY);
BOOL        WINAPI GetThreadTimes_k(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
BOOL        WINAPI GetTokenInformation_k(HANDLE,TOKEN_INFORMATION_CLASS,LPVOID,DWORD,LPDWORD);
BOOL        WINAPI GetUserNameA_k(LPSTR,LPDWORD);
BOOL        WINAPI GetUserNameW_k(LPWSTR,LPDWORD);
#define                       GetUserName WINELIB_NAME_AW(GetUserName)
DWORD       WINAPI GetVersion_k(void);
BOOL        WINAPI GetVersionExA_k(OSVERSIONINFOA*);
BOOL        WINAPI GetVersionExW_k(OSVERSIONINFOW*);
#define                       GetVersionEx WINELIB_NAME_AW(GetVersionEx)
BOOL        WINAPI GetVolumeInformationA_k(LPCSTR,LPSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPSTR,DWORD);
BOOL        WINAPI GetVolumeInformationW_k(LPCWSTR,LPWSTR,DWORD,LPDWORD,LPDWORD,LPDWORD,LPWSTR,DWORD);
#define                       GetVolumeInformation WINELIB_NAME_AW(GetVolumeInformation)
BOOL        WINAPI GetVolumeNameForVolumeMountPointA_k(LPCSTR,LPSTR,DWORD);
BOOL        WINAPI GetVolumeNameForVolumeMountPointW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetVolumeNameForVolumeMountPoint WINELIB_NAME_AW(GetVolumeNameForVolumeMountPoint)
BOOL        WINAPI GetVolumePathNameA_k(LPCSTR,LPSTR,DWORD);
BOOL        WINAPI GetVolumePathNameW_k(LPCWSTR,LPWSTR,DWORD);
#define                       GetVolumePathName WINELIB_NAME_AW(GetVolumePathName)
BOOL        WINAPI GetVolumePathNamesForVolumeNameA_k(LPCSTR,LPSTR,DWORD,PDWORD);
BOOL        WINAPI GetVolumePathNamesForVolumeNameW_k(LPCWSTR,LPWSTR,DWORD,PDWORD);
#define                       GetVolumePathNamesForVolumeName WINELIB_NAME_AW(GetVolumePathNamesForVolumeName)
UINT        WINAPI GetWindowsDirectoryA_k(LPSTR,UINT);
UINT        WINAPI GetWindowsDirectoryW_k(LPWSTR,UINT);
#define                       GetWindowsDirectory WINELIB_NAME_AW(GetWindowsDirectory)
UINT        WINAPI GetWriteWatch_k(DWORD,LPVOID,SIZE_T,LPVOID*,ULONG_PTR*,ULONG*);
ATOM        WINAPI GlobalAddAtomA_k(LPCSTR);
ATOM        WINAPI GlobalAddAtomW_k(LPCWSTR);
#define                       GlobalAddAtom WINELIB_NAME_AW(GlobalAddAtom)
HGLOBAL     WINAPI GlobalAlloc_k(UINT,SIZE_T);
SIZE_T      WINAPI GlobalCompact_k(DWORD);
ATOM        WINAPI GlobalDeleteAtom_k(ATOM);
ATOM        WINAPI GlobalFindAtomA_k(LPCSTR);
ATOM        WINAPI GlobalFindAtomW_k(LPCWSTR);
#define                       GlobalFindAtom WINELIB_NAME_AW(GlobalFindAtom)
VOID        WINAPI GlobalFix_k(HGLOBAL);
UINT        WINAPI GlobalFlags_k(HGLOBAL);
HGLOBAL     WINAPI GlobalFree_k(HGLOBAL);
UINT        WINAPI GlobalGetAtomNameA_k(ATOM,LPSTR,INT);
UINT        WINAPI GlobalGetAtomNameW_k(ATOM,LPWSTR,INT);
#define                       GlobalGetAtomName WINELIB_NAME_AW(GlobalGetAtomName)
HGLOBAL     WINAPI GlobalHandle_k(LPCVOID);
LPVOID      WINAPI GlobalLock_k(HGLOBAL);
VOID        WINAPI GlobalMemoryStatus_k(LPMEMORYSTATUS);
BOOL        WINAPI GlobalMemoryStatusEx_k(LPMEMORYSTATUSEX);
HGLOBAL     WINAPI GlobalReAlloc_k(HGLOBAL,SIZE_T,UINT) __WINE_ALLOC_SIZE_k(3);
SIZE_T      WINAPI GlobalSize_k(HGLOBAL);
VOID        WINAPI GlobalUnfix_k(HGLOBAL);
BOOL        WINAPI GlobalUnlock_k(HGLOBAL);
BOOL        WINAPI GlobalUnWire_k(HGLOBAL);
LPVOID      WINAPI GlobalWire_k(HGLOBAL);
#define                       HasOverlappedCompleted_k(lpOverlapped) _k(_k(lpOverlapped)->Internal != STATUS_PENDING)
LPVOID      WINAPI HeapAlloc_k(HANDLE,DWORD,SIZE_T) __WINE_ALLOC_SIZE_k(3);
SIZE_T      WINAPI HeapCompact_k(HANDLE,DWORD);
HANDLE      WINAPI HeapCreate_k(DWORD,SIZE_T,SIZE_T);
BOOL        WINAPI HeapDestroy_k(HANDLE);
BOOL        WINAPI HeapFree_k(HANDLE,DWORD,LPVOID);
BOOL        WINAPI HeapLock_k(HANDLE);
LPVOID      WINAPI HeapReAlloc_k(HANDLE,DWORD,LPVOID,SIZE_T) __WINE_ALLOC_SIZE_k(4);
BOOL        WINAPI HeapQueryInformation_k(HANDLE,HEAP_INFORMATION_CLASS,PVOID,SIZE_T,PSIZE_T);
BOOL        WINAPI HeapSetInformation_k(HANDLE,HEAP_INFORMATION_CLASS,PVOID,SIZE_T);
SIZE_T      WINAPI HeapSize_k(HANDLE,DWORD,LPCVOID);
BOOL        WINAPI HeapUnlock_k(HANDLE);
BOOL        WINAPI HeapValidate_k(HANDLE,DWORD,LPCVOID);
BOOL        WINAPI HeapWalk_k(HANDLE,LPPROCESS_HEAP_ENTRY);
BOOL        WINAPI InitAtomTable_k(DWORD);
BOOL        WINAPI InitializeAcl_k(PACL,DWORD,DWORD);
VOID        WINAPI InitializeConditionVariable_k(PCONDITION_VARIABLE);
void        WINAPI InitializeCriticalSection_k(CRITICAL_SECTION *lpCrit);
BOOL        WINAPI InitializeCriticalSectionAndSpinCount_k(CRITICAL_SECTION *,DWORD);
BOOL        WINAPI InitializeCriticalSectionEx_k(CRITICAL_SECTION *,DWORD,DWORD);
BOOL        WINAPI InitializeSecurityDescriptor_k(PSECURITY_DESCRIPTOR,DWORD);
BOOL        WINAPI InitializeSid_k(PSID,PSID_IDENTIFIER_AUTHORITY,BYTE);
VOID        WINAPI InitializeSListHead_k(PSLIST_HEADER);
VOID        WINAPI InitializeSRWLock_k(PSRWLOCK);
BOOL        WINAPI InitOnceBeginInitialize_k(PINIT_ONCE, DWORD, PBOOL, PVOID*);
BOOL        WINAPI InitOnceComplete_k(PINIT_ONCE, DWORD, PVOID);
BOOL        WINAPI InitOnceExecuteOnce_k(PINIT_ONCE,PINIT_ONCE_FN,PVOID,PVOID*);
VOID        WINAPI InitOnceInitialize_k(PINIT_ONCE);
PSLIST_ENTRY WINAPI InterlockedFlushSList_k(PSLIST_HEADER);
PSLIST_ENTRY WINAPI InterlockedPopEntrySList_k(PSLIST_HEADER);
PSLIST_ENTRY WINAPI InterlockedPushEntrySList_k(PSLIST_HEADER, PSLIST_ENTRY);
BOOL        WINAPI IsBadCodePtr_k(FARPROC);
BOOL        WINAPI IsBadHugeReadPtr_k(LPCVOID,UINT);
BOOL        WINAPI IsBadHugeWritePtr_k(LPVOID,UINT);
BOOL        WINAPI IsBadReadPtr_k(LPCVOID,UINT);
BOOL        WINAPI IsBadStringPtrA_k(LPCSTR,UINT);
BOOL        WINAPI IsBadStringPtrW_k(LPCWSTR,UINT);
#define                       IsBadStringPtr WINELIB_NAME_AW(IsBadStringPtr)
BOOL        WINAPI IsBadWritePtr_k(LPVOID,UINT);
BOOL        WINAPI IsDebuggerPresent_k(void);
BOOL        WINAPI IsSystemResumeAutomatic_k(void);
BOOL        WINAPI IsTextUnicode_k(LPCVOID,INT,LPINT);
BOOL        WINAPI IsTokenRestricted_k(HANDLE);
BOOL        WINAPI IsValidAcl_k(PACL);
BOOL        WINAPI IsValidSecurityDescriptor_k(PSECURITY_DESCRIPTOR);
BOOL        WINAPI IsValidSid_k(PSID);
BOOL        WINAPI IsWellKnownSid_k(PSID,WELL_KNOWN_SID_TYPE);
BOOL        WINAPI IsWow64Process_k(HANDLE,PBOOL);
BOOL        WINAPI ImpersonateLoggedOnUser_k(HANDLE);
BOOL        WINAPI ImpersonateNamedPipeClient_k(HANDLE);
BOOL        WINAPI ImpersonateSelf_k(SECURITY_IMPERSONATION_LEVEL);
BOOL        WINAPI IsProcessInJob_k(HANDLE,HANDLE,PBOOL);
BOOL        WINAPI IsProcessorFeaturePresent_k(DWORD);
#define LeaveCriticalSection_k RtlLeaveCriticalSection_k
HMODULE     WINAPI LoadLibraryA_k(LPCSTR);
HMODULE     WINAPI LoadLibraryW_k(LPCWSTR);
#define                       LoadLibrary WINELIB_NAME_AW(LoadLibrary)
HMODULE     WINAPI LoadLibraryExA_k(LPCSTR,HANDLE,DWORD);
HMODULE     WINAPI LoadLibraryExW_k(LPCWSTR,HANDLE,DWORD);
#define                       LoadLibraryEx WINELIB_NAME_AW(LoadLibraryEx)
DWORD       WINAPI LoadModule_k(LPCSTR,LPVOID);
HGLOBAL     WINAPI LoadResource_k(HMODULE,HRSRC);
HLOCAL      WINAPI LocalAlloc_k(UINT,SIZE_T) __WINE_ALLOC_SIZE_k(2);
SIZE_T      WINAPI LocalCompact_k(UINT);
UINT        WINAPI LocalFlags_k(HLOCAL);
HLOCAL      WINAPI LocalFree_k(HLOCAL);
HLOCAL      WINAPI LocalHandle_k(LPCVOID);
LPVOID      WINAPI LocalLock_k(HLOCAL);
HLOCAL      WINAPI LocalReAlloc_k(HLOCAL,SIZE_T,UINT) __WINE_ALLOC_SIZE_k(3);
SIZE_T      WINAPI LocalShrink_k(HGLOBAL,UINT);
SIZE_T      WINAPI LocalSize_k(HLOCAL);
BOOL        WINAPI LocalUnlock_k(HLOCAL);
LPVOID      WINAPI LockResource_k(HGLOBAL);
#define                       LockSegment_k(handle) GlobalFix_k(_k(HANDLE)_k(handle))
BOOL        WINAPI LookupAccountNameA_k(LPCSTR,LPCSTR,PSID,LPDWORD,LPSTR,LPDWORD,PSID_NAME_USE);
BOOL        WINAPI LookupAccountNameW_k(LPCWSTR,LPCWSTR,PSID,LPDWORD,LPWSTR,LPDWORD,PSID_NAME_USE);
#define                       LookupAccountName WINELIB_NAME_AW(LookupAccountName)
BOOL        WINAPI LookupAccountSidA_k(LPCSTR,PSID,LPSTR,LPDWORD,LPSTR,LPDWORD,PSID_NAME_USE);
BOOL        WINAPI LookupAccountSidW_k(LPCWSTR,PSID,LPWSTR,LPDWORD,LPWSTR,LPDWORD,PSID_NAME_USE);
#define                       LookupAccountSid WINELIB_NAME_AW(LookupAccountSid)
BOOL        WINAPI LocalFileTimeToFileTime_k(const FILETIME*,LPFILETIME);
BOOL        WINAPI LockFile_k(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL        WINAPI LockFileEx_k(HANDLE, DWORD, DWORD, DWORD, DWORD, LPOVERLAPPED);
BOOL        WINAPI LogonUserA_k(LPCSTR,LPCSTR,LPCSTR,DWORD,DWORD,PHANDLE);
BOOL        WINAPI LogonUserW_k(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,DWORD,PHANDLE);
#define                       LogonUser WINELIB_NAME_AW(LogonUser)
BOOL        WINAPI LookupPrivilegeDisplayNameA_k(LPCSTR,LPCSTR,LPSTR,LPDWORD,LPDWORD);
BOOL        WINAPI LookupPrivilegeDisplayNameW_k(LPCWSTR,LPCWSTR,LPWSTR,LPDWORD,LPDWORD);
#define                       LookupPrivilegeDisplayName WINELIB_NAME_AW(LookupPrivilegeDisplayName)
BOOL        WINAPI LookupPrivilegeNameA_k(LPCSTR,PLUID,LPSTR,LPDWORD);
BOOL        WINAPI LookupPrivilegeNameW_k(LPCWSTR,PLUID,LPWSTR,LPDWORD);
#define                       LookupPrivilegeName WINELIB_NAME_AW(LookupPrivilegeName)
BOOL        WINAPI LookupPrivilegeValueA_k(LPCSTR,LPCSTR,PLUID);
BOOL        WINAPI LookupPrivilegeValueW_k(LPCWSTR,LPCWSTR,PLUID);
#define                       LookupPrivilegeValue WINELIB_NAME_AW(LookupPrivilegeValue)
BOOL        WINAPI MakeAbsoluteSD_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD,PACL,LPDWORD,PACL,LPDWORD,PSID,LPDWORD,PSID,LPDWORD);
void        WINAPI MakeCriticalSectionGlobal_k(CRITICAL_SECTION *lpCrit);
#define                       MakeProcInstance_k(proc,inst) _k(proc)
BOOL        WINAPI MakeSelfRelativeSD_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD);
VOID        WINAPI MapGenericMask_k(PDWORD,PGENERIC_MAPPING);
HMODULE     WINAPI MapHModuleSL_k(WORD);
WORD        WINAPI MapHModuleLS_k(HMODULE);
LPVOID      WINAPI MapViewOfFile_k(HANDLE,DWORD,DWORD,DWORD,SIZE_T);
LPVOID      WINAPI MapViewOfFileEx_k(HANDLE,DWORD,DWORD,DWORD,SIZE_T,LPVOID);
BOOL        WINAPI MoveFileA_k(LPCSTR,LPCSTR);
BOOL        WINAPI MoveFileW_k(LPCWSTR,LPCWSTR);
#define                       MoveFile WINELIB_NAME_AW(MoveFile)
BOOL        WINAPI MoveFileExA_k(LPCSTR,LPCSTR,DWORD);
BOOL        WINAPI MoveFileExW_k(LPCWSTR,LPCWSTR,DWORD);
#define                       MoveFileEx WINELIB_NAME_AW(MoveFileEx)
BOOL        WINAPI MoveFileWithProgressA_k(LPCSTR,LPCSTR,LPPROGRESS_ROUTINE,LPVOID,DWORD);
BOOL        WINAPI MoveFileWithProgressW_k(LPCWSTR,LPCWSTR,LPPROGRESS_ROUTINE,LPVOID,DWORD);
#define                       MoveFileWithProgress WINELIB_NAME_AW(MoveFileWithProgress)
INT         WINAPI MulDiv_k(INT,INT,INT);
BOOL        WINAPI NeedCurrentDirectoryForExePathA_k(LPCSTR);
BOOL        WINAPI NeedCurrentDirectoryForExePathW_k(LPCWSTR);
#define                       NeedCurrentDirectoryForExePath WINELIB_NAME_AW(NeedCurrentDirectoryForExePath)
BOOL        WINAPI NotifyChangeEventLog_k(HANDLE,HANDLE);
BOOL        WINAPI ObjectCloseAuditAlarmA_k(LPCSTR,LPVOID,BOOL);
BOOL        WINAPI ObjectCloseAuditAlarmW_k(LPCWSTR,LPVOID,BOOL);
#define                       ObjectCloseAuditAlarm WINELIB_NAME_AW(ObjectCloseAuditAlarm)
BOOL        WINAPI ObjectDeleteAuditAlarmA_k(LPCSTR,LPVOID,BOOL);
BOOL        WINAPI ObjectDeleteAuditAlarmW_k(LPCWSTR,LPVOID,BOOL);
#define                       ObjectDeleteAuditAlarm WINELIB_NAME_AW(ObjectDeleteAuditAlarm)
BOOL        WINAPI ObjectOpenAuditAlarmA_k(LPCSTR,LPVOID,LPSTR,LPSTR,PSECURITY_DESCRIPTOR,HANDLE,DWORD,DWORD,PPRIVILEGE_SET,BOOL,BOOL,LPBOOL);
BOOL        WINAPI ObjectOpenAuditAlarmW_k(LPCWSTR,LPVOID,LPWSTR,LPWSTR,PSECURITY_DESCRIPTOR,HANDLE,DWORD,DWORD,PPRIVILEGE_SET,BOOL,BOOL,LPBOOL);
#define                       ObjectOpenAuditAlarm WINELIB_NAME_AW(ObjectOpenAuditAlarm)
BOOL        WINAPI ObjectPrivilegeAuditAlarmA_k(LPCSTR,LPVOID,HANDLE,DWORD,PPRIVILEGE_SET,BOOL);
BOOL        WINAPI ObjectPrivilegeAuditAlarmW_k(LPCWSTR,LPVOID,HANDLE,DWORD,PPRIVILEGE_SET,BOOL);
#define                       ObjectPrivilegeAuditAlarm WINELIB_NAME_AW(ObjectPrivilegeAuditAlarm)
HANDLE      WINAPI OpenBackupEventLogA_k(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenBackupEventLogW_k(LPCWSTR,LPCWSTR);
#define                       OpenBackupEventLog WINELIB_NAME_AW(OpenBackupEventLog)
HANDLE      WINAPI OpenEventA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenEventW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenEvent WINELIB_NAME_AW(OpenEvent)
HANDLE      WINAPI OpenEventLogA_k(LPCSTR,LPCSTR);
HANDLE      WINAPI OpenEventLogW_k(LPCWSTR,LPCWSTR);
#define                       OpenEventLog WINELIB_NAME_AW(OpenEventLog)
HFILE       WINAPI OpenFile_k(LPCSTR,OFSTRUCT*,UINT);
HANDLE      WINAPI OpenFileMappingA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenFileMappingW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenFileMapping WINELIB_NAME_AW(OpenFileMapping)
HANDLE      WINAPI OpenJobObjectA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenJobObjectW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenJobObject WINELIB_NAME_AW(OpenJobObject)
HANDLE      WINAPI OpenMutexA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenMutexW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenMutex WINELIB_NAME_AW(OpenMutex)
HANDLE      WINAPI OpenProcess_k(DWORD,BOOL,DWORD);
BOOL        WINAPI OpenProcessToken_k(HANDLE,DWORD,PHANDLE);
HANDLE      WINAPI OpenSemaphoreA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenSemaphoreW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenSemaphore WINELIB_NAME_AW(OpenSemaphore)
HANDLE      WINAPI OpenThread_k(DWORD,BOOL,DWORD);
BOOL        WINAPI OpenThreadToken_k(HANDLE,DWORD,BOOL,PHANDLE);
HANDLE      WINAPI OpenWaitableTimerA_k(DWORD,BOOL,LPCSTR);
HANDLE      WINAPI OpenWaitableTimerW_k(DWORD,BOOL,LPCWSTR);
#define                       OpenWaitableTimer WINELIB_NAME_AW(OpenWaitableTimer)
VOID        WINAPI OutputDebugStringA_k(LPCSTR);
VOID        WINAPI OutputDebugStringW_k(LPCWSTR);
#define                       OutputDebugString WINELIB_NAME_AW(OutputDebugString)
BOOL        WINAPI PeekNamedPipe_k(HANDLE,PVOID,DWORD,PDWORD,PDWORD,PDWORD);
BOOL        WINAPI PostQueuedCompletionStatus_k(HANDLE,DWORD,ULONG_PTR,LPOVERLAPPED);
DWORD       WINAPI PrepareTape_k(HANDLE,DWORD,BOOL);
BOOL        WINAPI ProcessIdToSessionId_k(DWORD,DWORD*);
BOOL        WINAPI PrivilegeCheck_k(HANDLE,PPRIVILEGE_SET,LPBOOL);
BOOL        WINAPI PrivilegedServiceAuditAlarmA_k(LPCSTR,LPCSTR,HANDLE,PPRIVILEGE_SET,BOOL);
BOOL        WINAPI PrivilegedServiceAuditAlarmW_k(LPCWSTR,LPCWSTR,HANDLE,PPRIVILEGE_SET,BOOL);
#define                       PrivilegedServiceAuditAlarm WINELIB_NAME_AW(PrivilegedServiceAuditAlarm)
BOOL        WINAPI PulseEvent_k(HANDLE);
BOOL        WINAPI PurgeComm_k(HANDLE,DWORD);
BOOL        WINAPI QueryActCtxW_k(DWORD,HANDLE,PVOID,ULONG,PVOID,SIZE_T,SIZE_T *);
USHORT      WINAPI QueryDepthSList_k(PSLIST_HEADER);
DWORD       WINAPI QueryDosDeviceA_k(LPCSTR,LPSTR,DWORD);
DWORD       WINAPI QueryDosDeviceW_k(LPCWSTR,LPWSTR,DWORD);
#define                       QueryDosDevice WINELIB_NAME_AW(QueryDosDevice)
BOOL        WINAPI QueryFullProcessImageNameA_k(HANDLE,DWORD,LPSTR,PDWORD);
BOOL        WINAPI QueryFullProcessImageNameW_k(HANDLE,DWORD,LPWSTR,PDWORD);
#define                       QueryFullProcessImageName WINELIB_NAME_AW(QueryFullProcessImageName)
BOOL        WINAPI QueryInformationJobObject_k(HANDLE,JOBOBJECTINFOCLASS,LPVOID,DWORD,DWORD*);
BOOL        WINAPI QueryMemoryResourceNotification_k(HANDLE,PBOOL);
BOOL        WINAPI QueryPerformanceCounter_k(LARGE_INTEGER*);
BOOL        WINAPI QueryPerformanceFrequency_k(LARGE_INTEGER*);
DWORD       WINAPI QueueUserAPC_k(PAPCFUNC,HANDLE,ULONG_PTR);
BOOL        WINAPI QueueUserWorkItem_k(LPTHREAD_START_ROUTINE,PVOID,ULONG);
void        WINAPI RaiseException_k(DWORD,DWORD,DWORD,const ULONG_PTR *);
BOOL        WINAPI ReadEventLogA_k(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
BOOL        WINAPI ReadEventLogW_k(HANDLE,DWORD,DWORD,LPVOID,DWORD,DWORD *,DWORD *);
#define                       ReadEventLog WINELIB_NAME_AW(ReadEventLog)
BOOL        WINAPI ReadDirectoryChangesW_k(HANDLE,LPVOID,DWORD,BOOL,DWORD,LPDWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI ReadFile_k(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI ReadFileEx_k(HANDLE,LPVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI ReadFileScatter_k(HANDLE,FILE_SEGMENT_ELEMENT*,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI ReadProcessMemory_k(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
HANDLE      WINAPI RegisterEventSourceA_k(LPCSTR,LPCSTR);
HANDLE      WINAPI RegisterEventSourceW_k(LPCWSTR,LPCWSTR);
#define                       RegisterEventSource WINELIB_NAME_AW(RegisterEventSource)
BOOL        WINAPI RegisterWaitForSingleObject_k(PHANDLE,HANDLE,WAITORTIMERCALLBACK,PVOID,ULONG,ULONG);
HANDLE      WINAPI RegisterWaitForSingleObjectEx_k(HANDLE,WAITORTIMERCALLBACK,PVOID,ULONG,ULONG);
VOID        WINAPI ReleaseActCtx_k(HANDLE);
BOOL        WINAPI ReleaseMutex_k(HANDLE);
BOOL        WINAPI ReleaseSemaphore_k(HANDLE,LONG,LPLONG);
VOID        WINAPI ReleaseSRWLockExclusive_k(PSRWLOCK);
VOID        WINAPI ReleaseSRWLockShared_k(PSRWLOCK);
ULONG       WINAPI RemoveVectoredExceptionHandler_k(PVOID);
BOOL        WINAPI ReplaceFileA_k(LPCSTR,LPCSTR,LPCSTR,DWORD,LPVOID,LPVOID);
BOOL        WINAPI ReplaceFileW_k(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPVOID,LPVOID);
#define                       ReplaceFile WINELIB_NAME_AW(ReplaceFile)
BOOL        WINAPI RemoveDirectoryA_k(LPCSTR);
BOOL        WINAPI RemoveDirectoryW_k(LPCWSTR);
#define                       RemoveDirectory WINELIB_NAME_AW(RemoveDirectory)
BOOL        WINAPI ReportEventA_k(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCSTR *,LPVOID);
BOOL        WINAPI ReportEventW_k(HANDLE,WORD,WORD,DWORD,PSID,WORD,DWORD,LPCWSTR *,LPVOID);
BOOL        WINAPI RequestWakeupLatency_k(LATENCY_TIME latency);
#define                       ReportEvent WINELIB_NAME_AW(ReportEvent)
BOOL        WINAPI ResetEvent_k(HANDLE);
UINT        WINAPI ResetWriteWatch_k(LPVOID,SIZE_T);
DWORD       WINAPI ResumeThread_k(HANDLE);
BOOL        WINAPI RevertToSelf_k(void);
DWORD       WINAPI SearchPathA_k(LPCSTR,LPCSTR,LPCSTR,DWORD,LPSTR,LPSTR*);
DWORD       WINAPI SearchPathW_k(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,LPWSTR,LPWSTR*);
#define                       SearchPath WINELIB_NAME_AW(SearchPath)
BOOL        WINAPI SetAclInformation_k(PACL,LPVOID,DWORD,ACL_INFORMATION_CLASS);
BOOL        WINAPI SetCommConfig_k(HANDLE,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetCommBreak_k(HANDLE);
BOOL        WINAPI SetCommMask_k(HANDLE,DWORD);
BOOL        WINAPI SetCommState_k(HANDLE,LPDCB);
BOOL        WINAPI SetCommTimeouts_k(HANDLE,LPCOMMTIMEOUTS);
BOOL        WINAPI SetComputerNameA_k(LPCSTR);
BOOL        WINAPI SetComputerNameW_k(LPCWSTR);
#define                       SetComputerName WINELIB_NAME_AW(SetComputerName)
BOOL        WINAPI SetComputerNameExA_k(COMPUTER_NAME_FORMAT,LPCSTR);
BOOL        WINAPI SetComputerNameExW_k(COMPUTER_NAME_FORMAT,LPCWSTR);
#define                       SetComputerNameEx WINELIB_NAME_AW(SetComputerNameEx)
DWORD       WINAPI SetCriticalSectionSpinCount_k(LPCRITICAL_SECTION,DWORD);
BOOL        WINAPI SetCurrentDirectoryA_k(LPCSTR);
BOOL        WINAPI SetCurrentDirectoryW_k(LPCWSTR);
#define                       SetCurrentDirectory WINELIB_NAME_AW(SetCurrentDirectory)
BOOL        WINAPI SetDefaultCommConfigA_k(LPCSTR,LPCOMMCONFIG,DWORD);
BOOL        WINAPI SetDefaultCommConfigW_k(LPCWSTR,LPCOMMCONFIG,DWORD);
#define                       SetDefaultCommConfig WINELIB_NAME_AW(SetDefaultCommConfig)
BOOL        WINAPI SetDllDirectoryA_k(LPCSTR);
BOOL        WINAPI SetDllDirectoryW_k(LPCWSTR);
#define                       SetDllDirectory WINELIB_NAME_AW(SetDllDirectory)
BOOL        WINAPI SetDynamicTimeZoneInformation_k(CONST DYNAMIC_TIME_ZONE_INFORMATION*);
BOOL        WINAPI SetEndOfFile_k(HANDLE);
BOOL        WINAPI SetEnvironmentVariableA_k(LPCSTR,LPCSTR);
BOOL        WINAPI SetEnvironmentVariableW_k(LPCWSTR,LPCWSTR);
#define                       SetEnvironmentVariable WINELIB_NAME_AW(SetEnvironmentVariable)
UINT        WINAPI SetErrorMode_k(UINT);
BOOL        WINAPI SetEvent_k(HANDLE);
VOID        WINAPI SetFileApisToANSI_k(void);
VOID        WINAPI SetFileApisToOEM_k(void);
BOOL        WINAPI SetFileAttributesA_k(LPCSTR,DWORD);
BOOL        WINAPI SetFileAttributesW_k(LPCWSTR,DWORD);
#define                       SetFileAttributes WINELIB_NAME_AW(SetFileAttributes)
DWORD       WINAPI SetFilePointer_k(HANDLE,LONG,LPLONG,DWORD);
BOOL        WINAPI SetFilePointerEx_k(HANDLE,LARGE_INTEGER,LARGE_INTEGER*,DWORD);
BOOL        WINAPI SetFileSecurityA_k(LPCSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
BOOL        WINAPI SetFileSecurityW_k(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
#define                       SetFileSecurity WINELIB_NAME_AW(SetFileSecurity)
BOOL        WINAPI SetFileTime_k(HANDLE,const FILETIME*,const FILETIME*,const FILETIME*);
BOOL        WINAPI SetFileValidData_k(HANDLE,LONGLONG);
UINT        WINAPI SetHandleCount_k(UINT);
BOOL        WINAPI SetHandleInformation_k(HANDLE,DWORD,DWORD);
BOOL        WINAPI SetInformationJobObject_k(HANDLE,JOBOBJECTINFOCLASS,LPVOID,DWORD);
BOOL        WINAPI SetKernelObjectSecurity_k(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
BOOL        WINAPI SetLocalTime_k(const SYSTEMTIME*);
BOOL        WINAPI SetMailslotInfo_k(HANDLE,DWORD);
BOOL        WINAPI SetNamedPipeHandleState_k(HANDLE,LPDWORD,LPDWORD,LPDWORD);
BOOL        WINAPI SetPriorityClass_k(HANDLE,DWORD);
BOOL        WINAPI SetPrivateObjectSecurity_k(SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR*,PGENERIC_MAPPING,HANDLE);
BOOL        WINAPI SetProcessAffinityMask_k(HANDLE,DWORD_PTR);
BOOL        WINAPI SetProcessPriorityBoost_k(HANDLE,BOOL);
BOOL        WINAPI SetProcessShutdownParameters_k(DWORD,DWORD);
BOOL        WINAPI SetProcessWorkingSetSize_k(HANDLE,SIZE_T,SIZE_T);
BOOL        WINAPI SetSecurityDescriptorControl_k(PSECURITY_DESCRIPTOR,SECURITY_DESCRIPTOR_CONTROL,SECURITY_DESCRIPTOR_CONTROL);
BOOL        WINAPI SetSecurityDescriptorDacl_k(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL        WINAPI SetSecurityDescriptorGroup_k(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorOwner_k(PSECURITY_DESCRIPTOR,PSID,BOOL);
BOOL        WINAPI SetSecurityDescriptorSacl_k(PSECURITY_DESCRIPTOR,BOOL,PACL,BOOL);
BOOL        WINAPI SetStdHandle_k(DWORD,HANDLE);
#define                       SetSwapAreaSize_k(w) _k(w)
BOOL        WINAPI SetSystemPowerState_k(BOOL,BOOL);
BOOL        WINAPI SetSystemTime_k(const SYSTEMTIME*);
BOOL        WINAPI SetSystemTimeAdjustment_k(DWORD,BOOL);
DWORD       WINAPI SetTapeParameters_k(HANDLE,DWORD,LPVOID);
DWORD       WINAPI SetTapePosition_k(HANDLE,DWORD,DWORD,DWORD,DWORD,BOOL);
DWORD_PTR   WINAPI SetThreadAffinityMask_k(HANDLE,DWORD_PTR);
BOOL        WINAPI SetThreadContext_k(HANDLE,const CONTEXT *);
BOOL        WINAPI SetThreadErrorMode_k(DWORD,LPDWORD);
DWORD       WINAPI SetThreadExecutionState_k(EXECUTION_STATE);
DWORD       WINAPI SetThreadIdealProcessor_k(HANDLE,DWORD);
BOOL        WINAPI SetThreadPriority_k(HANDLE,INT);
BOOL        WINAPI SetThreadPriorityBoost_k(HANDLE,BOOL);
BOOL        WINAPI SetThreadToken_k(PHANDLE,HANDLE);
HANDLE      WINAPI SetTimerQueueTimer_k(HANDLE,WAITORTIMERCALLBACK,PVOID,DWORD,DWORD,BOOL);
BOOL        WINAPI SetTimeZoneInformation_k(const TIME_ZONE_INFORMATION *);
BOOL        WINAPI SetTokenInformation_k(HANDLE,TOKEN_INFORMATION_CLASS,LPVOID,DWORD);
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter_k(LPTOP_LEVEL_EXCEPTION_FILTER);
BOOL        WINAPI SetVolumeLabelA_k(LPCSTR,LPCSTR);
BOOL        WINAPI SetVolumeLabelW_k(LPCWSTR,LPCWSTR);
#define                       SetVolumeLabel WINELIB_NAME_AW(SetVolumeLabel)
BOOL        WINAPI SetVolumeMountPointA_k(LPCSTR,LPCSTR);
BOOL        WINAPI SetVolumeMountPointW_k(LPCSTR,LPCSTR);
#define                       SetVolumeMountPoint WINELIB_NAME_AW(SetVolumeMountPoint)
BOOL        WINAPI SetWaitableTimer_k(HANDLE,const LARGE_INTEGER*,LONG,PTIMERAPCROUTINE,LPVOID,BOOL);
BOOL        WINAPI SetupComm_k(HANDLE,DWORD,DWORD);
DWORD       WINAPI SignalObjectAndWait_k(HANDLE,HANDLE,DWORD,BOOL);
DWORD       WINAPI SizeofResource_k(HMODULE,HRSRC);
VOID        WINAPI Sleep_k(DWORD);
BOOL        WINAPI SleepConditionVariableCS_k(PCONDITION_VARIABLE,PCRITICAL_SECTION,DWORD);
DWORD       WINAPI SleepEx_k(DWORD,BOOL);
DWORD       WINAPI SuspendThread_k(HANDLE);
void        WINAPI SwitchToFiber_k(LPVOID);
BOOL        WINAPI SwitchToThread_k(void);
BOOL        WINAPI SystemTimeToFileTime_k(const SYSTEMTIME*,LPFILETIME);
BOOL        WINAPI SystemTimeToTzSpecificLocalTime_k(const TIME_ZONE_INFORMATION*,const SYSTEMTIME*,LPSYSTEMTIME);
BOOL        WINAPI TerminateJobObject_k(HANDLE,UINT);
BOOL        WINAPI TerminateProcess_k(HANDLE,DWORD);
BOOL        WINAPI TerminateThread_k(HANDLE,DWORD);
DWORD       WINAPI TlsAlloc_k(void);
BOOL        WINAPI TlsFree_k(DWORD);
LPVOID      WINAPI TlsGetValue_k(DWORD);
BOOL        WINAPI TlsSetValue_k(DWORD,LPVOID);
BOOL        WINAPI TransactNamedPipe_k(HANDLE,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI TransmitCommChar_k(HANDLE,CHAR);
BOOL        WINAPI TryAcquireSRWLockExclusive_k(PSRWLOCK);
BOOL        WINAPI TryAcquireSRWLockShared_k(PSRWLOCK);
BOOL        WINAPI TryEnterCriticalSection_k(CRITICAL_SECTION *lpCrit);
BOOL        WINAPI TzSpecificLocalTimeToSystemTime_k(const TIME_ZONE_INFORMATION*,const SYSTEMTIME*,LPSYSTEMTIME);
LONG        WINAPI UnhandledExceptionFilter_k(PEXCEPTION_POINTERS);
BOOL        WINAPI UnlockFile_k(HANDLE,DWORD,DWORD,DWORD,DWORD);
BOOL        WINAPI UnlockFileEx_k(HANDLE,DWORD,DWORD,DWORD,LPOVERLAPPED);
#define                       UnlockSegment_k(handle) GlobalUnfix_k(_k(HANDLE)_k(handle))
BOOL        WINAPI UnmapViewOfFile_k(LPCVOID);
BOOL        WINAPI UnregisterWait_k(HANDLE);
BOOL        WINAPI UnregisterWaitEx_k(HANDLE,HANDLE);
BOOL        WINAPI UpdateResourceA_k(HANDLE,LPCSTR,LPCSTR,WORD,LPVOID,DWORD);
BOOL        WINAPI UpdateResourceW_k(HANDLE,LPCWSTR,LPCWSTR,WORD,LPVOID,DWORD);
#define                       UpdateResource WINELIB_NAME_AW(UpdateResource)
BOOL        WINAPI VerifyVersionInfoA_k(LPOSVERSIONINFOEXA,DWORD,DWORDLONG);
BOOL        WINAPI VerifyVersionInfoW_k(LPOSVERSIONINFOEXW,DWORD,DWORDLONG);
#define                       VerifyVersionInfo WINELIB_NAME_AW(VerifyVersionInfo)
LPVOID      WINAPI VirtualAlloc_k(LPVOID,SIZE_T,DWORD,DWORD);
LPVOID      WINAPI VirtualAllocEx_k(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
BOOL        WINAPI VirtualFree_k(LPVOID,SIZE_T,DWORD);
BOOL        WINAPI VirtualFreeEx_k(HANDLE,LPVOID,SIZE_T,DWORD);
BOOL        WINAPI VirtualLock_k(LPVOID,SIZE_T);
BOOL        WINAPI VirtualProtect_k(LPVOID,SIZE_T,DWORD,LPDWORD);
BOOL        WINAPI VirtualProtectEx_k(HANDLE,LPVOID,SIZE_T,DWORD,LPDWORD);
SIZE_T      WINAPI VirtualQuery_k(LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
SIZE_T      WINAPI VirtualQueryEx_k(HANDLE,LPCVOID,PMEMORY_BASIC_INFORMATION,SIZE_T);
BOOL        WINAPI VirtualUnlock_k(LPVOID,SIZE_T);
DWORD       WINAPI WTSGetActiveConsoleSessionId_k(void);
BOOL        WINAPI WaitCommEvent_k(HANDLE,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI WaitForDebugEvent_k(LPDEBUG_EVENT,DWORD);
DWORD       WINAPI WaitForMultipleObjects_k(DWORD,const HANDLE*,BOOL,DWORD);
DWORD       WINAPI WaitForMultipleObjectsEx_k(DWORD,const HANDLE*,BOOL,DWORD,BOOL);
DWORD       WINAPI WaitForSingleObject_k(HANDLE,DWORD);
DWORD       WINAPI WaitForSingleObjectEx_k(HANDLE,DWORD,BOOL);
BOOL        WINAPI WaitNamedPipeA_k(LPCSTR,DWORD);
BOOL        WINAPI WaitNamedPipeW_k(LPCWSTR,DWORD);
#define                       WaitNamedPipe WINELIB_NAME_AW(WaitNamedPipe)
VOID        WINAPI WakeAllConditionVariable_k(PCONDITION_VARIABLE);
VOID        WINAPI WakeConditionVariable_k(PCONDITION_VARIABLE);
UINT        WINAPI WinExec_k(LPCSTR,UINT);
BOOL        WINAPI Wow64DisableWow64FsRedirection_k(PVOID*);
BOOLEAN     WINAPI Wow64EnableWow64FsRedirection_k(BOOLEAN);
BOOL        WINAPI Wow64RevertWow64FsRedirection_k(PVOID);
BOOL        WINAPI WriteFile_k(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI WriteFileEx_k(HANDLE,LPCVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE);
BOOL        WINAPI WriteFileGather_k(HANDLE,FILE_SEGMENT_ELEMENT*,DWORD,LPDWORD,LPOVERLAPPED);
BOOL        WINAPI WritePrivateProfileSectionA_k(LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WritePrivateProfileSectionW_k(LPCWSTR,LPCWSTR,LPCWSTR);
#define                       WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL        WINAPI WritePrivateProfileStringA_k(LPCSTR,LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WritePrivateProfileStringW_k(LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR);
#define                       WritePrivateProfileString WINELIB_NAME_AW(WritePrivateProfileString)
BOOL        WINAPI WritePrivateProfileStructA_k(LPCSTR,LPCSTR,LPVOID,UINT,LPCSTR);
BOOL        WINAPI WritePrivateProfileStructW_k(LPCWSTR,LPCWSTR,LPVOID,UINT,LPCWSTR);
#define                       WritePrivateProfileStruct WINELIB_NAME_AW(WritePrivateProfileStruct)
BOOL        WINAPI WriteProcessMemory_k(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T*);
BOOL        WINAPI WriteProfileSectionA_k(LPCSTR,LPCSTR);
BOOL        WINAPI WriteProfileSectionW_k(LPCWSTR,LPCWSTR);
#define                       WritePrivateProfileSection WINELIB_NAME_AW(WritePrivateProfileSection)
BOOL        WINAPI WriteProfileStringA_k(LPCSTR,LPCSTR,LPCSTR);
BOOL        WINAPI WriteProfileStringW_k(LPCWSTR,LPCWSTR,LPCWSTR);
#define                       WriteProfileString WINELIB_NAME_AW(WriteProfileString)
DWORD       WINAPI WriteTapemark_k(HANDLE,DWORD,DWORD,BOOL);
#define                       Yield_k()
BOOL        WINAPI ZombifyActCtx_k(HANDLE);

INT         WINAPI lstrcmpA(LPCSTR,LPCSTR);
INT         WINAPI lstrcmpW(LPCWSTR,LPCWSTR);
INT         WINAPI lstrcmpiA(LPCSTR,LPCSTR);
INT         WINAPI lstrcmpiW(LPCWSTR,LPCWSTR);

LPSTR       WINAPI lstrcatA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcatW(LPWSTR,LPCWSTR);
LPSTR       WINAPI lstrcpyA(LPSTR,LPCSTR);
LPWSTR      WINAPI lstrcpyW(LPWSTR,LPCWSTR);
LPSTR       WINAPI lstrcpynA(LPSTR,LPCSTR,INT);
LPWSTR      WINAPI lstrcpynW(LPWSTR,LPCWSTR,INT);
INT         WINAPI lstrlenA(LPCSTR);
INT         WINAPI lstrlenW(LPCWSTR);

DWORD       WINAPI GetCurrentProcessId_k(void);
DWORD       WINAPI GetCurrentThreadId_k(void);
DWORD       WINAPI GetLastError_k(void);
HANDLE      WINAPI GetProcessHeap_k(void);
VOID        WINAPI SetLastError_k(DWORD);

LONG WINAPI InterlockedCompareExchange_k(LONG volatile*,LONG,LONG);
LONG WINAPI InterlockedDecrement_k(LONG volatile*);
LONG WINAPI InterlockedExchange_k(LONG volatile*,LONG);
LONG WINAPI InterlockedExchangeAdd_k(LONG volatile*,LONG);
LONG WINAPI InterlockedIncrement_k(LONG volatile*);
PVOID WINAPI InterlockedExchangePointer_k( PVOID volatile *, PVOID);
PVOID WINAPI InterlockedCompareExchangePointer_k( PVOID volatile *dest, PVOID xchg, PVOID compare );

#define TEB_THREAD_ID 0x24
#define TEB_PEB 0x30
#define TEB_LAST_ERROR 0x34
#define TEB_LOCALE 0xC4
#define TEB_LAST_STATUS 0xBF4
#define TEB_STATIC_UNICODE_BUFFER 0xC00

#define PEB_HEAP 0x18

DWORD NtCurrentTeb_k();
struct KThread* currentThread();
struct KProcess* currentProcess();

#include "wingdi.h"
#include "winuser.h"
#include "winnls.h"
#include "winternl.h"

#define debugstr_w(w) ""

#define debugstr_a(a) a

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

enum coords_relative
{
    COORDS_CLIENT,   /* relative to client area */
    COORDS_WINDOW,   /* relative to whole window area */
    COORDS_PARENT,   /* relative to parent's client area */
    COORDS_SCREEN    /* relative to screen origin */
};

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

struct winstation
{
    unsigned int       flags;              /* winstation flags */
    PblMap *           desktops;           /* list of desktops of this winstation */
    struct clipboard  *clipboard;          /* clipboard information */
    struct atom_table *atom_table;         /* global atom table */
    WCHAR              name[MAX_PATH];
};

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

#define stc1        0x0440

#define SEND_MSG_ABORT_IF_HUNG  0x01
#define SEND_HWMSG_INJECTED    0x01

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

#endif