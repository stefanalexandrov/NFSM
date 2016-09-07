// ThompsonsAlg.cpp : Implementation of CThompsonsAlg

#include "stdafx.h"
#include "ThompsonsAlg.h"
#include <comdef.h>



// CThompsonsAlg



STDMETHODIMP CThompsonsAlg::SetRegExpr(BSTR RegExprStr)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(RegExprStr != nullptr) {
		std::wstring wstmp(RegExprStr, SysStringLen(RegExprStr));
		m_regexpr = std::string(wstmp.begin(), wstmp.end());
		m_transfAlg = std::unique_ptr<Thompsons> (new Thompsons(m_regexpr));
	}

	return S_OK;
}


STDMETHODIMP CThompsonsAlg::Transform(UINT* InitState, UINT* FinalState)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here
	m_transfAlg->set_logging(true);
	StateCouple tmp = m_transfAlg->transform();
	*InitState = (UINT)tmp.m_init;
	*FinalState = (UINT)tmp.m_final;

	return S_OK;
}


STDMETHODIMP CThompsonsAlg::GetStates(UINT* States)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*States = (UINT)(m_transfAlg->get_states()).get();

	return S_OK;
}


STDMETHODIMP CThompsonsAlg::GetNumberOfStates(UINT* NStates)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here
	*NStates = m_transfAlg->get_number_of_states();

	return S_OK;
}



STDMETHODIMP CThompsonsAlg::GetLog(BSTR* Log)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here
	m_log = _bstr_t(m_transfAlg->get_log().c_str());
	Log = &m_log.GetBSTR();

	return S_OK;
}


STDMETHODIMP CThompsonsAlg::GetLog2(UINT* Log)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here
	m_log = _bstr_t(m_transfAlg->get_log().c_str());
	*Log = (UINT)m_log.copy();

	return S_OK;
}


STDMETHODIMP CThompsonsAlg::GetStatesArray(UINT* StartOfArray, UINT* SizeOgArray)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your implementation code here
	*StartOfArray = (UINT)(m_transfAlg->get_states()).get();
	*SizeOgArray = MAX_NUMBER_OF_STATES;

	return S_OK;
}
