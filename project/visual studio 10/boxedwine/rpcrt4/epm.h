

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


#ifndef __epm_h__
#define __epm_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_epm_0000_0000 */
/* [local] */ 

typedef unsigned small unsigned8;

typedef unsigned short unsigned16;

typedef unsigned long unsigned32;

typedef small signed8;

typedef short signed16;

typedef long signed32;

typedef unsigned32 boolean32;

typedef MIDL_uhyper u_int64;

typedef unsigned long u_int32;

typedef unsigned short u_int16;

typedef unsigned small u_int8;

typedef /* [full] */ GUID *uuid_p_t;

typedef struct twr_t
    {
    unsigned32 tower_length;
    byte tower_octet_string[ 1 ];
    } 	twr_t;

typedef /* [full] */ twr_t *twr_p_t;

typedef struct ndr_context_handle
    {
    unsigned32 context_handle_attributes;
    GUID context_handle_uuid;
    } 	ndr_context_handle;

#define	ndr_c_int_big_endian	( 0 )

#define	ndr_c_int_little_endian	( 1 )

#define	ndr_c_float_ieee	( 0 )

#define	ndr_c_float_vax	( 1 )

#define	ndr_c_float_cray	( 2 )

#define	ndr_c_float_ibm	( 3 )

#define	ndr_c_char_ascii	( 0 )

#define	ndr_c_char_ebcdic	( 1 )

typedef /* [public] */ struct __MIDL___MIDL_itf_epm_0000_0000_0001
    {
    unsigned8 int_rep;
    unsigned8 char_rep;
    unsigned8 float_rep;
    byte reserved;
    } 	ndr_format_t;

typedef struct __MIDL___MIDL_itf_epm_0000_0000_0001 *ndr_format_p_t;

typedef /* [public][public][public][public][public] */ struct __MIDL___MIDL_itf_epm_0000_0000_0002
    {
    GUID uuid;
    unsigned16 vers_major;
    unsigned16 vers_minor;
    } 	rpc_if_id_t;

typedef /* [unique] */ rpc_if_id_t *rpc_if_id_p_t;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_epm_0000_0000_0003
    {
    unsigned32 count;
    rpc_if_id_p_t if_id[ 1 ];
    } 	rpc_if_id_vector_t;

typedef /* [unique] */ rpc_if_id_vector_t *rpc_if_id_vector_p_t;

typedef /* [public] */ struct __MIDL___MIDL_itf_epm_0000_0000_0004
    {
    unsigned32 count;
    unsigned32 stats[ 1 ];
    } 	rpc_stats_vector_t;

typedef struct __MIDL___MIDL_itf_epm_0000_0000_0004 *rpc_stats_vector_p_t;

RPC_STATUS WINAPI TowerExplode(
    const twr_t *tower, PRPC_SYNTAX_IDENTIFIER object, PRPC_SYNTAX_IDENTIFIER syntax,
    char **protseq, char **endpoint, char **address);
RPC_STATUS WINAPI TowerConstruct(
    const RPC_SYNTAX_IDENTIFIER *object, const RPC_SYNTAX_IDENTIFIER *syntax,
    const char *protseq, const char *endpoint, const char *address,
    twr_t **tower);


extern RPC_IF_HANDLE __MIDL_itf_epm_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_epm_0000_0000_v0_0_s_ifspec;

#ifndef __epm_INTERFACE_DEFINED__
#define __epm_INTERFACE_DEFINED__

/* interface epm */
/* [ref][version][uuid] */ 

#define	ept_max_annotation_size	( 64 )

typedef /* [public][public][public][public] */ struct __MIDL_epm_0005
    {
    GUID object;
    twr_t *tower;
    unsigned char annotation[ 64 ];
    } 	ept_entry_t;

typedef struct __MIDL_epm_0005 *ept_entry_p_t;

typedef /* [context_handle] */ void *ept_lookup_handle_t;

void ept_insert( 
    /* [in] */ handle_t h,
    /* [in] */ unsigned32 num_ents,
    /* [size_is][in] */ ept_entry_t entries[  ],
    /* [in] */ boolean32 replace,
    /* [out] */ error_status_t *status);

void ept_delete( 
    /* [in] */ handle_t h,
    /* [in] */ unsigned32 num_ents,
    /* [size_is][in] */ ept_entry_t entries[  ],
    /* [out] */ error_status_t *status);

/* [idempotent] */ void ept_lookup( 
    /* [in] */ handle_t h,
    /* [in] */ unsigned32 inquiry_type,
    /* [in] */ uuid_p_t object,
    /* [in] */ rpc_if_id_p_t interface_id,
    /* [in] */ unsigned32 vers_option,
    /* [out][in] */ ept_lookup_handle_t *entry_handle,
    /* [in] */ unsigned32 max_ents,
    /* [out] */ unsigned32 *num_ents,
    /* [size_is][length_is][out] */ ept_entry_t entries[  ],
    /* [out] */ error_status_t *status);

/* [idempotent] */ void ept_map( 
    /* [in] */ handle_t h,
    /* [in] */ uuid_p_t object,
    /* [in] */ twr_p_t map_tower,
    /* [out][in] */ ept_lookup_handle_t *entry_handle,
    /* [in] */ unsigned32 max_towers,
    /* [out] */ unsigned32 *num_towers,
    /* [size_is][length_is][out] */ twr_p_t *towers,
    /* [out] */ error_status_t *status);

void ept_lookup_handle_free( 
    /* [in] */ handle_t h,
    /* [out][in] */ ept_lookup_handle_t *entry_handle,
    /* [out] */ error_status_t *status);

/* [idempotent] */ void ept_inq_object( 
    /* [in] */ handle_t h,
    /* [out] */ GUID *ept_object,
    /* [out] */ error_status_t *status);

void ept_mgmt_delete( 
    /* [in] */ handle_t h,
    /* [in] */ boolean32 object_speced,
    /* [in] */ uuid_p_t object,
    /* [in] */ twr_p_t tower,
    /* [out] */ error_status_t *status);



extern RPC_IF_HANDLE epm_v3_0_c_ifspec;
extern RPC_IF_HANDLE epm_v3_0_s_ifspec;
#endif /* __epm_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

void __RPC_USER ept_lookup_handle_t_rundown( ept_lookup_handle_t );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


