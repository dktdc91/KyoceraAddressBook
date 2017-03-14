#include "StdAfx.h"
#include "resource.h"
#include "AddressListView.h"

#include <EntryItem.h>

IMPLEMENT_DYNAMIC(AddressListView, CListCtrl)

AddressListView::AddressListView(CWnd *parent)
	:m_model(NULL)
{
	m_colRow1 = RGB(255,255,255);
	m_colRow2 = RGB(238,238,238);
	m_colGridline = RGB(210, 210, 210);
	m_rowHeight = 18;
}

AddressListView::~AddressListView()
{

}

BEGIN_MESSAGE_MAP(AddressListView, CListCtrl)
	//{{AFX_MSG_MAP(AddressListView)
	ON_WM_ERASEBKGND()
	//ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &AddressListView::OnNMCustomDraw)
	ON_MESSAGE(WM_ADDRBOOK_ADD,  &AddressListView::OnItemAdd)
	ON_MESSAGE(WM_ADDRBOOK_DELETE,  &AddressListView::OnItemDelete)
	ON_MESSAGE(WM_ADDRBOOK_EDIT,  &AddressListView::OnItemEdit)
	ON_MESSAGE(WM_ADDRBOOK_OPEN,  &AddressListView::OnItemReload)
	ON_MESSAGE(WM_ADDRBOOK_RERLOAD,  &AddressListView::OnItemReload)
	ON_MESSAGE(WM_ADDRBOOK_CLEAR,  &AddressListView::OnItemClear) 

	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &AddressListView::OnLvnItemchanged)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void AddressListView::initialize()
{
	//ModifyStyle(0, BS_OWNERDRAW);

	// set style for address listview
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	dwStyle &= ~LVS_TYPEMASK;
	dwStyle &= ~LVS_EDITLABELS;
	dwStyle |= LVS_REPORT| LVS_OWNERDATA | LVS_NOSORTHEADER ; /*| LVS_OWNERDRAWFIXED*/// crash
	SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

	DWORD dwStylesEx = GetExtendedStyle();
	dwStylesEx |= WS_EX_LEFT | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB | LVS_EX_SUBITEMIMAGES;
	SetExtendedStyle( dwStylesEx );
	setRowHeight(22);

	m_itemTypeImages.Create(21, 21, ILC_COLORDDB | ILC_MASK, 0, 1);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_CONTACT);
	m_itemTypeImages.Add(&bmp, RGB(0,0,0));
	bmp.DeleteObject();
	bmp.LoadBitmap(IDB_BITMAP_GROUP);
	m_itemTypeImages.Add(&bmp, RGB(0,0,0));
	bmp.DeleteObject();
	SetImageList(&m_itemTypeImages, LVSIL_SMALL);
}

void AddressListView::OnNMCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;

	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = lplvcd->nmcd.dwItemSpec;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT :
		{
			drawGridlines();
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}
		// Modify item text and or background
	case CDDS_ITEMPREPAINT:
		{
			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}

		// Modify sub item text and/or background
	case CDDS_SUBITEM  | CDDS_PREPAINT | CDDS_ITEM:
		{
			drawGridlines();;
			int nItem = static_cast<int>( lplvcd->nmcd.dwItemSpec );
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos != NULL)
			{
				//while (pos)
				{
					int index = GetNextSelectedItem(pos);
					if (index == nItem)
						lplvcd->clrTextBk =  RGB(51,153,255);
					else
						iRow % 2 ? lplvcd->clrTextBk = m_colRow2: lplvcd->clrTextBk = m_colRow1;
				}
			}
			else
			{
				iRow % 2 ? lplvcd->clrTextBk = m_colRow2: lplvcd->clrTextBk = m_colRow1;
			}
		
			*pResult = CDRF_NOTIFYSUBITEMDRAW;// CDRF_DODEFAULT;
			break;
		}
	}
}

BOOL AddressListView::OnEraseBkgnd(CDC* pDC)
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

	return TRUE;
	//return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL AddressListView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;

	if (pHDNotify->iItem == ABE::Role_Unuse 
		|| pHDNotify->iItem == ABE::Role_Type 
		|| pHDNotify->iItem == ABE::Role_Id)
	{
		return *pResult = TRUE;
	}

	// limit  minimum column width
	const int minWidth = 18;
	switch( pHDNotify->hdr.code )
	{
	case HDN_ITEMCHANGINGA:
	case HDN_ITEMCHANGINGW:
	case HDN_ENDTRACK:
		{
			if ( (pHDNotify->pitem->mask & HDI_WIDTH) 
				&& pHDNotify->pitem->cxy < minWidth 
				&& pHDNotify->pitem->cxy >= 0 )
			{
				pHDNotify->pitem->cxy = minWidth;
			}
		}
		break;

	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void AddressListView::reset()
{
	//CRect rect;   
	//this->GetClientRect(&rect);// cannot find CListCtrl object
	CRect rect(0,0,750,0);
	LV_COLUMN lvcolumn;
	for (int i = 0; i < ABE::Role_User; ++i)
	{
		CString str = CString(m_model->headerData(0, i).c_str()) ;

		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		GetColumn(0, &lvcolumn);
		lvcolumn.fmt &=~ LVCFMT_JUSTIFYMASK; 
		lvcolumn.fmt |= LVCFMT_LEFT;
		lvcolumn.pszText = (LPTSTR)(LPCTSTR)str;
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;

		if(ABE::Role_Unuse == i)
			lvcolumn.cx = 0;
		else if ( ABE::Role_Id == i)
			lvcolumn.cx = rect.right / 12;
		else if (ABE::Role_Type == i)
			lvcolumn.cx = rect.right / 14;
		else if(ABE::Role_ECM == i )
			lvcolumn.cx = rect.right / 14;
		else if ( ABE::Role_Source == i)
			lvcolumn.cx = rect.right / 9;
		else
			lvcolumn.cx = rect.right / 4;

		InsertColumn( i, &lvcolumn);
	}
	SetColumnWidth(ABE::Role_Speed, 200);

	for (int index = 0; index < m_model->rowCount(); ++index)
	{
		this->insertItem(index, index);
	}

}

void AddressListView::setEntryModel(AbstractModel *model)
{
	if( m_model == model )
		return;

	m_model = model;
	reset();
}


void AddressListView::drawGridlines()
{
	const MSG *msg = GetCurrentMessage();
	DefWindowProc( msg->message, msg->wParam, msg->lParam ); 
	if( (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
	{		
		CClientDC dc(this);

		// Get the number of columns
		CHeaderCtrl* pHeader = GetHeaderCtrl();
		int columnCount = pHeader->GetItemCount();

		// The bottom of the header corresponds to the top of the line
		RECT rect;
		pHeader->GetClientRect( &rect );
		int top = rect.bottom;

		// Now get the client rect so we know the line length and
		// when to stop
		GetClientRect( &rect );

		// The border of the column is offset by the horz scroll
		int offset = -1;
		int borderx = offset - GetScrollPos( SB_HORZ );

		CPen listSepPen(PS_SOLID, 1, m_colGridline);
		CPen *pOldPen = dc.SelectObject(&listSepPen);

		for( int i = 0; i < columnCount; i++ )
		{
			// Get the next border
			borderx += GetColumnWidth( i );

			// if next border is outside client area, break out
			if( borderx >= rect.right ) break;

			// Draw the line.
			dc.MoveTo( borderx, top);
			dc.LineTo( borderx, rect.bottom );
		}

		// Draw the horizontal grid lines

		// First get the height
		if( GetItemRect( 0, &rect, LVIR_BOUNDS ))
			m_rowHeight = rect.bottom - rect.top;
		int height = m_rowHeight;

		GetClientRect( &rect );
		int width = rect.right;

		for(int i = 1; i <= GetCountPerPage(); i++ )
		{
			dc.MoveTo( 0, top + height*i);
			dc.LineTo( width, top + height*i );
		}

		dc.SelectObject(pOldPen);
	}

}


void AddressListView::setRowHeight(int height)
{
	m_rowHeight = height;

// 	CImageList imagelist;
// 	imagelist.Create(1, m_rowHeight, ILC_COLORDDB | ILC_MASK , 0, 0);
// 	this->SetImageList(&imagelist, LVSIL_SMALL);

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

int AddressListView::rowHeight() const
{
	return m_rowHeight;
}

LRESULT AddressListView::OnItemAdd(WPARAM wParam, LPARAM lParam)
{
	if (!m_model)
		return -1;

	int index = (int)wParam;
	this->insertItem(index, index);
	return 0;
}

LRESULT AddressListView::OnItemDelete(WPARAM wParam, LPARAM lParam)
{
	if (!m_model)
		return -1;
	int index = (int)wParam;
	int total = this->GetItemCount();
	if ( index < 0 && index > total)
		return -1;

	this->deleteItem(index);

	clearSelected();
	if (index == total-1)
	{
		this->SetItemState(index-1, LVIS_SELECTED, LVIS_SELECTED | LVIS_DROPHILITED);
		SetSelectionMark(index-1);
	}
	else
	{
		this->SetItemState(index, LVIS_SELECTED, LVIS_SELECTED | LVIS_DROPHILITED);
		SetSelectionMark(index);
	}

	return 0;
}

LRESULT AddressListView::OnItemEdit(WPARAM wParam, LPARAM lParam)
{	
	if (!m_model)
		return -1;
	int index = (int)wParam;
	if ( index < 0 && index > this->GetItemCount())
		return -1;

	ABE::EntryType type = (ABE::EntryType)lParam;

	switch( type )
	{
	case ABE::ContactType:
		{
			SetItemText(index, ABE::Role_Name, CString(m_model->data(index, ABE::Role_Name).c_str()) );
			SetItemText(index, ABE::Role_Source, CString(m_model->data(index, ABE::Role_Source).c_str()) );
			SetItemText(index, ABE::Role_Fax, CString(m_model->data(index, ABE::Role_Fax).c_str()) );
			SetItemText(index, ABE::Role_Company, CString(m_model->data(index, ABE::Role_Company).c_str()) );
			SetItemText(index, ABE::Role_Department, CString(m_model->data(index, ABE::Role_Department).c_str()) );
			SetItemText(index, ABE::Role_ECM, CString(m_model->data(index, ABE::Role_ECM).c_str()) );
			SetItemText(index, ABE::Role_Speed, CString(m_model->data(index, ABE::Role_Speed).c_str()) );
		}
		break;
	case ABE::GroupType:
		{
			SetItemText(index, ABE::Role_Name, CString(m_model->data(index, ABE::Role_Name).c_str()) );
		}
		break;;
	default:break;
	}

	return 0;
}

LRESULT AddressListView::OnItemReload(WPARAM wParam, LPARAM lParam)
{
	CListCtrl::DeleteAllItems();
	for (int index = 0; index < m_model->rowCount(); ++index)
	{
		this->insertItem(index, index);
	}
	return 0;
}

LRESULT AddressListView::OnItemClear(WPARAM wParam, LPARAM lParam)
{
	CListCtrl::DeleteAllItems();
	return 0;
}

void AddressListView::clearSelected()
{
	for (int i = 0; i < this->GetItemCount(); ++i)
	{
		this->SetItemState(i, 0, -1);
		SetSelectionMark(-1);
	}
}

bool AddressListView::isEmpty()
{
	return this->GetItemCount() == 0;
}

BOOL AddressListView::deleteItem(int index)
{
	BOOL ret = CListCtrl::DeleteItem(index);

	// refresh view
	this->Invalidate();

	return ret;
}

int AddressListView::insertItem(int row, int index)
{
/*
	LV_ITEM lvitem = {0};
	lvitem.iImage = m_model->itemType(index) == ABE::GroupType ? 1 : 0;
	lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvitem.cColumns = m_model->columnCount();
	lvitem.pszText = (LPTSTR)(LPCTSTR)_T("");
	lvitem.iItem = row;
	lvitem.iSubItem = 0;
	//lvitem.state = LVIS_SELECTED;
	int i = CListCtrl::InsertItem(&lvitem);
*/
	CString strType;
	int t = m_model->itemType(index);
	strType.Format( _T("%d"), t);

	int i = CListCtrl::InsertItem(row, _T(""), -1);// 0 column unused.

	SetItem(i, ABE::Role_Id, LVIF_TEXT, CString(m_model->data(index, ABE::Role_Id).c_str()), 0, 0, 0, 0 );
	int img = m_model->itemType(index) == ABE::GroupType ? 1 : 0;
	SetItem(i, ABE::Role_Type, LVIF_IMAGE, strType, img, 0, 0, 0 );
	SetItem(i, ABE::Role_Name, LVIF_TEXT, CString(m_model->data(index, ABE::Role_Name).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_Source, LVIF_TEXT,CString(m_model->data(index, ABE::Role_Source).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_Fax, LVIF_TEXT,CString(m_model->data(index, ABE::Role_Fax).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_Company, LVIF_TEXT,CString(m_model->data(index, ABE::Role_Company).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_Department, LVIF_TEXT,CString(m_model->data(index, ABE::Role_Department).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_ECM, LVIF_TEXT,CString(m_model->data(index, ABE::Role_ECM).c_str()), 0, 0, 0, 0 );
	SetItem(i, ABE::Role_Speed, LVIF_TEXT,CString(m_model->data(index, ABE::Role_Speed).c_str()), 0, 0, 0, 0 );

/*
	SetItemText(row, ABE::Role_Id, CString(m_model->data(index, ABE::Role_Id).c_str()) );
	SetItemText(row, ABE::Role_Type, strType );
	SetItemText(row, ABE::Role_Name, CString(m_model->data(index, ABE::Role_Name).c_str()) );
	SetItemText(row, ABE::Role_Source, CString(m_model->data(index, ABE::Role_Source).c_str()) );
	SetItemText(row, ABE::Role_Fax, CString(m_model->data(index, ABE::Role_Fax).c_str()) );
	SetItemText(row, ABE::Role_Company, CString(m_model->data(index, ABE::Role_Company).c_str()) );
	SetItemText(row, ABE::Role_Department, CString(m_model->data(index, ABE::Role_Department).c_str()) );
	SetItemText(row, ABE::Role_ECM, CString(m_model->data(index, ABE::Role_ECM).c_str()) );
	SetItemText(row, ABE::Role_Speed, CString(m_model->data(index, ABE::Role_Speed).c_str()) );
*/
	clearSelected();
	this->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED | LVIS_DROPHILITED);
	SetSelectionMark(i);

	return i;
}

void AddressListView::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// call this function, must set style LVS_OWNERDRAWFIXED and SendMessage WM_WINDOWPOSCHANGED
	if (m_rowHeight > 0)
	{
		lpMeasureItemStruct->itemHeight = m_rowHeight;
	}
}

/*
void AddressListView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	TCHAR  lpBuffer[256];
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = lpDrawItemStruct->itemID ;  
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev ;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcprev.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	for ( int col = 0; GetColumn(col, &lvc); col++)
	{
		if( col == 1 )
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(col-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left += lvcprev.cx ;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left ; 

		}
		else
		{
			GetColumn(col-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left = 0;
			lpDrawItemStruct->rcItem.right = lvc.cx;
		}
		// Get the text 
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iSubItem = col;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_HIGHLIGHT));
			pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		}
		else
		{

			pDC->FillSolidRect(&lpDrawItemStruct->rcItem,  GetSysColor(COLOR_WINDOW));
			pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		}
		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		UINT uFormat = DT_LEFT ;
		::DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcslen(lpBuffer), &lpDrawItemStruct->rcItem, uFormat) ;
		pDC->SelectStockObject(SYSTEM_FONT) ;
	}

}
*/

// 
// BOOL AddressListView::PreCreateWindow(CREATESTRUCT& cs)
// {
// 	// TODO: Add your specialized code here and/or call the base class
// 
// 	cs.dwExStyle |= LVS_OWNERDRAWFIXED;
// 
// 	return CListCtrl::PreCreateWindow(cs);
// }

void AddressListView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV && (pNMLV->uChanged & LVIF_STATE))
	{
		if (pNMLV->uNewState & LVIS_SELECTED)
		{
			::SendMessage(GetParent()->GetSafeHwnd(), WM_LIST_EVENT_ITEM_SELECTIONCHANGED, pNMLV->iItem, 0);
		}
		if(GetSelectedCount() <= 0)
		{
			::SendMessage(GetParent()->GetSafeHwnd(), WM_LIST_EVENT_ITEM_SELECTIONCHANGED, -1, 0);
		}
	}
	*pResult = 0;
}

void AddressListView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode) 
	{ 
	case SB_LINELEFT:
	case SB_LINERIGHT:
		
		break;

	}

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
