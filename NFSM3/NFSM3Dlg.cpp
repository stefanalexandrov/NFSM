
// NFSM3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "NFSM3.h"
#include "NFSM3Dlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"
#include "nfsm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	EnableActiveAccessibility();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNFSM3Dlg dialog


IMPLEMENT_DYNAMIC(CNFSM3Dlg, CDialogEx);

CNFSM3Dlg::CNFSM3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_NFSM3_DIALOG, pParent)
{
	EnableActiveAccessibility();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
	correct = false;
}

CNFSM3Dlg::~CNFSM3Dlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CNFSM3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNFSM3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CNFSM3Dlg::OnCheck)
	ON_BN_CLICKED(IDC_BUTTON4, &CNFSM3Dlg::OnRunNFSM)
	ON_BN_CLICKED(IDC_BUTTON1, &CNFSM3Dlg::OnTransform)
	ON_BN_CLICKED(IDC_BUTTON5, &CNFSM3Dlg::OnFormal)
	ON_EN_CHANGE(IDC_EDIT1, &CNFSM3Dlg::OnChangeRegExpr)
	ON_STN_CLICKED(IDC_STATIC4, &CNFSM3Dlg::OnStnClickedStatic4)
END_MESSAGE_MAP()


// CNFSM3Dlg message handlers

BOOL CNFSM3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CWnd * wnd_reg_expr = GetDlgItem(IDC_EDIT1);
	wnd_reg_expr->SendMessage(EM_LIMITTEXT, (WPARAM)MAX_REG_EXPR_SIZE, 0);
	CWnd * wnd_transform = GetDlgItem(IDC_BUTTON1);
	wnd_transform->EnableWindow(FALSE);
	CWnd * wnd_run = GetDlgItem(IDC_BUTTON4);
	wnd_run->EnableWindow(FALSE);
	CWnd * wnd_formal = GetDlgItem(IDC_BUTTON5);
	wnd_formal->EnableWindow(FALSE);
	CButton * wnd_box = (CButton *)GetDlgItem(IDC_CHECK2);
	wnd_box->SetCheck(TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNFSM3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNFSM3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNFSM3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CNFSM3Dlg::OnClose()
{
	if (CanExit())
		CDialogEx::OnClose();
}

void CNFSM3Dlg::OnOK()
{
	//if (CanExit())
		//CDialogEx::OnOK();
}

void CNFSM3Dlg::OnCancel()
{
	if (CanExit())
		CDialogEx::OnCancel();
}

BOOL CNFSM3Dlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}



void CNFSM3Dlg::OnCheck()
{
	// TODO: Add your control notification handler code here
	CWnd * wnd_reg_expr = GetDlgItem(IDC_EDIT1);
	CWnd * wnd_output = GetDlgItem(IDC_EDIT3);
	CWnd * wnd_progress = GetDlgItem(IDC_PROGCTRL1);
	CWnd * wnd_transform = GetDlgItem(IDC_BUTTON1);
	
	
	int reg_expr_size = wnd_reg_expr->GetWindowTextLength();
	int output_size = wnd_output->GetWindowTextLength();
	if (reg_expr_size > MAX_REG_EXPR_SIZE) {
		output_error_ws = _T("Error: regular expression exceeds maximum allowed length: \r\n") + MAX_REG_EXPR_SIZE;
		wnd_output->SetWindowTextW(output_error_ws.c_str());
		return;
	}

	if (reg_expr_size == 0) {
		output_error_ws = _T("Error: no regular expression entered. \r\n");
		wnd_output->SetWindowTextW(output_error_ws.c_str());
		return;
	}

	reg_expr_ws = read_output_wnd(wnd_reg_expr); // from now on we work only with this wariable

	if (output_size > 0) {
		output_normal_ws = read_output_wnd(wnd_output);
	}
	//set up progress bar
	wnd_progress->SendMessage(PBM_SETRANGE, 0, MAKELPARAM(0, reg_expr_size));
	wnd_progress->SendMessage(PBM_SETSTEP, (WPARAM)1, 0);
	
	
	output_normal_ws.append(_T("Checking regular expression: \r\n"));
	output_normal_ws.append(reg_expr_ws);
	wnd_output->SetWindowTextW(output_normal_ws.c_str());


	wnd_progress->SendMessage(PBM_STEPIT, 0, 0);
	reg_expr_s = std::string(reg_expr_ws.begin(), reg_expr_ws.end());

	if (check_reg_expr(reg_expr_s) == -1) {
		//wnd_output->SetWindowTextW(_T("  -> incorrect!"));
		output_normal_ws.append(_T("  -> incorrect! \r\n"));
		wnd_transform->EnableWindow(FALSE);
		correct = false;
	} else {
		//wnd_output->SetWindowTextW(_T("  -> correct!"));
		output_normal_ws.append(_T("  -> correct! \r\n"));
		wnd_transform->EnableWindow(TRUE);
		correct = true;
	}

	wnd_output->SetWindowTextW(output_normal_ws.c_str());
	//wnd_output->SetWindowTextW(_T("\r\n"));
	wnd_progress->SendMessage(PBM_SETPOS, reg_expr_size, 0);
}

int check_reg_expr(std::string s) {

	std::size_t bs = 0;
	char bad_symbols[] = { '#','%','§','$','#','/','\"','\'', '&', '{', '}', '[', ']' };
	for (int i = 0; i < sizeof(bad_symbols); i++)
		if (s.find(bad_symbols[i]) != std::string::npos)
			return -1;

	std::vector<std::regex>  bad;
	bad.push_back(std::regex("(.*)([\*,\+,\?][*,+,?]+)(.*)"));
	bad.push_back(std::regex("(.*)([(][)])(.*)"));
	bad.push_back(std::regex("(.*)([(][*,+,?])(.*)"));
	bad.push_back(std::regex("(.*)([|][*,+,?])(.*)"));

	//std::regex bad_brackets("(.*)([\(]+)(.*)([\)]+)");
	std::smatch pieces_match;

	for (std::vector<std::regex>::iterator it = bad.begin(); it != bad.end(); ++it) {
		if (std::regex_match(s, pieces_match, *it)) {
			//std::cout << fname << '\n';
			if (pieces_match.size()) {
				return -1;
			}
		}
	}
	//check brackets
	if (std::count(s.begin(), s.end(), '(') != std::count(s.begin(), s.end(), ')') )
		return -1;
	/*
	//check that each right bracket has a *, + or ? after it
	if (s.back() == ')')
		return -1;
	for (std::string::iterator i = s.begin(); i != s.end(); i++) {
		if (*i == ')' && !is_star_plus_quest(*(++i)))
			return -1;
	}
	*/

	return 0;
}

void CNFSM3Dlg::OnChangeRegExpr() {
	CWnd * wnd_output = GetDlgItem(IDC_EDIT3);
	CWnd * wnd_progress = GetDlgItem(IDC_PROGCTRL1);
	CWnd * wnd_transform = GetDlgItem(IDC_BUTTON1);
	wnd_output->SetWindowTextW(_T(""));
	output_normal_ws = _T("");
	wnd_progress->SendMessage(PBM_SETPOS, 0, 0);
	wnd_transform->EnableWindow(FALSE);
	CWnd * wnd_run = GetDlgItem(IDC_BUTTON4);
	wnd_run->EnableWindow(FALSE);
	CWnd * wnd_formal = GetDlgItem(IDC_BUTTON5);
	wnd_formal->EnableWindow(FALSE);

	CWnd * wnd_formal_result = GetDlgItem(IDC_STATIC2);
	wnd_formal_result->SetWindowTextW(_T("not known"));
	if (m_nfsm.is_constructed()) {
		m_nfsm.clean();
	}
	return;
}

void CNFSM3Dlg::OnTransform() {
	CWnd * wnd_reg_expr = GetDlgItem(IDC_EDIT1);
	CWnd * wnd_output = GetDlgItem(IDC_EDIT3);
	CWnd * wnd_progress = GetDlgItem(IDC_PROGCTRL1);
	CWnd * wnd_transform = GetDlgItem(IDC_BUTTON1);
	CButton * chb_opt = (CButton *)GetDlgItem(IDC_CHECK2);
	//m_nfsm = new NFSM(reg_expr_s, wnd_output);
	m_nfsm.set_out_wnd(wnd_output);
	m_nfsm.set_regexpr(reg_expr_s);
	m_nfsm.construct();
	if (chb_opt->GetCheck() == BST_CHECKED)
		m_nfsm.optimize();
	m_nfsm.write_nfsm("output.txt");
	//enable run button
	CWnd * wnd_run = GetDlgItem(IDC_BUTTON4);
	wnd_run->EnableWindow(TRUE);
	//enable formal button
	CWnd * wnd_formal = GetDlgItem(IDC_BUTTON5);
	wnd_formal->EnableWindow(TRUE);
	wnd_transform->EnableWindow(FALSE);
	
}
void CNFSM3Dlg::OnFormal() {
	CWnd * wnd_output = GetDlgItem(IDC_STATIC2);
	CWnd * wnd_formal = GetDlgItem(IDC_BUTTON5);
	std::wstring output_ws;
	RUN check(&m_nfsm, wnd_output);
	//check = std::move(RUN(m_nfsm));
	int bound = check.formal(0);
	TCHAR number[50];
	swprintf_s(number, 50, _T("%d"), bound);
	output_ws.append(number);
	wnd_output->SetWindowTextW(output_ws.c_str());
	wnd_formal->EnableWindow(FALSE);
}

void CNFSM3Dlg::OnRunNFSM()
{
	CWnd * wnd_string = GetDlgItem(IDC_EDIT4);
	CWnd * wnd_output = GetDlgItem(IDC_EDIT3);
	CWnd * wnd_result = GetDlgItem(IDC_STATIC4);

	std::wstring string_ws = read_output_wnd(wnd_string);
	std::wstring output_ws = read_output_wnd(wnd_output);
	std::string string_s;
	string_s = std::string(string_ws.begin(), string_ws.end());

	output_ws.append(_T("Matching string... "));
	output_ws.append(string_ws);
	output_ws.append(_T(" \r\n"));
	wnd_output->SetWindowTextW(output_ws.c_str());

	//RUN check = RUN(m_nfsm);
	RUN check(&m_nfsm, wnd_output);

	bool accepted = false;
	TransType result = TransType::NOT_FINAL;
	if (string_s.length() == 0) {
		result = check.make_transition();
		if (result == TransType::FINAL_STATE_LAMBDA)
			accepted = true;
		else if (result == TransType::NOT_FINAL_LAMBDA) {
			accepted = false;
		}
	}
	else {
		for (std::string::iterator i = string_s.begin(); i != string_s.end(); i++) {
			if (i == --string_s.end()) 
				result = check.make_transition(*i, true);
			else
				result = check.make_transition(*i, false);
			if ((result == TransType::FINAL_STATE || result == TransType::FINAL_STATE_LAMBDA) && i == --string_s.end())
				accepted = true;
			else if ((result == TransType::INVALID_TRANSITION || result == TransType::NOT_FINAL) && i == --string_s.end()) {
				accepted = false;
			}
		}
	}
	if (accepted) {
		wnd_result->SetWindowText(_T("accepted"));
		output_ws.append(_T("done... accepted!"));
	}
	else {
		wnd_result->SetWindowText(_T("declined"));
		output_ws.append(_T("done... declined!"));
	}

	output_ws.append(_T(" \r\n"));
}

void CNFSM3Dlg::OnStnClickedStatic4()
{
	// TODO: Add your control notification handler code here
}
