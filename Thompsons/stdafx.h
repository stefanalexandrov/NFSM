// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <afxodlgs.h>
#include <afxrich.h>
#include <afxhtml.h>
#include <afxcview.h>
#include <afxwinappex.h>
#include <afxframewndex.h>
#include <afxmdiframewndex.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

const int MAX_NUMBER_OF_STATES = 100000;
const int MAX_NUMBER_OF_NFSM_COPIES = 1000;
const char LAMBDA_CH = '§'; //character to represent a lambda-transition
const char BETA_CH = '#'; //character to represent a beta-transition
						  //(like lambda-transition, but consumes one symbol) for "." metachar
const char BRACKET_DELIM = '$';
const char OR_DELIM = '%';
#include <vector>
static std::vector<char> METACHAR_ = { '(',')','*','+','?', '|' }; // '.' is not treated as metachar for simplisity
#include <comutil.h>
