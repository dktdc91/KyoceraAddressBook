#pragma once
#include "AddressListView.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include <AddressBook.h>

// AddGroupDlg dialog
class AddGroupDlg : public CDialog
{
	DECLARE_DYNAMIC(AddGroupDlg)

public:
	AddGroupDlg( CWnd* pParent = NULL);   // standard constructor
	AddGroupDlg(EntryItem *gourpItem, CWnd* pParent = NULL);
	virtual ~AddGroupDlg();

	void setModel(AbstractModel *model);
	bool hasChanged() const;

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDGROUP };

private:
	void InsertToRightList(EntryItem *item);
	void selectContact();
	void deselectContact();
	void CALLBACK delayedLoadData();

private:
	AddressListView m_leftList;
	CListCtrl m_rightList;

	AbstractModel *m_model;
	EntryItem *m_groupItem;

	CEdit m_groupName;
	CButton m_addButton;
	CButton m_removeButton;

	bool m_isChanged;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnButtonAddClicked();
	afx_msg void OnButtonRemoveClicked();
	afx_msg void OnButtonOkClicked();
	afx_msg LRESULT OnItemSelectionChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnItemchangedRightList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
