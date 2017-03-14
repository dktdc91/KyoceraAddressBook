// BlockedNumberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BlockedNumberDlg.h"

#define ID_TOOLBAR_LOAD	100
#define ID_TOOLBAR_SAVE 101


IMPLEMENT_DYNAMIC(BlcokedNumberList, CListCtrl)
BEGIN_MESSAGE_MAP(BlcokedNumberList, CListCtrl)
	//{{AFX_MSG_MAP(BlcokedNumberList)
	ON_WM_ERASEBKGND()
	//ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////
// BlockedNumberDlg dialog

IMPLEMENT_DYNAMIC(BlockedNumberDlg, CDialog)

BlockedNumberDlg::BlockedNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(BlockedNumberDlg::IDD, pParent)
{

}

BlockedNumberDlg::~BlockedNumberDlg()
{
}

void BlockedNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NUMBERS, m_numbersListCtrl);
	DDX_Control(pDX, IDC_EDIT_BLOCKNUMBER, m_blocknumberEdit);
	DDX_Control(pDX, IDC_CHECK_BLOCKING, m_blockingCheckbox);
	DDX_Control(pDX, IDC_CHECK_BLOCK_ALL, m_blockAllCkeckbox);
	DDX_Control(pDX, IDC_BUTTON_BLOCKNUM_ADD, m_addButton);
	DDX_Control(pDX, IDC_BUTTON_BLOCKNUM_DELETE, m_deleteButton);
}


BEGIN_MESSAGE_MAP(BlockedNumberDlg, CDialog)
END_MESSAGE_MAP()


// BlockedNumberDlg message handlers

BOOL BlockedNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( !m_toolbarCtrl.Create(WS_CHILD | WS_VISIBLE  | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | TBSTYLE_TOOLTIPS
		| CBRS_SIZE_DYNAMIC | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, CRect(0,0,1000,40),this,0) ) 
	{ 
		TRACE0("Cannot create toolbar.\n"); 
		return FALSE;
	}
	m_toolbarImages.Create(32, 32, ILC_COLORDDB | ILC_MASK, 0, 11);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_BLOCKEDNUM_TOOLBAR);
	m_toolbarImages.Add(&bmp, RGB(0, 0, 0)); // black mask
	bmp.DeleteObject();
	m_toolbarCtrl.SetImageList(&m_toolbarImages);
	const int		buttonCount = 3;
	TBBUTTON		tbbutton[buttonCount] = {0};
	const CSize		button_size(32,32);      
	const CSize		bitmap_size(32,32);
	for(int i = 0; i < buttonCount; ++i)
	{
		tbbutton[i].iBitmap = i;
		tbbutton[i].fsState = TBSTATE_ENABLED;
		tbbutton[i].fsStyle = TBSTYLE_BUTTON;
		tbbutton[i].idCommand = ID_TOOLBAR_LOAD + i;
		tbbutton[i].dwData = i;
		if (2 == i)
		{
			tbbutton[i].idCommand = 0;
			tbbutton[i].fsStyle = TBSTYLE_SEP;
			tbbutton[i].fsState = TBSTATE_ENABLED;
			tbbutton[i].iString = 0;
			tbbutton[i].iBitmap = 0;
			tbbutton[i].dwData = 0;
		}
	}
	m_toolbarCtrl.SetButtonSize(button_size);
	m_toolbarCtrl.SetBitmapSize( bitmap_size);
	m_toolbarCtrl.AddButtons(buttonCount,tbbutton);

	m_numbersListCtrl.Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL BlockedNumberDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *hdr = (NMHDR*)lParam;
	
	if (hdr->code == WM_LBUTTONDOWN )
	{

	}


	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL BlockedNumberDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	int nCode = HIWORD(lParam);
	//if (nCode == WM_LBUTTONDOWN)
	{
		switch(id)  
		{
		case IDC_CHECK_BLOCKING:
			{
				int checked = m_blockingCheckbox.GetCheck();
				m_blockAllCkeckbox.EnableWindow(checked);
				m_blocknumberEdit.EnableWindow(checked);
				m_addButton.EnableWindow(checked);
				m_deleteButton.EnableWindow(checked);
				m_numbersListCtrl.EnableWindow(checked);
			}
			break;
		case IDC_CHECK_BLOCK_ALL:
			{

			}
			break;
		case  IDC_BUTTON_BLOCKNUM_ADD:
			{
				CString strNumber;
				m_blocknumberEdit.GetWindowText(strNumber);
				if (!strNumber.IsEmpty())
				{
					m_numbersListCtrl.InsertItem(m_numbersListCtrl.GetItemCount(), strNumber);
					m_numbersListCtrl.Invalidate();
				}
			}
			break;
		case IDC_BUTTON_BLOCKNUM_DELETE:
			{

			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}
