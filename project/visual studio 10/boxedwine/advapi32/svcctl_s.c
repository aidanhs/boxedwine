

/* this ALWAYS GENERATED file contains the RPC server stubs */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "svcctl.h"

#define TYPE_FORMAT_STRING_SIZE   433                               
#define PROC_FORMAT_STRING_SIZE   1995                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _svcctl_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } svcctl_MIDL_TYPE_FORMAT_STRING;

typedef struct _svcctl_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } svcctl_MIDL_PROC_FORMAT_STRING;

typedef struct _svcctl_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } svcctl_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const svcctl_MIDL_TYPE_FORMAT_STRING svcctl__MIDL_TypeFormatString;
extern const svcctl_MIDL_PROC_FORMAT_STRING svcctl__MIDL_ProcFormatString;
extern const svcctl_MIDL_EXPR_FORMAT_STRING svcctl__MIDL_ExprFormatString;

/* Standard interface: __MIDL_itf_svcctl_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Standard interface: svcctl, ver. 2.0,
   GUID={0x367abb81,0x9844,0x35f1,{0xad,0x32,0x98,0xf0,0x38,0x00,0x10,0x03}} */


extern const MIDL_SERVER_INFO svcctl_ServerInfo;
static const RPC_PROTSEQ_ENDPOINT __RpcProtseqEndpoint[] = 
    {
    {(unsigned char *) "ncacn_np", (unsigned char *) "\\pipe\\svcctl"}
    };


extern const RPC_DISPATCH_TABLE svcctl_v2_0_DispatchTable;

static const RPC_SERVER_INTERFACE svcctl___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x367abb81,0x9844,0x35f1,{0xad,0x32,0x98,0xf0,0x38,0x00,0x10,0x03}},{2,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&svcctl_v2_0_DispatchTable,
    1,
    (RPC_PROTSEQ_ENDPOINT *)__RpcProtseqEndpoint,
    0,
    &svcctl_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE svcctl_v2_0_s_ifspec = (RPC_IF_HANDLE)& svcctl___RpcServerInterface;

extern const MIDL_STUB_DESC svcctl_StubDesc;

extern const NDR_RUNDOWN RundownRoutines[];
extern const EXPR_EVAL ExprEvalRoutines[];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const svcctl_MIDL_PROC_FORMAT_STRING svcctl__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure svcctl_CloseServiceHandle */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 16 */	NdrFcShort( 0x38 ),	/* 56 */
/* 18 */	NdrFcShort( 0x40 ),	/* 64 */
/* 20 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 22 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter handle */

/* 30 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 32 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 34 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 36 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 38 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 40 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_ControlService */

/* 42 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 44 */	NdrFcLong( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x1 ),	/* 1 */
/* 50 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 52 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 54 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 56 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 58 */	NdrFcShort( 0x2c ),	/* 44 */
/* 60 */	NdrFcShort( 0x48 ),	/* 72 */
/* 62 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 64 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 72 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 74 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 76 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter dwControl */

/* 78 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 80 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 82 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpServiceStatus */

/* 84 */	NdrFcShort( 0x8112 ),	/* Flags:  must free, out, simple ref, srv alloc size=32 */
/* 86 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 88 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 90 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_DeleteService */

/* 96 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 98 */	NdrFcLong( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x2 ),	/* 2 */
/* 104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 106 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 108 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 110 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 112 */	NdrFcShort( 0x24 ),	/* 36 */
/* 114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 116 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 118 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 126 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 128 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 130 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Return value */

/* 132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_LockServiceDatabase */

/* 138 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x3 ),	/* 3 */
/* 146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 148 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 150 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 152 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 154 */	NdrFcShort( 0x24 ),	/* 36 */
/* 156 */	NdrFcShort( 0x40 ),	/* 64 */
/* 158 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 160 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hSCManager */

/* 168 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 170 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 172 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter phLock */

/* 174 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	NdrFcShort( 0x22 ),	/* Type Offset=34 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceObjectSecurity */

/* 186 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x4 ),	/* 4 */
/* 194 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 196 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 198 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 206 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 216 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_SetServiceObjectSecurity */


	/* Return value */

/* 220 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x5 ),	/* 5 */
/* 228 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 230 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 232 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 238 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 240 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 248 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 250 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceStatus */


	/* Return value */

/* 254 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x6 ),	/* 6 */
/* 262 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 264 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 266 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 274 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_SetServiceStatus */


	/* Return value */

/* 288 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x7 ),	/* 7 */
/* 296 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 298 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 300 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 302 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 304 */	NdrFcShort( 0x64 ),	/* 100 */
/* 306 */	NdrFcShort( 0x8 ),	/* 8 */
/* 308 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 310 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hServiceStatus */

/* 318 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 320 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 322 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter lpServiceStatus */

/* 324 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 326 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 328 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */

	/* Return value */

/* 330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 332 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_UnlockServiceDatabase */

/* 336 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x8 ),	/* 8 */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 348 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 350 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 352 */	NdrFcShort( 0x38 ),	/* 56 */
/* 354 */	NdrFcShort( 0x40 ),	/* 64 */
/* 356 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 358 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter phLock */

/* 366 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 368 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 370 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_NotifyBootConfigStatus */

/* 378 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x9 ),	/* 9 */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 390 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x8 ),	/* 8 */
/* 396 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 398 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 406 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_SCSetServiceBitsW */


	/* Return value */

/* 412 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0xa ),	/* 10 */
/* 420 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 422 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 424 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 430 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 432 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 438 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 442 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_ChangeServiceConfigW */


	/* Return value */

/* 446 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 452 */	NdrFcShort( 0xb ),	/* 11 */
/* 454 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 456 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 458 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 460 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 462 */	NdrFcShort( 0x68 ),	/* 104 */
/* 464 */	NdrFcShort( 0x24 ),	/* 36 */
/* 466 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xe,		/* 14 */
/* 468 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x1 ),	/* 1 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 476 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 478 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 480 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter dwServiceType */

/* 482 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 484 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwStartType */

/* 488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 490 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwErrorControl */

/* 494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 496 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpBinaryPathName */

/* 500 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 502 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 504 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter lpLoadOrderGroupKey */

/* 506 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 508 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 510 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter lpdwTagId */

/* 512 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 514 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 516 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter lpDependencies */

/* 518 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 520 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 522 */	NdrFcShort( 0x3a ),	/* Type Offset=58 */

	/* Parameter dwDependenciesSize */

/* 524 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 526 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpServiceStartName */

/* 530 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 532 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 534 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter lpPassword */

/* 536 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 538 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 540 */	NdrFcShort( 0x4a ),	/* Type Offset=74 */

	/* Parameter dwPasswordSize */

/* 542 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 544 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 546 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpDisplayName */

/* 548 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 550 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 552 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 556 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_CreateServiceW */

/* 560 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0xc ),	/* 12 */
/* 568 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 570 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 572 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 574 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 576 */	NdrFcShort( 0x70 ),	/* 112 */
/* 578 */	NdrFcShort( 0x5c ),	/* 92 */
/* 580 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x11,		/* 17 */
/* 582 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x1 ),	/* 1 */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hSCManager */

/* 590 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 592 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 594 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter lpServiceName */

/* 596 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 598 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 600 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter lpDisplayName */

/* 602 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 604 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 606 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter dwDesiredAccess */

/* 608 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 610 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 612 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwServiceType */

/* 614 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 616 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwStartType */

/* 620 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 622 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwErrorControl */

/* 626 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 628 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpBinaryPathName */

/* 632 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 634 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 636 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter lpLoadOrderGroup */

/* 638 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 640 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 642 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter lpdwTagId */

/* 644 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 646 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 648 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter lpDependencies */

/* 650 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 652 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 654 */	NdrFcShort( 0x4a ),	/* Type Offset=74 */

	/* Parameter dwDependenciesSize */

/* 656 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 658 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 660 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpServiceStartName */

/* 662 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 664 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 666 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter lpPassword */

/* 668 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 670 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 672 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Parameter dwPasswordSize */

/* 674 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 676 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter phService */

/* 680 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 682 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 684 */	NdrFcShort( 0x72 ),	/* Type Offset=114 */

	/* Return value */

/* 686 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 688 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumDependentServicesW */

/* 692 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 694 */	NdrFcLong( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0xd ),	/* 13 */
/* 700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 702 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 704 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 710 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 712 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 716 */	NdrFcShort( 0x0 ),	/* 0 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 720 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumServicesStatusW */


	/* Return value */

/* 726 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 732 */	NdrFcShort( 0xe ),	/* 14 */
/* 734 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 736 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 738 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 740 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 742 */	NdrFcShort( 0x3c ),	/* 60 */
/* 744 */	NdrFcShort( 0x40 ),	/* 64 */
/* 746 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x8,		/* 8 */
/* 748 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 750 */	NdrFcShort( 0x1 ),	/* 1 */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmngr */

/* 756 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 758 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 760 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter type */

/* 762 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 764 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 768 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 770 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 774 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 776 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 778 */	NdrFcShort( 0x7a ),	/* Type Offset=122 */

	/* Parameter size */

/* 780 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 782 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter needed */

/* 786 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 788 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned */

/* 792 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 794 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 800 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_OpenSCManagerW */

/* 804 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 810 */	NdrFcShort( 0xf ),	/* 15 */
/* 812 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 814 */	0x31,		/* FC_BIND_GENERIC */
			0x4,		/* 4 */
/* 816 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 818 */	0x0,		/* 0 */
			0x5c,		/* FC_PAD */
/* 820 */	NdrFcShort( 0x8 ),	/* 8 */
/* 822 */	NdrFcShort( 0x40 ),	/* 64 */
/* 824 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 826 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x0 ),	/* 0 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter MachineName */

/* 834 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 836 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 838 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter DatabaseName */

/* 840 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 842 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 844 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Parameter dwAccessMask */

/* 846 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 848 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter handle */

/* 852 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 854 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 856 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Return value */

/* 858 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 860 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 862 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_OpenServiceW */

/* 864 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 866 */	NdrFcLong( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0x10 ),	/* 16 */
/* 872 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 874 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 876 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 878 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 880 */	NdrFcShort( 0x2c ),	/* 44 */
/* 882 */	NdrFcShort( 0x40 ),	/* 64 */
/* 884 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 886 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 888 */	NdrFcShort( 0x0 ),	/* 0 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hSCManager */

/* 894 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 896 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 898 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter lpServiceName */

/* 900 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 902 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 904 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter dwDesiredAccess */

/* 906 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 908 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter phService */

/* 912 */	NdrFcShort( 0x110 ),	/* Flags:  out, simple ref, */
/* 914 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 916 */	NdrFcShort( 0x72 ),	/* Type Offset=114 */

	/* Return value */

/* 918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 920 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceConfigW */

/* 924 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x11 ),	/* 17 */
/* 932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 934 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 936 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 938 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 940 */	NdrFcShort( 0x24 ),	/* 36 */
/* 942 */	NdrFcShort( 0x8 ),	/* 8 */
/* 944 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 946 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 948 */	NdrFcShort( 0x0 ),	/* 0 */
/* 950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 954 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 956 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 958 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter config */

/* 960 */	NdrFcShort( 0xa113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=40 */
/* 962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 964 */	NdrFcShort( 0x96 ),	/* Type Offset=150 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 968 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceLockStatusW */

/* 972 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x12 ),	/* 18 */
/* 980 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 982 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 984 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x8 ),	/* 8 */
/* 990 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 992 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 998 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1002 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_StartServiceW */


	/* Return value */

/* 1006 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1014 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1016 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1018 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1020 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1022 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1024 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1026 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1028 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 1036 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1038 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1040 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter dwNumServiceArgs */

/* 1042 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1044 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpServiceArgVectors */

/* 1048 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1050 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1052 */	NdrFcShort( 0xda ),	/* Type Offset=218 */

	/* Return value */

/* 1054 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1056 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_GetServiceDisplayNameW */

/* 1060 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1062 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1066 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1068 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1070 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1072 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1074 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1076 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1078 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1080 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1082 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1084 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1088 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hSCManager */

/* 1090 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1092 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1094 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter lpServiceName */

/* 1096 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1098 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1100 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter lpBuffer */

/* 1102 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1106 */	NdrFcShort( 0xfe ),	/* Type Offset=254 */

	/* Parameter cchBufSize */

/* 1108 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1110 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1116 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_GetServiceKeyNameW */

/* 1120 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1128 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1130 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1132 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1134 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1136 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1138 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1140 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 1142 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1144 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hSCManager */

/* 1150 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1152 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1154 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter lpServiceDisplayName */

/* 1156 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1158 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1160 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter lpBuffer */

/* 1162 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1164 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1166 */	NdrFcShort( 0x10a ),	/* Type Offset=266 */

	/* Parameter cchBufSize */

/* 1168 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1170 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1176 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_SCSetServiceBitsA */

/* 1180 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1186 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1188 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1190 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1192 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1198 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1200 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1210 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_ChangeServiceConfigA */


	/* Return value */

/* 1214 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1220 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1222 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1224 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1226 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1232 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1234 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1240 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1244 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_CreateServiceA */


	/* Return value */

/* 1248 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1254 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1256 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1258 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1260 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1266 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1268 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1274 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1276 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1278 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumDependentServicesA */


	/* Return value */

/* 1282 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1284 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1288 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1290 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1292 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1294 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1300 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1302 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1310 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1312 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumServicesStatusA */


	/* Return value */

/* 1316 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1318 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1322 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1324 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1326 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1328 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1332 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1334 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1336 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1342 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1344 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1346 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_OpenSCManagerA */


	/* Return value */

/* 1350 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1352 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1358 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1360 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1362 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1368 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1370 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1378 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_OpenServiceA */


	/* Return value */

/* 1384 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1386 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1390 */	NdrFcShort( 0x1c ),	/* 28 */
/* 1392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1394 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1396 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1400 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1402 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1404 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1410 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1414 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceConfigA */


	/* Return value */

/* 1418 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x1d ),	/* 29 */
/* 1426 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1428 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1430 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1436 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1438 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1444 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1446 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1448 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceLockStatusA */


	/* Return value */

/* 1452 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1454 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1458 */	NdrFcShort( 0x1e ),	/* 30 */
/* 1460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1462 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1464 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1466 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1468 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1470 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1472 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1482 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_StartServiceA */


	/* Return value */

/* 1486 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1492 */	NdrFcShort( 0x1f ),	/* 31 */
/* 1494 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1496 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1498 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1504 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1506 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1512 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1514 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1516 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1518 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_GetServiceDisplayNameA */


	/* Return value */

/* 1520 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1522 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1526 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1528 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1530 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1532 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1538 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1540 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1546 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1550 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_GetServiceKeyNameA */


	/* Return value */

/* 1554 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1560 */	NdrFcShort( 0x21 ),	/* 33 */
/* 1562 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1564 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1566 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1568 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1572 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1574 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1578 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1580 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1582 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1584 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_GetCurrentGroupStateW */


	/* Return value */

/* 1588 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1594 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1596 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1598 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1600 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1606 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1608 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1614 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1616 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1618 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1620 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumServiceGroupW */


	/* Return value */

/* 1622 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1624 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1628 */	NdrFcShort( 0x23 ),	/* 35 */
/* 1630 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1632 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1634 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1636 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1638 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1640 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1642 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1644 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1646 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1648 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1652 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_ChangeServiceConfig2A */


	/* Return value */

/* 1656 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1662 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1664 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1666 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1668 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1670 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1672 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1674 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1676 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1678 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1684 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1686 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_ChangeServiceConfig2W */


	/* Return value */

/* 1690 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1692 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1696 */	NdrFcShort( 0x25 ),	/* 37 */
/* 1698 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1700 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1702 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1704 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1706 */	NdrFcShort( 0x2c ),	/* 44 */
/* 1708 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1710 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1712 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1716 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1718 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 1720 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1722 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1724 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter InfoLevel */

/* 1726 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1728 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter config */

/* 1732 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1734 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1736 */	NdrFcShort( 0x116 ),	/* Type Offset=278 */

	/* Return value */

/* 1738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceConfig2A */

/* 1744 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1750 */	NdrFcShort( 0x26 ),	/* 38 */
/* 1752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1754 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 1756 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1758 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1760 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1762 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1764 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1768 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1770 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter IDL_handle */

/* 1772 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1774 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1776 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceConfig2W */


	/* Return value */

/* 1778 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1780 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1784 */	NdrFcShort( 0x27 ),	/* 39 */
/* 1786 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1788 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1790 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1792 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1794 */	NdrFcShort( 0x34 ),	/* 52 */
/* 1796 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1798 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 1800 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1802 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1806 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 1808 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1810 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1812 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter InfoLevel */

/* 1814 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1816 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1818 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter lpBuffer */

/* 1820 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1822 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1824 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Parameter cbBufSize */

/* 1826 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1828 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbBytesNeeded */

/* 1832 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1834 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1838 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1840 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_QueryServiceStatusEx */

/* 1844 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1846 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1850 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1852 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1854 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1856 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1858 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1860 */	NdrFcShort( 0x32 ),	/* 50 */
/* 1862 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1864 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x6,		/* 6 */
/* 1866 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1868 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1872 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hService */

/* 1874 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1876 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1878 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter InfoLevel */

/* 1880 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1882 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1884 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter lpBuffer */

/* 1886 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1888 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1890 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Parameter cbBufSize */

/* 1892 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1894 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1896 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcbBytesNeeded */

/* 1898 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1900 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1904 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1906 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1908 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure svcctl_EnumServicesStatusExW */

/* 1910 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 1912 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1916 */	NdrFcShort( 0x29 ),	/* 41 */
/* 1918 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1920 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 1922 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1924 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1926 */	NdrFcShort( 0x3c ),	/* 60 */
/* 1928 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1930 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 1932 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1934 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1938 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hmngr */

/* 1940 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 1942 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 1944 */	NdrFcShort( 0xa ),	/* Type Offset=10 */

	/* Parameter type */

/* 1946 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1948 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter state */

/* 1952 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1954 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1956 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 1958 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 1960 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1962 */	NdrFcShort( 0x7a ),	/* Type Offset=122 */

	/* Parameter size */

/* 1964 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1966 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter needed */

/* 1970 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1972 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter returned */

/* 1976 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1978 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter group */

/* 1982 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1984 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1986 */	NdrFcShort( 0x32 ),	/* Type Offset=50 */

	/* Return value */

/* 1988 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1990 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const svcctl_MIDL_TYPE_FORMAT_STRING svcctl__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/*  8 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 10 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 12 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 14 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 16 */	NdrFcShort( 0x2 ),	/* Offset= 2 (18) */
/* 18 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 20 */	NdrFcShort( 0x1c ),	/* 28 */
/* 22 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 24 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 26 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 28 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 30 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 32 */	NdrFcShort( 0x2 ),	/* Offset= 2 (34) */
/* 34 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 36 */	0x1,		/* 1 */
			0x1,		/* 1 */
/* 38 */	
			0x11, 0x0,	/* FC_RP */
/* 40 */	NdrFcShort( 0xffea ),	/* Offset= -22 (18) */
/* 42 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 48 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 50 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 52 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 54 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 56 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 58 */	
			0x12, 0x0,	/* FC_UP */
/* 60 */	NdrFcShort( 0x2 ),	/* Offset= 2 (62) */
/* 62 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 64 */	NdrFcShort( 0x1 ),	/* 1 */
/* 66 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 68 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 70 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 72 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 74 */	
			0x12, 0x0,	/* FC_UP */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 80 */	NdrFcShort( 0x1 ),	/* 1 */
/* 82 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 84 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 86 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 88 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 90 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 92 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 94 */	
			0x12, 0x0,	/* FC_UP */
/* 96 */	NdrFcShort( 0x2 ),	/* Offset= 2 (98) */
/* 98 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 100 */	NdrFcShort( 0x1 ),	/* 1 */
/* 102 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 104 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 106 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 108 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 110 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 112 */	NdrFcShort( 0x2 ),	/* Offset= 2 (114) */
/* 114 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 116 */	0x0,		/* 0 */
			0x1,		/* 1 */
/* 118 */	
			0x11, 0x0,	/* FC_RP */
/* 120 */	NdrFcShort( 0x2 ),	/* Offset= 2 (122) */
/* 122 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 124 */	NdrFcShort( 0x1 ),	/* 1 */
/* 126 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 128 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 130 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 132 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 134 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 136 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 138 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 140 */	NdrFcShort( 0x2 ),	/* Offset= 2 (142) */
/* 142 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 144 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 146 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 148 */	NdrFcShort( 0x2 ),	/* Offset= 2 (150) */
/* 150 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 152 */	NdrFcShort( 0x24 ),	/* 36 */
/* 154 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 156 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 158 */	NdrFcShort( 0xc ),	/* 12 */
/* 160 */	NdrFcShort( 0xc ),	/* 12 */
/* 162 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 164 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 166 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 168 */	NdrFcShort( 0x10 ),	/* 16 */
/* 170 */	NdrFcShort( 0x10 ),	/* 16 */
/* 172 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 174 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 176 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 178 */	NdrFcShort( 0x18 ),	/* 24 */
/* 180 */	NdrFcShort( 0x18 ),	/* 24 */
/* 182 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 184 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 186 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 188 */	NdrFcShort( 0x1c ),	/* 28 */
/* 190 */	NdrFcShort( 0x1c ),	/* 28 */
/* 192 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 194 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 196 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 198 */	NdrFcShort( 0x20 ),	/* 32 */
/* 200 */	NdrFcShort( 0x20 ),	/* 32 */
/* 202 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 204 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 206 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 208 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 210 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 212 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 214 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 216 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 218 */	
			0x12, 0x0,	/* FC_UP */
/* 220 */	NdrFcShort( 0x2 ),	/* Offset= 2 (222) */
/* 222 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 224 */	NdrFcShort( 0x4 ),	/* 4 */
/* 226 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 228 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 230 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 232 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 234 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 236 */	NdrFcShort( 0x4 ),	/* 4 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x1 ),	/* 1 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 248 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 250 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 252 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 254 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 256 */	0x20,		/* Corr desc:  parameter,  */
			0x59,		/* FC_CALLBACK */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 262 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 264 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 266 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 268 */	0x20,		/* Corr desc:  parameter,  */
			0x59,		/* FC_CALLBACK */
/* 270 */	NdrFcShort( 0x1 ),	/* 1 */
/* 272 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 274 */	
			0x11, 0x0,	/* FC_RP */
/* 276 */	NdrFcShort( 0x2 ),	/* Offset= 2 (278) */
/* 278 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 280 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 282 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 284 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 286 */	NdrFcShort( 0x2 ),	/* Offset= 2 (288) */
/* 288 */	NdrFcShort( 0x14 ),	/* 20 */
/* 290 */	NdrFcShort( 0x3 ),	/* 3 */
/* 292 */	NdrFcLong( 0x1 ),	/* 1 */
/* 296 */	NdrFcShort( 0x10 ),	/* Offset= 16 (312) */
/* 298 */	NdrFcLong( 0x2 ),	/* 2 */
/* 302 */	NdrFcShort( 0x40 ),	/* Offset= 64 (366) */
/* 304 */	NdrFcLong( 0x7 ),	/* 7 */
/* 308 */	NdrFcShort( 0x66 ),	/* Offset= 102 (410) */
/* 310 */	NdrFcShort( 0xffff ),	/* Offset= -1 (309) */
/* 312 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 314 */	NdrFcShort( 0x4 ),	/* 4 */
/* 316 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 318 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 326 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 328 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 330 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 332 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 334 */	NdrFcShort( 0x8 ),	/* 8 */
/* 336 */	NdrFcShort( 0x0 ),	/* 0 */
/* 338 */	NdrFcShort( 0x0 ),	/* Offset= 0 (338) */
/* 340 */	0xd,		/* FC_ENUM16 */
			0x8,		/* FC_LONG */
/* 342 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 344 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 350 */	NdrFcShort( 0xc ),	/* 12 */
/* 352 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 354 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 358 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 360 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 362 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (332) */
/* 364 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 366 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 368 */	NdrFcShort( 0x14 ),	/* 20 */
/* 370 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 372 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 374 */	NdrFcShort( 0x4 ),	/* 4 */
/* 376 */	NdrFcShort( 0x4 ),	/* 4 */
/* 378 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 380 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 382 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 390 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 392 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 394 */	NdrFcShort( 0x10 ),	/* 16 */
/* 396 */	NdrFcShort( 0x10 ),	/* 16 */
/* 398 */	0x12, 0x0,	/* FC_UP */
/* 400 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (344) */
/* 402 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 404 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 406 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 408 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 410 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 412 */	NdrFcShort( 0x4 ),	/* 4 */
/* 414 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 416 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 418 */	NdrFcShort( 0x1 ),	/* 1 */
/* 420 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 424 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 426 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 428 */	
			0x11, 0x0,	/* FC_RP */
/* 430 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (416) */

			0x0
        }
    };

static const NDR_RUNDOWN RundownRoutines[] = 
    {
    SC_RPC_HANDLE_rundown
    ,SC_RPC_LOCK_rundown
    };


static void __RPC_USER svcctl_svcctl_GetServiceDisplayNameWExprEval_0000( PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        NDR_SCONTEXT hSCManager;
        LPCWSTR lpServiceName;
        WCHAR ( *lpBuffer )[  ];
        DWORD *cchBufSize;
        DWORD _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT *pS	=	( struct _PARAM_STRUCT * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( *pS->cchBufSize + 1 )  );
}

static void __RPC_USER svcctl_svcctl_GetServiceKeyNameWExprEval_0001( PMIDL_STUB_MESSAGE pStubMsg )
{
    #pragma pack(4)
    struct _PARAM_STRUCT
        {
        NDR_SCONTEXT hSCManager;
        LPCWSTR lpServiceDisplayName;
        WCHAR ( *lpBuffer )[  ];
        DWORD *cchBufSize;
        DWORD _RetVal;
        };
    #pragma pack()
    struct _PARAM_STRUCT *pS	=	( struct _PARAM_STRUCT * )pStubMsg->StackTop;
    
    pStubMsg->Offset = 0;
    pStubMsg->MaxCount = ( unsigned long ) ( ( *pS->cchBufSize + 1 )  );
}

static const EXPR_EVAL ExprEvalRoutines[] = 
    {
    svcctl_svcctl_GetServiceDisplayNameWExprEval_0000
    ,svcctl_svcctl_GetServiceKeyNameWExprEval_0001
    };


static const unsigned short svcctl_FormatStringOffsetTable[] =
    {
    0,
    42,
    96,
    138,
    186,
    220,
    254,
    288,
    336,
    378,
    412,
    446,
    560,
    692,
    726,
    804,
    864,
    924,
    972,
    1006,
    1060,
    1120,
    1180,
    1214,
    1248,
    1282,
    1316,
    1350,
    1384,
    1418,
    1452,
    1486,
    1520,
    1554,
    1588,
    1622,
    1656,
    1690,
    1744,
    1778,
    1844,
    1910
    };


static const MIDL_STUB_DESC svcctl_StubDesc = 
    {
    (void *)& svcctl___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    RundownRoutines,
    0,
    ExprEvalRoutines,
    0,
    svcctl__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static const RPC_DISPATCH_FUNCTION svcctl_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE svcctl_v2_0_DispatchTable = 
    {
    42,
    (RPC_DISPATCH_FUNCTION*)svcctl_table
    };

static const SERVER_ROUTINE svcctl_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)svcctl_CloseServiceHandle,
    (SERVER_ROUTINE)svcctl_ControlService,
    (SERVER_ROUTINE)svcctl_DeleteService,
    (SERVER_ROUTINE)svcctl_LockServiceDatabase,
    (SERVER_ROUTINE)svcctl_QueryServiceObjectSecurity,
    (SERVER_ROUTINE)svcctl_SetServiceObjectSecurity,
    (SERVER_ROUTINE)svcctl_QueryServiceStatus,
    (SERVER_ROUTINE)svcctl_SetServiceStatus,
    (SERVER_ROUTINE)svcctl_UnlockServiceDatabase,
    (SERVER_ROUTINE)svcctl_NotifyBootConfigStatus,
    (SERVER_ROUTINE)svcctl_SCSetServiceBitsW,
    (SERVER_ROUTINE)svcctl_ChangeServiceConfigW,
    (SERVER_ROUTINE)svcctl_CreateServiceW,
    (SERVER_ROUTINE)svcctl_EnumDependentServicesW,
    (SERVER_ROUTINE)svcctl_EnumServicesStatusW,
    (SERVER_ROUTINE)svcctl_OpenSCManagerW,
    (SERVER_ROUTINE)svcctl_OpenServiceW,
    (SERVER_ROUTINE)svcctl_QueryServiceConfigW,
    (SERVER_ROUTINE)svcctl_QueryServiceLockStatusW,
    (SERVER_ROUTINE)svcctl_StartServiceW,
    (SERVER_ROUTINE)svcctl_GetServiceDisplayNameW,
    (SERVER_ROUTINE)svcctl_GetServiceKeyNameW,
    (SERVER_ROUTINE)svcctl_SCSetServiceBitsA,
    (SERVER_ROUTINE)svcctl_ChangeServiceConfigA,
    (SERVER_ROUTINE)svcctl_CreateServiceA,
    (SERVER_ROUTINE)svcctl_EnumDependentServicesA,
    (SERVER_ROUTINE)svcctl_EnumServicesStatusA,
    (SERVER_ROUTINE)svcctl_OpenSCManagerA,
    (SERVER_ROUTINE)svcctl_OpenServiceA,
    (SERVER_ROUTINE)svcctl_QueryServiceConfigA,
    (SERVER_ROUTINE)svcctl_QueryServiceLockStatusA,
    (SERVER_ROUTINE)svcctl_StartServiceA,
    (SERVER_ROUTINE)svcctl_GetServiceDisplayNameA,
    (SERVER_ROUTINE)svcctl_GetServiceKeyNameA,
    (SERVER_ROUTINE)svcctl_GetCurrentGroupStateW,
    (SERVER_ROUTINE)svcctl_EnumServiceGroupW,
    (SERVER_ROUTINE)svcctl_ChangeServiceConfig2A,
    (SERVER_ROUTINE)svcctl_ChangeServiceConfig2W,
    (SERVER_ROUTINE)svcctl_QueryServiceConfig2A,
    (SERVER_ROUTINE)svcctl_QueryServiceConfig2W,
    (SERVER_ROUTINE)svcctl_QueryServiceStatusEx,
    (SERVER_ROUTINE)svcctl_EnumServicesStatusExW
    };

static const MIDL_SERVER_INFO svcctl_ServerInfo = 
    {
    &svcctl_StubDesc,
    svcctl_ServerRoutineTable,
    svcctl__MIDL_ProcFormatString.Format,
    svcctl_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

