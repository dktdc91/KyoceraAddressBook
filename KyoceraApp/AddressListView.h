#pragma once
#include "afxcmn.h"

#include <AddressBook.h>
#include <AbstractModel.h>

// list control view item message events 
#define WM_LIST_EVENT_ITEM_SELECTIONCHANGED	(WM_USER+110)
#define WM_LIST_EVENT_ITEM_CLICKED			(WM_USER+111)
#define WM_LIST_EVENT_ITEM_DOUBLECLICKED	(WM_USER+112)
#define WM_LIST_EVENT_ITEM_TEXTCHANGED		(WM_USER+113)

class AddressListView : public CListCtrl
{
	DECLARE_DYNAMIC(AddressListView)

public:
	AddressListView(CWnd *parent = 0);
	virtual ~AddressListView();

	void initialize();

	void reset();
	void setEntryModel(AbstractModel *model);

	int rowHeight() const;
	void setRowHeight(int height);
	bool isEmpty();
	int insertItem(int row, int model_index);
	BOOL deleteItem(int index);

private:
	void drawGridlines();
	void clearSelected();

private:
	COLORREF m_colRow1;
	COLORREF m_colRow2;
	COLORREF m_colGridline;
	int m_rowHeight;
	AbstractModel *m_model;		// Maintaining data
	CImageList m_itemTypeImages;

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnNMCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
// 	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnItemAdd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemReload(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemClear(WPARAM wParam, LPARAM lParam);

// protected:
// 	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
