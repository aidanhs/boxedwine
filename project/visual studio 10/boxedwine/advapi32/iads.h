

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __iads_h__
#define __iads_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IADsContainer_FWD_DEFINED__
#define __IADsContainer_FWD_DEFINED__
typedef interface IADsContainer IADsContainer;
#endif 	/* __IADsContainer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_iads_0000_0000 */
/* [local] */ 

typedef /* [public] */ 
enum __MIDL___MIDL_itf_iads_0000_0000_0001
    {	ADS_RIGHT_DS_CREATE_CHILD	= 0x1,
	ADS_RIGHT_DS_DELETE_CHILD	= 0x2,
	ADS_RIGHT_ACTRL_DS_LIST	= 0x4,
	ADS_RIGHT_DS_SELF	= 0x8,
	ADS_RIGHT_DS_READ_PROP	= 0x10,
	ADS_RIGHT_DS_WRITE_PROP	= 0x20,
	ADS_RIGHT_DS_DELETE_TREE	= 0x40,
	ADS_RIGHT_DS_LIST_OBJECT	= 0x80,
	ADS_RIGHT_DS_CONTROL_ACCESS	= 0x100,
	ADS_RIGHT_DELETE	= 0x10000,
	ADS_RIGHT_READ_CONTROL	= 0x20000,
	ADS_RIGHT_WRITE_DAC	= 0x40000,
	ADS_RIGHT_WRITE_OWNER	= 0x80000,
	ADS_RIGHT_SYNCHRONIZE	= 0x100000,
	ADS_RIGHT_ACCESS_SYSTEM_SECURITY	= 0x200000,
	ADS_RIGHT_GENERIC_ALL	= 0x10000000,
	ADS_RIGHT_GENERIC_EXECUTE	= 0x20000000,
	ADS_RIGHT_GENERIC_WRITE	= 0x40000000,
	ADS_RIGHT_GENERIC_READ	= 0x80000000
    } 	ADS_RIGHTS_ENUM;



extern RPC_IF_HANDLE __MIDL_itf_iads_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_iads_0000_0000_v0_0_s_ifspec;

#ifndef __IADsContainer_INTERFACE_DEFINED__
#define __IADsContainer_INTERFACE_DEFINED__

/* interface IADsContainer */
/* [uuid][object] */ 


EXTERN_C const IID IID_IADsContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("001677d0-fd16-11ce-abc4-02608c9e7553")
    IADsContainer : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *retval) = 0;
        
        virtual /* [restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **retval) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filter( 
            /* [retval][out] */ VARIANT *pvFilter) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Filter( 
            /* [in] */ VARIANT vFilter) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Hints( 
            /* [retval][out] */ VARIANT *pvHints) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Hints( 
            /* [in] */ VARIANT vHints) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObject( 
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName,
            /* [retval][out] */ IDispatch **ppObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName,
            /* [retval][out] */ IDispatch **ppObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyHere( 
            /* [in] */ BSTR bstrSourceName,
            /* [in] */ BSTR bstrNewName,
            /* [retval][out] */ IDispatch **ppObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveHere( 
            /* [in] */ BSTR bstrSourceName,
            /* [in] */ BSTR bstrNewName,
            /* [retval][out] */ IDispatch **ppObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IADsContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IADsContainer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IADsContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IADsContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IADsContainer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IADsContainer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IADsContainer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IADsContainer * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IADsContainer * This,
            /* [retval][out] */ long *retval);
        
        /* [restricted][propget] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IADsContainer * This,
            /* [retval][out] */ IUnknown **retval);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Filter )( 
            IADsContainer * This,
            /* [retval][out] */ VARIANT *pvFilter);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Filter )( 
            IADsContainer * This,
            /* [in] */ VARIANT vFilter);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Hints )( 
            IADsContainer * This,
            /* [retval][out] */ VARIANT *pvHints);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Hints )( 
            IADsContainer * This,
            /* [in] */ VARIANT vHints);
        
        HRESULT ( STDMETHODCALLTYPE *GetObject )( 
            IADsContainer * This,
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName,
            /* [retval][out] */ IDispatch **ppObject);
        
        HRESULT ( STDMETHODCALLTYPE *Create )( 
            IADsContainer * This,
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName,
            /* [retval][out] */ IDispatch **ppObject);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IADsContainer * This,
            /* [in] */ BSTR bstrClassName,
            /* [in] */ BSTR bstrRelativeName);
        
        HRESULT ( STDMETHODCALLTYPE *CopyHere )( 
            IADsContainer * This,
            /* [in] */ BSTR bstrSourceName,
            /* [in] */ BSTR bstrNewName,
            /* [retval][out] */ IDispatch **ppObject);
        
        HRESULT ( STDMETHODCALLTYPE *MoveHere )( 
            IADsContainer * This,
            /* [in] */ BSTR bstrSourceName,
            /* [in] */ BSTR bstrNewName,
            /* [retval][out] */ IDispatch **ppObject);
        
        END_INTERFACE
    } IADsContainerVtbl;

    interface IADsContainer
    {
        CONST_VTBL struct IADsContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IADsContainer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IADsContainer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IADsContainer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IADsContainer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IADsContainer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IADsContainer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IADsContainer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IADsContainer_get_Count(This,retval)	\
    ( (This)->lpVtbl -> get_Count(This,retval) ) 

#define IADsContainer_get__NewEnum(This,retval)	\
    ( (This)->lpVtbl -> get__NewEnum(This,retval) ) 

#define IADsContainer_get_Filter(This,pvFilter)	\
    ( (This)->lpVtbl -> get_Filter(This,pvFilter) ) 

#define IADsContainer_put_Filter(This,vFilter)	\
    ( (This)->lpVtbl -> put_Filter(This,vFilter) ) 

#define IADsContainer_get_Hints(This,pvHints)	\
    ( (This)->lpVtbl -> get_Hints(This,pvHints) ) 

#define IADsContainer_put_Hints(This,vHints)	\
    ( (This)->lpVtbl -> put_Hints(This,vHints) ) 

#define IADsContainer_GetObject(This,bstrClassName,bstrRelativeName,ppObject)	\
    ( (This)->lpVtbl -> GetObject(This,bstrClassName,bstrRelativeName,ppObject) ) 

#define IADsContainer_Create(This,bstrClassName,bstrRelativeName,ppObject)	\
    ( (This)->lpVtbl -> Create(This,bstrClassName,bstrRelativeName,ppObject) ) 

#define IADsContainer_Delete(This,bstrClassName,bstrRelativeName)	\
    ( (This)->lpVtbl -> Delete(This,bstrClassName,bstrRelativeName) ) 

#define IADsContainer_CopyHere(This,bstrSourceName,bstrNewName,ppObject)	\
    ( (This)->lpVtbl -> CopyHere(This,bstrSourceName,bstrNewName,ppObject) ) 

#define IADsContainer_MoveHere(This,bstrSourceName,bstrNewName,ppObject)	\
    ( (This)->lpVtbl -> MoveHere(This,bstrSourceName,bstrNewName,ppObject) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IADsContainer_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


