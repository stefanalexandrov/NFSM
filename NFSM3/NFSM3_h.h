

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jul 21 19:22:12 2016
 */
/* Compiler settings for NFSM3.idl:
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


#ifndef __NFSM3_h_h__
#define __NFSM3_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INFSM3_FWD_DEFINED__
#define __INFSM3_FWD_DEFINED__
typedef interface INFSM3 INFSM3;

#endif 	/* __INFSM3_FWD_DEFINED__ */


#ifndef __NFSM3_FWD_DEFINED__
#define __NFSM3_FWD_DEFINED__

#ifdef __cplusplus
typedef class NFSM3 NFSM3;
#else
typedef struct NFSM3 NFSM3;
#endif /* __cplusplus */

#endif 	/* __NFSM3_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __NFSM3_LIBRARY_DEFINED__
#define __NFSM3_LIBRARY_DEFINED__

/* library NFSM3 */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_NFSM3;

#ifndef __INFSM3_DISPINTERFACE_DEFINED__
#define __INFSM3_DISPINTERFACE_DEFINED__

/* dispinterface INFSM3 */
/* [uuid] */ 


EXTERN_C const IID DIID_INFSM3;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E55AB773-13A7-4E4A-AB24-38964C55F6DB")
    INFSM3 : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct INFSM3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INFSM3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INFSM3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INFSM3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INFSM3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INFSM3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INFSM3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INFSM3 * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } INFSM3Vtbl;

    interface INFSM3
    {
        CONST_VTBL struct INFSM3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INFSM3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INFSM3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INFSM3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INFSM3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INFSM3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INFSM3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INFSM3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __INFSM3_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_NFSM3;

#ifdef __cplusplus

class DECLSPEC_UUID("2EB3DE97-9377-4663-93A1-68D235066A39")
NFSM3;
#endif
#endif /* __NFSM3_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


