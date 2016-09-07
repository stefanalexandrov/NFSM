// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "Thompsons_i.h"
#include "dllmain.h"

CThompsonsModule _AtlModule;

class CThompsonsApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CThompsonsApp, CWinApp)
END_MESSAGE_MAP()

CThompsonsApp theApp;

BOOL CThompsonsApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CThompsonsApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
