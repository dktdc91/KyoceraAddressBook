#pragma once
#include "afxwin.h"


// ProgressBarDlg dialog

class ProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(ProgressBarDlg)

public:
	ProgressBarDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ProgressBarDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROGRESS };

	void setProgressPos(int pos);
	int progressPos() const;
	void setStatusText(const CString &text);
	void setStepText(const CString &text);
	void centerWindow(CWnd* pAlternateOwner /* = NULL */);

private:
	CProgressCtrl m_progress;
	CStatic m_statusText;
	CStatic m_stepText;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


};
