#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"


class BlcokedNumberList : public CListCtrl
{
	DECLARE_DYNAMIC(BlcokedNumberList)
public:
	BlcokedNumberList(CWnd *parent = 0)
	{
		m_colRow1 = RGB(255,255,255);
		m_colRow2 = RGB(238,238,238);
		m_rowHeight = 22;
	}
	~BlcokedNumberList(){}

	void Initialize()
	{
		DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		dwStyle &= ~LVS_TYPEMASK;
		dwStyle &= ~LVS_EDITLABELS;
		dwStyle |= LVS_REPORT| LVS_OWNERDATA | LVS_NOSORTHEADER;
		SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

		DWORD dwStylesEx = GetExtendedStyle();
		dwStylesEx |= WS_EX_LEFT | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB;
		SetExtendedStyle( dwStylesEx );

		CImageList imagelist;
		imagelist.Create(1, m_rowHeight, ILC_COLORDDB | ILC_MASK , 0, 0);
		this->SetImageList(&imagelist, LVSIL_SMALL);

		LV_COLUMN lvcolumn;
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		GetColumn(0, &lvcolumn);
		lvcolumn.fmt &=~ LVCFMT_JUSTIFYMASK; 
		lvcolumn.fmt |= LVCFMT_LEFT;
		lvcolumn.pszText = _T("Number");
		lvcolumn.iSubItem = 0;
		lvcolumn.iOrder = 0;

		CRect rect;
		this->GetWindowRect(&rect);
		lvcolumn.cx = rect.right;
		InsertColumn( 0, &lvcolumn);
	}

private:
	COLORREF m_colRow1;
	COLORREF m_colRow2;
	int m_rowHeight;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC)
	{
		CRect ctrl;
		CRect rect;
		CRect head;

		CDC *pPaintDC = GetDC();	// non-clipped DC
		//DetermineScheme();
		CBrush brush0(m_colRow1);
		CBrush brush1(m_colRow2);

		this->GetClientRect(ctrl);
		GetHeaderCtrl()->GetClientRect(head);
		pDC->FillRect(&ctrl, &brush0);
		//this->SetOutlineColor(RGB(255,0,0));

		int index = GetTopIndex();
		int last_visible_index = index + GetCountPerPage()+1; // since it's rare that an exact number fits, last visible may be a partial
		if (GetItemRect(index, &rect, LVIR_BOUNDS) != 0)
		{
			while (index <= last_visible_index)
			{
				GetItemRect(index, &rect, LVIR_BOUNDS);
				m_rowHeight = rect.Height();
				pDC->FillRect(&rect, index % 2 ? &brush1 : &brush0);// fill with column data

				// without column data 
				ctrl.left = rect.right;
				ctrl.top = max(rect.top, head.bottom);
				ctrl.bottom = rect.bottom;
				pPaintDC->FillRect(&ctrl, index % 2 ? &brush1 : &brush0);	// fill part of control without column data

				rect.top = rect.bottom;
				rect.bottom = rect.top + m_rowHeight;
				index++;
			}
		}
		else
		{	// fill part of control without column data
			CRect rect = head;
			while (index <= last_visible_index)
			{
				ctrl.left = rect.left;
				ctrl.top = rect.bottom;
				ctrl.bottom = rect.bottom + m_rowHeight;
				pPaintDC->FillRect(&ctrl, index % 2 ? &brush1 : &brush0);

				rect.top = ctrl.top;
				rect.bottom = ctrl.bottom;
				index++;
			}
		}

		brush0.DeleteObject();
		brush1.DeleteObject();
		ReleaseDC(pPaintDC);

		return FALSE;
	}
protected:
	DECLARE_MESSAGE_MAP()
};

// BlockedNumberDlg dialog

class BlockedNumberDlg : public CDialog
{
	DECLARE_DYNAMIC(BlockedNumberDlg)

public:
	BlockedNumberDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~BlockedNumberDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_BLOCKNUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	BlcokedNumberList m_numbersListCtrl;
	CEdit m_blocknumberEdit;
	CButton m_blockingCheckbox;
	CButton m_blockAllCkeckbox;
	CButton m_addButton;
	CButton m_deleteButton;
	CToolBarCtrl m_toolbarCtrl;
	CImageList m_toolbarImages;
public:
	virtual BOOL OnInitDialog();
};
