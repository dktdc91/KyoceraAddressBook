// ProgressBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddressBookApplication.h"
#include "ProgressBarDlg.h"
#include "AddressBookApplicationDlg.h"


// ProgressBarDlg dialog

IMPLEMENT_DYNAMIC(ProgressBarDlg, CDialog)

ProgressBarDlg::ProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ProgressBarDlg::IDD, pParent)
{

}

ProgressBarDlg::~ProgressBarDlg()
{
}

void ProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_statusText);
	DDX_Control(pDX, IDC_STATIC_PROGRESS_STEP, m_stepText);
}


BEGIN_MESSAGE_MAP(ProgressBarDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()

void ProgressBarDlg::setProgressPos(int pos)
{
	m_progress.SetPos(pos);
}

int ProgressBarDlg::progressPos() const
{
	return m_progress.GetPos();
}

// ProgressBarDlg message handlers

int ProgressBarDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL ProgressBarDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	m_progress.SetRange(0,100); 
	m_progress.SetPos(0);

	return CDialog::PreCreateWindow(cs);
}

void ProgressBarDlg::setStatusText(const CString &text)
{
	m_statusText.SetWindowText(text);
}

void ProgressBarDlg::setStepText(const CString &text)
{
	m_stepText.SetWindowText(text);
}

void ProgressBarDlg::centerWindow(CWnd* pAlternateOwner /* = NULL */)
{
	this->CenterWindow(pAlternateOwner);
}
