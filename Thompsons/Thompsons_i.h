

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Sep 07 12:16:10 2016
 */
/* Compiler settings for Thompsons.idl:
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

#ifndef __Thompsons_i_h__
#define __Thompsons_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IThompsonsAlg_FWD_DEFINED__
#define __IThompsonsAlg_FWD_DEFINED__
typedef interface IThompsonsAlg IThompsonsAlg;

#endif 	/* __IThompsonsAlg_FWD_DEFINED__ */


#ifndef __ThompsonsAlg_FWD_DEFINED__
#define __ThompsonsAlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class ThompsonsAlg ThompsonsAlg;
#else
typedef struct ThompsonsAlg ThompsonsAlg;
#endif /* __cplusplus */

#endif 	/* __ThompsonsAlg_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IThompsonsAlg_INTERFACE_DEFINED__
#define __IThompsonsAlg_INTERFACE_DEFINED__

/* interface IThompsonsAlg */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IThompsonsAlg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5A59C3B0-56FF-4ACA-B082-8FAD71376878")
    IThompsonsAlg : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetRegExpr( 
            /* [in] */ BSTR RegExprStr) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Transform( 
            /* [out] */ UINT *InitState,
            /* [out] */ UINT *FinalState) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStates( 
            /* [out] */ UINT *States) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetNumberOfStates( 
            /* [out] */ UINT *NStates) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLog( 
            /* [out] */ BSTR *Log) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLog2( 
            UINT *Log) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStatesArray( 
            /* [out] */ UINT *StartOfArray,
            /* [out] */ UINT *SizeOgArray) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IThompsonsAlgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IThompsonsAlg * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IThompsonsAlg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IThompsonsAlg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IThompsonsAlg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IThompsonsAlg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IThompsonsAlg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IThompsonsAlg * This,
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
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetRegExpr )( 
            IThompsonsAlg * This,
            /* [in] */ BSTR RegExprStr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Transform )( 
            IThompsonsAlg * This,
            /* [out] */ UINT *InitState,
            /* [out] */ UINT *FinalState);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStates )( 
            IThompsonsAlg * This,
            /* [out] */ UINT *States);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetNumberOfStates )( 
            IThompsonsAlg * This,
            /* [out] */ UINT *NStates);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLog )( 
            IThompsonsAlg * This,
            /* [out] */ BSTR *Log);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLog2 )( 
            IThompsonsAlg * This,
            UINT *Log);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatesArray )( 
            IThompsonsAlg * This,
            /* [out] */ UINT *StartOfArray,
            /* [out] */ UINT *SizeOgArray);
        
        END_INTERFACE
    } IThompsonsAlgVtbl;

    interface IThompsonsAlg
    {
        CONST_VTBL struct IThompsonsAlgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IThompsonsAlg_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IThompsonsAlg_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IThompsonsAlg_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IThompsonsAlg_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IThompsonsAlg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IThompsonsAlg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IThompsonsAlg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IThompsonsAlg_SetRegExpr(This,RegExprStr)	\
    ( (This)->lpVtbl -> SetRegExpr(This,RegExprStr) ) 

#define IThompsonsAlg_Transform(This,InitState,FinalState)	\
    ( (This)->lpVtbl -> Transform(This,InitState,FinalState) ) 

#define IThompsonsAlg_GetStates(This,States)	\
    ( (This)->lpVtbl -> GetStates(This,States) ) 

#define IThompsonsAlg_GetNumberOfStates(This,NStates)	\
    ( (This)->lpVtbl -> GetNumberOfStates(This,NStates) ) 

#define IThompsonsAlg_GetLog(This,Log)	\
    ( (This)->lpVtbl -> GetLog(This,Log) ) 

#define IThompsonsAlg_GetLog2(This,Log)	\
    ( (This)->lpVtbl -> GetLog2(This,Log) ) 

#define IThompsonsAlg_GetStatesArray(This,StartOfArray,SizeOgArray)	\
    ( (This)->lpVtbl -> GetStatesArray(This,StartOfArray,SizeOgArray) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IThompsonsAlg_INTERFACE_DEFINED__ */



#ifndef __ThompsonsLib_LIBRARY_DEFINED__
#define __ThompsonsLib_LIBRARY_DEFINED__

/* library ThompsonsLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_ThompsonsLib;

EXTERN_C const CLSID CLSID_ThompsonsAlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6F7C0D0B-F6C1-48DD-B6C0-AB408846F52E")
ThompsonsAlg;
#endif
#endif /* __ThompsonsLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


