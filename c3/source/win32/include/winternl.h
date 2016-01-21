/*
 * Internal NT APIs and data structures
 *
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_WINTERNL_H
#define __WINE_WINTERNL_H

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */


/**********************************************************************
 * Fundamental types and data structures
 */

#ifndef WINE_NTSTATUS_DECLARED
#define WINE_NTSTATUS_DECLARED
typedef LONG NTSTATUS;
#endif

typedef CONST char *PCSZ;

typedef short CSHORT;
typedef CSHORT *PCSHORT;

#ifndef __STRING_DEFINED__
#define __STRING_DEFINED__
typedef struct _STRING {
  USHORT Length;
  USHORT MaximumLength;
  PCHAR Buffer;
} STRING, *PSTRING;
#endif

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef const STRING *PCANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef const STRING *PCOEM_STRING;

#ifndef __UNICODE_STRING_DEFINED__
#define __UNICODE_STRING_DEFINED__
typedef struct _UNICODE_STRING {
  USHORT Length;        /* bytes */
  USHORT MaximumLength; /* bytes */
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
#endif

typedef const UNICODE_STRING *PCUNICODE_STRING;

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

typedef struct _GDI_TEB_BATCH
{
    ULONG  Offset;
    HANDLE HDC;
    ULONG  Buffer[0x136];
} GDI_TEB_BATCH;

typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME
{
    struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME *Previous;
    struct _ACTIVATION_CONTEXT                 *ActivationContext;
    ULONG                                       Flags;
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _ACTIVATION_CONTEXT_STACK
{
    ULONG                               Flags;
    ULONG                               NextCookieSequenceNumber;
    RTL_ACTIVATION_CONTEXT_STACK_FRAME *ActiveFrame;
    LIST_ENTRY                          FrameListCache;
} ACTIVATION_CONTEXT_STACK, *PACTIVATION_CONTEXT_STACK;

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


/***********************************************************************
 * TEB data structure
 */
typedef struct _TEB
{                                                                 /* win32/win64 */
    NT_TIB                       Tib;                               /* 000/0000 */
    PVOID                        EnvironmentPointer;                /* 01c/0038 */
    CLIENT_ID                    ClientId;                          /* 020/0040 */
    PVOID                        ActiveRpcHandle;                   /* 028/0050 */
    PVOID                        ThreadLocalStoragePointer;         /* 02c/0058 */
    PPEB                         Peb;                               /* 030/0060 */
    ULONG                        LastErrorValue;                    /* 034/0068 */
    ULONG                        CountOfOwnedCriticalSections;      /* 038/006c */
    PVOID                        CsrClientThread;                   /* 03c/0070 */
    PVOID                        Win32ThreadInfo;                   /* 040/0078 */
    ULONG                        Win32ClientInfo[31];               /* 044/0080 used for user32 private data in Wine */
    PVOID                        WOW32Reserved;                     /* 0c0/0100 */
    ULONG                        CurrentLocale;                     /* 0c4/0108 */
    ULONG                        FpSoftwareStatusRegister;          /* 0c8/010c */
    PVOID                        SystemReserved1[54];               /* 0cc/0110 used for kernel32 private data in Wine */
    LONG                         ExceptionCode;                     /* 1a4/02c0 */
    ACTIVATION_CONTEXT_STACK     ActivationContextStack;            /* 1a8/02c8 */
    BYTE                         SpareBytes1[24];                   /* 1bc/02e8 used for ntdll private data in Wine */
    PVOID                        SystemReserved2[10];               /* 1d4/0300 used for ntdll private data in Wine */
    GDI_TEB_BATCH                GdiTebBatch;                       /* 1fc/0350 used for vm86 private data in Wine */
    HANDLE                       gdiRgn;                            /* 6dc/0838 */
    HANDLE                       gdiPen;                            /* 6e0/0840 */
    HANDLE                       gdiBrush;                          /* 6e4/0848 */
    CLIENT_ID                    RealClientId;                      /* 6e8/0850 */
    HANDLE                       GdiCachedProcessHandle;            /* 6f0/0860 */
    ULONG                        GdiClientPID;                      /* 6f4/0868 */
    ULONG                        GdiClientTID;                      /* 6f8/086c */
    PVOID                        GdiThreadLocaleInfo;               /* 6fc/0870 */
    ULONG                        UserReserved[5];                   /* 700/0878 */
    PVOID                        glDispachTable[280];               /* 714/0890 */
    PVOID                        glReserved1[26];                   /* b74/1150 */
    PVOID                        glReserved2;                       /* bdc/1220 */
    PVOID                        glSectionInfo;                     /* be0/1228 */
    PVOID                        glSection;                         /* be4/1230 */
    PVOID                        glTable;                           /* be8/1238 */
    PVOID                        glCurrentRC;                       /* bec/1240 */
    PVOID                        glContext;                         /* bf0/1248 */
    ULONG                        LastStatusValue;                   /* bf4/1250 */
    UNICODE_STRING               StaticUnicodeString;               /* bf8/1258 used by advapi32 */
    WCHAR                        StaticUnicodeBuffer[261];          /* c00/1268 used by advapi32 */
    PVOID                        DeallocationStack;                 /* e0c/1478 */
    PVOID                        TlsSlots[64];                      /* e10/1480 */
    LIST_ENTRY                   TlsLinks;                          /* f10/1680 */
    PVOID                        Vdm;                               /* f18/1690 */
    PVOID                        ReservedForNtRpc;                  /* f1c/1698 */
    PVOID                        DbgSsReserved[2];                  /* f20/16a0 */
    ULONG                        HardErrorDisabled;                 /* f28/16b0 */
    PVOID                        Instrumentation[16];               /* f2c/16b8 */
    PVOID                        WinSockData;                       /* f6c/1738 */
    ULONG                        GdiBatchCount;                     /* f70/1740 */
    ULONG                        Spare2;                            /* f74/1744 */
    PVOID                        Spare3;                            /* f78/1748 */
    PVOID                        Spare4;                            /* f7c/1750 */
    PVOID                        ReservedForOle;                    /* f80/1758 */
    ULONG                        WaitingOnLoaderLock;               /* f84/1760 */
    PVOID                        Reserved5[3];                      /* f88/1768 */
    PVOID                       *TlsExpansionSlots;                 /* f94/1780 */
    ULONG                        ImpersonationLocale;               /* f98/1788 */
    ULONG                        IsImpersonating;                   /* f9c/178c */
    PVOID                        NlsCache;                          /* fa0/1790 */
    PVOID                        ShimData;                          /* fa4/1798 */
    ULONG                        HeapVirtualAffinity;               /* fa8/17a0 */
    PVOID                        CurrentTransactionHandle;          /* fac/17a8 */
    PVOID                        ActiveFrame;                       /* fb0/17b0 */
#ifdef _WIN64
    PVOID                        unknown[2];                        /*     17b8 */
#endif
    PVOID                       *FlsSlots;                          /* fb4/17c8 */
} TEB, *PTEB;

/***********************************************************************
 * Enums
 */

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

typedef struct _FILE_DIRECTORY_INFORMATION {
    ULONG               NextEntryOffset;
    ULONG               FileIndex;
    LARGE_INTEGER       CreationTime;
    LARGE_INTEGER       LastAccessTime;
    LARGE_INTEGER       LastWriteTime;
    LARGE_INTEGER       ChangeTime;
    LARGE_INTEGER       EndOfFile;
    LARGE_INTEGER       AllocationSize;
    ULONG               FileAttributes;
    ULONG               FileNameLength;
    WCHAR               FileName[ANYSIZE_ARRAY];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef struct _FILE_FULL_DIRECTORY_INFORMATION {
    ULONG               NextEntryOffset;
    ULONG               FileIndex;
    LARGE_INTEGER       CreationTime;
    LARGE_INTEGER       LastAccessTime;
    LARGE_INTEGER       LastWriteTime;
    LARGE_INTEGER       ChangeTime;
    LARGE_INTEGER       EndOfFile;
    LARGE_INTEGER       AllocationSize;
    ULONG               FileAttributes;
    ULONG               FileNameLength;
    ULONG               EaSize;
    WCHAR               FileName[ANYSIZE_ARRAY];
} FILE_FULL_DIRECTORY_INFORMATION, *PFILE_FULL_DIRECTORY_INFORMATION,
  FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIRECTORY_INFORMATION {
    ULONG               NextEntryOffset;
    ULONG               FileIndex;
    LARGE_INTEGER       CreationTime;
    LARGE_INTEGER       LastAccessTime;
    LARGE_INTEGER       LastWriteTime;
    LARGE_INTEGER       ChangeTime;
    LARGE_INTEGER       EndOfFile;
    LARGE_INTEGER       AllocationSize;
    ULONG               FileAttributes;
    ULONG               FileNameLength;
    ULONG               EaSize;
    LARGE_INTEGER       FileId;
    WCHAR               FileName[ANYSIZE_ARRAY];
} FILE_ID_FULL_DIRECTORY_INFORMATION, *PFILE_ID_FULL_DIRECTORY_INFORMATION;

typedef struct _FILE_BOTH_DIRECTORY_INFORMATION {
    ULONG               NextEntryOffset;
    ULONG               FileIndex;
    LARGE_INTEGER       CreationTime;
    LARGE_INTEGER       LastAccessTime;
    LARGE_INTEGER       LastWriteTime;
    LARGE_INTEGER       ChangeTime;
    LARGE_INTEGER       EndOfFile;
    LARGE_INTEGER       AllocationSize;
    ULONG               FileAttributes;
    ULONG               FileNameLength;
    ULONG               EaSize;
    CHAR                ShortNameLength;
    WCHAR               ShortName[12];
    WCHAR               FileName[ANYSIZE_ARRAY];
} FILE_BOTH_DIRECTORY_INFORMATION, *PFILE_BOTH_DIRECTORY_INFORMATION,
  FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIRECTORY_INFORMATION {
    ULONG               NextEntryOffset;
    ULONG               FileIndex;
    LARGE_INTEGER       CreationTime;
    LARGE_INTEGER       LastAccessTime;
    LARGE_INTEGER       LastWriteTime;
    LARGE_INTEGER       ChangeTime;
    LARGE_INTEGER       EndOfFile;
    LARGE_INTEGER       AllocationSize;
    ULONG               FileAttributes;
    ULONG               FileNameLength;
    ULONG               EaSize;
    CHAR                ShortNameLength;
    WCHAR               ShortName[12];
    LARGE_INTEGER       FileId;
    WCHAR               FileName[ANYSIZE_ARRAY];
} FILE_ID_BOTH_DIRECTORY_INFORMATION, *PFILE_ID_BOTH_DIRECTORY_INFORMATION;

typedef struct _FILE_BASIC_INFORMATION {
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

typedef struct _FILE_STANDARD_INFORMATION {
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

typedef struct _FILE_INTERNAL_INFORMATION {
    LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

typedef struct _FILE_EA_INFORMATION {
    ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_NAME_INFORMATION {
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef struct _FILE_RENAME_INFORMATION {
    BOOLEAN Replace;
    HANDLE RootDir;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

typedef struct _FILE_DISPOSITION_INFORMATION {
    BOOLEAN DoDeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

typedef struct _FILE_POSITION_INFORMATION {
    LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

typedef struct _FILE_ALIGNMENT_INFORMATION {
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

typedef struct _FILE_ALLOCATION_INFORMATION {
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

typedef struct _FILE_END_OF_FILE_INFORMATION {
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

typedef struct _FILE_MODE_INFORMATION {
    ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION
{
    ULONG FileAttributes;
    ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;

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

typedef struct _FILE_ALL_INFORMATION {
    FILE_BASIC_INFORMATION     BasicInformation;
    FILE_STANDARD_INFORMATION  StandardInformation;
    FILE_INTERNAL_INFORMATION  InternalInformation;
    FILE_EA_INFORMATION        EaInformation;
    FILE_ACCESS_INFORMATION    AccessInformation;
    FILE_POSITION_INFORMATION  PositionInformation;
    FILE_MODE_INFORMATION      ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION      NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsControlInformation,
    FileFsFullSizeInformation,
    FileFsObjectIdInformation,
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation
} KEY_INFORMATION_CLASS;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;

typedef enum _OBJECT_INFORMATION_CLASS {
    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    ObjectAllInformation,
    ObjectDataInformation
} OBJECT_INFORMATION_CLASS, *POBJECT_INFORMATION_CLASS;

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation = 0,
    ProcessQuotaLimits = 1,
    ProcessIoCounters = 2,
    ProcessVmCounters = 3,
    ProcessTimes = 4,
    ProcessBasePriority = 5,
    ProcessRaisePriority = 6,
    ProcessDebugPort = 7,
    ProcessExceptionPort = 8,
    ProcessAccessToken = 9,
    ProcessLdtInformation = 10,
    ProcessLdtSize = 11,
    ProcessDefaultHardErrorMode = 12,
    ProcessIoPortHandlers = 13,
    ProcessPooledUsageAndLimits = 14,
    ProcessWorkingSetWatch = 15,
    ProcessUserModeIOPL = 16,
    ProcessEnableAlignmentFaultFixup = 17,
    ProcessPriorityClass = 18,
    ProcessWx86Information = 19,
    ProcessHandleCount = 20,
    ProcessAffinityMask = 21,
    ProcessPriorityBoost = 22,
    ProcessDeviceMap = 23,
    ProcessSessionInformation = 24,
    ProcessForegroundInformation = 25,
    ProcessWow64Information = 26,
    ProcessImageFileName = 27,
    ProcessLUIDDeviceMapsEnabled = 28,
    ProcessBreakOnTermination = 29,
    ProcessDebugObjectHandle = 30,
    ProcessDebugFlags = 31,
    ProcessHandleTracing = 32,
    ProcessExecuteFlags = 34,
    MaxProcessInfoClass
} PROCESSINFOCLASS, PROCESS_INFORMATION_CLASS;

#define MEM_EXECUTE_OPTION_DISABLE   0x01
#define MEM_EXECUTE_OPTION_ENABLE    0x02
#define MEM_EXECUTE_OPTION_PERMANENT 0x08

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemCpuInformation = 1,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3, /* was SystemTimeInformation */
    Unknown4,
    SystemProcessInformation = 5,
    Unknown6,
    Unknown7,
    SystemProcessorPerformanceInformation = 8,
    Unknown9,
    Unknown10,
    SystemModuleInformation = 11,
    Unknown12,
    Unknown13,
    Unknown14,
    Unknown15,
    SystemHandleInformation = 16,
    Unknown17,
    SystemPageFileInformation = 18,
    Unknown19,
    Unknown20,
    SystemCacheInformation = 21,
    Unknown22,
    SystemInterruptInformation = 23,
    SystemDpcBehaviourInformation = 24,
    SystemFullMemoryInformation = 25,
    SystemNotImplemented6 = 25,
    SystemLoadImage = 26,
    SystemUnloadImage = 27,
    SystemTimeAdjustmentInformation = 28,
    SystemTimeAdjustment = 28,
    SystemSummaryMemoryInformation = 29,
    SystemNotImplemented7 = 29,
    SystemNextEventIdInformation = 30,
    SystemNotImplemented8 = 30,
    SystemEventIdsInformation = 31,
    SystemCrashDumpInformation = 32,
    SystemExceptionInformation = 33,
    SystemCrashDumpStateInformation = 34,
    SystemKernelDebuggerInformation = 35,
    SystemContextSwitchInformation = 36,
    SystemRegistryQuotaInformation = 37,
    SystemCurrentTimeZoneInformation = 44,
    SystemTimeZoneInformation = 44,
    SystemLookasideInformation = 45,
    SystemSetTimeSlipEvent = 46,
    SystemCreateSession = 47,
    SystemDeleteSession = 48,
    SystemInvalidInfoClass4 = 49,
    SystemRangeStartInformation = 50,
    SystemVerifierInformation = 51,
    SystemAddVerifier = 52,
    SystemSessionProcessesInformation	= 53,
    Unknown54,
    Unknown55,
    Unknown56,
    Unknown57,
    Unknown58,
    Unknown59,
    Unknown60,
    Unknown61,
    Unknown62,
    Unknown63,
    Unknown64,
    Unknown65,
    Unknown66,
    Unknown67,
    Unknown68,
    Unknown69,
    Unknown70,
    Unknown71,
    Unknown72,
    SystemLogicalProcessorInformation = 73,
    SystemInformationClassMax
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef enum _TIMER_TYPE {
    NotificationTimer,
    SynchronizationTimer
} TIMER_TYPE;

typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair_Reusable,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger,
    MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS  ExitStatus;
    PVOID     TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    LONG      Priority;
    LONG      BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _THREAD_DESCRIPTOR_INFORMATION
{
    DWORD       Selector;
    LDT_ENTRY   Entry;
} THREAD_DESCRIPTOR_INFORMATION, *PTHREAD_DESCRIPTOR_INFORMATION;

typedef struct _KERNEL_USER_TIMES {
    LARGE_INTEGER  CreateTime;
    LARGE_INTEGER  ExitTime;
    LARGE_INTEGER  KernelTime;
    LARGE_INTEGER  UserTime;
} KERNEL_USER_TIMES, *PKERNEL_USER_TIMES;

typedef enum _WINSTATIONINFOCLASS {
    WinStationInformation = 8
} WINSTATIONINFOCLASS;

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;

typedef struct _MEMORY_SECTION_NAME
{
    UNICODE_STRING SectionFileName;
} MEMORY_SECTION_NAME, *PMEMORY_SECTION_NAME;

typedef enum _MUTANT_INFORMATION_CLASS
{
    MutantBasicInformation
} MUTANT_INFORMATION_CLASS, *PMUTANT_INFORMATION_CLASS;

typedef struct _MUTANT_BASIC_INFORMATION {
    LONG        CurrentCount;
    BOOLEAN     OwnedByCaller;
    BOOLEAN     AbandonedState;
} MUTANT_BASIC_INFORMATION, *PMUTANT_BASIC_INFORMATION;

typedef enum _TIMER_INFORMATION_CLASS
{
    TimerBasicInformation = 0
} TIMER_INFORMATION_CLASS;

typedef struct _TIMER_BASIC_INFORMATION
{
    LARGE_INTEGER RemainingTime;
    BOOLEAN       TimerState;
} TIMER_BASIC_INFORMATION, *PTIMER_BASIC_INFORMATION;


/* return type of RtlDetermineDosPathNameType_U (FIXME: not the correct names) */
typedef enum
{
    INVALID_PATH = 0,
    UNC_PATH,              /* "//foo" */
    ABSOLUTE_DRIVE_PATH,   /* "c:/foo" */
    RELATIVE_DRIVE_PATH,   /* "c:foo" */
    ABSOLUTE_PATH,         /* "/foo" */
    RELATIVE_PATH,         /* "foo" */
    DEVICE_PATH,           /* "//./foo" */
    UNC_DOT_PATH           /* "//." */
} DOS_PATHNAME_TYPE;


/***********************************************************************
 * Types and data structures
 */

/* This is used by NtQuerySystemInformation */
typedef struct _SYSTEM_THREAD_INFORMATION
{                                    /* win32/win64 */
    LARGE_INTEGER KernelTime;          /* 00/00 */
    LARGE_INTEGER UserTime;            /* 08/08 */
    LARGE_INTEGER CreateTime;          /* 10/10 */
    DWORD         dwTickCount;         /* 18/18 */
    LPVOID        StartAddress;        /* 1c/20 */
    CLIENT_ID     ClientId;            /* 20/28 */
    DWORD         dwCurrentPriority;   /* 28/38 */
    DWORD         dwBasePriority;      /* 2c/3c */
    DWORD         dwContextSwitches;   /* 30/40 */
    DWORD         dwThreadState;       /* 34/44 */
    DWORD         dwWaitReason;        /* 38/48 */
    DWORD         dwUnknown;           /* 3c/4c */
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  } DUMMYUNIONNAME;

  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef void (WINAPI * PIO_APC_ROUTINE)(PVOID,PIO_STATUS_BLOCK,ULONG);

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG         TitleIndex;
    ULONG         NameLength;
    WCHAR         Name[1];
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION
{
    LARGE_INTEGER LastWriteTime;
    ULONG         TitleIndex;
    ULONG         ClassOffset;
    ULONG         ClassLength;
    ULONG         NameLength;
    WCHAR         Name[1];
   /* Class[1]; */
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION
{
    LARGE_INTEGER LastWriteTime;
    ULONG         TitleIndex;
    ULONG         ClassOffset;
    ULONG         ClassLength;
    ULONG         SubKeys;
    ULONG         MaxNameLen;
    ULONG         MaxClassLen;
    ULONG         Values;
    ULONG         MaxValueNameLen;
    ULONG         MaxValueDataLen;
    WCHAR         Class[1];
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef struct _KEY_NAME_INFORMATION {
    ULONG         NameLength;
    WCHAR         Name[1];
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef struct _KEY_VALUE_ENTRY
{
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG NameLength;
    WCHAR Name[1];
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataOffset;
    ULONG DataLength;
    ULONG NameLength;
    WCHAR Name[1];
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG TitleIndex;
    ULONG Type;
    ULONG DataLength;
    UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

#ifndef __OBJECT_ATTRIBUTES_DEFINED__
#define __OBJECT_ATTRIBUTES_DEFINED__
typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;       /* type SECURITY_DESCRIPTOR */
  PVOID SecurityQualityOfService; /* type SECURITY_QUALITY_OF_SERVICE */
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
#endif

typedef struct _OBJECT_DATA_INFORMATION {
    BOOLEAN InheritHandle;
    BOOLEAN ProtectFromClose;
} OBJECT_DATA_INFORMATION, *POBJECT_DATA_INFORMATION;

typedef struct _OBJECT_BASIC_INFORMATION {
    ULONG  Attributes;
    ACCESS_MASK  GrantedAccess;
    ULONG  HandleCount;
    ULONG  PointerCount;
    ULONG  PagedPoolUsage;
    ULONG  NonPagedPoolUsage;
    ULONG  Reserved[3];
    ULONG  NameInformationLength;
    ULONG  TypeInformationLength;
    ULONG  SecurityDescriptorLength;
    LARGE_INTEGER  CreateTime;
} OBJECT_BASIC_INFORMATION, *POBJECT_BASIC_INFORMATION;

typedef struct _OBJECT_NAME_INFORMATION {
    UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct __OBJECT_TYPE_INFORMATION {
    UNICODE_STRING TypeName;
    ULONG Reserved [22];
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _PROCESS_BASIC_INFORMATION {
#ifdef __WINESRC__
    DWORD_PTR ExitStatus;
    PPEB PebBaseAddress;
    DWORD_PTR AffinityMask;
    DWORD_PTR BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
#else
    PVOID Reserved1;
    PPEB PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
#endif
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

#define PROCESS_PRIOCLASS_IDLE          1
#define PROCESS_PRIOCLASS_NORMAL        2
#define PROCESS_PRIOCLASS_HIGH          3
#define PROCESS_PRIOCLASS_REALTIME      4
#define PROCESS_PRIOCLASS_BELOW_NORMAL  5
#define PROCESS_PRIOCLASS_ABOVE_NORMAL  6

typedef struct _PROCESS_PRIORITY_CLASS {
    BOOLEAN     Foreground;
    UCHAR       PriorityClass;
} PROCESS_PRIORITY_CLASS, *PPROCESS_PRIORITY_CLASS;

typedef struct _RTL_HEAP_DEFINITION {
    ULONG Length; /* = sizeof(RTL_HEAP_DEFINITION) */

    ULONG Unknown[11];
} RTL_HEAP_DEFINITION, *PRTL_HEAP_DEFINITION;

typedef struct _RTL_RWLOCK {
    RTL_CRITICAL_SECTION rtlCS;

    HANDLE hSharedReleaseSemaphore;
    UINT   uSharedWaiters;

    HANDLE hExclusiveReleaseSemaphore;
    UINT   uExclusiveWaiters;

    INT    iNumberActive;
    HANDLE hOwningThreadId;
    DWORD  dwTimeoutBoost;
    PVOID  pDebugInfo;
} RTL_RWLOCK, *LPRTL_RWLOCK;

/* System Information Class 0x00 */

typedef struct _SYSTEM_BASIC_INFORMATION {
#ifdef __WINESRC__
    DWORD     unknown;
    ULONG     KeMaximumIncrement;
    ULONG     PageSize;
    ULONG     MmNumberOfPhysicalPages;
    ULONG     MmLowestPhysicalPage;
    ULONG     MmHighestPhysicalPage;
    ULONG_PTR AllocationGranularity;
    PVOID     LowestUserAddress;
    PVOID     HighestUserAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    BYTE      NumberOfProcessors;
#else
    BYTE Reserved1[24];
    PVOID Reserved2[4];
    CCHAR NumberOfProcessors;
#endif
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

/* System Information Class 0x01 */

typedef struct _SYSTEM_CPU_INFORMATION {
    WORD Architecture;
    WORD Level;
    WORD Revision;       /* combination of CPU model and stepping */
    WORD Reserved;       /* always zero */
    DWORD FeatureSet;    /* see bit flags below */
} SYSTEM_CPU_INFORMATION, *PSYSTEM_CPU_INFORMATION;

/* definitions of bits in the Feature set for the x86 processors */
#define CPU_FEATURE_VME    0x00000005   /* Virtual 86 Mode Extensions */
#define CPU_FEATURE_TSC    0x00000002   /* Time Stamp Counter available */
#define CPU_FEATURE_CMOV   0x00000008   /* Conditional Move instruction*/
#define CPU_FEATURE_PGE    0x00000014   /* Page table Entry Global bit */ 
#define CPU_FEATURE_PSE    0x00000024   /* Page Size Extension */
#define CPU_FEATURE_MTRR   0x00000040   /* Memory Type Range Registers */
#define CPU_FEATURE_CX8    0x00000080   /* Compare and eXchange 8 byte instr. */
#define CPU_FEATURE_MMX    0x00000100   /* Multi Media eXtensions */
#define CPU_FEATURE_X86    0x00000200   /* seems to be always ON, on the '86 */
#define CPU_FEATURE_PAT    0x00000400   /* Page Attribute Table */
#define CPU_FEATURE_FXSR   0x00000800   /* FXSAVE and FXSTORE instructions */
#define CPU_FEATURE_SEP    0x00001000   /* SYSENTER and SYSEXIT instructions */
#define CPU_FEATURE_SSE    0x00002000   /* SSE extensions (ext. MMX) */
#define CPU_FEATURE_3DNOW  0x00004000   /* 3DNOW instructions available */
#define CPU_FEATURE_SSE2   0x00010000   /* SSE2 extensions (XMMI64) */
#define CPU_FEATURE_DS     0x00020000   /* Debug Store */
#define CPU_FEATURE_HTT    0x00040000   /* Hyper Threading Technology */

/* System Information Class 0x02 */

/* Documented in "Windows NT/2000 Native API Reference" by Gary Nebbett. */
typedef struct _SYSTEM_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    ULONG ReadOperationCount;
    ULONG WriteOperationCount;
    ULONG OtherOperationCount;
    ULONG AvailablePages;
    ULONG TotalCommittedPages;
    ULONG TotalCommitLimit;
    ULONG PeakCommitment;
    ULONG PageFaults;
    ULONG WriteCopyFaults;
    ULONG TransitionFaults;
    ULONG Reserved1;
    ULONG DemandZeroFaults;
    ULONG PagesRead;
    ULONG PageReadIos;
    ULONG Reserved2[2];
    ULONG PagefilePagesWritten;
    ULONG PagefilePageWriteIos;
    ULONG MappedFilePagesWritten;
    ULONG MappedFilePageWriteIos;
    ULONG PagedPoolUsage;
    ULONG NonPagedPoolUsage;
    ULONG PagedPoolAllocs;
    ULONG PagedPoolFrees;
    ULONG NonPagedPoolAllocs;
    ULONG NonPagedPoolFrees;
    ULONG TotalFreeSystemPtes;
    ULONG SystemCodePage;
    ULONG TotalSystemDriverPages;
    ULONG TotalSystemCodePages;
    ULONG SmallNonPagedLookasideListAllocateHits;
    ULONG SmallPagedLookasideListAllocateHits;
    ULONG Reserved3;
    ULONG MmSystemCachePage;
    ULONG PagedPoolPage;
    ULONG SystemDriverPage;
    ULONG FastReadNoWait;
    ULONG FastReadWait;
    ULONG FastReadResourceMiss;
    ULONG FastReadNotPossible;
    ULONG FastMdlReadNoWait;
    ULONG FastMdlReadWait;
    ULONG FastMdlReadResourceMiss;
    ULONG FastMdlReadNotPossible;
    ULONG MapDataNoWait;
    ULONG MapDataWait;
    ULONG MapDataNoWaitMiss;
    ULONG MapDataWaitMiss;
    ULONG PinMappedDataCount;
    ULONG PinReadNoWait;
    ULONG PinReadWait;
    ULONG PinReadNoWaitMiss;
    ULONG PinReadWaitMiss;
    ULONG CopyReadNoWait;
    ULONG CopyReadWait;
    ULONG CopyReadNoWaitMiss;
    ULONG CopyReadWaitMiss;
    ULONG MdlReadNoWait;
    ULONG MdlReadWait;
    ULONG MdlReadNoWaitMiss;
    ULONG MdlReadWaitMiss;
    ULONG ReadAheadIos;
    ULONG LazyWriteIos;
    ULONG LazyWritePages;
    ULONG DataFlushes;
    ULONG DataPages;
    ULONG ContextSwitches;
    ULONG FirstLevelTbFills;
    ULONG SecondLevelTbFills;
    ULONG SystemCalls;
} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

/* System Information Class 0x03 */

typedef struct _SYSTEM_TIMEOFDAY_INFORMATION {
#ifdef __WINESRC__
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG uCurrentTimeZoneId;
    DWORD dwUnknown1[5];
#else
    BYTE Reserved1[48];
#endif
} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION; /* was SYSTEM_TIME_INFORMATION */

/* System Information Class 0x08 */

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER Reserved1[2];
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

/* System Information Class 0x0b */

typedef struct _SYSTEM_DRIVER_INFORMATION {
    PVOID pvAddress;
    DWORD dwUnknown1;
    DWORD dwUnknown2;
    DWORD dwEntryIndex;
    DWORD dwUnknown3;
    char szName[MAX_PATH + 1];
} SYSTEM_DRIVER_INFORMATION, *PSYSTEM_DRIVER_INFORMATION;

/* System Information Class 0x10 */

typedef struct _SYSTEM_HANDLE_ENTRY {
    ULONG  OwnerPid;
    BYTE   ObjectType;
    BYTE   HandleFlags;
    USHORT HandleValue;
    PVOID  ObjectPointer;
    ULONG  AccessMask;
} SYSTEM_HANDLE_ENTRY, *PSYSTEM_HANDLE_ENTRY;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG               Count;
    SYSTEM_HANDLE_ENTRY Handle[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

/* System Information Class 0x15 */

typedef struct _SYSTEM_CACHE_INFORMATION {
    ULONG CurrentSize;
    ULONG PeakSize;
    ULONG PageFaultCount;
    ULONG MinimumWorkingSet;
    ULONG MaximumWorkingSet;
    ULONG unused[4];
#ifdef _WIN64
    ULONG unknown64[7];
#endif
} SYSTEM_CACHE_INFORMATION, *PSYSTEM_CACHE_INFORMATION;

/* System Information Class 0x17 */

typedef struct _SYSTEM_INTERRUPT_INFORMATION {
    BYTE Reserved1[24];
} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

typedef struct _SYSTEM_CONFIGURATION_INFO {
    union {
        ULONG	OemId;
        struct {
	    WORD ProcessorArchitecture;
	    WORD Reserved;
	} tag1;
    } tag2;
    ULONG PageSize;
    PVOID MinimumApplicationAddress;
    PVOID MaximumApplicationAddress;
    ULONG ActiveProcessorMask;
    ULONG NumberOfProcessors;
    ULONG ProcessorType;
    ULONG AllocationGranularity;
    WORD  ProcessorLevel;
    WORD  ProcessorRevision;
} SYSTEM_CONFIGURATION_INFO, *PSYSTEM_CONFIGURATION_INFO;

typedef struct _SYSTEM_EXCEPTION_INFORMATION {
    BYTE Reserved1[16];
} SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;

typedef struct _SYSTEM_LOOKASIDE_INFORMATION {
    BYTE Reserved1[32];
} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
	BOOLEAN  DebuggerEnabled;
	BOOLEAN  DebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

/* System Information Class 0x05 */

typedef struct _VM_COUNTERS_ {
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG  PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
} VM_COUNTERS, *PVM_COUNTERS;

typedef struct _SYSTEM_PROCESS_INFORMATION {
#ifdef __WINESRC__                  /* win32/win64 */
    ULONG NextEntryOffset;             /* 00/00 */
    DWORD dwThreadCount;               /* 04/04 */
    DWORD dwUnknown1[6];               /* 08/08 */
    LARGE_INTEGER CreationTime;        /* 20/20 */
    LARGE_INTEGER UserTime;            /* 28/28 */
    LARGE_INTEGER KernelTime;          /* 30/30 */
    UNICODE_STRING ProcessName;        /* 38/38 */
    DWORD dwBasePriority;              /* 40/48 */
    HANDLE UniqueProcessId;            /* 44/50 */
    HANDLE ParentProcessId;            /* 48/58 */
    ULONG HandleCount;                 /* 4c/60 */
    DWORD dwUnknown3;                  /* 50/64 */
    DWORD dwUnknown4;                  /* 54/68 */
    VM_COUNTERS vmCounters;            /* 58/70 */
    IO_COUNTERS ioCounters;            /* 88/d0 */
    SYSTEM_THREAD_INFORMATION ti[1];   /* b8/100 */
#else
    ULONG NextEntryOffset;             /* 00/00 */
    BYTE Reserved1[52];                /* 04/04 */
    PVOID Reserved2[3];                /* 38/38 */
    HANDLE UniqueProcessId;            /* 44/50 */
    PVOID Reserved3;                   /* 48/58 */
    ULONG HandleCount;                 /* 4c/60 */
    BYTE Reserved4[4];                 /* 50/64 */
    PVOID Reserved5[11];               /* 54/68 */
    SIZE_T PeakPagefileUsage;          /* 80/c0 */
    SIZE_T PrivatePageCount;           /* 84/c8 */
    LARGE_INTEGER Reserved6[6];        /* 88/d0 */
#endif
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION {
    ULONG RegistryQuotaAllowed;
    ULONG RegistryQuotaUsed;
    PVOID Reserved1;
} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

typedef struct _SYSTEM_TIME_ADJUSTMENT {
    ULONG   TimeAdjustment;
    BOOLEAN TimeAdjustmentDisabled;
} SYSTEM_TIME_ADJUSTMENT, *PSYSTEM_TIME_ADJUSTMENT;

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

typedef struct _WINSTATIONINFORMATIONW {
  BYTE Reserved2[70];
  ULONG LogonId;
  BYTE Reserved3[1140];
} WINSTATIONINFORMATIONW, *PWINSTATIONINFORMATIONW;

typedef BOOLEAN (WINAPI * PWINSTATIONQUERYINFORMATIONW)(HANDLE,ULONG,WINSTATIONINFOCLASS,PVOID,ULONG,PULONG);

typedef struct _LDR_RESOURCE_INFO
{
    ULONG_PTR Type;
    ULONG_PTR Name;
    ULONG Language;
} LDR_RESOURCE_INFO, *PLDR_RESOURCE_INFO;


/* debug buffer definitions */

typedef struct _DEBUG_BUFFER {
  HANDLE SectionHandle;
  PVOID  SectionBase;
  PVOID  RemoteSectionBase;
  ULONG  SectionBaseDelta;
  HANDLE EventPairHandle;
  ULONG  Unknown[2];
  HANDLE RemoteThreadHandle;
  ULONG  InfoClassMask;
  ULONG  SizeOfInfo;
  ULONG  AllocatedSize;
  ULONG  SectionSize;
  PVOID  ModuleInformation;
  PVOID  BackTraceInformation;
  PVOID  HeapInformation;
  PVOID  LockInformation;
  PVOID  Reserved[8];
} DEBUG_BUFFER, *PDEBUG_BUFFER;

#define PDI_MODULES                       0x01
#define PDI_BACKTRACE                     0x02
#define PDI_HEAPS                         0x04
#define PDI_HEAP_TAGS                     0x08
#define PDI_HEAP_BLOCKS                   0x10
#define PDI_LOCKS                         0x20

typedef struct _DEBUG_MODULE_INFORMATION {
  ULONG  Reserved[2];
  ULONG  Base;
  ULONG  Size;
  ULONG  Flags;
  USHORT Index;
  USHORT Unknown;
  USHORT LoadCount;
  USHORT ModuleNameOffset;
  CHAR   ImageName[256];
} DEBUG_MODULE_INFORMATION, *PDEBUG_MODULE_INFORMATION;

typedef struct _DEBUG_HEAP_INFORMATION {
  ULONG  Base;
  ULONG  Flags;
  USHORT Granularity;
  USHORT Unknown;
  ULONG  Allocated;
  ULONG  Committed;
  ULONG  TagCount;
  ULONG  BlockCount;
  ULONG  Reserved[7];
  PVOID  Tags;
  PVOID  Blocks;
} DEBUG_HEAP_INFORMATION, *PDEBUG_HEAP_INFORMATION;

typedef struct _DEBUG_LOCK_INFORMATION {
  PVOID  Address;
  USHORT Type;
  USHORT CreatorBackTraceIndex;
  ULONG  OwnerThreadId;
  ULONG  ActiveCount;
  ULONG  ContentionCount;
  ULONG  EntryCount;
  ULONG  RecursionCount;
  ULONG  NumberOfSharedWaiters;
  ULONG  NumberOfExclusiveWaiters;
} DEBUG_LOCK_INFORMATION, *PDEBUG_LOCK_INFORMATION;

typedef struct _PORT_MESSAGE_HEADER {
  USHORT DataSize;
  USHORT MessageSize;
  USHORT MessageType;
  USHORT VirtualRangesOffset;
  CLIENT_ID ClientId;
  ULONG MessageId;
  ULONG SectionSize;
} PORT_MESSAGE_HEADER, *PPORT_MESSAGE_HEADER, PORT_MESSAGE, *PPORT_MESSAGE;

typedef unsigned short RTL_ATOM, *PRTL_ATOM;

/* Wine doesn't implement atom table as NT does:
 * - in NT, atom tables are user space tables, which ntdll directly accesses
 * - on Wine, (even local) atom tables are wineserver objects, hence a HANDLE
 */
typedef struct atom_table *RTL_ATOM_TABLE, **PRTL_ATOM_TABLE;

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

/* FIXME: names probably not correct */
typedef struct _RTL_HANDLE
{
    struct _RTL_HANDLE * Next;
} RTL_HANDLE;

/* FIXME: names probably not correct */
typedef struct _RTL_HANDLE_TABLE
{
    ULONG MaxHandleCount;  /* 0x00 */
    ULONG HandleSize;      /* 0x04 */
    ULONG Unused[2];       /* 0x08-0x0c */
    PVOID NextFree;        /* 0x10 */
    PVOID FirstHandle;     /* 0x14 */
    PVOID ReservedMemory;  /* 0x18 */
    PVOID MaxHandle;       /* 0x1c */
} RTL_HANDLE_TABLE;

/***********************************************************************
 * Defines
 */

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

#define INTERNAL_TS_ACTIVE_CONSOLE_ID ( *((volatile ULONG*)(0x7ffe02d8)) )

#define LOGONID_CURRENT    ((ULONG)-1)

#define OBJ_INHERIT          0x00000002
#define OBJ_PERMANENT        0x00000010
#define OBJ_EXCLUSIVE        0x00000020
#define OBJ_CASE_INSENSITIVE 0x00000040
#define OBJ_OPENIF           0x00000080
#define OBJ_OPENLINK         0x00000100
#define OBJ_KERNEL_HANDLE    0x00000200
#define OBJ_VALID_ATTRIBUTES 0x000003F2

#define SERVERNAME_CURRENT ((HANDLE)NULL)

typedef void (CALLBACK *PNTAPCFUNC)(ULONG_PTR,ULONG_PTR,ULONG_PTR); /* FIXME: not the right name */
typedef void (CALLBACK *PRTL_THREAD_START_ROUTINE)(LPVOID); /* FIXME: not the right name */
typedef DWORD (CALLBACK *PRTL_WORK_ITEM_ROUTINE)(LPVOID); /* FIXME: not the right name */
typedef void (NTAPI *RTL_WAITORTIMERCALLBACKFUNC)(PVOID,BOOLEAN); /* FIXME: not the right name */


/* DbgPrintEx default levels */
#define DPFLTR_ERROR_LEVEL     0
#define DPFLTR_WARNING_LEVEL   1
#define DPFLTR_TRACE_LEVEL     2
#define DPFLTR_INFO_LEVEL      3
#define DPFLTR_MASK    0x8000000

/* Well-known LUID values */
#define SE_MIN_WELL_KNOWN_PRIVILEGE       2
#define SE_CREATE_TOKEN_PRIVILEGE         2
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE   3
#define SE_LOCK_MEMORY_PRIVILEGE          4
#define SE_INCREASE_QUOTA_PRIVILEGE       5
#define SE_UNSOLICITED_INPUT_PRIVILEGE    6 /* obsolete */
#define SE_MACHINE_ACCOUNT_PRIVILEGE      6
#define SE_TCB_PRIVILEGE                  7
#define SE_SECURITY_PRIVILEGE             8
#define SE_TAKE_OWNERSHIP_PRIVILEGE       9
#define SE_LOAD_DRIVER_PRIVILEGE         10
#define SE_SYSTEM_PROFILE_PRIVILEGE      11
#define SE_SYSTEMTIME_PRIVILEGE          12
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE 13
#define SE_INC_BASE_PRIORITY_PRIVILEGE   14
#define SE_CREATE_PAGEFILE_PRIVILEGE     15
#define SE_CREATE_PERMANENT_PRIVILEGE    16
#define SE_BACKUP_PRIVILEGE              17
#define SE_RESTORE_PRIVILEGE             18
#define SE_SHUTDOWN_PRIVILEGE            19
#define SE_DEBUG_PRIVILEGE               20
#define SE_AUDIT_PRIVILEGE               21
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE  22
#define SE_CHANGE_NOTIFY_PRIVILEGE       23
#define SE_REMOTE_SHUTDOWN_PRIVILEGE     24
#define SE_UNDOCK_PRIVILEGE              25
#define SE_SYNC_AGENT_PRIVILEGE          26
#define SE_ENABLE_DELEGATION_PRIVILEGE   27
#define SE_MANAGE_VOLUME_PRIVILEGE       28
#define SE_IMPERSONATE_PRIVILEGE         29
#define SE_CREATE_GLOBAL_PRIVILEGE       30
#define SE_MAX_WELL_KNOWN_PRIVILEGE      SE_CREATE_GLOBAL_PRIVILEGE

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

/* Rtl*Registry* functions structs and defines */
#define RTL_REGISTRY_ABSOLUTE             0
#define RTL_REGISTRY_SERVICES             1
#define RTL_REGISTRY_CONTROL              2
#define RTL_REGISTRY_WINDOWS_NT           3
#define RTL_REGISTRY_DEVICEMAP            4
#define RTL_REGISTRY_USER                 5

#define RTL_REGISTRY_HANDLE       0x40000000
#define RTL_REGISTRY_OPTIONAL     0x80000000

#define RTL_QUERY_REGISTRY_SUBKEY         0x00000001
#define RTL_QUERY_REGISTRY_TOPKEY         0x00000002
#define RTL_QUERY_REGISTRY_REQUIRED       0x00000004
#define RTL_QUERY_REGISTRY_NOVALUE        0x00000008
#define RTL_QUERY_REGISTRY_NOEXPAND       0x00000010
#define RTL_QUERY_REGISTRY_DIRECT         0x00000020
#define RTL_QUERY_REGISTRY_DELETE         0x00000040

typedef NTSTATUS (WINAPI *PRTL_QUERY_REGISTRY_ROUTINE)( PCWSTR ValueName,
                                                        ULONG  ValueType,
                                                        PVOID  ValueData,
                                                        ULONG  ValueLength,
                                                        PVOID  Context,
                                                        PVOID  EntryContext);

typedef struct _RTL_QUERY_REGISTRY_TABLE
{
  PRTL_QUERY_REGISTRY_ROUTINE  QueryRoutine;
  ULONG  Flags;
  PWSTR  Name;
  PVOID  EntryContext;
  ULONG  DefaultType;
  PVOID  DefaultData;
  ULONG  DefaultLength;
} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;

typedef struct _KEY_MULTIPLE_VALUE_INFORMATION
{
  PUNICODE_STRING ValueName;
  ULONG DataLength;
  ULONG DataOffset;
  ULONG Type;
} KEY_MULTIPLE_VALUE_INFORMATION, *PKEY_MULTIPLE_VALUE_INFORMATION;

typedef VOID (CALLBACK *PRTL_OVERLAPPED_COMPLETION_ROUTINE)(DWORD,DWORD,LPVOID);

typedef VOID (CALLBACK *PTIMER_APC_ROUTINE) ( PVOID, ULONG, LONG );

typedef enum _EVENT_TYPE {
  NotificationEvent,
  SynchronizationEvent
} EVENT_TYPE, *PEVENT_TYPE;

typedef enum _EVENT_INFORMATION_CLASS {
  EventBasicInformation
} EVENT_INFORMATION_CLASS, *PEVENT_INFORMATION_CLASS;

typedef struct _EVENT_BASIC_INFORMATION {
  EVENT_TYPE EventType;
  LONG EventState;
} EVENT_BASIC_INFORMATION, *PEVENT_BASIC_INFORMATION;

typedef enum _SEMAPHORE_INFORMATION_CLASS {
  SemaphoreBasicInformation
} SEMAPHORE_INFORMATION_CLASS, *PSEMAPHORE_INFORMATION_CLASS;

typedef struct _SEMAPHORE_BASIC_INFORMATION {
  ULONG CurrentCount;
  ULONG MaximumCount;
} SEMAPHORE_BASIC_INFORMATION, *PSEMAPHORE_BASIC_INFORMATION;

typedef enum _SECTION_INFORMATION_CLASS
{
  SectionBasicInformation,
  SectionImageInformation,
} SECTION_INFORMATION_CLASS;

typedef struct _SECTION_BASIC_INFORMATION {
  ULONG BaseAddress;
  ULONG Attributes;
  LARGE_INTEGER Size;
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

typedef struct _SECTION_IMAGE_INFORMATION {
  PVOID EntryPoint;
  ULONG StackZeroBits;
  ULONG StackReserved;
  ULONG StackCommit;
  ULONG ImageSubsystem;
  WORD SubsystemVersionLow;
  WORD SubsystemVersionHigh;
  ULONG Unknown1;
  ULONG ImageCharacteristics;
  ULONG ImageMachineType;
  ULONG Unknown2[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

typedef struct _LPC_SECTION_WRITE {
  ULONG Length;
  HANDLE SectionHandle;
  ULONG SectionOffset;
  ULONG ViewSize;
  PVOID ViewBase;
  PVOID TargetViewBase;
} LPC_SECTION_WRITE, *PLPC_SECTION_WRITE;

typedef struct _LPC_SECTION_READ {
  ULONG Length;
  ULONG ViewSize;
  PVOID ViewBase;
} LPC_SECTION_READ, *PLPC_SECTION_READ;

typedef struct _LPC_MESSAGE {
  USHORT DataSize;
  USHORT MessageSize;
  USHORT MessageType;
  USHORT VirtualRangesOffset;
  CLIENT_ID ClientId;
  ULONG_PTR MessageId;
  ULONG_PTR SectionSize;
  UCHAR Data[ANYSIZE_ARRAY];
} LPC_MESSAGE, *PLPC_MESSAGE;

typedef enum _SHUTDOWN_ACTION {
  ShutdownNoReboot,
  ShutdownReboot,
  ShutdownPowerOff
} SHUTDOWN_ACTION, *PSHUTDOWN_ACTION;

typedef enum _KPROFILE_SOURCE {
  ProfileTime,
  ProfileAlignmentFixup,
  ProfileTotalIssues,
  ProfilePipelineDry,
  ProfileLoadInstructions,
  ProfilePipelineFrozen,
  ProfileBranchInstructions,
  ProfileTotalNonissues,
  ProfileDcacheMisses,
  ProfileIcacheMisses,
  ProfileCacheMisses,
  ProfileBranchMispredictions,
  ProfileStoreInstructions,
  ProfileFpInstructions,
  ProfileIntegerInstructions,
  Profile2Issue,
  Profile3Issue,
  Profile4Issue,
  ProfileSpecialInstructions,
  ProfileTotalCycles,
  ProfileIcacheIssues,
  ProfileDcacheAccesses,
  ProfileMemoryBarrierCycles,
  ProfileLoadLinkedIssues,
  ProfileMaximum
} KPROFILE_SOURCE, *PKPROFILE_SOURCE;

typedef struct _DIRECTORY_BASIC_INFORMATION {
  UNICODE_STRING ObjectName;
  UNICODE_STRING ObjectTypeName;
} DIRECTORY_BASIC_INFORMATION, *PDIRECTORY_BASIC_INFORMATION;

typedef struct _INITIAL_TEB {
  PVOID StackBase;
  PVOID StackLimit;
  PVOID StackCommit;
  PVOID StackCommitMax;
  PVOID StackReserved;
} INITIAL_TEB, *PINITIAL_TEB;

typedef enum _PORT_INFORMATION_CLASS {
  PortNoInformation
} PORT_INFORMATION_CLASS, *PPORT_INFORMATION_CLASS;

typedef enum _IO_COMPLETION_INFORMATION_CLASS {
  IoCompletionBasicInformation
} IO_COMPLETION_INFORMATION_CLASS, *PIO_COMPLETION_INFORMATION_CLASS;

typedef struct _FILE_COMPLETION_INFORMATION {
    HANDLE CompletionPort;
    ULONG_PTR CompletionKey;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

#define IO_COMPLETION_QUERY_STATE  0x0001
#define IO_COMPLETION_MODIFY_STATE 0x0002
#define IO_COMPLETION_ALL_ACCESS   (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)

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

typedef enum _SYSDBG_COMMAND {
  SysDbgQueryModuleInformation,
  SysDbgQueryTraceInformation,
  SysDbgSetTracepoint,
  SysDbgSetSpecialCall,
  SysDbgClearSpecialCalls,
  SysDbgQuerySpecialCalls,
  SysDbgBreakPoint,
  SysDbgQueryVersion,
  SysDbgReadVirtual,
  SysDbgWriteVirtual,
  SysDbgReadPhysical,
  SysDbgWritePhysical,
  SysDbgReadControlSpace,
  SysDbgWriteControlSpace,
  SysDbgReadIoSpace,
  SysDbgWriteIoSpace,
  SysDbgReadMsr,
  SysDbgWriteMsr,
  SysDbgReadBusData,
  SysDbgWriteBusData
} SYSDBG_COMMAND, *PSYSDBG_COMMAND;


/*************************************************************************
 * Loader structures
 *
 * Those are not part of standard Winternl.h
 */
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

/* flag for LdrAddRefDll */
#define LDR_ADDREF_DLL_PIN              0x00000001

/* FIXME: to be checked */
#define MAXIMUM_FILENAME_LENGTH 256

typedef struct _SYSTEM_MODULE
{
    PVOID               Reserved1;                      /* 00/00 */
    PVOID               Reserved2;                      /* 04/08 */
    PVOID               ImageBaseAddress;               /* 08/10 */
    ULONG               ImageSize;                      /* 0c/18 */
    ULONG               Flags;                          /* 10/1c */
    WORD                Id;                             /* 14/20 */
    WORD                Rank;                           /* 16/22 */
    WORD                Unknown;                        /* 18/24 */
    WORD                NameOffset;                     /* 1a/26 */
    BYTE                Name[MAXIMUM_FILENAME_LENGTH];  /* 1c/28 */
} SYSTEM_MODULE, *PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION
{
    ULONG               ModulesCount;
    SYSTEM_MODULE       Modules[1]; /* FIXME: should be Modules[0] */
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

/***********************************************************************
 * Function declarations
 */

#if defined(__i386__) && defined(__GNUC__)
static inline void WINAPI DbgBreakPoint(void) { __asm__ __volatile__("int3"); }
static inline void WINAPI DbgUserBreakPoint(void) { __asm__ __volatile__("int3"); }
#else  /* __i386__ && __GNUC__ */
NTSYSAPI void WINAPI DbgBreakPoint_k(void);
NTSYSAPI void WINAPI DbgUserBreakPoint_k(void);
#endif  /* __i386__ && __GNUC__ */
NTSYSAPI NTSTATUS WINAPIV DbgPrint_k(LPCSTR fmt, ...);
NTSYSAPI NTSTATUS WINAPIV DbgPrintEx_k(ULONG iComponentId, ULONG Level, LPCSTR fmt, ...);
NTSYSAPI NTSTATUS  WINAPI LdrAccessResource_k(HMODULE,const IMAGE_RESOURCE_DATA_ENTRY*,void**,PULONG);
NTSYSAPI NTSTATUS  WINAPI LdrAddRefDll_k(ULONG,HMODULE);
NTSYSAPI NTSTATUS  WINAPI LdrFindResourceDirectory_U_k(HMODULE,const LDR_RESOURCE_INFO*,ULONG,const IMAGE_RESOURCE_DIRECTORY**);
NTSYSAPI NTSTATUS  WINAPI LdrFindResource_U_k(HMODULE,const LDR_RESOURCE_INFO*,ULONG,const IMAGE_RESOURCE_DATA_ENTRY**);
NTSYSAPI NTSTATUS  WINAPI LdrDisableThreadCalloutsForDll_k(HMODULE);
NTSYSAPI NTSTATUS  WINAPI LdrFindEntryForAddress_k(const void*, PLDR_MODULE*);
NTSYSAPI NTSTATUS  WINAPI LdrGetDllHandle_k(LPCWSTR, ULONG, const UNICODE_STRING*, HMODULE*);
NTSYSAPI NTSTATUS  WINAPI LdrGetProcedureAddress_k(HMODULE, const ANSI_STRING*, ULONG, void**);
NTSYSAPI void      WINAPI LdrInitializeThunk_k(void*,ULONG_PTR,ULONG_PTR,ULONG_PTR);
NTSYSAPI NTSTATUS  WINAPI LdrLoadDll_k(LPCWSTR, DWORD, const UNICODE_STRING*, HMODULE*);
NTSYSAPI NTSTATUS  WINAPI LdrLockLoaderLock_k(ULONG,ULONG*,ULONG*);
IMAGE_BASE_RELOCATION * WINAPI LdrProcessRelocationBlock_k(void*,UINT,USHORT*,INT_PTR);
NTSYSAPI NTSTATUS  WINAPI LdrQueryImageFileExecutionOptions_k(const UNICODE_STRING*,LPCWSTR,ULONG,void*,ULONG,ULONG*);
NTSYSAPI NTSTATUS  WINAPI LdrQueryProcessModuleInformation_k(SYSTEM_MODULE_INFORMATION*, ULONG, ULONG*);
NTSYSAPI void      WINAPI LdrShutdownProcess_k(void);
NTSYSAPI void      WINAPI LdrShutdownThread_k(void);
NTSYSAPI NTSTATUS  WINAPI LdrUnloadDll_k(HMODULE);
NTSYSAPI NTSTATUS  WINAPI LdrUnlockLoaderLock_k(ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtAcceptConnectPort_k(PHANDLE,ULONG,PLPC_MESSAGE,BOOLEAN,PLPC_SECTION_WRITE,PLPC_SECTION_READ);
NTSYSAPI NTSTATUS  WINAPI NtAccessCheck_k(PSECURITY_DESCRIPTOR,HANDLE,ACCESS_MASK,PGENERIC_MAPPING,PPRIVILEGE_SET,PULONG,PULONG,NTSTATUS*);
NTSYSAPI NTSTATUS  WINAPI NtAccessCheckAndAuditAlarm_k(PUNICODE_STRING,HANDLE,PUNICODE_STRING,PUNICODE_STRING,PSECURITY_DESCRIPTOR,ACCESS_MASK,PGENERIC_MAPPING,BOOLEAN,PACCESS_MASK,PBOOLEAN,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtAddAtom_k(const WCHAR*,ULONG,RTL_ATOM*);
NTSYSAPI NTSTATUS  WINAPI NtAdjustGroupsToken_k(HANDLE,BOOLEAN,PTOKEN_GROUPS,ULONG,PTOKEN_GROUPS,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtAdjustPrivilegesToken_k(HANDLE,BOOLEAN,PTOKEN_PRIVILEGES,DWORD,PTOKEN_PRIVILEGES,PDWORD);
NTSYSAPI NTSTATUS  WINAPI NtAlertResumeThread_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtAlertThread_k(HANDLE ThreadHandle);
NTSYSAPI NTSTATUS  WINAPI NtAllocateLocallyUniqueId_k(PLUID lpLuid);
NTSYSAPI NTSTATUS  WINAPI NtAllocateUuids_k(PULARGE_INTEGER,PULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtAllocateVirtualMemory_k(HANDLE,PVOID*,ULONG,SIZE_T*,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtAreMappedFilesTheSame_k(PVOID,PVOID);
NTSYSAPI NTSTATUS  WINAPI NtAssignProcessToJobObject_k(HANDLE,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtCallbackReturn_k(PVOID,ULONG,NTSTATUS);
NTSYSAPI NTSTATUS  WINAPI NtCancelIoFile_k(HANDLE,PIO_STATUS_BLOCK);
NTSYSAPI NTSTATUS  WINAPI NtCancelIoFileEx_k(HANDLE,PIO_STATUS_BLOCK,PIO_STATUS_BLOCK);
NTSYSAPI NTSTATUS  WINAPI NtCancelTimer_k(HANDLE, BOOLEAN*);
NTSYSAPI NTSTATUS  WINAPI NtClearEvent_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtClose_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtCloseObjectAuditAlarm_k(PUNICODE_STRING,HANDLE,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtCompleteConnectPort_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtConnectPort_k(PHANDLE,PUNICODE_STRING,PSECURITY_QUALITY_OF_SERVICE,PLPC_SECTION_WRITE,PLPC_SECTION_READ,PULONG,PVOID,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtContinue_k(PCONTEXT,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtCreateDirectoryObject_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtCreateEvent_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES *,EVENT_TYPE,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtCreateEventPair_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtCreateFile_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,PLARGE_INTEGER,ULONG,ULONG,ULONG,ULONG,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateIoCompletion_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateJobObject_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtCreateKey_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*,ULONG,const UNICODE_STRING*,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateKeyedEvent_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateMailslotFile_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG,ULONG,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtCreateMutant_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtCreateNamedPipeFile_k(PHANDLE,ULONG,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtCreatePagingFile_k(PUNICODE_STRING,PLARGE_INTEGER,PLARGE_INTEGER,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtCreatePort_k(PHANDLE,POBJECT_ATTRIBUTES,ULONG,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateProcess_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,HANDLE,BOOLEAN,HANDLE,HANDLE,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtCreateProfile_k(PHANDLE,HANDLE,PVOID,ULONG,ULONG,PVOID,ULONG,KPROFILE_SOURCE,KAFFINITY);
NTSYSAPI NTSTATUS  WINAPI NtCreateSection_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*,const LARGE_INTEGER*,ULONG,ULONG,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtCreateSemaphore_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*,LONG,LONG);
NTSYSAPI NTSTATUS  WINAPI NtCreateSymbolicLinkObject_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI NtCreateThread_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,HANDLE,PCLIENT_ID,PCONTEXT,PINITIAL_TEB,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtCreateTimer_k(HANDLE*, ACCESS_MASK, const OBJECT_ATTRIBUTES*, TIMER_TYPE);
NTSYSAPI NTSTATUS  WINAPI NtCreateToken_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,TOKEN_TYPE,PLUID,PLARGE_INTEGER,PTOKEN_USER,PTOKEN_GROUPS,PTOKEN_PRIVILEGES,PTOKEN_OWNER,PTOKEN_PRIMARY_GROUP,PTOKEN_DEFAULT_DACL,PTOKEN_SOURCE);
NTSYSAPI NTSTATUS  WINAPI NtDelayExecution_k(BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtDeleteAtom_k(RTL_ATOM);
NTSYSAPI NTSTATUS  WINAPI NtDeleteFile_k(POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtDeleteKey_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtDeleteValueKey_k(HANDLE,const UNICODE_STRING *);
NTSYSAPI NTSTATUS  WINAPI NtDeviceIoControlFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,ULONG,PVOID,ULONG,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtDisplayString_k(PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI NtDuplicateObject_k(HANDLE,HANDLE,HANDLE,PHANDLE,ACCESS_MASK,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtDuplicateToken_k(HANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,SECURITY_IMPERSONATION_LEVEL,TOKEN_TYPE,PHANDLE);
NTSYSAPI NTSTATUS  WINAPI NtEnumerateKey_k(HANDLE,ULONG,KEY_INFORMATION_CLASS,void *,DWORD,DWORD *);
NTSYSAPI NTSTATUS  WINAPI NtEnumerateValueKey_k(HANDLE,ULONG,KEY_VALUE_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtExtendSection_k(HANDLE,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtFindAtom_k(const WCHAR*,ULONG,RTL_ATOM*);
NTSYSAPI NTSTATUS  WINAPI NtFlushBuffersFile_k(HANDLE,IO_STATUS_BLOCK*);
NTSYSAPI NTSTATUS  WINAPI NtFlushInstructionCache_k(HANDLE,LPCVOID,SIZE_T);
NTSYSAPI NTSTATUS  WINAPI NtFlushKey_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtFlushVirtualMemory_k(HANDLE,LPCVOID*,SIZE_T*,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtFlushWriteBuffer_k(VOID);
NTSYSAPI NTSTATUS  WINAPI NtFreeVirtualMemory_k(HANDLE,PVOID*,SIZE_T*,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtFsControlFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,ULONG,PVOID,ULONG,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtGetContextThread_k(HANDLE,CONTEXT*);
NTSYSAPI NTSTATUS  WINAPI NtGetPlugPlayEvent_k(ULONG,ULONG,PVOID,ULONG);
NTSYSAPI ULONG     WINAPI NtGetTickCount_k(VOID);
NTSYSAPI NTSTATUS  WINAPI NtGetWriteWatch_k(HANDLE,ULONG,PVOID,SIZE_T,PVOID*,ULONG_PTR*,ULONG*);
NTSYSAPI NTSTATUS  WINAPI NtImpersonateAnonymousToken_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtImpersonateClientOfPort_k(HANDLE,PPORT_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtImpersonateThread_k(HANDLE,HANDLE,PSECURITY_QUALITY_OF_SERVICE);
NTSYSAPI NTSTATUS  WINAPI NtInitializeRegistry_k(BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtInitiatePowerAction_k(POWER_ACTION,SYSTEM_POWER_STATE,ULONG,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtIsProcessInJob_k(HANDLE,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtListenPort_k(HANDLE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtLoadDriver_k(const UNICODE_STRING *);
NTSYSAPI NTSTATUS  WINAPI NtLoadKey_k(const OBJECT_ATTRIBUTES *,OBJECT_ATTRIBUTES *);
NTSYSAPI NTSTATUS  WINAPI NtLockFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,void*,PIO_STATUS_BLOCK,PLARGE_INTEGER,PLARGE_INTEGER,ULONG*,BOOLEAN,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtLockVirtualMemory_k(HANDLE,PVOID*,SIZE_T*,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtMakeTemporaryObject_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtMapViewOfSection_k(HANDLE,HANDLE,PVOID*,ULONG,SIZE_T,const LARGE_INTEGER*,SIZE_T*,SECTION_INHERIT,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtNotifyChangeDirectoryFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,ULONG,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtNotifyChangeKey_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,ULONG,BOOLEAN,PVOID,ULONG,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtOpenDirectoryObject_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtOpenEvent_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES *);
NTSYSAPI NTSTATUS  WINAPI NtOpenEventPair_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtOpenFile_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES,PIO_STATUS_BLOCK,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtOpenIoCompletion_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtOpenJobObject_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtOpenKey_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES *);
NTSYSAPI NTSTATUS  WINAPI NtOpenKeyedEvent_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtOpenMutant_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtOpenObjectAuditAlarm_k(PUNICODE_STRING,PHANDLE,PUNICODE_STRING,PUNICODE_STRING,PSECURITY_DESCRIPTOR,HANDLE,ACCESS_MASK,ACCESS_MASK,PPRIVILEGE_SET,BOOLEAN,BOOLEAN,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtOpenProcess_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*,const CLIENT_ID*);
NTSYSAPI NTSTATUS  WINAPI NtOpenProcessToken_k(HANDLE,DWORD,HANDLE *);
NTSYSAPI NTSTATUS  WINAPI NtOpenProcessTokenEx_k(HANDLE,DWORD,DWORD,HANDLE *);
NTSYSAPI NTSTATUS  WINAPI NtOpenSection_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtOpenSemaphore_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtOpenSymbolicLinkObject_k(PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtOpenThread_k(HANDLE*,ACCESS_MASK,const OBJECT_ATTRIBUTES*,const CLIENT_ID*);
NTSYSAPI NTSTATUS  WINAPI NtOpenThreadToken_k(HANDLE,DWORD,BOOLEAN,HANDLE *);
NTSYSAPI NTSTATUS  WINAPI NtOpenThreadTokenEx_k(HANDLE,DWORD,BOOLEAN,DWORD,HANDLE *);
NTSYSAPI NTSTATUS  WINAPI NtOpenTimer_k(HANDLE*, ACCESS_MASK, const OBJECT_ATTRIBUTES*);
NTSYSAPI NTSTATUS  WINAPI NtPowerInformation_k(POWER_INFORMATION_LEVEL,PVOID,ULONG,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtPrivilegeCheck_k(HANDLE,PPRIVILEGE_SET,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtPrivilegeObjectAuditAlarm_k(PUNICODE_STRING,HANDLE,HANDLE,ULONG,PPRIVILEGE_SET,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtPrivilegedServiceAuditAlarm_k(PUNICODE_STRING,PUNICODE_STRING,HANDLE,PPRIVILEGE_SET,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtProtectVirtualMemory_k(HANDLE,PVOID*,SIZE_T*,ULONG,ULONG*);
NTSYSAPI NTSTATUS  WINAPI NtPulseEvent_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueueApcThread_k(HANDLE,PNTAPCFUNC,ULONG_PTR,ULONG_PTR,ULONG_PTR);
NTSYSAPI NTSTATUS  WINAPI NtQueryAttributesFile_k(const OBJECT_ATTRIBUTES*,FILE_BASIC_INFORMATION*);
NTSYSAPI NTSTATUS  WINAPI NtQueryDefaultLocale_k(BOOLEAN,LCID*);
NTSYSAPI NTSTATUS  WINAPI NtQueryDefaultUILanguage_k(LANGID*);
NTSYSAPI NTSTATUS  WINAPI NtQueryDirectoryFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,FILE_INFORMATION_CLASS,BOOLEAN,PUNICODE_STRING,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtQueryDirectoryObject_k(HANDLE,PDIRECTORY_BASIC_INFORMATION,ULONG,BOOLEAN,BOOLEAN,PULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryEaFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,BOOLEAN,PVOID,ULONG,PULONG,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI NtQueryEvent_k(HANDLE,EVENT_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryFullAttributesFile_k(const OBJECT_ATTRIBUTES*,FILE_NETWORK_OPEN_INFORMATION*);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationAtom_k(RTL_ATOM,ATOM_INFORMATION_CLASS,PVOID,ULONG,ULONG*);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,LONG,FILE_INFORMATION_CLASS);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationJobObject_k(HANDLE,JOBOBJECTINFOCLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationPort_k(HANDLE,PORT_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationProcess_k(HANDLE,PROCESSINFOCLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationThread_k(HANDLE,THREADINFOCLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryInformationToken_k(HANDLE,TOKEN_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryInstallUILanguage_k(LANGID*);
NTSYSAPI NTSTATUS  WINAPI NtQueryIntervalProfile_k(KPROFILE_SOURCE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryIoCompletion_k(HANDLE,IO_COMPLETION_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryKey_k(HANDLE,KEY_INFORMATION_CLASS,void *,DWORD,DWORD *);
NTSYSAPI NTSTATUS  WINAPI NtQueryMultipleValueKey_k(HANDLE,PKEY_MULTIPLE_VALUE_INFORMATION,ULONG,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryMutant_k(HANDLE,MUTANT_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryObject_k(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryOpenSubKeys_k(POBJECT_ATTRIBUTES,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryPerformanceCounter_k(PLARGE_INTEGER, PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtQuerySecurityObject_k(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySection_k(HANDLE,SECTION_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySemaphore_k(HANDLE,SEMAPHORE_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySymbolicLinkObject_k(HANDLE,PUNICODE_STRING,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySystemEnvironmentValue_k(PUNICODE_STRING,PWCHAR,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySystemInformation_k(SYSTEM_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQuerySystemTime_k(PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtQueryTimer_k(HANDLE,TIMER_INFORMATION_CLASS,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryTimerResolution_k(PULONG,PULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtQueryValueKey_k(HANDLE,const UNICODE_STRING *,KEY_VALUE_INFORMATION_CLASS,void *,DWORD,DWORD *);
NTSYSAPI NTSTATUS  WINAPI NtQueryVirtualMemory_k(HANDLE,LPCVOID,MEMORY_INFORMATION_CLASS,PVOID,SIZE_T,SIZE_T*);
NTSYSAPI NTSTATUS  WINAPI NtQueryVolumeInformationFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,FS_INFORMATION_CLASS);
NTSYSAPI NTSTATUS  WINAPI NtRaiseException_k(PEXCEPTION_RECORD,PCONTEXT,BOOL);
NTSYSAPI NTSTATUS  WINAPI NtRaiseHardError_k(NTSTATUS,ULONG,PUNICODE_STRING,PVOID*,HARDERROR_RESPONSE_OPTION,PHARDERROR_RESPONSE);
NTSYSAPI NTSTATUS  WINAPI NtReadFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,PVOID,ULONG,PLARGE_INTEGER,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtReadFileScatter_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,FILE_SEGMENT_ELEMENT*,ULONG,PLARGE_INTEGER,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtReadRequestData_k(HANDLE,PLPC_MESSAGE,ULONG,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtReadVirtualMemory_k(HANDLE,const void*,void*,SIZE_T,SIZE_T*);
NTSYSAPI NTSTATUS  WINAPI NtRegisterThreadTerminatePort_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtReleaseKeyedEvent_k(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtReleaseMutant_k(HANDLE,PLONG);
NTSYSAPI NTSTATUS  WINAPI NtReleaseSemaphore_k(HANDLE,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtRemoveIoCompletion_k(HANDLE,PULONG_PTR,PULONG_PTR,PIO_STATUS_BLOCK,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtReplaceKey_k(POBJECT_ATTRIBUTES,HANDLE,POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtReplyPort_k(HANDLE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtReplyWaitReceivePort_k(HANDLE,PULONG,PLPC_MESSAGE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtReplyWaitReceivePortEx_k(HANDLE,PVOID*,PPORT_MESSAGE,PPORT_MESSAGE,PLARGE_INTEGER);
NTSYSAPI NTSTATUS  WINAPI NtReplyWaitReplyPort_k(HANDLE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtRequestPort_k(HANDLE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtRequestWaitReplyPort_k(HANDLE,PLPC_MESSAGE,PLPC_MESSAGE);
NTSYSAPI NTSTATUS  WINAPI NtResetEvent_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtResetWriteWatch_k(HANDLE,PVOID,SIZE_T);
NTSYSAPI NTSTATUS  WINAPI NtRestoreKey_k(HANDLE,HANDLE,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtResumeThread_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtSaveKey_k(HANDLE,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSecureConnectPort_k(PHANDLE,PUNICODE_STRING,PSECURITY_QUALITY_OF_SERVICE,PLPC_SECTION_WRITE,PSID,PLPC_SECTION_READ,PULONG,PVOID,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetContextThread_k(HANDLE,const CONTEXT*);
NTSYSAPI NTSTATUS  WINAPI NtSetDefaultHardErrorPort_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSetDefaultLocale_k(BOOLEAN,LCID);
NTSYSAPI NTSTATUS  WINAPI NtSetDefaultUILanguage_k(LANGID);
NTSYSAPI NTSTATUS  WINAPI NtSetEaFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetEvent_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetHighEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSetHighWaitLowEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSetHighWaitLowThread_k(VOID);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,FILE_INFORMATION_CLASS);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationJobObject_k(HANDLE,JOBOBJECTINFOCLASS,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationKey_k(HANDLE,const int,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationObject_k(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationProcess_k(HANDLE,PROCESS_INFORMATION_CLASS,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationThread_k(HANDLE,THREADINFOCLASS,LPCVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetInformationToken_k(HANDLE,TOKEN_INFORMATION_CLASS,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetIntervalProfile_k(ULONG,KPROFILE_SOURCE);
NTSYSAPI NTSTATUS  WINAPI NtSetIoCompletion_k(HANDLE,ULONG_PTR,ULONG_PTR,NTSTATUS,SIZE_T);
NTSYSAPI NTSTATUS  WINAPI NtSetLdtEntries_k(ULONG,LDT_ENTRY,ULONG,LDT_ENTRY);
NTSYSAPI NTSTATUS  WINAPI NtSetLowEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSetLowWaitHighEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSetLowWaitHighThread_k(VOID);
NTSYSAPI NTSTATUS  WINAPI NtSetSecurityObject_k(HANDLE,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);
NTSYSAPI NTSTATUS  WINAPI NtSetSystemEnvironmentValue_k(PUNICODE_STRING,PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI NtSetSystemInformation_k(SYSTEM_INFORMATION_CLASS,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetSystemPowerState_k(POWER_ACTION,SYSTEM_POWER_STATE,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetSystemTime_k(const LARGE_INTEGER*,LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtSetTimer_k(HANDLE, const LARGE_INTEGER*, PTIMER_APC_ROUTINE, PVOID, BOOLEAN, ULONG, BOOLEAN*);
NTSYSAPI NTSTATUS  WINAPI NtSetTimerResolution_k(ULONG,BOOLEAN,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetValueKey_k(HANDLE,const UNICODE_STRING *,ULONG,ULONG,const void *,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtSetVolumeInformationFile_k(HANDLE,PIO_STATUS_BLOCK,PVOID,ULONG,FS_INFORMATION_CLASS);
NTSYSAPI NTSTATUS  WINAPI NtSignalAndWaitForSingleObject_k(HANDLE,HANDLE,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtShutdownSystem_k(SHUTDOWN_ACTION);
NTSYSAPI NTSTATUS  WINAPI NtStartProfile_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtStopProfile_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtSuspendThread_k(HANDLE,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtSystemDebugControl_k(SYSDBG_COMMAND,PVOID,ULONG,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtTerminateJobObject_k(HANDLE,NTSTATUS);
NTSYSAPI NTSTATUS  WINAPI NtTerminateProcess_k(HANDLE,LONG);
NTSYSAPI NTSTATUS  WINAPI NtTerminateThread_k(HANDLE,LONG);
NTSYSAPI NTSTATUS  WINAPI NtTestAlert_k(VOID);
NTSYSAPI NTSTATUS  WINAPI NtUnloadDriver_k(const UNICODE_STRING *);
NTSYSAPI NTSTATUS  WINAPI NtUnloadKey_k(POBJECT_ATTRIBUTES);
NTSYSAPI NTSTATUS  WINAPI NtUnloadKeyEx_k(POBJECT_ATTRIBUTES,HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtUnlockFile_k(HANDLE,PIO_STATUS_BLOCK,PLARGE_INTEGER,PLARGE_INTEGER,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtUnlockVirtualMemory_k(HANDLE,PVOID*,SIZE_T*,ULONG);
NTSYSAPI NTSTATUS  WINAPI NtUnmapViewOfSection_k(HANDLE,PVOID);
NTSYSAPI NTSTATUS  WINAPI NtVdmControl_k(ULONG,PVOID);
NTSYSAPI NTSTATUS  WINAPI NtWaitForKeyedEvent_k(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtWaitForSingleObject_k(HANDLE,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtWaitForMultipleObjects_k(ULONG,const HANDLE*,BOOLEAN,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtWaitHighEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtWaitLowEventPair_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtWriteFile_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,const void*,ULONG,PLARGE_INTEGER,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtWriteFileGather_k(HANDLE,HANDLE,PIO_APC_ROUTINE,PVOID,PIO_STATUS_BLOCK,FILE_SEGMENT_ELEMENT*,ULONG,PLARGE_INTEGER,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtWriteRequestData_k(HANDLE,PLPC_MESSAGE,ULONG,PVOID,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtWriteVirtualMemory_k(HANDLE,void*,const void*,SIZE_T,SIZE_T*);
NTSYSAPI NTSTATUS  WINAPI NtYieldExecution_k(void);

NTSYSAPI void      WINAPI RtlAcquirePebLock_k(void);
NTSYSAPI BYTE      WINAPI RtlAcquireResourceExclusive_k(LPRTL_RWLOCK,BYTE);
NTSYSAPI BYTE      WINAPI RtlAcquireResourceShared_k(LPRTL_RWLOCK,BYTE);
NTSYSAPI NTSTATUS  WINAPI RtlActivateActivationContext_k(DWORD,HANDLE,ULONG_PTR*);
NTSYSAPI NTSTATUS  WINAPI RtlAddAce_k(PACL,DWORD,DWORD,PACE_HEADER,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlAddAccessAllowedAce_k(PACL,DWORD,DWORD,PSID);
NTSYSAPI NTSTATUS  WINAPI RtlAddAccessAllowedAceEx_k(PACL,DWORD,DWORD,DWORD,PSID);
NTSYSAPI NTSTATUS  WINAPI RtlAddAccessDeniedAce_k(PACL,DWORD,DWORD,PSID);
NTSYSAPI NTSTATUS  WINAPI RtlAddAccessDeniedAceEx_k(PACL,DWORD,DWORD,DWORD,PSID);
NTSYSAPI NTSTATUS  WINAPI RtlAddAtomToAtomTable_k(RTL_ATOM_TABLE,const WCHAR*,RTL_ATOM*);
NTSYSAPI NTSTATUS  WINAPI RtlAddAuditAccessAce_k(PACL,DWORD,DWORD,PSID,BOOL,BOOL);
NTSYSAPI NTSTATUS  WINAPI RtlAddAuditAccessAceEx_k(PACL,DWORD,DWORD,DWORD,PSID,BOOL,BOOL);
NTSYSAPI void      WINAPI RtlAddRefActivationContext_k(HANDLE);
NTSYSAPI PVOID     WINAPI RtlAddVectoredExceptionHandler_k(ULONG,PVECTORED_EXCEPTION_HANDLER);
NTSYSAPI NTSTATUS  WINAPI RtlAdjustPrivilege_k(ULONG,BOOLEAN,BOOLEAN,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlAllocateAndInitializeSid_k(PSID_IDENTIFIER_AUTHORITY,BYTE,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,PSID *);
NTSYSAPI RTL_HANDLE * WINAPI RtlAllocateHandle_k(RTL_HANDLE_TABLE *,ULONG *);
NTSYSAPI PVOID     WINAPI RtlAllocateHeap_k(HANDLE,ULONG,SIZE_T) __WINE_ALLOC_SIZE_k(3);
NTSYSAPI WCHAR     WINAPI RtlAnsiCharToUnicodeChar_k(LPSTR *);
NTSYSAPI DWORD     WINAPI RtlAnsiStringToUnicodeSize_k(const STRING *);
NTSYSAPI NTSTATUS  WINAPI RtlAnsiStringToUnicodeString_k(PUNICODE_STRING,PCANSI_STRING,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlAppendAsciizToString_k(STRING *,LPCSTR);
NTSYSAPI NTSTATUS  WINAPI RtlAppendStringToString_k(STRING *,const STRING *);
NTSYSAPI NTSTATUS  WINAPI RtlAppendUnicodeStringToString_k(UNICODE_STRING *,const UNICODE_STRING *);
NTSYSAPI NTSTATUS  WINAPI RtlAppendUnicodeToString_k(UNICODE_STRING *,LPCWSTR);
NTSYSAPI BOOLEAN   WINAPI RtlAreAllAccessesGranted_k(ACCESS_MASK,ACCESS_MASK);
NTSYSAPI BOOLEAN   WINAPI RtlAreAnyAccessesGranted_k(ACCESS_MASK,ACCESS_MASK);
NTSYSAPI BOOLEAN   WINAPI RtlAreBitsSet_k(PCRTL_BITMAP,ULONG,ULONG);
NTSYSAPI BOOLEAN   WINAPI RtlAreBitsClear_k(PCRTL_BITMAP,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlCharToInteger_k(PCSZ,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI RtlCheckRegistryKey_k(ULONG, PWSTR);
NTSYSAPI void      WINAPI RtlClearAllBits_k(PRTL_BITMAP);
NTSYSAPI void      WINAPI RtlClearBits_k(PRTL_BITMAP,ULONG,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlCreateActivationContext_k(HANDLE*,const void*);
NTSYSAPI PDEBUG_BUFFER WINAPI RtlCreateQueryDebugBuffer_k(ULONG,BOOLEAN);
NTSYSAPI ULONG     WINAPI RtlCompactHeap_k(HANDLE,ULONG);
NTSYSAPI LONG      WINAPI RtlCompareString_k(const STRING*,const STRING*,BOOLEAN);
NTSYSAPI LONG      WINAPI RtlCompareUnicodeString_k(const UNICODE_STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlCompressBuffer_k(USHORT,PUCHAR,ULONG,PUCHAR,ULONG,ULONG,PULONG,PVOID);
NTSYSAPI DWORD     WINAPI RtlComputeCrc32_k(DWORD,const BYTE*,INT);
NTSYSAPI NTSTATUS  WINAPI RtlConvertSidToUnicodeString_k(PUNICODE_STRING,PSID,BOOLEAN);
NTSYSAPI void      WINAPI RtlCopyLuid_k(PLUID,const LUID*);
NTSYSAPI void      WINAPI RtlCopyLuidAndAttributesArray_k(ULONG,const LUID_AND_ATTRIBUTES*,PLUID_AND_ATTRIBUTES);
NTSYSAPI BOOLEAN   WINAPI RtlCopySid_k(DWORD,PSID,PSID);
NTSYSAPI NTSTATUS  WINAPI RtlCopySecurityDescriptor_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR);
NTSYSAPI void      WINAPI RtlCopyString_k(STRING*,const STRING*);
NTSYSAPI void      WINAPI RtlCopyUnicodeString_k(UNICODE_STRING*,const UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlCreateAcl_k(PACL,DWORD,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlCreateAtomTable_k(ULONG,RTL_ATOM_TABLE*);
NTSYSAPI NTSTATUS  WINAPI RtlCreateEnvironment_k(BOOLEAN, PWSTR*);
NTSYSAPI HANDLE    WINAPI RtlCreateHeap_k(ULONG,PVOID,SIZE_T,SIZE_T,PVOID,PRTL_HEAP_DEFINITION);
NTSYSAPI NTSTATUS  WINAPI RtlCreateProcessParameters_k(RTL_USER_PROCESS_PARAMETERS**,const UNICODE_STRING*,const UNICODE_STRING*,const UNICODE_STRING*,const UNICODE_STRING*,PWSTR,const UNICODE_STRING*,const UNICODE_STRING*,const UNICODE_STRING*,const UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlCreateSecurityDescriptor_k(PSECURITY_DESCRIPTOR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlCreateTimerQueue_k(PHANDLE);
NTSYSAPI NTSTATUS  WINAPI RtlCreateTimer_k(PHANDLE, HANDLE, RTL_WAITORTIMERCALLBACKFUNC, PVOID, DWORD, DWORD, ULONG);
NTSYSAPI BOOLEAN   WINAPI RtlCreateUnicodeString_k(PUNICODE_STRING,LPCWSTR);
NTSYSAPI BOOLEAN   WINAPI RtlCreateUnicodeStringFromAsciiz_k(PUNICODE_STRING,LPCSTR);
NTSYSAPI NTSTATUS  WINAPI RtlCreateUserThread_k(HANDLE,const SECURITY_DESCRIPTOR*,BOOLEAN,PVOID,SIZE_T,SIZE_T,PRTL_THREAD_START_ROUTINE,void*,HANDLE*,CLIENT_ID*);
NTSYSAPI void      WINAPI RtlDeactivateActivationContext_k(DWORD,ULONG_PTR);
NTSYSAPI PVOID     WINAPI RtlDecodePointer_k(PVOID);
NTSYSAPI NTSTATUS  WINAPI RtlDecompressBuffer_k(USHORT,PUCHAR,ULONG,PUCHAR,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteAce_k(PACL,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteAtomFromAtomTable_k(RTL_ATOM_TABLE,RTL_ATOM);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteRegistryValue_k(ULONG, PCWSTR, PCWSTR);
NTSYSAPI void      WINAPI RtlDeleteResource_k(LPRTL_RWLOCK);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteSecurityObject_k(PSECURITY_DESCRIPTOR*);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteTimer_k(HANDLE, HANDLE, HANDLE);
NTSYSAPI NTSTATUS  WINAPI RtlDeleteTimerQueueEx_k(HANDLE, HANDLE);
NTSYSAPI PRTL_USER_PROCESS_PARAMETERS WINAPI RtlDeNormalizeProcessParams_k(RTL_USER_PROCESS_PARAMETERS*);
NTSYSAPI NTSTATUS  WINAPI RtlDeregisterWait_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI RtlDeregisterWaitEx_k(HANDLE,HANDLE);
NTSYSAPI NTSTATUS  WINAPI RtlDestroyAtomTable_k(RTL_ATOM_TABLE);
NTSYSAPI NTSTATUS  WINAPI RtlDestroyEnvironment_k(PWSTR);
NTSYSAPI NTSTATUS  WINAPI RtlDestroyHandleTable_k(RTL_HANDLE_TABLE *);
NTSYSAPI HANDLE    WINAPI RtlDestroyHeap_k(HANDLE);
NTSYSAPI void      WINAPI RtlDestroyProcessParameters_k(RTL_USER_PROCESS_PARAMETERS*);
NTSYSAPI NTSTATUS  WINAPI RtlDestroyQueryDebugBuffer_k(PDEBUG_BUFFER);
NTSYSAPI DOS_PATHNAME_TYPE WINAPI RtlDetermineDosPathNameType_U_k(PCWSTR);
NTSYSAPI BOOLEAN   WINAPI RtlDllShutdownInProgress_k(void);
NTSYSAPI BOOLEAN   WINAPI RtlDoesFileExists_U_k(LPCWSTR);
NTSYSAPI BOOLEAN   WINAPI RtlDosPathNameToNtPathName_U_k(PCWSTR,PUNICODE_STRING,PWSTR*,CURDIR*);
NTSYSAPI ULONG     WINAPI RtlDosSearchPath_U_k(LPCWSTR, LPCWSTR, LPCWSTR, ULONG, LPWSTR, LPWSTR*);
NTSYSAPI WCHAR     WINAPI RtlDowncaseUnicodeChar_k(WCHAR);
NTSYSAPI NTSTATUS  WINAPI RtlDowncaseUnicodeString_k(UNICODE_STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI void      WINAPI RtlDumpResource_k(LPRTL_RWLOCK);
NTSYSAPI NTSTATUS  WINAPI RtlDuplicateUnicodeString_k(int,const UNICODE_STRING*,UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlEmptyAtomTable_k(RTL_ATOM_TABLE,BOOLEAN);
NTSYSAPI PVOID     WINAPI RtlEncodePointer_k(PVOID);
NTSYSAPI NTSTATUS  WINAPI RtlEnterCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI void      WINAPI RtlEraseUnicodeString_k(UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlEqualComputerName_k(const UNICODE_STRING*,const UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlEqualDomainName_k(const UNICODE_STRING*,const UNICODE_STRING*);
NTSYSAPI BOOLEAN   WINAPI RtlEqualLuid_k(const LUID*,const LUID*);
NTSYSAPI BOOL      WINAPI RtlEqualPrefixSid_k(PSID,PSID);
NTSYSAPI BOOL      WINAPI RtlEqualSid_k(PSID,PSID);
NTSYSAPI BOOLEAN   WINAPI RtlEqualString_k(const STRING*,const STRING*,BOOLEAN);
NTSYSAPI BOOLEAN   WINAPI RtlEqualUnicodeString_k(const UNICODE_STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI void      DECLSPEC_NORETURN WINAPI RtlExitUserThread_k(ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlExpandEnvironmentStrings_U_k(PCWSTR, const UNICODE_STRING*, UNICODE_STRING*, ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlFindActivationContextSectionString_k(ULONG,const GUID*,ULONG,const UNICODE_STRING*,PVOID);
NTSYSAPI NTSTATUS  WINAPI RtlFindCharInUnicodeString_k(int,const UNICODE_STRING*,const UNICODE_STRING*,USHORT*);
NTSYSAPI ULONG     WINAPI RtlFindClearBits_k(PCRTL_BITMAP,ULONG,ULONG);
NTSYSAPI ULONG     WINAPI RtlFindClearBitsAndSet_k(PRTL_BITMAP,ULONG,ULONG);
NTSYSAPI ULONG     WINAPI RtlFindClearRuns_k(PCRTL_BITMAP,PRTL_BITMAP_RUN,ULONG,BOOLEAN);
NTSYSAPI ULONG     WINAPI RtlFindLastBackwardRunSet_k(PCRTL_BITMAP,ULONG,PULONG);
NTSYSAPI ULONG     WINAPI RtlFindLastBackwardRunClear_k(PCRTL_BITMAP,ULONG,PULONG);
NTSYSAPI CCHAR     WINAPI RtlFindLeastSignificantBit_k(ULONGLONG);
NTSYSAPI ULONG     WINAPI RtlFindLongestRunSet_k(PCRTL_BITMAP,PULONG);
NTSYSAPI ULONG     WINAPI RtlFindLongestRunClear_k(PCRTL_BITMAP,PULONG);
NTSYSAPI NTSTATUS  WINAPI RtlFindMessage_k(HMODULE,ULONG,ULONG,ULONG,const MESSAGE_RESOURCE_ENTRY**);
NTSYSAPI CCHAR     WINAPI RtlFindMostSignificantBit_k(ULONGLONG);
NTSYSAPI ULONG     WINAPI RtlFindNextForwardRunSet_k(PCRTL_BITMAP,ULONG,PULONG);
NTSYSAPI ULONG     WINAPI RtlFindNextForwardRunClear_k(PCRTL_BITMAP,ULONG,PULONG);
NTSYSAPI ULONG     WINAPI RtlFindSetBits_k(PCRTL_BITMAP,ULONG,ULONG);
NTSYSAPI ULONG     WINAPI RtlFindSetBitsAndClear_k(PRTL_BITMAP,ULONG,ULONG);
NTSYSAPI ULONG     WINAPI RtlFindSetRuns_k(PCRTL_BITMAP,PRTL_BITMAP_RUN,ULONG,BOOLEAN);
NTSYSAPI BOOLEAN   WINAPI RtlFirstFreeAce_k(PACL,PACE_HEADER *);
NTSYSAPI NTSTATUS  WINAPI RtlFormatCurrentUserKeyPath_k(PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI RtlFormatMessage_k(LPWSTR,UCHAR,BOOLEAN,BOOLEAN,BOOLEAN,__ms_va_list *,LPWSTR,ULONG);
NTSYSAPI void      WINAPI RtlFreeAnsiString_k(PANSI_STRING);
NTSYSAPI BOOLEAN   WINAPI RtlFreeHandle_k(RTL_HANDLE_TABLE *,RTL_HANDLE *);
NTSYSAPI BOOLEAN   WINAPI RtlFreeHeap_k(HANDLE,ULONG,PVOID);
NTSYSAPI void      WINAPI RtlFreeOemString_k(POEM_STRING);
NTSYSAPI DWORD     WINAPI RtlFreeSid_k(PSID);
NTSYSAPI void      WINAPI RtlFreeThreadActivationContextStack_k(void);
NTSYSAPI void      WINAPI RtlFreeUnicodeString_k(PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI RtlGetAce_k(PACL,DWORD,LPVOID *);
NTSYSAPI NTSTATUS  WINAPI RtlGetActiveActivationContext_k(HANDLE*);
NTSYSAPI NTSTATUS  WINAPI RtlGetCompressionWorkSpaceSize_k(USHORT,PULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI RtlGetControlSecurityDescriptor_k(PSECURITY_DESCRIPTOR, PSECURITY_DESCRIPTOR_CONTROL,LPDWORD);
NTSYSAPI NTSTATUS  WINAPI RtlGetCurrentDirectory_U_k(ULONG, LPWSTR);
NTSYSAPI PEB *     WINAPI RtlGetCurrentPeb_k(void);
NTSYSAPI NTSTATUS  WINAPI RtlGetDaclSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PBOOLEAN,PACL *,PBOOLEAN);
NTSYSAPI ULONG     WINAPI RtlGetFullPathName_U_k(PCWSTR,ULONG,PWSTR,PWSTR*);
NTSYSAPI NTSTATUS  WINAPI RtlGetGroupSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PSID *,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlGetLastNtStatus_k(void);
NTSYSAPI DWORD     WINAPI RtlGetLastWin32Error_k(void);
NTSYSAPI DWORD     WINAPI RtlGetLongestNtPathLength_k(void);
NTSYSAPI ULONG     WINAPI RtlGetNtGlobalFlags_k(void);
NTSYSAPI BOOLEAN   WINAPI RtlGetNtProductType_k(LPDWORD);
NTSYSAPI NTSTATUS  WINAPI RtlGetOwnerSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PSID *,PBOOLEAN);
NTSYSAPI ULONG     WINAPI RtlGetProcessHeaps_k(ULONG,HANDLE*);
NTSYSAPI DWORD     WINAPI RtlGetThreadErrorMode_k(void);
NTSYSAPI NTSTATUS  WINAPI RtlGetSaclSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PBOOLEAN,PACL *,PBOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlGetVersion_k(RTL_OSVERSIONINFOEXW*);
NTSYSAPI NTSTATUS  WINAPI RtlGUIDFromString_k(PUNICODE_STRING,GUID*);
NTSYSAPI PSID_IDENTIFIER_AUTHORITY WINAPI RtlIdentifierAuthoritySid_k(PSID);
NTSYSAPI PVOID     WINAPI RtlImageDirectoryEntryToData_k(HMODULE,BOOL,WORD,ULONG *);
NTSYSAPI PIMAGE_NT_HEADERS WINAPI RtlImageNtHeader_k(HMODULE);
NTSYSAPI PIMAGE_SECTION_HEADER WINAPI RtlImageRvaToSection_k(const IMAGE_NT_HEADERS *,HMODULE,DWORD);
NTSYSAPI PVOID     WINAPI RtlImageRvaToVa_k(const IMAGE_NT_HEADERS *,HMODULE,DWORD,IMAGE_SECTION_HEADER **);
NTSYSAPI NTSTATUS  WINAPI RtlImpersonateSelf_k(SECURITY_IMPERSONATION_LEVEL);
NTSYSAPI void      WINAPI RtlInitString_k(PSTRING,PCSZ);
NTSYSAPI void      WINAPI RtlInitAnsiString_k(PANSI_STRING,PCSZ);
NTSYSAPI NTSTATUS  WINAPI RtlInitAnsiStringEx_k(PANSI_STRING,PCSZ);
NTSYSAPI void      WINAPI RtlInitUnicodeString_k(PUNICODE_STRING,PCWSTR);
NTSYSAPI NTSTATUS  WINAPI RtlInitUnicodeStringEx_k(PUNICODE_STRING,PCWSTR);
NTSYSAPI NTSTATUS  WINAPI RtlInitializeCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS  WINAPI RtlInitializeCriticalSectionAndSpinCount_k(RTL_CRITICAL_SECTION *,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlInitializeCriticalSectionEx_k(RTL_CRITICAL_SECTION *,ULONG,ULONG);
NTSYSAPI void      WINAPI RtlInitializeBitMap_k(PRTL_BITMAP,PULONG,ULONG);
NTSYSAPI void      WINAPI RtlInitializeHandleTable_k(ULONG,ULONG,RTL_HANDLE_TABLE *);
NTSYSAPI void      WINAPI RtlInitializeResource_k(LPRTL_RWLOCK);
NTSYSAPI BOOL      WINAPI RtlInitializeSid_k(PSID,PSID_IDENTIFIER_AUTHORITY,BYTE);
NTSYSAPI NTSTATUS  WINAPI RtlInt64ToUnicodeString_k(ULONGLONG,ULONG,UNICODE_STRING *);
NTSYSAPI NTSTATUS  WINAPI RtlIntegerToChar_k(ULONG,ULONG,ULONG,PCHAR);
NTSYSAPI NTSTATUS  WINAPI RtlIntegerToUnicodeString_k(ULONG,ULONG,UNICODE_STRING *);
NTSYSAPI BOOLEAN   WINAPI RtlIsActivationContextActive_k(HANDLE);
NTSYSAPI ULONG     WINAPI RtlIsDosDeviceName_U_k(PCWSTR);
NTSYSAPI BOOLEAN   WINAPI RtlIsNameLegalDOS8Dot3_k(const UNICODE_STRING*,POEM_STRING,PBOOLEAN);
NTSYSAPI BOOLEAN   WINAPI RtlIsTextUnicode_k(LPCVOID,INT,INT *);
NTSYSAPI BOOLEAN   WINAPI RtlIsValidHandle_k(const RTL_HANDLE_TABLE *, const RTL_HANDLE *);
NTSYSAPI BOOLEAN   WINAPI RtlIsValidIndexHandle_k(const RTL_HANDLE_TABLE *, ULONG Index, RTL_HANDLE **);
NTSYSAPI NTSTATUS  WINAPI RtlLeaveCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI DWORD     WINAPI RtlLengthRequiredSid_k(DWORD);
NTSYSAPI ULONG     WINAPI RtlLengthSecurityDescriptor_k(PSECURITY_DESCRIPTOR);
NTSYSAPI DWORD     WINAPI RtlLengthSid_k(PSID);
NTSYSAPI NTSTATUS  WINAPI RtlLocalTimeToSystemTime_k(const LARGE_INTEGER*,PLARGE_INTEGER);
NTSYSAPI BOOLEAN   WINAPI RtlLockHeap_k(HANDLE);
NTSYSAPI NTSTATUS  WINAPI RtlLookupAtomInAtomTable_k(RTL_ATOM_TABLE,const WCHAR*,RTL_ATOM*);
NTSYSAPI NTSTATUS  WINAPI RtlMakeSelfRelativeSD_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,LPDWORD);
NTSYSAPI void      WINAPI RtlMapGenericMask_k(PACCESS_MASK,const GENERIC_MAPPING*);
NTSYSAPI NTSTATUS  WINAPI RtlMultiByteToUnicodeN_k(LPWSTR,DWORD,LPDWORD,LPCSTR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlMultiByteToUnicodeSize_k(DWORD*,LPCSTR,UINT);
NTSYSAPI NTSTATUS  WINAPI RtlNewSecurityObject_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR*,BOOLEAN,HANDLE,PGENERIC_MAPPING);
NTSYSAPI PRTL_USER_PROCESS_PARAMETERS WINAPI RtlNormalizeProcessParams_k(RTL_USER_PROCESS_PARAMETERS*);
NTSYSAPI ULONG     WINAPI RtlNtStatusToDosError_k(NTSTATUS);
NTSYSAPI ULONG     WINAPI RtlNtStatusToDosErrorNoTeb_k(NTSTATUS);
NTSYSAPI ULONG     WINAPI RtlNumberOfSetBits_k(PCRTL_BITMAP);
NTSYSAPI ULONG     WINAPI RtlNumberOfClearBits_k(PCRTL_BITMAP);
NTSYSAPI UINT      WINAPI RtlOemStringToUnicodeSize_k(const STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlOemStringToUnicodeString_k(UNICODE_STRING*,const STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlOemToUnicodeN_k(LPWSTR,DWORD,LPDWORD,LPCSTR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlOpenCurrentUser_k(ACCESS_MASK,PHANDLE);
NTSYSAPI PVOID     WINAPI RtlPcToFileHeader_k(PVOID,PVOID*);
NTSYSAPI NTSTATUS  WINAPI RtlPinAtomInAtomTable_k(RTL_ATOM_TABLE,RTL_ATOM);
NTSYSAPI BOOLEAN   WINAPI RtlPrefixString_k(const STRING*,const STRING*,BOOLEAN);
NTSYSAPI BOOLEAN   WINAPI RtlPrefixUnicodeString_k(const UNICODE_STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlQueryAtomInAtomTable_k(RTL_ATOM_TABLE,RTL_ATOM,ULONG*,ULONG*,WCHAR*,ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlQueryEnvironmentVariable_U_k(PWSTR,PUNICODE_STRING,PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI RtlQueryHeapInformation_k(HANDLE,HEAP_INFORMATION_CLASS,PVOID,SIZE_T,PSIZE_T);
NTSYSAPI NTSTATUS  WINAPI RtlQueryInformationAcl_k(PACL,LPVOID,DWORD,ACL_INFORMATION_CLASS);
NTSYSAPI NTSTATUS  WINAPI RtlQueryInformationActivationContext_k(ULONG,HANDLE,PVOID,ULONG,PVOID,SIZE_T,SIZE_T*);
NTSYSAPI NTSTATUS  WINAPI RtlQueryProcessDebugInformation_k(ULONG,ULONG,PDEBUG_BUFFER);
NTSYSAPI NTSTATUS  WINAPI RtlQueryRegistryValues_k(ULONG, PCWSTR, PRTL_QUERY_REGISTRY_TABLE, PVOID, PVOID);
NTSYSAPI NTSTATUS  WINAPI RtlQueryTimeZoneInformation_k(RTL_TIME_ZONE_INFORMATION*);
NTSYSAPI NTSTATUS  WINAPI RtlQueueWorkItem_k(PRTL_WORK_ITEM_ROUTINE,PVOID,ULONG);
NTSYSAPI void      WINAPI RtlRaiseException_k(PEXCEPTION_RECORD);
NTSYSAPI void      WINAPI RtlRaiseStatus_k(NTSTATUS);
NTSYSAPI ULONG     WINAPI RtlRandom_k(PULONG);
NTSYSAPI PVOID     WINAPI RtlReAllocateHeap_k(HANDLE,ULONG,PVOID,SIZE_T);
NTSYSAPI NTSTATUS  WINAPI RtlRegisterWait_k(PHANDLE,HANDLE,RTL_WAITORTIMERCALLBACKFUNC,PVOID,ULONG,ULONG);
NTSYSAPI void      WINAPI RtlReleaseActivationContext_k(HANDLE);
NTSYSAPI void      WINAPI RtlReleasePebLock_k(void);
NTSYSAPI void      WINAPI RtlReleaseResource_k(LPRTL_RWLOCK);
NTSYSAPI ULONG     WINAPI RtlRemoveVectoredExceptionHandler_k(PVOID);
NTSYSAPI void      WINAPI RtlRestoreLastWin32Error_k(DWORD);
NTSYSAPI void      WINAPI RtlSecondsSince1970ToTime_k(DWORD,LARGE_INTEGER *);
NTSYSAPI void      WINAPI RtlSecondsSince1980ToTime_k(DWORD,LARGE_INTEGER *);
NTSYSAPI NTSTATUS  WINAPI RtlSelfRelativeToAbsoluteSD_k(PSECURITY_DESCRIPTOR,PSECURITY_DESCRIPTOR,PDWORD,PACL,PDWORD,PACL,PDWORD,PSID,PDWORD,PSID,PDWORD);
NTSYSAPI void      WINAPI RtlSetAllBits_k(PRTL_BITMAP);
NTSYSAPI void      WINAPI RtlSetBits_k(PRTL_BITMAP,ULONG,ULONG);
NTSYSAPI ULONG     WINAPI RtlSetCriticalSectionSpinCount_k(RTL_CRITICAL_SECTION*,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlSetControlSecurityDescriptor_k(PSECURITY_DESCRIPTOR,SECURITY_DESCRIPTOR_CONTROL,SECURITY_DESCRIPTOR_CONTROL);
NTSYSAPI NTSTATUS  WINAPI RtlSetCurrentDirectory_U_k(const UNICODE_STRING*);
NTSYSAPI void      WINAPI RtlSetCurrentEnvironment_k(PWSTR, PWSTR*);
NTSYSAPI NTSTATUS  WINAPI RtlSetDaclSecurityDescriptor_k(PSECURITY_DESCRIPTOR,BOOLEAN,PACL,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlSetEnvironmentVariable_k(PWSTR*,PUNICODE_STRING,PUNICODE_STRING);
NTSYSAPI NTSTATUS  WINAPI RtlSetOwnerSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PSID,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlSetGroupSecurityDescriptor_k(PSECURITY_DESCRIPTOR,PSID,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlSetIoCompletionCallback_k(HANDLE,PRTL_OVERLAPPED_COMPLETION_ROUTINE,ULONG);
NTSYSAPI void      WINAPI RtlSetLastWin32Error_k(DWORD);
NTSYSAPI void      WINAPI RtlSetLastWin32ErrorAndNtStatusFromNtStatus_k(NTSTATUS);
NTSYSAPI NTSTATUS  WINAPI RtlSetSaclSecurityDescriptor_k(PSECURITY_DESCRIPTOR,BOOLEAN,PACL,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlSetThreadErrorMode_k(DWORD,LPDWORD);
NTSYSAPI NTSTATUS  WINAPI RtlSetTimeZoneInformation_k(const RTL_TIME_ZONE_INFORMATION*);
NTSYSAPI SIZE_T    WINAPI RtlSizeHeap_k(HANDLE,ULONG,const void*);
NTSYSAPI NTSTATUS  WINAPI RtlStringFromGUID_k(REFGUID,PUNICODE_STRING);
NTSYSAPI LPDWORD   WINAPI RtlSubAuthoritySid_k(PSID,DWORD);
NTSYSAPI LPBYTE    WINAPI RtlSubAuthorityCountSid_k(PSID);
NTSYSAPI NTSTATUS  WINAPI RtlSystemTimeToLocalTime_k(const LARGE_INTEGER*,PLARGE_INTEGER);
NTSYSAPI void      WINAPI RtlTimeToTimeFields_k(const LARGE_INTEGER*,PTIME_FIELDS);
NTSYSAPI BOOLEAN   WINAPI RtlTimeFieldsToTime_k(PTIME_FIELDS,PLARGE_INTEGER);
NTSYSAPI void      WINAPI RtlTimeToElapsedTimeFields_k(const LARGE_INTEGER *,PTIME_FIELDS);
NTSYSAPI BOOLEAN   WINAPI RtlTimeToSecondsSince1970_k(const LARGE_INTEGER *,LPDWORD);
NTSYSAPI BOOLEAN   WINAPI RtlTimeToSecondsSince1980_k(const LARGE_INTEGER *,LPDWORD);
NTSYSAPI BOOL      WINAPI RtlTryEnterCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI ULONGLONG __cdecl RtlUlonglongByteSwap_k(ULONGLONG);
NTSYSAPI DWORD     WINAPI RtlUnicodeStringToAnsiSize_k(const UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeStringToAnsiString_k(PANSI_STRING,PCUNICODE_STRING,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeStringToInteger_k(const UNICODE_STRING *,ULONG,ULONG *);
NTSYSAPI DWORD     WINAPI RtlUnicodeStringToOemSize_k(const UNICODE_STRING*);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeStringToOemString_k(POEM_STRING,PCUNICODE_STRING,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeToMultiByteN_k(LPSTR,DWORD,LPDWORD,LPCWSTR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeToMultiByteSize_k(PULONG,PCWSTR,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlUnicodeToOemN_k(LPSTR,DWORD,LPDWORD,LPCWSTR,DWORD);
NTSYSAPI ULONG     WINAPI RtlUniform_k(PULONG);
NTSYSAPI BOOLEAN   WINAPI RtlUnlockHeap_k(HANDLE);
NTSYSAPI void      WINAPI RtlUnwind_k(PVOID,PVOID,PEXCEPTION_RECORD,PVOID);
#ifdef __x86_64__
NTSYSAPI void      WINAPI RtlUnwindEx_k(PVOID,PVOID,PEXCEPTION_RECORD,PVOID,PCONTEXT,PUNWIND_HISTORY_TABLE);
#elif defined(__ia64__)
NTSYSAPI void      WINAPI RtlUnwind2_k(FRAME_POINTERS,PVOID,PEXCEPTION_RECORD,PVOID,PCONTEXT);
NTSYSAPI void      WINAPI RtlUnwindEx_k(FRAME_POINTERS,PVOID,PEXCEPTION_RECORD,PVOID,PCONTEXT,PUNWIND_HISTORY_TABLE);
#endif
NTSYSAPI WCHAR     WINAPI RtlUpcaseUnicodeChar_k(WCHAR);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeString_k(UNICODE_STRING*,const UNICODE_STRING *,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeStringToAnsiString_k(STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeStringToCountedOemString_k(STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeStringToOemString_k(STRING*,const UNICODE_STRING*,BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeToMultiByteN_k(LPSTR,DWORD,LPDWORD,LPCWSTR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlUpcaseUnicodeToOemN_k(LPSTR,DWORD,LPDWORD,LPCWSTR,DWORD);
NTSYSAPI NTSTATUS  WINAPI RtlUpdateTimer_k(HANDLE, HANDLE, DWORD, DWORD);
NTSYSAPI CHAR      WINAPI RtlUpperChar_k(CHAR);
NTSYSAPI void      WINAPI RtlUpperString_k(STRING *,const STRING *);
NTSYSAPI NTSTATUS  WINAPI RtlValidSecurityDescriptor_k(PSECURITY_DESCRIPTOR);
NTSYSAPI BOOLEAN   WINAPI RtlValidAcl_k(PACL);
NTSYSAPI BOOLEAN   WINAPI RtlValidSid_k(PSID);
NTSYSAPI BOOLEAN   WINAPI RtlValidateHeap_k(HANDLE,ULONG,LPCVOID);
NTSYSAPI NTSTATUS  WINAPI RtlVerifyVersionInfo_k(const RTL_OSVERSIONINFOEXW*,DWORD,DWORDLONG);
NTSYSAPI NTSTATUS  WINAPI RtlWalkHeap_k(HANDLE,PVOID);
NTSYSAPI NTSTATUS  WINAPI RtlWow64EnableFsRedirection_k(BOOLEAN);
NTSYSAPI NTSTATUS  WINAPI RtlWow64EnableFsRedirectionEx_k(ULONG,ULONG*);
NTSYSAPI NTSTATUS  WINAPI RtlWriteRegistryValue_k(ULONG,PCWSTR,PCWSTR,ULONG,PVOID,ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlpNtCreateKey_k(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*,ULONG,const UNICODE_STRING*,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI RtlpNtEnumerateSubKey_k(HANDLE,UNICODE_STRING *, ULONG);
NTSYSAPI NTSTATUS  WINAPI RtlpWaitForCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS  WINAPI RtlpUnWaitCriticalSection_k(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS  WINAPI vDbgPrintEx_k(ULONG,ULONG,LPCSTR,__ms_va_list);
NTSYSAPI NTSTATUS  WINAPI vDbgPrintExWithPrefix_k(LPCSTR,ULONG,ULONG,LPCSTR,__ms_va_list);

/* 32-bit only functions */

#ifndef _WIN64
NTSYSAPI LONGLONG  WINAPI RtlConvertLongToLargeInteger_k(LONG);
NTSYSAPI ULONGLONG WINAPI RtlConvertUlongToLargeInteger_k(ULONG);
NTSYSAPI LONGLONG  WINAPI RtlEnlargedIntegerMultiply_k(INT,INT);
NTSYSAPI ULONGLONG WINAPI RtlEnlargedUnsignedMultiply_k(UINT,UINT);
NTSYSAPI UINT      WINAPI RtlEnlargedUnsignedDivide_k(ULONGLONG,UINT,UINT *);
NTSYSAPI LONGLONG  WINAPI RtlExtendedMagicDivide_k(LONGLONG,LONGLONG,INT);
NTSYSAPI LONGLONG  WINAPI RtlExtendedIntegerMultiply_k(LONGLONG,INT);
NTSYSAPI LONGLONG  WINAPI RtlExtendedLargeIntegerDivide_k(LONGLONG,INT,INT *);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerAdd_k(LONGLONG,LONGLONG);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerArithmeticShift_k(LONGLONG,INT);
NTSYSAPI ULONGLONG WINAPI RtlLargeIntegerDivide_k( ULONGLONG,ULONGLONG,ULONGLONG *);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerNegate_k(LONGLONG);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerShiftLeft_k(LONGLONG,INT);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerShiftRight_k(LONGLONG,INT);
NTSYSAPI LONGLONG  WINAPI RtlLargeIntegerSubtract_k(LONGLONG,LONGLONG);
NTSYSAPI NTSTATUS  WINAPI RtlLargeIntegerToChar_k(const ULONGLONG *,ULONG,ULONG,PCHAR);
#endif

/* Wine internal functions */

NTSYSAPI NTSTATUS CDECL wine_nt_to_unix_file_name( const UNICODE_STRING *nameW, ANSI_STRING *unix_name_ret,
                                                   UINT disposition, BOOLEAN check_case );
NTSYSAPI NTSTATUS CDECL wine_unix_to_nt_file_name( const ANSI_STRING *name, UNICODE_STRING *nt );


/***********************************************************************
 * Inline functions
 */

#define InitializeObjectAttributes_k(p,n,a,r,s) \
    do { \
        (p)->Length = sizeof(OBJECT_ATTRIBUTES); \
        (p)->RootDirectory = r; \
        (p)->Attributes = a; \
        (p)->ObjectName = n; \
        (p)->SecurityDescriptor = s; \
        (p)->SecurityQualityOfService = NULL; \
    } while (0)

#define NtCurrentProcess_k() ((HANDLE)-1)

#define RtlFillMemory_k(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlMoveMemory_k(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlStoreUlong_k(p,v)  do { ULONG _v = (v); memcpy((p), &_v, sizeof(_v)); } while (0)
#define RtlStoreUlonglong_k(p,v) do { ULONGLONG _v = (v); memcpy((p), &_v, sizeof(_v)); } while (0)
#define RtlRetrieveUlong_k(p,s) memcpy((p), (s), sizeof(ULONG))
#define RtlRetrieveUlonglong_k(p,s) memcpy((p), (s), sizeof(ULONGLONG))
#define RtlZeroMemory_k(Destination,Length) memset((Destination),0,(Length))

static INLINE BOOLEAN RtlCheckBit_k(PCRTL_BITMAP lpBits, ULONG ulBit)
{
    if (lpBits && ulBit < lpBits->SizeOfBitMap &&
        lpBits->Buffer[ulBit >> 5] & (1 << (ulBit & 31)))
        return TRUE;
    return FALSE;
}

/* These are implemented as __fastcall, so we can't let Winelib apps link with them */
static INLINE USHORT RtlUshortByteSwap_k(USHORT s)
{
    return (s >> 8) | (s << 8);
}
static INLINE ULONG RtlUlongByteSwap_k(ULONG i)
{
#if defined(__i386__) && defined(__GNUC__)
    ULONG ret;
    __asm__("bswap %0" : "=r" (ret) : "0" (i) );
    return ret;
#else
    return ((ULONG)RtlUshortByteSwap_k((USHORT)i) << 16) | RtlUshortByteSwap_k((USHORT)(i >> 16));
#endif
}

/* list manipulation macros */
#define InitializeListHead(le)  (void)((le)->Flink = (le)->Blink = (le))
#define InsertHeadList(le,e)    do { PLIST_ENTRY f = (le)->Flink; (e)->Flink = f; (e)->Blink = (le); f->Blink = (e); (le)->Flink = (e); } while (0)
#define InsertTailList(le,e)    do { PLIST_ENTRY b = (le)->Blink; (e)->Flink = (le); (e)->Blink = b; b->Flink = (e); (le)->Blink = (e); } while (0)
#define IsListEmpty(le)         ((le)->Flink == (le))
#define RemoveEntryList(e)      do { PLIST_ENTRY f = (e)->Flink, b = (e)->Blink; f->Blink = b; b->Flink = f; (e)->Flink = (e)->Blink = NULL; } while (0)
static INLINE PLIST_ENTRY RemoveHeadList(PLIST_ENTRY le)
{
    PLIST_ENTRY f, b, e;

    e = le->Flink;
    f = le->Flink->Flink;
    b = le->Flink->Blink;
    f->Blink = b;
    b->Flink = f;

    if (e != le) e->Flink = e->Blink = NULL;
    return e;
}
static INLINE PLIST_ENTRY RemoveTailList(PLIST_ENTRY le)
{
    PLIST_ENTRY f, b, e;

    e = le->Blink;
    f = le->Blink->Flink;
    b = le->Blink->Blink;
    f->Blink = b;
    b->Flink = f;

    if (e != le) e->Flink = e->Blink = NULL;
    return e;
}


#ifdef __WINESRC__

/* FIXME: private structure for vm86 mode, stored in teb->GdiTebBatch */
typedef struct
{
    DWORD        dpmi_vif;
    DWORD        vm86_pending;
} WINE_VM86_TEB_INFO;

static inline WINE_VM86_TEB_INFO *get_vm86_teb_info(void)
{
    return (WINE_VM86_TEB_INFO *)&NtCurrentTeb()->GdiTebBatch;
}

/* The thread information for 16-bit threads */
/* NtCurrentTeb()->SubSystemTib points to this */
typedef struct
{
    void           *unknown;    /* 00 unknown */
    UNICODE_STRING *exe_name;   /* 04 exe module name */

    /* the following fields do not exist under Windows */
    UNICODE_STRING  exe_str;    /* exe name string pointed to by exe_name */
    CURDIR          curdir;     /* current directory */
    WCHAR           curdir_buffer[MAX_PATH];
} WIN16_SUBSYSTEM_TIB;

#endif /* __WINESRC__ */

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif  /* __WINE_WINTERNL_H */
