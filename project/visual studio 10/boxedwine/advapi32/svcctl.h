

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Jul 12 13:29:17 2012
 */
/* Compiler settings for ..\..\..\..\wine\dlls\advapi32\svcctl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __svcctl_h__
#define __svcctl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_svcctl_0000_0000 */
/* [local] */ 

#include "winsvc.h"
#define SVCCTL_TRANSPORT {'n','c','a','c','n','_','n','p',0}
#define SVCCTL_ENDPOINT {'\\','p','i','p','e','\\','s','v','c','c','t','l',0}
#define SVCCTL_STARTED_EVENT {'_','_','w','i','n','e','_','S','v','c','c','t','l','S','t','a','r','t','e','d',0}

enum service_pipe_command
    {	WINESERV_STARTINFO	= 1,
	WINESERV_SENDCONTROL	= 2
    } ;
typedef struct service_start_info_t
    {
    enum service_pipe_command cmd;
    DWORD total_size;
    DWORD name_size;
    DWORD control;
    WCHAR data[ 1 ];
    } 	service_start_info;



extern RPC_IF_HANDLE __MIDL_itf_svcctl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_svcctl_0000_0000_v0_0_s_ifspec;

#ifndef __svcctl_INTERFACE_DEFINED__
#define __svcctl_INTERFACE_DEFINED__

/* interface svcctl */
/* [endpoint][unique][version][uuid] */ 

typedef /* [handle] */ LPCWSTR MACHINE_HANDLEW;

typedef /* [context_handle] */ void *SC_RPC_HANDLE;

typedef /* [context_handle] */ void *SC_RPC_LOCK;

#define SERVICE_SET_STATUS   0x8000
#if 0 /* already defined in winsvc.h */
typedef struct _QUERY_SERVICE_CONFIGW
    {
    DWORD dwServiceType;
    DWORD dwStartType;
    DWORD dwErrorControl;
    LPWSTR lpBinaryPathName;
    LPWSTR lpLoadOrderGroup;
    DWORD dwTagId;
    LPWSTR lpDependencies;
    LPWSTR lpServiceStartName;
    LPWSTR lpDisplayName;
    } 	QUERY_SERVICE_CONFIGW;

typedef struct _QUERY_SERVICE_CONFIGW *LPQUERY_SERVICE_CONFIGW;

typedef struct _SERVICE_STATUS
    {
    DWORD dwServiceType;
    DWORD dwCurrentState;
    DWORD dwControlsAccepted;
    DWORD dwWin32ExitCode;
    DWORD dwServiceSpecificExitCode;
    DWORD dwCheckPoint;
    DWORD dwWaitHint;
    } 	SERVICE_STATUS;

typedef struct _SERVICE_STATUS *LPSERVICE_STATUS;

typedef 
enum _SC_STATUS_TYPE
    {	SC_STATUS_PROCESS_INFO	= 0
    } 	SC_STATUS_TYPE;

typedef struct _SERVICE_DESCRIPTIONW
    {
    LPWSTR lpDescription;
    } 	SERVICE_DESCRIPTIONW;

typedef struct _SERVICE_DESCRIPTIONW *LPSERVICE_DESCRIPTIONW;

typedef 
enum _SC_ACTION_TYPE
    {	SC_ACTION_NONE	= 0,
	SC_ACTION_RESTART	= 1,
	SC_ACTION_REBOOT	= 2,
	SC_ACTION_RUN_COMMAND	= 3
    } 	SC_ACTION_TYPE;

typedef struct _SC_ACTION
    {
    SC_ACTION_TYPE Type;
    DWORD Delay;
    } 	SC_ACTION;

typedef struct _SC_ACTION *LPSC_ACTION;

typedef struct _SERVICE_FAILURE_ACTIONSW
    {
    DWORD dwResetPeriod;
    LPWSTR lpRebootMsg;
    LPWSTR lpCommand;
    DWORD cActions;
    SC_ACTION *lpsaActions;
    } 	SERVICE_FAILURE_ACTIONSW;

typedef struct _SERVICE_FAILURE_ACTIONSW *LPSERVICE_FAILURE_ACTIONSW;

typedef struct _SERVICE_PRESHUTDOWN_INFO
    {
    DWORD dwPreshutdownTimeout;
    } 	SERVICE_PRESHUTDOWN_INFO;

typedef struct _SERVICE_PRESHUTDOWN_INFO *LPSERVICE_PRESHUTDOWN_INFO;

typedef struct _ENUM_SERVICE_STATUSW
    {
    LPWSTR lpServiceName;
    LPWSTR lpDisplayName;
    SERVICE_STATUS ServiceStatus;
    } 	ENUM_SERVICE_STATUSW;

typedef struct _ENUM_SERVICE_STATUSW *LPENUM_SERVICE_STATUSW;

#endif
typedef /* [public][public][switch_type] */ union __MIDL_svcctl_0001
    {
    SERVICE_DESCRIPTIONW descr;
    SERVICE_FAILURE_ACTIONSW actions;
    SERVICE_PRESHUTDOWN_INFO preshutdown;
    } 	SERVICE_CONFIG2W;

DWORD svcctl_CloseServiceHandle( 
    /* [out][in] */ SC_RPC_HANDLE *handle);

DWORD svcctl_ControlService( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ DWORD dwControl,
    /* [out] */ SERVICE_STATUS *lpServiceStatus);

DWORD svcctl_DeleteService( 
    /* [in] */ SC_RPC_HANDLE hService);

DWORD svcctl_LockServiceDatabase( 
    /* [in] */ SC_RPC_HANDLE hSCManager,
    /* [out] */ SC_RPC_LOCK *phLock);

DWORD svcctl_QueryServiceObjectSecurity( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_SetServiceObjectSecurity( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_QueryServiceStatus( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_SetServiceStatus( 
    /* [in] */ SC_RPC_HANDLE hServiceStatus,
    /* [in] */ LPSERVICE_STATUS lpServiceStatus);

DWORD svcctl_UnlockServiceDatabase( 
    /* [out][in] */ SC_RPC_LOCK *phLock);

DWORD svcctl_NotifyBootConfigStatus( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_SCSetServiceBitsW( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_ChangeServiceConfigW( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ DWORD dwServiceType,
    /* [in] */ DWORD dwStartType,
    /* [in] */ DWORD dwErrorControl,
    /* [unique][in] */ LPCWSTR lpBinaryPathName,
    /* [unique][in] */ LPCWSTR lpLoadOrderGroupKey,
    /* [unique][out][in] */ DWORD *lpdwTagId,
    /* [size_is][unique][in] */ const BYTE *lpDependencies,
    /* [in] */ DWORD dwDependenciesSize,
    /* [unique][in] */ LPCWSTR lpServiceStartName,
    /* [size_is][unique][in] */ const BYTE *lpPassword,
    /* [in] */ DWORD dwPasswordSize,
    /* [unique][in] */ LPCWSTR lpDisplayName);

DWORD svcctl_CreateServiceW( 
    /* [in] */ SC_RPC_HANDLE hSCManager,
    /* [in] */ LPCWSTR lpServiceName,
    /* [unique][in] */ LPCWSTR lpDisplayName,
    /* [in] */ DWORD dwDesiredAccess,
    /* [in] */ DWORD dwServiceType,
    /* [in] */ DWORD dwStartType,
    /* [in] */ DWORD dwErrorControl,
    /* [in] */ LPCWSTR lpBinaryPathName,
    /* [unique][in] */ LPCWSTR lpLoadOrderGroup,
    /* [unique][out][in] */ DWORD *lpdwTagId,
    /* [size_is][unique][in] */ const BYTE *lpDependencies,
    /* [in] */ DWORD dwDependenciesSize,
    /* [unique][in] */ LPCWSTR lpServiceStartName,
    /* [size_is][unique][in] */ const BYTE *lpPassword,
    /* [in] */ DWORD dwPasswordSize,
    /* [out] */ SC_RPC_HANDLE *phService);

DWORD svcctl_EnumDependentServicesW( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_EnumServicesStatusW( 
    /* [in] */ SC_RPC_HANDLE hmngr,
    /* [in] */ DWORD type,
    /* [in] */ DWORD state,
    /* [size_is][out] */ BYTE *buffer,
    /* [in] */ DWORD size,
    /* [out] */ LPDWORD needed,
    /* [out] */ LPDWORD returned);

DWORD svcctl_OpenSCManagerW( 
    /* [unique][in] */ MACHINE_HANDLEW MachineName,
    /* [unique][in] */ LPCWSTR DatabaseName,
    /* [in] */ DWORD dwAccessMask,
    /* [out] */ SC_RPC_HANDLE *handle);

DWORD svcctl_OpenServiceW( 
    /* [in] */ SC_RPC_HANDLE hSCManager,
    /* [in] */ LPCWSTR lpServiceName,
    /* [in] */ DWORD dwDesiredAccess,
    /* [out] */ SC_RPC_HANDLE *phService);

DWORD svcctl_QueryServiceConfigW( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [out] */ QUERY_SERVICE_CONFIGW *config);

DWORD svcctl_QueryServiceLockStatusW( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_StartServiceW( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ DWORD dwNumServiceArgs,
    /* [size_is][unique][in] */ LPCWSTR *lpServiceArgVectors);

DWORD svcctl_GetServiceDisplayNameW( 
    /* [in] */ SC_RPC_HANDLE hSCManager,
    /* [in] */ LPCWSTR lpServiceName,
    /* [size_is][string][out] */ WCHAR lpBuffer[  ],
    /* [out][in] */ DWORD *cchBufSize);

DWORD svcctl_GetServiceKeyNameW( 
    /* [in] */ SC_RPC_HANDLE hSCManager,
    /* [in] */ LPCWSTR lpServiceDisplayName,
    /* [size_is][string][out] */ WCHAR lpBuffer[  ],
    /* [out][in] */ DWORD *cchBufSize);

DWORD svcctl_SCSetServiceBitsA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_ChangeServiceConfigA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_CreateServiceA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_EnumDependentServicesA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_EnumServicesStatusA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_OpenSCManagerA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_OpenServiceA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_QueryServiceConfigA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_QueryServiceLockStatusA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_StartServiceA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_GetServiceDisplayNameA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_GetServiceKeyNameA( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_GetCurrentGroupStateW( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_EnumServiceGroupW( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_ChangeServiceConfig2A( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_ChangeServiceConfig2W( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ DWORD InfoLevel,
    /* [switch_is][in] */ SERVICE_CONFIG2W *config);

DWORD svcctl_QueryServiceConfig2A( 
    /* [in] */ handle_t IDL_handle);

DWORD svcctl_QueryServiceConfig2W( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ DWORD InfoLevel,
    /* [size_is][out] */ BYTE lpBuffer[  ],
    /* [in] */ DWORD cbBufSize,
    /* [out] */ LPDWORD pcbBytesNeeded);

DWORD svcctl_QueryServiceStatusEx( 
    /* [in] */ SC_RPC_HANDLE hService,
    /* [in] */ SC_STATUS_TYPE InfoLevel,
    /* [size_is][out] */ BYTE *lpBuffer,
    /* [in] */ DWORD cbBufSize,
    /* [out] */ LPDWORD pcbBytesNeeded);

DWORD svcctl_EnumServicesStatusExW( 
    /* [in] */ SC_RPC_HANDLE hmngr,
    /* [in] */ DWORD type,
    /* [in] */ DWORD state,
    /* [size_is][out] */ BYTE *buffer,
    /* [in] */ DWORD size,
    /* [out] */ LPDWORD needed,
    /* [out] */ LPDWORD returned,
    /* [unique][in] */ LPCWSTR group);



extern RPC_IF_HANDLE svcctl_v2_0_c_ifspec;
extern RPC_IF_HANDLE svcctl_v2_0_s_ifspec;
#endif /* __svcctl_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

handle_t __RPC_USER MACHINE_HANDLEW_bind ( MACHINE_HANDLEW );
void     __RPC_USER MACHINE_HANDLEW_unbind( MACHINE_HANDLEW,  handle_t );

void __RPC_USER SC_RPC_HANDLE_rundown( SC_RPC_HANDLE );
void __RPC_USER SC_RPC_LOCK_rundown( SC_RPC_LOCK );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


