
// DlgProxy.h: header file
//

#pragma once

class CNFSM3Dlg;


// CNFSM3DlgAutoProxy command target

class CNFSM3DlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CNFSM3DlgAutoProxy)

	CNFSM3DlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CNFSM3Dlg* m_pDialog;

// Operations
public:

// Overrides
	public:
	virtual void OnFinalRelease();

// Implementation
protected:
	virtual ~CNFSM3DlgAutoProxy();

	// Generated message map functions

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CNFSM3DlgAutoProxy)

	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

