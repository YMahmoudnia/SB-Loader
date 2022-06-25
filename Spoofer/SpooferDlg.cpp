
// SpooferDlg.cpp : implementation file
//


#include "pch.h"
#include "framework.h"
#include "Spoofer.h"
#include "SpooferDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char szName[] = "CrackMe01.loader";

void WriteTargetMemory(PROCESS_INFORMATION* piTarget, DWORD nAddress, BYTE* bData, BYTE nBytes)
{
	int nRet = 0;
	DWORD dwWritten = 0;

	if (piTarget->hProcess != INVALID_HANDLE_VALUE)
	{
		nRet = WriteProcessMemory(piTarget->hProcess, (void*)nAddress, bData, nBytes, &dwWritten);
	}
	if (dwWritten != nBytes || nRet == 0)
	{
		MessageBoxA(NULL, "Failed to patch remote process!", szName, MB_OK | MB_ICONERROR);
	}
}

VOID CSpooferDlg::errorHandle(LPTSTR pszMessage, DWORD dwLastError)
{
	CString translatedErrorNum;
	CString calledAPI = pszMessage;
	CString delimiter = _T("\n");

	HLOCAL errorMessage = NULL;
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (PTSTR)&errorMessage, 0, NULL))
	{
		translatedErrorNum = calledAPI + delimiter + (LPCWSTR)errorMessage;
		MessageBox(translatedErrorNum, L"SecureByte", MB_OK | MB_ICONERROR);
		LocalFree(errorMessage);
	}
}

BOOL CSpooferDlg::setAdjustToken()
{
	HANDLE tokenHandle;
	TOKEN_PRIVILEGES sTP;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle))
	{
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sTP.Privileges[0].Luid))
		{
			CloseHandle(tokenHandle);
			return FALSE;
		}

		sTP.PrivilegeCount = 1;
		sTP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(tokenHandle, 0, &sTP, sizeof(sTP), NULL, NULL))
		{
			CloseHandle(tokenHandle);
			return FALSE;
		}

		CloseHandle(tokenHandle);
		return TRUE;
	}
	return FALSE;
}

DWORD CSpooferDlg::getProcessID(const wchar_t* pName)
{
	PROCESSENTRY32 pEntry;
	HANDLE snapshotHandle;

	pEntry.dwSize = sizeof(PROCESSENTRY32);
	snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshotHandle, &pEntry) == TRUE) 
	{
		while (Process32Next(snapshotHandle, &pEntry) == TRUE) 
		{
			if (_wcsicmp(pEntry.szExeFile, pName) == 0) 
			{
				return pEntry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshotHandle);

	return 0;
}

VOID CSpooferDlg::getAllProcess()
{
	HANDLE processSnapHandle;
	
	PROCESSENTRY32 pe32;

	DWORD dwPriorityClass;

	processSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (processSnapHandle == INVALID_HANDLE_VALUE)
	{
		errorHandle(_T("CreateToolhelp32Snapshot"), GetLastError());
		return;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(processSnapHandle, &pe32))
	{
		errorHandle(_T("Process32First"), GetLastError());
		CloseHandle(processSnapHandle);
		return;
	}

	do
	{		
		p_combo1.AddString(pe32.szExeFile);

	} while (Process32Next(processSnapHandle, &pe32));

	CloseHandle(processSnapHandle);
}
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
public:
	afx_msg void OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()


// CSpooferDlg dialog



CSpooferDlg::CSpooferDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SPOOFER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpooferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, p_combo1);
	DDX_Control(pDX, IDC_EDIT1, child_process);
	DDX_Control(pDX, IDC_EDIT3, cmdline);
	DDX_Control(pDX, IDC_BUTTON1, btn_browse);
	DDX_Control(pDX, IDC_EDIT2, ppid);
}

BEGIN_MESSAGE_MAP(CSpooferDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSpooferDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CSpooferDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CSpooferDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON2, &CSpooferDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CSpooferDlg message handlers

BOOL CSpooferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	getAllProcess();
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpooferDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpooferDlg::OnPaint()
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
HCURSOR CSpooferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSpooferDlg::OnBnClickedOk()
{
	CString sParentProcess, sChildProcess, dPPID, pCmdLine, pDelimiter, fProcess;
	DWORD prentProcessID;

	p_combo1.GetWindowTextW(sParentProcess);
	child_process.GetWindowTextW(sChildProcess);
	ppid.GetWindowTextW(dPPID);
	cmdline.GetWindowTextW(pCmdLine);
	pDelimiter = _T(" ");

	STARTUPINFOEXW si;
	PROCESS_INFORMATION pi;
	SIZE_T attributeSize;
	ZeroMemory(&si, sizeof(STARTUPINFOEXW));

	setAdjustToken();

	if (sParentProcess.IsEmpty() && sChildProcess.IsEmpty())
	{
		MessageBox(L"Please select parent and child process", L"SecureByte Loader", MB_OK | MB_ICONERROR);
		return;
	}

	if (!sParentProcess.IsEmpty() && !dPPID.IsEmpty())
	{
		MessageBox(L"Please select one method for parent process", L"SecureByte Loader", MB_OK | MB_ICONERROR);
		p_combo1.SetCurSel(-1);
		ppid.SetWindowTextW(L"");
		return;
	}

	if (dPPID.IsEmpty())
	{
		if (sChildProcess.IsEmpty())
		{
			MessageBox(L"Please select child process", L"SecureByte Loader", MB_OK | MB_ICONERROR);
			return;
		}
		else if (sParentProcess.IsEmpty())
		{
			MessageBox(L"Please select parent process", L"SecureByte Loader", MB_OK | MB_ICONERROR);
			return;
		}

		prentProcessID = getProcessID(sParentProcess);
	}
	else
	{
		if (sChildProcess.IsEmpty())
		{
			MessageBox(L"Please select child process", L"SecureByte Loader", MB_OK | MB_ICONERROR);
			return;
		}

		prentProcessID = _wtol(dPPID);
	}

	HANDLE ppHandle = OpenProcess(MAXIMUM_ALLOWED, false, prentProcessID);

	if (NULL == ppHandle)
	{
		errorHandle(_T("OpenProcess"), GetLastError());
		return;
	}

	InitializeProcThreadAttributeList(NULL, 1, 0, &attributeSize);
	si.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attributeSize);
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &attributeSize);
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &ppHandle, sizeof(HANDLE), NULL, NULL);
	si.StartupInfo.cb = sizeof(STARTUPINFOEXW);

	fProcess = sChildProcess + pDelimiter + pCmdLine;

	if (!CreateProcess(NULL, fProcess.GetBuffer(MAX_PATH), NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED, NULL, NULL, &si.StartupInfo, &pi))
	{
		errorHandle(_T("CreateProcess"), GetLastError());
		return;
	}

	CONTEXT cxTarget = { 0 };
	cxTarget.ContextFlags = CONTEXT_ALL;
	GetThreadContext(pi.hThread, &cxTarget);

	DWORD dwImageBase = 0;
	ReadProcessMemory(pi.hProcess, (void*)(cxTarget.Ebx + 8), &dwImageBase, sizeof(dwImageBase), NULL);

	if (dwImageBase > 0)
	{
		DWORD   dwAddressA = 0x1BCE,
			    dwAddressB = 0x1B0A;
		BYTE    bDataA[2] = { 0x90, 0x90 },
			    bDataB[6] = { 0xE9, 0x8A, 0x00, 0x00, 0x00, 0x90 };

		WriteTargetMemory(&pi, dwImageBase + dwAddressA, bDataA, sizeof(bDataA));

		WriteTargetMemory(&pi, dwImageBase + dwAddressB, bDataB, sizeof(bDataB));
	}

	DWORD dwReturn = ResumeThread(pi.hThread);
	if (dwReturn == -1)
	{
		MessageBoxA(NULL, "Failed to resume the main thread of target!", szName, MB_OK | MB_ICONERROR);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


void CSpooferDlg::OnBnClickedButton1()
{
	const TCHAR szFilter[] = _T("EXE Files (*.exe)|*.exe|");

	CFileDialog openDlg(TRUE, _T("exe"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	if (openDlg.DoModal() == IDOK)
	{
		CString sFilePath = openDlg.GetPathName();
		child_process.SetWindowText(sFilePath);
	}
}


void CSpooferDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CSpooferDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CAboutDlg::OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	ShellExecute(NULL, L"open", L"https://t.me/securebyte", NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}
