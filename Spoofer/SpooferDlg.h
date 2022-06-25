
// SpooferDlg.h : header file
//

#pragma once


// CSpooferDlg dialog
class CSpooferDlg : public CDialogEx
{
// Construction
public:
	CSpooferDlg(CWnd* pParent = nullptr);	// standard constructor
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPOOFER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	VOID getAllProcess();
	VOID errorHandle(LPTSTR pszMessage, DWORD dwLastError);
	DWORD getProcessID(const wchar_t* pName);
	BOOL setAdjustToken();
	CComboBox p_combo1;
	CEdit child_process;
	CEdit cmdline;
	CButton btn_browse;
	CEdit ppid;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton2();
};
