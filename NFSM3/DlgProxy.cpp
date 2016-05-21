
// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "NFSM3.h"
#include "DlgProxy.h"
#include "NFSM3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNFSM3DlgAutoProxy

IMPLEMENT_DYNCREATE(CNFSM3DlgAutoProxy, CCmdTarget)

CNFSM3DlgAutoProxy::CNFSM3DlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CNFSM3Dlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CNFSM3Dlg)))
		{
			m_pDialog = reinterpret_cast<CNFSM3Dlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CNFSM3DlgAutoProxy::~CNFSM3DlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CNFSM3DlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CNFSM3DlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CNFSM3DlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// Note: we add support for IID_INFSM3 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {E55AB773-13A7-4E4A-AB24-38964C55F6DB}
static const IID IID_INFSM3 =
{ 0xE55AB773, 0x13A7, 0x4E4A, { 0xAB, 0x24, 0x38, 0x96, 0x4C, 0x55, 0xF6, 0xDB } };

BEGIN_INTERFACE_MAP(CNFSM3DlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CNFSM3DlgAutoProxy, IID_INFSM3, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {2EB3DE97-9377-4663-93A1-68D235066A39}
IMPLEMENT_OLECREATE2(CNFSM3DlgAutoProxy, "NFSM3.Application", 0x2eb3de97, 0x9377, 0x4663, 0x93, 0xa1, 0x68, 0xd2, 0x35, 0x6, 0x6a, 0x39)


// CNFSM3DlgAutoProxy message handlers
