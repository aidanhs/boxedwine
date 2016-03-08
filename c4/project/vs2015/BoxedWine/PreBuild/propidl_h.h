

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Feb 17 09:54:03 2016
 */
/* Compiler settings for ..\..\..\..\include\propidl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __propidl_h_h__
#define __propidl_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPropertyStorage_FWD_DEFINED__
#define __IPropertyStorage_FWD_DEFINED__
typedef interface IPropertyStorage IPropertyStorage;

#endif 	/* __IPropertyStorage_FWD_DEFINED__ */


#ifndef __IPropertySetStorage_FWD_DEFINED__
#define __IPropertySetStorage_FWD_DEFINED__
typedef interface IPropertySetStorage IPropertySetStorage;

#endif 	/* __IPropertySetStorage_FWD_DEFINED__ */


#ifndef __IEnumSTATPROPSTG_FWD_DEFINED__
#define __IEnumSTATPROPSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSTG IEnumSTATPROPSTG;

#endif 	/* __IEnumSTATPROPSTG_FWD_DEFINED__ */


#ifndef __IEnumSTATPROPSETSTG_FWD_DEFINED__
#define __IEnumSTATPROPSETSTG_FWD_DEFINED__
typedef interface IEnumSTATPROPSETSTG IEnumSTATPROPSETSTG;

#endif 	/* __IEnumSTATPROPSETSTG_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_propidl_0000_0000 */
/* [local] */ 

#ifdef PID_DICTIONARY
#undef PID_DICTIONARY
#endif
#ifdef PID_CODEPAGE
#undef PID_CODEPAGE
#endif
#ifdef PID_SECURITY
#undef PID_SECURITY
#endif




extern RPC_IF_HANDLE __MIDL_itf_propidl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_propidl_0000_0000_v0_0_s_ifspec;

#ifndef __IPropertyStorage_INTERFACE_DEFINED__
#define __IPropertyStorage_INTERFACE_DEFINED__

/* interface IPropertyStorage */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IPropertyStorage *LPPROPERTYSTORAGE;

#define	PROPSETFLAG_DEFAULT	( 0 )

#define	PROPSETFLAG_NONSIMPLE	( 1 )

#define	PROPSETFLAG_ANSI	( 2 )

#define	PROPSETFLAG_UNBUFFERED	( 4 )

#define	PROPSETFLAG_CASE_SENSITIVE	( 8 )

#define	PROPSET_BEHAVIOR_CASE_SENSITIVE	( 1 )

#define	PID_DICTIONARY	( 0 )

#define	PID_CODEPAGE	( 0x1 )

#define	PID_FIRST_USABLE	( 0x2 )

#define	PID_FIRST_NAME_DEFAULT	( 0xfff )

#define	PID_LOCALE	( 0x80000000 )

#define	PID_MODIFY_TIME	( 0x80000001 )

#define	PID_SECURITY	( 0x80000002 )

#define	PID_BEHAVIOR	( 0x80000003 )

#define	PID_ILLEGAL	( 0xffffffff )

#define	PID_MIN_READONLY	( 0x80000000 )

#define	PID_MAX_READONLY	( 0xbfffffff )

typedef struct tagPROPVARIANT PROPVARIANT;

typedef struct tagCAUB
    {
    ULONG cElems;
    /* [size_is] */ unsigned char *pElems;
    } 	CAUB;

typedef struct tagCAI
    {
    ULONG cElems;
    /* [size_is] */ short *pElems;
    } 	CAI;

typedef struct tagCAUI
    {
    ULONG cElems;
    /* [size_is] */ USHORT *pElems;
    } 	CAUI;

typedef struct tagCAL
    {
    ULONG cElems;
    /* [size_is] */ long *pElems;
    } 	CAL;

typedef struct tagCAUL
    {
    ULONG cElems;
    /* [size_is] */ ULONG *pElems;
    } 	CAUL;

typedef struct tagCAFLT
    {
    ULONG cElems;
    /* [size_is] */ float *pElems;
    } 	CAFLT;

typedef struct tagCADBL
    {
    ULONG cElems;
    /* [size_is] */ double *pElems;
    } 	CADBL;

typedef struct tagCACY
    {
    ULONG cElems;
    /* [size_is] */ CY *pElems;
    } 	CACY;

typedef struct tagCADATE
    {
    ULONG cElems;
    /* [size_is] */ DATE *pElems;
    } 	CADATE;

typedef struct tagCABSTR
    {
    ULONG cElems;
    /* [size_is] */ BSTR *pElems;
    } 	CABSTR;

typedef struct tagCABSTRBLOB
    {
    ULONG cElems;
    /* [size_is] */ BSTRBLOB *pElems;
    } 	CABSTRBLOB;

typedef struct tagCABOOL
    {
    ULONG cElems;
    /* [size_is] */ VARIANT_BOOL *pElems;
    } 	CABOOL;

typedef struct tagCASCODE
    {
    ULONG cElems;
    /* [size_is] */ SCODE *pElems;
    } 	CASCODE;

typedef struct tagCAPROPVARIANT
    {
    ULONG cElems;
    /* [size_is] */ PROPVARIANT *pElems;
    } 	CAPROPVARIANT;

typedef struct tagCAH
    {
    ULONG cElems;
    /* [size_is] */ LARGE_INTEGER *pElems;
    } 	CAH;

typedef struct tagCAUH
    {
    ULONG cElems;
    /* [size_is] */ ULARGE_INTEGER *pElems;
    } 	CAUH;

typedef struct tagCALPSTR
    {
    ULONG cElems;
    /* [size_is] */ LPSTR *pElems;
    } 	CALPSTR;

typedef struct tagCALPWSTR
    {
    ULONG cElems;
    /* [size_is] */ LPWSTR *pElems;
    } 	CALPWSTR;

typedef struct tagCAFILETIME
    {
    ULONG cElems;
    /* [size_is] */ FILETIME *pElems;
    } 	CAFILETIME;

typedef struct tagCACLIPDATA
    {
    ULONG cElems;
    /* [size_is] */ CLIPDATA *pElems;
    } 	CACLIPDATA;

typedef struct tagCACLSID
    {
    ULONG cElems;
    /* [size_is] */ CLSID *pElems;
    } 	CACLSID;

struct tagPROPVARIANT
    {
    VARTYPE vt;
    WORD wReserved1;
    WORD wReserved2;
    WORD wReserved3;
    /* [switch_is] */ /* [switch_type] */ union 
        {
        /* [case()] */  /* Empty union arm */ 
        /* [case()] */ unsigned char cVal;
        /* [case()] */ UCHAR bVal;
        /* [case()] */ short iVal;
        /* [case()] */ USHORT uiVal;
        /* [case()] */ VARIANT_BOOL boolVal;
        /* [case()] */ long lVal;
        /* [case()] */ ULONG ulVal;
        /* [case()] */ float fltVal;
        /* [case()] */ SCODE scode;
        /* [case()] */ LARGE_INTEGER hVal;
        /* [case()] */ ULARGE_INTEGER uhVal;
        /* [case()] */ double dblVal;
        /* [case()] */ CY cyVal;
        /* [case()] */ DATE date;
        /* [case()] */ FILETIME filetime;
        /* [case()] */ CLSID *puuid;
        /* [case()] */ BLOB blob;
        /* [case()] */ CLIPDATA *pclipdata;
        /* [case()] */ IStream *pStream;
        /* [case()] */ IStorage *pStorage;
        /* [case()] */ BSTR bstrVal;
        /* [case()] */ BSTRBLOB bstrblobVal;
        /* [case()] */ LPSTR pszVal;
        /* [case()] */ LPWSTR pwszVal;
        /* [case()] */ CAUB caub;
        /* [case()] */ CAI cai;
        /* [case()] */ CAUI caui;
        /* [case()] */ CABOOL cabool;
        /* [case()] */ CAL cal;
        /* [case()] */ CAUL caul;
        /* [case()] */ CAFLT caflt;
        /* [case()] */ CASCODE cascode;
        /* [case()] */ CAH cah;
        /* [case()] */ CAUH cauh;
        /* [case()] */ CADBL cadbl;
        /* [case()] */ CACY cacy;
        /* [case()] */ CADATE cadate;
        /* [case()] */ CAFILETIME cafiletime;
        /* [case()] */ CACLSID cauuid;
        /* [case()] */ CACLIPDATA caclipdata;
        /* [case()] */ CABSTR cabstr;
        /* [case()] */ CABSTRBLOB cabstrblob;
        /* [case()] */ CALPSTR calpstr;
        /* [case()] */ CALPWSTR calpwstr;
        /* [case()] */ CAPROPVARIANT capropvar;
        /* [case()] */ LPSAFEARRAY parray;
        /* [case()] */ CHAR *pcVal;
        /* [case()] */ UCHAR *pbVal;
        /* [case()] */ SHORT *piVal;
        /* [case()] */ USHORT *puiVal;
        /* [case()] */ LONG *plVal;
        /* [case()] */ ULONG *pulVal;
        /* [case()] */ INT *pintVal;
        /* [case()] */ UINT *puintVal;
        /* [case()] */ FLOAT *pfltVal;
        /* [case()] */ DOUBLE *pdblVal;
        /* [case()] */ VARIANT_BOOL *pboolVal;
        /* [case()] */ DECIMAL *pdecVal;
        /* [case()] */ SCODE *pscode;
        /* [case()] */ CY *pcyVal;
        /* [case()] */ DATE *pdate;
        /* [case()] */ BSTR *pbstrVal;
        /* [case()] */ IUnknown **ppunkVal;
        /* [case()] */ IDispatch **ppdispVal;
        /* [case()] */ LPSAFEARRAY *pparray;
        /* [case()] */ PROPVARIANT *pvarVal;
        } 	;
    } ;
typedef struct tagPROPVARIANT *LPPROPVARIANT;

#if 0
typedef const PROPVARIANT *REFPROPVARIANT;

#endif
#ifndef _REFPROPVARIANT_DEFINED
#define _REFPROPVARIANT_DEFINED
#ifdef __cplusplus
#define REFPROPVARIANT const PROPVARIANT &
#else
#define REFPROPVARIANT const PROPVARIANT * __MIDL_CONST
#endif
#endif
#define PIDDI_THUMBNAIL 0x00000002L /* VT_BLOB */

#define PIDSI_TITLE        0x00000002L /* VT_LPSTR */
#define PIDSI_SUBJECT      0x00000003L /* VT_LPSTR */
#define PIDSI_AUTHOR       0x00000004L /* VT_LPSTR */
#define PIDSI_KEYWORDS     0x00000005L /* VT_LPSTR */
#define PIDSI_COMMENTS     0x00000006L /* VT_LPSTR */
#define PIDSI_TEMPLATE     0x00000007L /* VT_LPSTR */
#define PIDSI_LASTAUTHOR   0x00000008L /* VT_LPSTR */
#define PIDSI_REVNUMBER    0x00000009L /* VT_LPSTR */
#define PIDSI_EDITTIME     0x0000000AL /* VT_FILETIME (UTC) */
#define PIDSI_LASTPRINTED  0x0000000BL /* VT_FILETIME (UTC) */
#define PIDSI_CREATE_DTM   0x0000000CL /* VT_FILETIME (UTC) */
#define PIDSI_LASTSAVE_DTM 0x0000000DL /* VT_FILETIME (UTC) */
#define PIDSI_PAGECOUNT    0x0000000EL /* VT_I4 */
#define PIDSI_WORDCOUNT    0x0000000FL /* VT_I4 */
#define PIDSI_CHARCOUNT    0x00000010L /* VT_I4 */
#define PIDSI_THUMBNAIL    0x00000011L /* VT_CF */
#define PIDSI_APPNAME      0x00000012L /* VT_LPSTR */
#define PIDSI_DOC_SECURITY 0x00000013L /* VT_I4 */

#define PIDDSI_CATEGORY    0x00000002L /* VT_LPSTR */
#define PIDDSI_PRESFORMAT  0x00000003L /* VT_LPSTR */
#define PIDDSI_BYTECOUNT   0x00000004L /* VT_I4 */
#define PIDDSI_LINECOUNT   0x00000005L /* VT_I4 */
#define PIDDSI_PARCOUNT    0x00000006L /* VT_I4 */
#define PIDDSI_SLIDECOUNT  0x00000007L /* VT_I4 */
#define PIDDSI_NOTECOUNT   0x00000008L /* VT_I4 */
#define PIDDSI_HIDDENCOUNT 0x00000009L /* VT_I4 */
#define PIDDSI_MMCLIPCOUNT 0x0000000AL /* VT_I4 */
#define PIDDSI_SCALE       0x0000000BL /* VT_BOOL */
#define PIDDSI_HEADINGPAIR 0x0000000CL /* VT_VARIANT|VT_VECTOR */
#define PIDDSI_DOCPARTS    0x0000000DL /* VT_LPSTR|VT_VECTOR */
#define PIDDSI_MANAGER     0x0000000EL /* VT_LPSTR */
#define PIDDSI_COMPANY     0x0000000FL /* VT_LPSTR */
#define PIDDSI_LINKSDIRTY  0x00000010L /* VT_B0OL */
#define	PRSPEC_INVALID	( 0xffffffff )

#define	PRSPEC_LPWSTR	( 0 )

#define	PRSPEC_PROPID	( 1 )

typedef struct tagPROPSPEC
    {
    ULONG ulKind;
    /* [switch_is] */ /* [switch_type] */ union 
        {
        /* [case()] */ PROPID propid;
        /* [case()] */ LPOLESTR lpwstr;
        /* [default] */  /* Empty union arm */ 
        } 	DUMMYUNIONNAME;
    } 	PROPSPEC;

typedef struct tagSTATPROPSTG
    {
    LPOLESTR lpwstrName;
    PROPID propid;
    VARTYPE vt;
    } 	STATPROPSTG;

/* Macros for dwOSVersion member of STATPROPSETSTG */
#define PROPSETHDR_OSVER_KIND(dwOSVer)  HIWORD((dwOSVer))
#define PROPSETHDR_OSVER_MAJOR(dwOSVer) LOBYTE(LOWORD((dwOSVer)))
#define PROPSETHDR_OSVER_MINOR(dwOSVer) HIBYTE(LOWORD((dwOSVer)))
#define PROPSETHDR_OSVERSION_UNKNOWN    0xffffffff
typedef struct tagSTATPROPSETSTG
    {
    FMTID fmtid;
    CLSID clsid;
    DWORD grfFlags;
    FILETIME mtime;
    FILETIME ctime;
    FILETIME atime;
    DWORD dwOSVersion;
    } 	STATPROPSETSTG;


EXTERN_C const IID IID_IPropertyStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000138-0000-0000-C000-000000000046")
    IPropertyStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReadMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ],
            /* [size_is][out] */ PROPVARIANT rgpropvar[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ],
            /* [size_is][in] */ const PROPVARIANT rgpropvar[  ],
            /* [in] */ PROPID propidNameFirst) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteMultiple( 
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadPropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ],
            /* [size_is][out] */ LPOLESTR rglpwstrName[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WritePropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ],
            /* [size_is][in] */ const LPOLESTR rglpwstrName[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeletePropertyNames( 
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ DWORD grfCommitFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Revert( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enum( 
            /* [out] */ IEnumSTATPROPSTG **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTimes( 
            /* [in] */ const FILETIME *pctime,
            /* [in] */ const FILETIME *patime,
            /* [in] */ const FILETIME *pmtime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClass( 
            /* [in] */ REFCLSID clsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stat( 
            /* [out] */ STATPROPSETSTG *statpsstg) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPropertyStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertyStorage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertyStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertyStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReadMultiple )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ],
            /* [size_is][out] */ PROPVARIANT rgpropvar[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *WriteMultiple )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ],
            /* [size_is][in] */ const PROPVARIANT rgpropvar[  ],
            /* [in] */ PROPID propidNameFirst);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteMultiple )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpspec,
            /* [size_is][in] */ const PROPSPEC rgpspec[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *ReadPropertyNames )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ],
            /* [size_is][out] */ LPOLESTR rglpwstrName[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *WritePropertyNames )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ],
            /* [size_is][in] */ const LPOLESTR rglpwstrName[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *DeletePropertyNames )( 
            IPropertyStorage * This,
            /* [in] */ ULONG cpropid,
            /* [size_is][in] */ const PROPID rgpropid[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            IPropertyStorage * This,
            /* [in] */ DWORD grfCommitFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Revert )( 
            IPropertyStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *Enum )( 
            IPropertyStorage * This,
            /* [out] */ IEnumSTATPROPSTG **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *SetTimes )( 
            IPropertyStorage * This,
            /* [in] */ const FILETIME *pctime,
            /* [in] */ const FILETIME *patime,
            /* [in] */ const FILETIME *pmtime);
        
        HRESULT ( STDMETHODCALLTYPE *SetClass )( 
            IPropertyStorage * This,
            /* [in] */ REFCLSID clsid);
        
        HRESULT ( STDMETHODCALLTYPE *Stat )( 
            IPropertyStorage * This,
            /* [out] */ STATPROPSETSTG *statpsstg);
        
        END_INTERFACE
    } IPropertyStorageVtbl;

    interface IPropertyStorage
    {
        CONST_VTBL struct IPropertyStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertyStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertyStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertyStorage_ReadMultiple(This,cpspec,rgpspec,rgpropvar)	\
    ( (This)->lpVtbl -> ReadMultiple(This,cpspec,rgpspec,rgpropvar) ) 

#define IPropertyStorage_WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst)	\
    ( (This)->lpVtbl -> WriteMultiple(This,cpspec,rgpspec,rgpropvar,propidNameFirst) ) 

#define IPropertyStorage_DeleteMultiple(This,cpspec,rgpspec)	\
    ( (This)->lpVtbl -> DeleteMultiple(This,cpspec,rgpspec) ) 

#define IPropertyStorage_ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName)	\
    ( (This)->lpVtbl -> ReadPropertyNames(This,cpropid,rgpropid,rglpwstrName) ) 

#define IPropertyStorage_WritePropertyNames(This,cpropid,rgpropid,rglpwstrName)	\
    ( (This)->lpVtbl -> WritePropertyNames(This,cpropid,rgpropid,rglpwstrName) ) 

#define IPropertyStorage_DeletePropertyNames(This,cpropid,rgpropid)	\
    ( (This)->lpVtbl -> DeletePropertyNames(This,cpropid,rgpropid) ) 

#define IPropertyStorage_Commit(This,grfCommitFlags)	\
    ( (This)->lpVtbl -> Commit(This,grfCommitFlags) ) 

#define IPropertyStorage_Revert(This)	\
    ( (This)->lpVtbl -> Revert(This) ) 

#define IPropertyStorage_Enum(This,ppenum)	\
    ( (This)->lpVtbl -> Enum(This,ppenum) ) 

#define IPropertyStorage_SetTimes(This,pctime,patime,pmtime)	\
    ( (This)->lpVtbl -> SetTimes(This,pctime,patime,pmtime) ) 

#define IPropertyStorage_SetClass(This,clsid)	\
    ( (This)->lpVtbl -> SetClass(This,clsid) ) 

#define IPropertyStorage_Stat(This,statpsstg)	\
    ( (This)->lpVtbl -> Stat(This,statpsstg) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPropertyStorage_INTERFACE_DEFINED__ */


#ifndef __IPropertySetStorage_INTERFACE_DEFINED__
#define __IPropertySetStorage_INTERFACE_DEFINED__

/* interface IPropertySetStorage */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IPropertySetStorage *LPPROPERTYSETSTORAGE;


EXTERN_C const IID IID_IPropertySetStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000013A-0000-0000-C000-000000000046")
    IPropertySetStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ REFFMTID rfmtid,
            /* [unique][in] */ const CLSID *pclsid,
            /* [in] */ DWORD grfFlags,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage **ppprstg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ REFFMTID rfmtid,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage **ppprstg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ REFFMTID rfmtid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enum( 
            /* [out] */ IEnumSTATPROPSETSTG **ppenum) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IPropertySetStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertySetStorage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertySetStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertySetStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *Create )( 
            IPropertySetStorage * This,
            /* [in] */ REFFMTID rfmtid,
            /* [unique][in] */ const CLSID *pclsid,
            /* [in] */ DWORD grfFlags,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage **ppprstg);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IPropertySetStorage * This,
            /* [in] */ REFFMTID rfmtid,
            /* [in] */ DWORD grfMode,
            /* [out] */ IPropertyStorage **ppprstg);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IPropertySetStorage * This,
            /* [in] */ REFFMTID rfmtid);
        
        HRESULT ( STDMETHODCALLTYPE *Enum )( 
            IPropertySetStorage * This,
            /* [out] */ IEnumSTATPROPSETSTG **ppenum);
        
        END_INTERFACE
    } IPropertySetStorageVtbl;

    interface IPropertySetStorage
    {
        CONST_VTBL struct IPropertySetStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertySetStorage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertySetStorage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertySetStorage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertySetStorage_Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg)	\
    ( (This)->lpVtbl -> Create(This,rfmtid,pclsid,grfFlags,grfMode,ppprstg) ) 

#define IPropertySetStorage_Open(This,rfmtid,grfMode,ppprstg)	\
    ( (This)->lpVtbl -> Open(This,rfmtid,grfMode,ppprstg) ) 

#define IPropertySetStorage_Delete(This,rfmtid)	\
    ( (This)->lpVtbl -> Delete(This,rfmtid) ) 

#define IPropertySetStorage_Enum(This,ppenum)	\
    ( (This)->lpVtbl -> Enum(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPropertySetStorage_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSTG_INTERFACE_DEFINED__

/* interface IEnumSTATPROPSTG */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumSTATPROPSTG *LPENUMSTATPROPSTG;


EXTERN_C const IID IID_IEnumSTATPROPSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000139-0000-0000-C000-000000000046")
    IEnumSTATPROPSTG : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumSTATPROPSTG **ppenum) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumSTATPROPSTGVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumSTATPROPSTG * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumSTATPROPSTG * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumSTATPROPSTG * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumSTATPROPSTG * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumSTATPROPSTG * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumSTATPROPSTG * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumSTATPROPSTG * This,
            /* [out] */ IEnumSTATPROPSTG **ppenum);
        
        END_INTERFACE
    } IEnumSTATPROPSTGVtbl;

    interface IEnumSTATPROPSTG
    {
        CONST_VTBL struct IEnumSTATPROPSTGVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATPROPSTG_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumSTATPROPSTG_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumSTATPROPSTG_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumSTATPROPSTG_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumSTATPROPSTG_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumSTATPROPSTG_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumSTATPROPSTG_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumSTATPROPSTG_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATPROPSTG_INTERFACE_DEFINED__ */


#ifndef __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__
#define __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__

/* interface IEnumSTATPROPSETSTG */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumSTATPROPSETSTG *LPENUMSTATPROPSETSTG;


EXTERN_C const IID IID_IEnumSTATPROPSETSTG;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000013B-0000-0000-C000-000000000046")
    IEnumSTATPROPSETSTG : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumSTATPROPSETSTG **ppenum) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IEnumSTATPROPSETSTGVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumSTATPROPSETSTG * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumSTATPROPSETSTG * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumSTATPROPSETSTG * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumSTATPROPSETSTG * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumSTATPROPSETSTG * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumSTATPROPSETSTG * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumSTATPROPSETSTG * This,
            /* [out] */ IEnumSTATPROPSETSTG **ppenum);
        
        END_INTERFACE
    } IEnumSTATPROPSETSTGVtbl;

    interface IEnumSTATPROPSETSTG
    {
        CONST_VTBL struct IEnumSTATPROPSETSTGVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumSTATPROPSETSTG_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEnumSTATPROPSETSTG_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEnumSTATPROPSETSTG_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEnumSTATPROPSETSTG_Next(This,celt,rgelt,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched) ) 

#define IEnumSTATPROPSETSTG_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IEnumSTATPROPSETSTG_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IEnumSTATPROPSETSTG_Clone(This,ppenum)	\
    ( (This)->lpVtbl -> Clone(This,ppenum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_RemoteNext_Proxy( 
    IEnumSTATPROPSETSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumSTATPROPSETSTG_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumSTATPROPSETSTG_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_propidl_0000_0004 */
/* [local] */ 

typedef struct SERIALIZEDPROPERTYVALUE
    {
    DWORD dwType;
    BYTE rgb[ 1 ];
    } 	SERIALIZEDPROPERTYVALUE;

HRESULT WINAPI FreePropVariantArray(ULONG,PROPVARIANT*);
HRESULT WINAPI PropVariantClear(PROPVARIANT*);
HRESULT WINAPI PropVariantCopy(PROPVARIANT*,const PROPVARIANT*);

#define _PROPVARIANT_INIT_DEFINED_
#define PropVariantInit(p) memset((p), 0, sizeof(PROPVARIANT))

#ifndef _STGCREATEPROPSTG_DEFINED_
#define _STGCREATEPROPSTG_DEFINED_
HRESULT WINAPI StgCreatePropStg(IUnknown *, REFFMTID, const CLSID *, DWORD, DWORD, IPropertyStorage **);
HRESULT WINAPI StgOpenPropStg(IUnknown *, REFFMTID, DWORD, DWORD, IPropertyStorage **);
HRESULT WINAPI StgCreatePropSetStg(IStorage *, DWORD, IPropertySetStorage **);
#define CCH_MAX_PROPSTG_NAME 31
HRESULT WINAPI FmtIdToPropStgName(const FMTID *, LPOLESTR);
HRESULT WINAPI PropStgNameToFmtId(const LPOLESTR, FMTID *);
#endif /* _STGCREATEPROPSTG_DEFINED_ */



extern RPC_IF_HANDLE __MIDL_itf_propidl_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_propidl_0000_0004_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_Next_Proxy( 
    IEnumSTATPROPSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSTG_Next_Stub( 
    IEnumSTATPROPSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_Next_Proxy( 
    IEnumSTATPROPSETSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumSTATPROPSETSTG_Next_Stub( 
    IEnumSTATPROPSETSTG * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ STATPROPSETSTG *rgelt,
    /* [out] */ ULONG *pceltFetched);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


