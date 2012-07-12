

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Jul 12 13:29:17 2012
 */
/* Compiler settings for ..\..\..\..\wine\include\iads.idl:
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
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "iads.h"

#define TYPE_FORMAT_STRING_SIZE   1043                              
#define PROC_FORMAT_STRING_SIZE   451                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _iads_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } iads_MIDL_TYPE_FORMAT_STRING;

typedef struct _iads_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } iads_MIDL_PROC_FORMAT_STRING;

typedef struct _iads_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } iads_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const iads_MIDL_TYPE_FORMAT_STRING iads__MIDL_TypeFormatString;
extern const iads_MIDL_PROC_FORMAT_STRING iads__MIDL_ProcFormatString;
extern const iads_MIDL_EXPR_FORMAT_STRING iads__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IADsContainer_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IADsContainer_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const iads_MIDL_PROC_FORMAT_STRING iads__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure get_Count */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter retval */

/* 24 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get__NewEnum */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x8 ),	/* 8 */
/* 50 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter retval */

/* 60 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Filter */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x9 ),	/* 9 */
/* 80 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x8 ),	/* 8 */
/* 86 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 88 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 90 */	NdrFcShort( 0x1 ),	/* 1 */
/* 92 */	NdrFcShort( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pvFilter */

/* 96 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 98 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 100 */	NdrFcShort( 0x3e8 ),	/* Type Offset=1000 */

	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Filter */

/* 108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0xa ),	/* 10 */
/* 116 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 122 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 124 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0x1 ),	/* 1 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter vFilter */

/* 132 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	NdrFcShort( 0x3f6 ),	/* Type Offset=1014 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure get_Hints */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0xb ),	/* 11 */
/* 152 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 160 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 162 */	NdrFcShort( 0x1 ),	/* 1 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pvHints */

/* 168 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	NdrFcShort( 0x3e8 ),	/* Type Offset=1000 */

	/* Return value */

/* 174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure put_Hints */

/* 180 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0xc ),	/* 12 */
/* 188 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 196 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 200 */	NdrFcShort( 0x1 ),	/* 1 */
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter vHints */

/* 204 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 208 */	NdrFcShort( 0x3f6 ),	/* Type Offset=1014 */

	/* Return value */

/* 210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 212 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetObject */

/* 216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 222 */	NdrFcShort( 0xd ),	/* 13 */
/* 224 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 232 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x1 ),	/* 1 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrClassName */

/* 240 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 242 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 244 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter bstrRelativeName */

/* 246 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 248 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 250 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter ppObject */

/* 252 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 254 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 256 */	NdrFcShort( 0x40e ),	/* Type Offset=1038 */

	/* Return value */

/* 258 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 260 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Create */

/* 264 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 266 */	NdrFcLong( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0xe ),	/* 14 */
/* 272 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x8 ),	/* 8 */
/* 278 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 280 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0x1 ),	/* 1 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrClassName */

/* 288 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 290 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 292 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter bstrRelativeName */

/* 294 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 296 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 298 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter ppObject */

/* 300 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 304 */	NdrFcShort( 0x40e ),	/* Type Offset=1038 */

	/* Return value */

/* 306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 308 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0xf ),	/* 15 */
/* 320 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 326 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 328 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x1 ),	/* 1 */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrClassName */

/* 336 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 338 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 340 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter bstrRelativeName */

/* 342 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 344 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 346 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CopyHere */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x10 ),	/* 16 */
/* 362 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 368 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 370 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x1 ),	/* 1 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrSourceName */

/* 378 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 382 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter bstrNewName */

/* 384 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter ppObject */

/* 390 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 394 */	NdrFcShort( 0x40e ),	/* Type Offset=1038 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MoveHere */

/* 402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x11 ),	/* 17 */
/* 410 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 418 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 420 */	NdrFcShort( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x1 ),	/* 1 */
/* 424 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bstrSourceName */

/* 426 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 428 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 430 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter bstrNewName */

/* 432 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 434 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 436 */	NdrFcShort( 0x404 ),	/* Type Offset=1028 */

	/* Parameter ppObject */

/* 438 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 440 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 442 */	NdrFcShort( 0x40e ),	/* Type Offset=1038 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const iads_MIDL_TYPE_FORMAT_STRING iads__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 12 */	NdrFcLong( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x0 ),	/* 0 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 22 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 24 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 26 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 28 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 30 */	NdrFcShort( 0x3ca ),	/* Offset= 970 (1000) */
/* 32 */	
			0x13, 0x0,	/* FC_OP */
/* 34 */	NdrFcShort( 0x3b2 ),	/* Offset= 946 (980) */
/* 36 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x7,		/* FC_USHORT */
/* 38 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 40 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 42 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	NdrFcShort( 0x10 ),	/* 16 */
/* 48 */	NdrFcShort( 0x2b ),	/* 43 */
/* 50 */	NdrFcLong( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x0 ),	/* Offset= 0 (54) */
/* 56 */	NdrFcLong( 0x1 ),	/* 1 */
/* 60 */	NdrFcShort( 0x0 ),	/* Offset= 0 (60) */
/* 62 */	NdrFcLong( 0x10 ),	/* 16 */
/* 66 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 68 */	NdrFcLong( 0x12 ),	/* 18 */
/* 72 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 74 */	NdrFcLong( 0x13 ),	/* 19 */
/* 78 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 80 */	NdrFcLong( 0x16 ),	/* 22 */
/* 84 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 86 */	NdrFcLong( 0x17 ),	/* 23 */
/* 90 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 92 */	NdrFcLong( 0x11 ),	/* 17 */
/* 96 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 98 */	NdrFcLong( 0x2 ),	/* 2 */
/* 102 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 104 */	NdrFcLong( 0x3 ),	/* 3 */
/* 108 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 110 */	NdrFcLong( 0x4 ),	/* 4 */
/* 114 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 116 */	NdrFcLong( 0x5 ),	/* 5 */
/* 120 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 122 */	NdrFcLong( 0xb ),	/* 11 */
/* 126 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 128 */	NdrFcLong( 0xa ),	/* 10 */
/* 132 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 134 */	NdrFcLong( 0x7 ),	/* 7 */
/* 138 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 140 */	NdrFcLong( 0x8 ),	/* 8 */
/* 144 */	NdrFcShort( 0xa6 ),	/* Offset= 166 (310) */
/* 146 */	NdrFcLong( 0x6 ),	/* 6 */
/* 150 */	NdrFcShort( 0xba ),	/* Offset= 186 (336) */
/* 152 */	NdrFcLong( 0xe ),	/* 14 */
/* 156 */	NdrFcShort( 0xba ),	/* Offset= 186 (342) */
/* 158 */	NdrFcLong( 0xd ),	/* 13 */
/* 162 */	NdrFcShort( 0xff68 ),	/* Offset= -152 (10) */
/* 164 */	NdrFcLong( 0x9 ),	/* 9 */
/* 168 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (352) */
/* 170 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 174 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (370) */
/* 176 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 180 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (924) */
/* 182 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 186 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (928) */
/* 188 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 192 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (932) */
/* 194 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 198 */	NdrFcShort( 0x2de ),	/* Offset= 734 (932) */
/* 200 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 204 */	NdrFcShort( 0x2d8 ),	/* Offset= 728 (932) */
/* 206 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 210 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (924) */
/* 212 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 216 */	NdrFcShort( 0x2c8 ),	/* Offset= 712 (928) */
/* 218 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 222 */	NdrFcShort( 0x2c6 ),	/* Offset= 710 (932) */
/* 224 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 228 */	NdrFcShort( 0x2c4 ),	/* Offset= 708 (936) */
/* 230 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 234 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (940) */
/* 236 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 240 */	NdrFcShort( 0x2b0 ),	/* Offset= 688 (928) */
/* 242 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 246 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (932) */
/* 248 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 252 */	NdrFcShort( 0x2b0 ),	/* Offset= 688 (940) */
/* 254 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 258 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (944) */
/* 260 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 264 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (948) */
/* 266 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 270 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (956) */
/* 272 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 276 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (960) */
/* 278 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 282 */	NdrFcShort( 0x2aa ),	/* Offset= 682 (964) */
/* 284 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 288 */	NdrFcShort( 0x2a8 ),	/* Offset= 680 (968) */
/* 290 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 294 */	NdrFcShort( 0x2a6 ),	/* Offset= 678 (972) */
/* 296 */	NdrFcLong( 0x24 ),	/* 36 */
/* 300 */	NdrFcShort( 0x2a4 ),	/* Offset= 676 (976) */
/* 302 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 306 */	NdrFcShort( 0x29e ),	/* Offset= 670 (976) */
/* 308 */	NdrFcShort( 0xffff ),	/* Offset= -1 (307) */
/* 310 */	
			0x13, 0x0,	/* FC_OP */
/* 312 */	NdrFcShort( 0xe ),	/* Offset= 14 (326) */
/* 314 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 316 */	NdrFcShort( 0x2 ),	/* 2 */
/* 318 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 320 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 322 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 324 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 326 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 328 */	NdrFcShort( 0x8 ),	/* 8 */
/* 330 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (314) */
/* 332 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 334 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 336 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 342 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 344 */	NdrFcShort( 0x10 ),	/* 16 */
/* 346 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 348 */	0x2,		/* FC_CHAR */
			0x8,		/* FC_LONG */
/* 350 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 352 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 354 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */
/* 362 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 364 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 366 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 368 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 370 */	
			0x13, 0x0,	/* FC_OP */
/* 372 */	NdrFcShort( 0x216 ),	/* Offset= 534 (906) */
/* 374 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 376 */	NdrFcShort( 0x18 ),	/* 24 */
/* 378 */	NdrFcShort( 0xa ),	/* 10 */
/* 380 */	NdrFcLong( 0x8 ),	/* 8 */
/* 384 */	NdrFcShort( 0x5a ),	/* Offset= 90 (474) */
/* 386 */	NdrFcLong( 0xd ),	/* 13 */
/* 390 */	NdrFcShort( 0x7e ),	/* Offset= 126 (516) */
/* 392 */	NdrFcLong( 0x9 ),	/* 9 */
/* 396 */	NdrFcShort( 0x9e ),	/* Offset= 158 (554) */
/* 398 */	NdrFcLong( 0xc ),	/* 12 */
/* 402 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (602) */
/* 404 */	NdrFcLong( 0x24 ),	/* 36 */
/* 408 */	NdrFcShort( 0x124 ),	/* Offset= 292 (700) */
/* 410 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 414 */	NdrFcShort( 0x140 ),	/* Offset= 320 (734) */
/* 416 */	NdrFcLong( 0x10 ),	/* 16 */
/* 420 */	NdrFcShort( 0x15a ),	/* Offset= 346 (766) */
/* 422 */	NdrFcLong( 0x2 ),	/* 2 */
/* 426 */	NdrFcShort( 0x174 ),	/* Offset= 372 (798) */
/* 428 */	NdrFcLong( 0x3 ),	/* 3 */
/* 432 */	NdrFcShort( 0x18e ),	/* Offset= 398 (830) */
/* 434 */	NdrFcLong( 0x14 ),	/* 20 */
/* 438 */	NdrFcShort( 0x1a8 ),	/* Offset= 424 (862) */
/* 440 */	NdrFcShort( 0xffff ),	/* Offset= -1 (439) */
/* 442 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 444 */	NdrFcShort( 0x4 ),	/* 4 */
/* 446 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 452 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 454 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 456 */	NdrFcShort( 0x4 ),	/* 4 */
/* 458 */	NdrFcShort( 0x0 ),	/* 0 */
/* 460 */	NdrFcShort( 0x1 ),	/* 1 */
/* 462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x0 ),	/* 0 */
/* 466 */	0x13, 0x0,	/* FC_OP */
/* 468 */	NdrFcShort( 0xff72 ),	/* Offset= -142 (326) */
/* 470 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 472 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 474 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 476 */	NdrFcShort( 0x8 ),	/* 8 */
/* 478 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 480 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 482 */	NdrFcShort( 0x4 ),	/* 4 */
/* 484 */	NdrFcShort( 0x4 ),	/* 4 */
/* 486 */	0x11, 0x0,	/* FC_RP */
/* 488 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (442) */
/* 490 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 492 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 494 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 498 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 504 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 508 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 510 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 512 */	NdrFcShort( 0xfe0a ),	/* Offset= -502 (10) */
/* 514 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 516 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 522 */	NdrFcShort( 0x6 ),	/* Offset= 6 (528) */
/* 524 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 526 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 528 */	
			0x11, 0x0,	/* FC_RP */
/* 530 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (494) */
/* 532 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 540 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 542 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 546 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 548 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 550 */	NdrFcShort( 0xff3a ),	/* Offset= -198 (352) */
/* 552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 554 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 556 */	NdrFcShort( 0x8 ),	/* 8 */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x6 ),	/* Offset= 6 (566) */
/* 562 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 564 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 566 */	
			0x11, 0x0,	/* FC_RP */
/* 568 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (532) */
/* 570 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 572 */	NdrFcShort( 0x4 ),	/* 4 */
/* 574 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 578 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 580 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 582 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 584 */	NdrFcShort( 0x4 ),	/* 4 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x1 ),	/* 1 */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x0 ),	/* 0 */
/* 594 */	0x13, 0x0,	/* FC_OP */
/* 596 */	NdrFcShort( 0x180 ),	/* Offset= 384 (980) */
/* 598 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 600 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 602 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x6 ),	/* Offset= 6 (614) */
/* 610 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 612 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 614 */	
			0x11, 0x0,	/* FC_RP */
/* 616 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (570) */
/* 618 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 620 */	NdrFcLong( 0x2f ),	/* 47 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 628 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 630 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 632 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 634 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 636 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 638 */	NdrFcShort( 0x1 ),	/* 1 */
/* 640 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 642 */	NdrFcShort( 0x4 ),	/* 4 */
/* 644 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 646 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 648 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 650 */	NdrFcShort( 0x10 ),	/* 16 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0xa ),	/* Offset= 10 (664) */
/* 656 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 658 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 660 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (618) */
/* 662 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 664 */	
			0x13, 0x0,	/* FC_OP */
/* 666 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (636) */
/* 668 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 670 */	NdrFcShort( 0x4 ),	/* 4 */
/* 672 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 676 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 678 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 680 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 682 */	NdrFcShort( 0x4 ),	/* 4 */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x1 ),	/* 1 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x0 ),	/* 0 */
/* 692 */	0x13, 0x0,	/* FC_OP */
/* 694 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (648) */
/* 696 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 698 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 700 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x8 ),	/* 8 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x6 ),	/* Offset= 6 (712) */
/* 708 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 710 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 712 */	
			0x11, 0x0,	/* FC_RP */
/* 714 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (668) */
/* 716 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 718 */	NdrFcShort( 0x8 ),	/* 8 */
/* 720 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 722 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 724 */	NdrFcShort( 0x10 ),	/* 16 */
/* 726 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 728 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 730 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (716) */
			0x5b,		/* FC_END */
/* 734 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 736 */	NdrFcShort( 0x18 ),	/* 24 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0xa ),	/* Offset= 10 (750) */
/* 742 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 744 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 746 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (722) */
/* 748 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 750 */	
			0x11, 0x0,	/* FC_RP */
/* 752 */	NdrFcShort( 0xfefe ),	/* Offset= -258 (494) */
/* 754 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 756 */	NdrFcShort( 0x1 ),	/* 1 */
/* 758 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 760 */	NdrFcShort( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 764 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 766 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 772 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 774 */	NdrFcShort( 0x4 ),	/* 4 */
/* 776 */	NdrFcShort( 0x4 ),	/* 4 */
/* 778 */	0x13, 0x0,	/* FC_OP */
/* 780 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (754) */
/* 782 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 784 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 786 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 788 */	NdrFcShort( 0x2 ),	/* 2 */
/* 790 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 796 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 798 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 800 */	NdrFcShort( 0x8 ),	/* 8 */
/* 802 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 804 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 806 */	NdrFcShort( 0x4 ),	/* 4 */
/* 808 */	NdrFcShort( 0x4 ),	/* 4 */
/* 810 */	0x13, 0x0,	/* FC_OP */
/* 812 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (786) */
/* 814 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 816 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 818 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 820 */	NdrFcShort( 0x4 ),	/* 4 */
/* 822 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 828 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 830 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 832 */	NdrFcShort( 0x8 ),	/* 8 */
/* 834 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 836 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 838 */	NdrFcShort( 0x4 ),	/* 4 */
/* 840 */	NdrFcShort( 0x4 ),	/* 4 */
/* 842 */	0x13, 0x0,	/* FC_OP */
/* 844 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (818) */
/* 846 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 848 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 850 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 854 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 860 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 862 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 864 */	NdrFcShort( 0x8 ),	/* 8 */
/* 866 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 868 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 870 */	NdrFcShort( 0x4 ),	/* 4 */
/* 872 */	NdrFcShort( 0x4 ),	/* 4 */
/* 874 */	0x13, 0x0,	/* FC_OP */
/* 876 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (850) */
/* 878 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 880 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 882 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 884 */	NdrFcShort( 0x8 ),	/* 8 */
/* 886 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 888 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 890 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 892 */	NdrFcShort( 0x8 ),	/* 8 */
/* 894 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 896 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 898 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 900 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 902 */	NdrFcShort( 0xffec ),	/* Offset= -20 (882) */
/* 904 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 906 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 908 */	NdrFcShort( 0x28 ),	/* 40 */
/* 910 */	NdrFcShort( 0xffec ),	/* Offset= -20 (890) */
/* 912 */	NdrFcShort( 0x0 ),	/* Offset= 0 (912) */
/* 914 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 916 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 918 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 920 */	NdrFcShort( 0xfdde ),	/* Offset= -546 (374) */
/* 922 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 924 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 926 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 928 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 930 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 932 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 934 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 936 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 938 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 940 */	
			0x13, 0x8,	/* FC_OP [simple_pointer] */
/* 942 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 944 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 946 */	NdrFcShort( 0xfd84 ),	/* Offset= -636 (310) */
/* 948 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 950 */	NdrFcShort( 0x2 ),	/* Offset= 2 (952) */
/* 952 */	
			0x13, 0x0,	/* FC_OP */
/* 954 */	NdrFcShort( 0x1a ),	/* Offset= 26 (980) */
/* 956 */	
			0x13, 0x0,	/* FC_OP */
/* 958 */	NdrFcShort( 0xfd92 ),	/* Offset= -622 (336) */
/* 960 */	
			0x13, 0x0,	/* FC_OP */
/* 962 */	NdrFcShort( 0xfd94 ),	/* Offset= -620 (342) */
/* 964 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 966 */	NdrFcShort( 0xfc44 ),	/* Offset= -956 (10) */
/* 968 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 970 */	NdrFcShort( 0xfd96 ),	/* Offset= -618 (352) */
/* 972 */	
			0x13, 0x10,	/* FC_OP [pointer_deref] */
/* 974 */	NdrFcShort( 0xfda4 ),	/* Offset= -604 (370) */
/* 976 */	
			0x13, 0x0,	/* FC_OP */
/* 978 */	NdrFcShort( 0xfeb6 ),	/* Offset= -330 (648) */
/* 980 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 982 */	NdrFcShort( 0x20 ),	/* 32 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0x0 ),	/* Offset= 0 (986) */
/* 988 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 990 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 992 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 994 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 996 */	NdrFcShort( 0xfc40 ),	/* Offset= -960 (36) */
/* 998 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1000 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1008 */	NdrFcShort( 0xfc30 ),	/* Offset= -976 (32) */
/* 1010 */	
			0x12, 0x0,	/* FC_UP */
/* 1012 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (980) */
/* 1014 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1022 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1010) */
/* 1024 */	
			0x12, 0x0,	/* FC_UP */
/* 1026 */	NdrFcShort( 0xfd44 ),	/* Offset= -700 (326) */
/* 1028 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1030 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1032 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1024) */
/* 1038 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1040 */	NdrFcShort( 0xfd50 ),	/* Offset= -688 (352) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            },
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Standard interface: __MIDL_itf_iads_0000_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IADsContainer, ver. 0.0,
   GUID={0x001677d0,0xfd16,0x11ce,{0xab,0xc4,0x02,0x60,0x8c,0x9e,0x75,0x53}} */

#pragma code_seg(".orpc")
static const unsigned short IADsContainer_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    72,
    108,
    144,
    180,
    216,
    264,
    312,
    354,
    402
    };

static const MIDL_STUBLESS_PROXY_INFO IADsContainer_ProxyInfo =
    {
    &Object_StubDesc,
    iads__MIDL_ProcFormatString.Format,
    &IADsContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IADsContainer_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    iads__MIDL_ProcFormatString.Format,
    &IADsContainer_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(18) _IADsContainerProxyVtbl = 
{
    &IADsContainer_ProxyInfo,
    &IID_IADsContainer,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::get_Count */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::get__NewEnum */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::get_Filter */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::put_Filter */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::get_Hints */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::put_Hints */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::GetObject */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::Create */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::Delete */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::CopyHere */ ,
    (void *) (INT_PTR) -1 /* IADsContainer::MoveHere */
};


static const PRPC_STUB_FUNCTION IADsContainer_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IADsContainerStubVtbl =
{
    &IID_IADsContainer,
    &IADsContainer_ServerInfo,
    18,
    &IADsContainer_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    iads__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _iads_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IADsContainerProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _iads_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IADsContainerStubVtbl,
    0
};

PCInterfaceName const _iads_InterfaceNamesList[] = 
{
    "IADsContainer",
    0
};

const IID *  const _iads_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _iads_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _iads, pIID, n)

int __stdcall _iads_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_iads_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo iads_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _iads_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _iads_StubVtblList,
    (const PCInterfaceName * ) & _iads_InterfaceNamesList,
    (const IID ** ) & _iads_BaseIIDList,
    & _iads_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

