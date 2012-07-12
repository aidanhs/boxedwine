

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri Jun 01 15:18:49 2012
 */
/* Compiler settings for ..\..\..\..\wine\dlls\rpcrt4\epm.idl:
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
#include "epm.h"

#define TYPE_FORMAT_STRING_SIZE   197                               
#define PROC_FORMAT_STRING_SIZE   383                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _epm_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } epm_MIDL_TYPE_FORMAT_STRING;

typedef struct _epm_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } epm_MIDL_PROC_FORMAT_STRING;

typedef struct _epm_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } epm_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const epm_MIDL_TYPE_FORMAT_STRING epm__MIDL_TypeFormatString;
extern const epm_MIDL_PROC_FORMAT_STRING epm__MIDL_ProcFormatString;
extern const epm_MIDL_EXPR_FORMAT_STRING epm__MIDL_ExprFormatString;

/* Standard interface: __MIDL_itf_epm_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Standard interface: epm, ver. 3.0,
   GUID={0xe1af8308,0x5d1f,0x11c9,{0x91,0xa4,0x08,0x00,0x2b,0x14,0xa0,0xfa}} */


extern const MIDL_SERVER_INFO epm_ServerInfo;

extern const RPC_DISPATCH_TABLE epm_v3_0_DispatchTable;

static const RPC_SERVER_INTERFACE epm___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0xe1af8308,0x5d1f,0x11c9,{0x91,0xa4,0x08,0x00,0x2b,0x14,0xa0,0xfa}},{3,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&epm_v3_0_DispatchTable,
    0,
    0,
    0,
    &epm_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE epm_v3_0_s_ifspec = (RPC_IF_HANDLE)& epm___RpcServerInterface;

extern const MIDL_STUB_DESC epm_StubDesc;

extern const NDR_RUNDOWN RundownRoutines[];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const epm_MIDL_PROC_FORMAT_STRING epm__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure ept_insert */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x10 ),	/* 16 */
/* 16 */	NdrFcShort( 0x1c ),	/* 28 */
/* 18 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x4,		/* 4 */
/* 20 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x1 ),	/* 1 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 28 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter num_ents */

/* 34 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	NdrFcShort( 0x42 ),	/* Type Offset=66 */

	/* Parameter entries */

/* 40 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 42 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 44 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter replace */

/* 46 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 48 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 50 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_delete */


	/* Parameter status */

/* 52 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 54 */	NdrFcLong( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x1 ),	/* 1 */
/* 60 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 62 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 64 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 66 */	NdrFcShort( 0x8 ),	/* 8 */
/* 68 */	NdrFcShort( 0x1c ),	/* 28 */
/* 70 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x3,		/* 3 */
/* 72 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x1 ),	/* 1 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 80 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 82 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 84 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter num_ents */

/* 86 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 88 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 90 */	NdrFcShort( 0x42 ),	/* Type Offset=66 */

	/* Parameter entries */

/* 92 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 94 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 96 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_lookup */


	/* Parameter status */

/* 98 */	0x0,		/* 0 */
			0x49,		/* Old Flags:  full ptr, */
/* 100 */	NdrFcLong( 0x1 ),	/* 1 */
/* 104 */	NdrFcShort( 0x2 ),	/* 2 */
/* 106 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 108 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 110 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 112 */	NdrFcShort( 0xec ),	/* 236 */
/* 114 */	NdrFcShort( 0x70 ),	/* 112 */
/* 116 */	0x41,		/* Oi2 Flags:  srv must size, has ext, */
			0x9,		/* 9 */
/* 118 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 120 */	NdrFcShort( 0x1 ),	/* 1 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter inquiry_type */

/* 132 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 134 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 136 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter object */

/* 138 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 140 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 142 */	NdrFcShort( 0x60 ),	/* Type Offset=96 */

	/* Parameter interface_id */

/* 144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 146 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter vers_option */

/* 150 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 152 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 154 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter entry_handle */

/* 156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 158 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter max_ents */

/* 162 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 164 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter num_ents */

/* 168 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 170 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 172 */	NdrFcShort( 0x7c ),	/* Type Offset=124 */

	/* Parameter entries */

/* 174 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 176 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 178 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_map */


	/* Parameter status */

/* 180 */	0x0,		/* 0 */
			0x49,		/* Old Flags:  full ptr, */
/* 182 */	NdrFcLong( 0x1 ),	/* 1 */
/* 186 */	NdrFcShort( 0x3 ),	/* 3 */
/* 188 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 190 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 192 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 194 */	NdrFcShort( 0x84 ),	/* 132 */
/* 196 */	NdrFcShort( 0x70 ),	/* 112 */
/* 198 */	0x43,		/* Oi2 Flags:  srv must size, clt must size, has ext, */
			0x7,		/* 7 */
/* 200 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 202 */	NdrFcShort( 0x1 ),	/* 1 */
/* 204 */	NdrFcShort( 0x1 ),	/* 1 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 208 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 210 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 212 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter object */

/* 214 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 216 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 218 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter map_tower */

/* 220 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 222 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 224 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter entry_handle */

/* 226 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 228 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 230 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter max_towers */

/* 232 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 234 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter num_towers */

/* 238 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 240 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 242 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Parameter towers */

/* 244 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 246 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 248 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_lookup_handle_free */


	/* Parameter status */

/* 250 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 252 */	NdrFcLong( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x4 ),	/* 4 */
/* 258 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 260 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 262 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 264 */	NdrFcShort( 0x38 ),	/* 56 */
/* 266 */	NdrFcShort( 0x54 ),	/* 84 */
/* 268 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 270 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 278 */	NdrFcShort( 0x118 ),	/* Flags:  in, out, simple ref, */
/* 280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 282 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Parameter entry_handle */

/* 284 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 286 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 288 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_inq_object */


	/* Parameter status */

/* 290 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 292 */	NdrFcLong( 0x1 ),	/* 1 */
/* 296 */	NdrFcShort( 0x5 ),	/* 5 */
/* 298 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 300 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 302 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 306 */	NdrFcShort( 0x60 ),	/* 96 */
/* 308 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 310 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 318 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 320 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 322 */	NdrFcShort( 0x8 ),	/* Type Offset=8 */

	/* Parameter ept_object */

/* 324 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 328 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

	/* Procedure ept_mgmt_delete */


	/* Parameter status */

/* 330 */	0x0,		/* 0 */
			0x49,		/* Old Flags:  full ptr, */
/* 332 */	NdrFcLong( 0x0 ),	/* 0 */
/* 336 */	NdrFcShort( 0x6 ),	/* 6 */
/* 338 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 340 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 342 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 344 */	NdrFcShort( 0x4c ),	/* 76 */
/* 346 */	NdrFcShort( 0x1c ),	/* 28 */
/* 348 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x4,		/* 4 */
/* 350 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x1 ),	/* 1 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter h */

/* 358 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 360 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 362 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter object_speced */

/* 364 */	NdrFcShort( 0xa ),	/* Flags:  must free, in, */
/* 366 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 368 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */

	/* Parameter object */

/* 370 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 372 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 374 */	NdrFcShort( 0x92 ),	/* Type Offset=146 */

	/* Parameter tower */

/* 376 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 378 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 380 */	0x10,		/* FC_ERROR_STATUS_T */
			0x0,		/* 0 */

			0x0
        }
    };

static const epm_MIDL_TYPE_FORMAT_STRING epm__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/*  4 */	NdrFcShort( 0x8 ),	/* 8 */
/*  6 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/*  8 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 10 */	NdrFcShort( 0x10 ),	/* 16 */
/* 12 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 14 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 16 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (2) */
			0x5b,		/* FC_END */
/* 20 */	
			0x26,		/* FC_CSTRING */
			0x5c,		/* FC_PAD */
/* 22 */	NdrFcShort( 0x40 ),	/* 64 */
/* 24 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 26 */	NdrFcShort( 0x1 ),	/* 1 */
/* 28 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 30 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 32 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 34 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 36 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 38 */	NdrFcShort( 0x4 ),	/* 4 */
/* 40 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (24) */
/* 42 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 44 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 46 */	NdrFcShort( 0x54 ),	/* 84 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0xc ),	/* Offset= 12 (62) */
/* 52 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 54 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (8) */
/* 56 */	0x36,		/* FC_POINTER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 58 */	0x0,		/* 0 */
			NdrFcShort( 0xffd9 ),	/* Offset= -39 (20) */
			0x5b,		/* FC_END */
/* 62 */	
			0x11, 0x0,	/* FC_RP */
/* 64 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (36) */
/* 66 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 72 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 74 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 76 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 80 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 82 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 84 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (44) */
/* 86 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 88 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 90 */	0x10,		/* FC_ERROR_STATUS_T */
			0x5c,		/* FC_PAD */
/* 92 */	
			0x14, 0x0,	/* FC_FP */
/* 94 */	NdrFcShort( 0xffaa ),	/* Offset= -86 (8) */
/* 96 */	
			0x12, 0x0,	/* FC_UP */
/* 98 */	NdrFcShort( 0x2 ),	/* Offset= 2 (100) */
/* 100 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 102 */	NdrFcShort( 0x14 ),	/* 20 */
/* 104 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 106 */	NdrFcShort( 0xff9e ),	/* Offset= -98 (8) */
/* 108 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 110 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 112 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 114 */	NdrFcShort( 0x2 ),	/* Offset= 2 (116) */
/* 116 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 118 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 120 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 122 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 124 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 130 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 132 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 134 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 136 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 138 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 140 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 142 */	NdrFcShort( 0xff9e ),	/* Offset= -98 (44) */
/* 144 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 146 */	
			0x14, 0x0,	/* FC_FP */
/* 148 */	NdrFcShort( 0xff90 ),	/* Offset= -112 (36) */
/* 150 */	
			0x11, 0x0,	/* FC_RP */
/* 152 */	NdrFcShort( 0x2 ),	/* Offset= 2 (154) */
/* 154 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 156 */	NdrFcShort( 0x4 ),	/* 4 */
/* 158 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 160 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 162 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 164 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 166 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 168 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 170 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 172 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x4a,		/* FC_VARIABLE_OFFSET */
/* 174 */	NdrFcShort( 0x4 ),	/* 4 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x1 ),	/* 1 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */
/* 184 */	0x14, 0x0,	/* FC_FP */
/* 186 */	NdrFcShort( 0xff6a ),	/* Offset= -150 (36) */
/* 188 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 190 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 192 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 194 */	NdrFcShort( 0xff46 ),	/* Offset= -186 (8) */

			0x0
        }
    };

static const NDR_RUNDOWN RundownRoutines[] = 
    {
    ept_lookup_handle_t_rundown
    };


static const unsigned short epm_FormatStringOffsetTable[] =
    {
    0,
    52,
    98,
    180,
    250,
    290,
    330
    };


static const MIDL_STUB_DESC epm_StubDesc = 
    {
    (void *)& epm___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    RundownRoutines,
    0,
    0,
    0,
    epm__MIDL_TypeFormatString.Format,
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

static const RPC_DISPATCH_FUNCTION epm_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE epm_v3_0_DispatchTable = 
    {
    7,
    (RPC_DISPATCH_FUNCTION*)epm_table
    };

static const SERVER_ROUTINE epm_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)ept_insert,
    (SERVER_ROUTINE)ept_delete,
    (SERVER_ROUTINE)ept_lookup,
    (SERVER_ROUTINE)ept_map,
    (SERVER_ROUTINE)ept_lookup_handle_free,
    (SERVER_ROUTINE)ept_inq_object,
    (SERVER_ROUTINE)ept_mgmt_delete
    };

static const MIDL_SERVER_INFO epm_ServerInfo = 
    {
    &epm_StubDesc,
    epm_ServerRoutineTable,
    epm__MIDL_ProcFormatString.Format,
    epm_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

