
// NewInstallerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NewInstaller.h"
#include "NewInstallerDlg.h"
#include "afxdialogex.h"
#include <CatchFishInstaller/Feature/Install/Installer.h>
#include <CatchFishInstaller/Feature/Socket/Client/IocpClient.h>
#include <CatchFishInstaller/Feature/Clear/ClearFile.h>
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
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNewInstallerDlg dialog



CNewInstallerDlg::CNewInstallerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEWINSTALLER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNewInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNewInstallerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_INSTALL, &CNewInstallerDlg::OnBnClickedBtnInstall)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CNewInstallerDlg message handlers
DWORD WINAPI _InitializeThread(_In_ LPVOID lpParam)
{
	CNewInstallerDlg* dlg = reinterpret_cast<CNewInstallerDlg *>(lpParam);
	if (::OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"CATCHFISH_INSTALLER") != NULL)
	{
		AfxMessageBox(L"安装程序已经运行了……禁止运行多个!");
		return 0;
	}


	::CreateMutexW(NULL, FALSE, L"CATCHFISH_INSTALLER");
	//libTools::CLog::GetInstance().SetClientName(L"CatchFishInstaller", libTools::CCharacter::MakeCurrentPath(L"\\"));
	if (!CIocpClient::GetInstance().Run(L"120.79.171.219", SERVER_PORT, 10 * 1000))
	{
		AfxMessageBox(L"连接服务器失败!");
		return 0;
	}

	reinterpret_cast<CButton *>(dlg->GetDlgItem(IDC_BTN_INSTALL))->EnableWindow(TRUE);
	return 0;
}

BOOL CNewInstallerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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
	::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)_InitializeThread, this, NULL, NULL);
	reinterpret_cast<CButton *>(this->GetDlgItem(IDC_BTN_INSTALL))->EnableWindow(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNewInstallerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CNewInstallerDlg::OnPaint()
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
HCURSOR CNewInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI _InstallThread(LPVOID lpParam)
{
	CNewInstallerDlg* dlg = reinterpret_cast<CNewInstallerDlg *>(lpParam);
	CInstaller::GetInstance().Install(L"");
	return 0;
}

void CNewInstallerDlg::OnBnClickedBtnInstall()
{
	/*if (CClearFile::GetInstance().ClearGameRestoreSystem_YouLingQianTing("F:\\Persi0.sys"))
	{
		AfxMessageBox(L"YES!");
	}*/
	CInstaller::GetInstance().SetPrinter([this](CONST std::wstring& wsText)
	{
		CEdit* pEdit = reinterpret_cast<CEdit *>(this->GetDlgItem(IDC_EDIT_LOG));
		if (pEdit != nullptr)
		{
			CStringW str;
			pEdit->GetWindowTextW(str);
			str += wsText.c_str();
			str += "\r\n";
			pEdit->SetWindowTextW(str);
			pEdit->LineScroll(pEdit->GetLineCount());
		}
	});

	reinterpret_cast<CButton *>(this->GetDlgItem(IDC_BTN_INSTALL))->EnableWindow(FALSE);
	_hThread = ::CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)_InstallThread, this, NULL, NULL);
}


void CNewInstallerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (::WaitForSingleObject(_hThread, 1000) == WAIT_TIMEOUT)
	{
		::TerminateProcess(::GetCurrentProcess(), 0);
	}
	CDialogEx::OnClose();
}
