
// NFSM3Dlg.h : header file
//
#pragma once
#define MAX_REG_EXPR_SIZE 500
#define MAX_OUTPUT_SIZE 2000
#include <string>
#include <regex>
#include <algorithm>
#include "nfsm.h"
#include "Resource.h"
class CNFSM3DlgAutoProxy;

struct bad_seq {
	std::regex reg;
};


// CNFSM3Dlg dialog
class CNFSM3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNFSM3Dlg);
	friend class CNFSM3DlgAutoProxy;

// Construction
public:
	CNFSM3Dlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CNFSM3Dlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NFSM3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CNFSM3DlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCheck();
	afx_msg void OnTransform();
	afx_msg void OnChangeRegExpr();
	afx_msg void OnRunNFSM();
	afx_msg void OnFormal();
	LPTSTR reg_expr;
	LPTSTR output;
	std::wstring reg_expr_ws;
	std::string reg_expr_s;
	bool correct;
	std::wstring output_normal_ws;
	std::wstring output_error_ws;
	NFSM m_nfsm;
	afx_msg void OnStnClickedStatic4();
	afx_msg void OnBnClickedCheck3();
};

int check_reg_expr(std::string s);