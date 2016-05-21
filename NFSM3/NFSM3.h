
// NFSM3.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CNFSM3App:
// See NFSM3.cpp for the implementation of this class
//

class CNFSM3App : public CWinApp
{
public:
	CNFSM3App();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNFSM3App theApp;