// ThompsonsAlg.h : Declaration of the CThompsonsAlg

#pragma once
#include "resource.h"       // main symbols



#include "Thompsons_i.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include "..\NFSM3\ThompsonsAlgImpl.h"
#include "..\NFSM3\Logger.h"
#include "..\NFSM3\Exceptions.h"
#include "..\NFSM3\SimpleLogger.h"




#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CThompsonsAlg

class ATL_NO_VTABLE CThompsonsAlg :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThompsonsAlg, &CLSID_ThompsonsAlg>,
	public IDispatchImpl<IThompsonsAlg, &IID_IThompsonsAlg, &LIBID_ThompsonsLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CThompsonsAlg(): m_transfAlg(nullptr), m_regexpr(""), m_log(L"")
	{

	}

DECLARE_REGISTRY_RESOURCEID(IDR_THOMPSONSALG)


BEGIN_COM_MAP(CThompsonsAlg)
	COM_INTERFACE_ENTRY(IThompsonsAlg)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:


	STDMETHOD(SetRegExpr)(BSTR RegExprStr);
	STDMETHOD(Transform)(UINT* InitState, UINT* FinalState);
private:
	std::string m_regexpr;
	std::unique_ptr<Thompsons> m_transfAlg;
	_bstr_t m_log;
public:
	STDMETHOD(GetStates)(UINT* States);
	STDMETHOD(GetNumberOfStates)(UINT* NStates);

	STDMETHOD(GetLog)(BSTR* Log);
	STDMETHOD(GetLog2)(UINT* Log);
	STDMETHOD(GetStatesArray)(UINT* StartOfArray, UINT* SizeOgArray);
};

OBJECT_ENTRY_AUTO(__uuidof(ThompsonsAlg), CThompsonsAlg)
