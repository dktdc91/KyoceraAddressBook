// AddGroupDlg.cpp : implementation file
//

#include "stdafx.h"

#include "AddressBookApplication.h"
#include "AddressBookApplicationDlg.h"
#include "AddGroupDlg.h"
#include "AddressListView.h"
#include <list>
#include <EntryItem.h>

// AddGroupDlg dialog

IMPLEMENT_DYNAMIC(AddGroupDlg, CDialog)


AddGroupDlg::AddGroupDlg(CWnd* pParent /*= NULL*/)
:  m_groupItem(NULL), CDialog(AddGroupDlg::IDD, pParent), m_isChanged(false)
{

}

AddGroupDlg::AddGroupDlg(EntryItem *groupItem, CWnd* pParent /*=NULL*/)
	: m_groupItem(groupItem), CDialog(AddGroupDlg::IDD, pParent)
{
	
}

AddGroupDlg::~AddGroupDlg()
{
}

void AddGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALL_CONTACTS, m_leftList);
	DDX_Control(pDX, IDC_EDIT_GROUPNAME, m_groupName);
	DDX_Control(pDX, IDC_LIST_SELECTED_CONTACTS, m_rightList);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_addButton);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_removeButton);
}


BEGIN_MESSAGE_MAP(AddGroupDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &AddGroupDlg::OnButtonAddClicked)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &AddGroupDlg::OnButtonRemoveClicked)
	ON_BN_CLICKED(IDOK, &AddGroupDlg::OnButtonOkClicked)
	ON_MESSAGE(WM_LIST_EVENT_ITEM_SELECTIONCHANGED, &AddGroupDlg::OnItemSelectionChanged)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SELECTED_CONTACTS, &AddGroupDlg::OnItemchangedRightList)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void AddGroupDlg::setModel(AbstractModel *model)
{
	if( m_model == model )
		return;

	m_model = model;
}

// AddGroupDlg message handlers

BOOL AddGroupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	// initialize selected contacts list
	DWORD stylesEx = m_rightList.GetExtendedStyle();
	stylesEx |= WS_EX_LEFT | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB;
	m_rightList.SetExtendedStyle( stylesEx );

	LV_COLUMN lvcolumn;
	for (int col = 0; col < 3; ++col)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER | LVCFMT_CENTER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = (LPTSTR)(LPCTSTR)_T("");
		lvcolumn.iSubItem = col;
		lvcolumn.iOrder = col;
		col == 0 ? 	lvcolumn.cx = 0 : lvcolumn.cx = 120;
		m_rightList.InsertColumn( col, &lvcolumn);
	}
	m_rightList.SetColumnWidth(2, 200);

	// set style for address list view(left list view)
	m_leftList.initialize();

	if (m_groupItem)
	{
		CString groupName = CString(m_groupItem->data(ABE::Role_Name).c_str());
		m_groupName.SetWindowText(groupName);
	}
	SetTimer(TIMER_LIST_INIT, 200, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void AddGroupDlg::InsertToRightList(EntryItem *item)
{
	int count = m_rightList.GetItemCount();
	LV_ITEM lvitem = {0};
	lvitem.iImage = 0;
	lvitem.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCFMT_LEFT;
	lvitem.cColumns = 2;
	//lvitem.pszText =  (LPTSTR)(LPCTSTR)_T("");
	lvitem.iItem = count;
	lvitem.iSubItem = 0;
	m_rightList.InsertItem(&lvitem);
	m_rightList.SetItemText(count, 0, CString(item->data(ABE::Role_Id).c_str()) );
	m_rightList.SetItemText(count, 1, CString(item->data(ABE::Role_Name).c_str()) );
	m_rightList.SetItemText(count, 2, CString(item->data( ABE::Role_Fax).c_str()) );
}

void AddGroupDlg::selectContact()
{
	list<int> cacheIndexes;

	POSITION pos = m_leftList.GetFirstSelectedItemPosition();
	while( pos )
	{
		int idx = -1;
		idx = m_leftList.GetNextSelectedItem(pos);

		if(idx >= 0 && idx < m_leftList.GetItemCount())
		{
			CString strId = m_leftList.GetItemText(idx, ABE::Role_Id);
			int nId = ::_ttoi(strId.GetBuffer());
			EntryItem *item = m_model->itemFromId(nId, ABE::ContactType);
			if (item)
			{
				this->InsertToRightList(item);
				// save indexes, note: don't delete row item at here
				cacheIndexes.push_back(idx);
			}
		}
	}

	// note: Ensure listctrl item index dose not be modified, must remove row item from back to font
	//delete from left list;
	while( !cacheIndexes.empty() )
	{
		m_leftList.DeleteItem(cacheIndexes.back());
		cacheIndexes.pop_back();
	}
}

void AddGroupDlg::deselectContact()
{
	list<int> cacheIndexes;
	POSITION pos = m_rightList.GetFirstSelectedItemPosition();
	while( pos )
	{
		int idx = -1;
		idx = m_rightList.GetNextSelectedItem(pos);
		// save indexes, don't delete row item at here
		cacheIndexes.push_back(idx);
	}

	// remove from back
	while( !cacheIndexes.empty() )
	{
		int idx = cacheIndexes.back();
		CString strId = m_rightList.GetItemText(idx, 0);
		//delete from right list;
		m_rightList.DeleteItem(idx);
		cacheIndexes.pop_back();

		// insert into left list;
		int nId = ::_ttoi(strId.GetBuffer());
		int index = m_model->index(nId, ABE::ContactType);
		if (index != -1)
		{
			m_leftList.insertItem(m_leftList.GetItemCount(), index);
		}
	}
}

void AddGroupDlg::OnButtonAddClicked()
{
	selectContact();
	m_isChanged = true;
}

void AddGroupDlg::OnButtonRemoveClicked()
{
	deselectContact();
	m_isChanged = true;
}

void AddGroupDlg::OnButtonOkClicked()
{
	m_isChanged = false;
	CString groupName;
	m_groupName.GetWindowText(groupName);
	if ( groupName.IsEmpty() )
	{
		CString str;
		str.LoadString(IDS_ERROR_GROUPNAME);
		MessageBox(str, _T(""), MB_OK | MB_ICONERROR);
		return;
	}

	bool isEdit = true;
	// add group item to model
	if( !m_groupItem)
	{
		// new a group item
		isEdit = false;
		m_groupItem = new EntryItem(Util::WToA(groupName), ABE::GroupType);
		int pos = m_model->rowCount();

		// id auto ordering...
		int id = 1;
		while( m_model->hasId(id, ABE::GroupType) )
			++id;

		char buff[10];
		sprintf_s(buff, "%d", id);
		m_groupItem->setData( buff, ABE::Role_Id );

		if( m_model->insertEntry(pos, m_groupItem) )
		{
			m_isChanged = true;
			CString total;
			total.Format(_T("items:%d"), m_model->rowCount());
			CAddressBookApplicationDlg *dlg = dynamic_cast<CAddressBookApplicationDlg*>(GetParent());
			dlg->GetDlgItem(IDC_STATIC_ITEM_COUNT)->SetWindowText(total);
			::SendMessage(dlg->addressView(), WM_ADDRBOOK_ADD, pos, ABE::GroupType);
		}
	}
	else
	{
		// edit group item
		int index = m_model->index(m_groupItem);
		// is group name changed?
		string name = m_groupItem->data(ABE::Role_Name);
		CString oldName(name.c_str());
		if ( oldName != groupName )
		{
			m_isChanged = true;
			m_groupItem->setData( Util::WToA(groupName), ABE::Role_Name );
			CAddressBookApplicationDlg *dlg = dynamic_cast<CAddressBookApplicationDlg*>(GetParent());
			::SendMessage(dlg->addressView(), WM_ADDRBOOK_EDIT, index, ABE::GroupType);
		}
	}

	// clear children
	while (!m_groupItem->children().empty())
	{
		int id = m_groupItem->children().back();
		m_groupItem->removeChild(id);
	}
	// add children
	for (int i = 0; i < m_rightList.GetItemCount(); ++i)
	{
		CString strId = m_rightList.GetItemText(i, 0);
		int nId = ::_ttoi(strId);
		if( nId >= 0)
			m_groupItem->addChild(nId);// will be ignored add it if child has exists.
	}

	OnOK();
}

bool AddGroupDlg::hasChanged() const
{
	return m_isChanged;
}

LRESULT AddGroupDlg::OnItemSelectionChanged(WPARAM wParam, LPARAM lParam)
{
	int idx = (int)wParam;
	idx < 0 ?  m_addButton.EnableWindow(FALSE) : m_addButton.EnableWindow(TRUE);
	return 0;
}

void AddGroupDlg::OnItemchangedRightList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV && (pNMLV->uChanged & LVIF_STATE))
	{
		if (pNMLV->uNewState & LVIS_SELECTED)
		{
			m_removeButton.EnableWindow(TRUE);
		}
		if( m_rightList.GetSelectedCount() <= 0)
		{
			m_removeButton.EnableWindow(FALSE);
		}
	}

	*pResult = 0;
}

void CALLBACK AddGroupDlg::delayedLoadData()
{
	// set data model
	m_leftList.setEntryModel(m_model);

	// remove group item and has exist child item if gourpItem exist
	int total = m_model->rowCount();
	for (int i = total-1; i >= 0; --i)
	{
		if( m_model->itemType(i) == ABE::GroupType )
		{
			m_leftList.DeleteItem(i);
		}
		else if ( m_groupItem )
		{
			if( m_model->itemFromIndex(i) )
			{
				string strId = m_model->itemFromIndex(i)->data(ABE::Role_Id);
				int nId = Util::ToNumber(strId);
				if ( m_groupItem->hasChild(nId) )
					m_leftList.DeleteItem(i);
			}
		}
	}
	if (m_groupItem)
	{
		CString groupName = CString(m_groupItem->data(ABE::Role_Name).c_str());
		m_groupName.SetWindowText(groupName);

		std::vector<int> itemsId = m_groupItem->children();
		for (size_t i = 0; i < itemsId.size(); ++i)
		{
			// insert item to right list
			// note: maybe some children item have been deleted, so we remove it witch does not exist.
			int id = itemsId.at(i);
			EntryItem *item = m_model->itemFromId(id, ABE::ContactType);
			if( item )
				InsertToRightList(item);
			else
				m_groupItem->removeChild(id);
		}
	}
}

void AddGroupDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch( nIDEvent )
	{
	case TIMER_LIST_INIT:
		{
			delayedLoadData();
			KillTimer(TIMER_LIST_INIT);
		}
		break;

	default:break;
	}

	CDialog::OnTimer(nIDEvent);
}
