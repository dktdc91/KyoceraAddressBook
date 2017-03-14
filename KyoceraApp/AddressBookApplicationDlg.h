
// AddressBookApplicationDlg.h : header file
//
#pragma once

#include "AddressListView.h"
#include "anchor.h"
#include "ProgressBarDlg.h"
#include "BlockedNumberDlg.h"
#include "afxcmn.h"
#include "afxwin.h"

#define TIMER_LIST_INIT					1
#define TIMER_DEVICE_CNNT				2
#define TIMER_PROGRESS_HIDE				3

// For toolbar buttons
#define ID_TOOLBAR_NEW					202
#define ID_TOOLBAR_OPEN					203
#define ID_TOOLBAR_SAVE					204
#define ID_TOOLBAR_SAVEAS				205
#define ID_TOOLBAR_LOADFROMDEVICE		206
#define ID_TOOLBAR_SAVETODEVICE			207
#define ID_TOOLBAR_ADDGROUP				208
#define ID_TOOLBAR_EDITGROUP			209
#define ID_TOOLBAR_DELETEDUPLICATES		210
#define ID_TOOLBAR_BLOCKNUMBERS			211
#define ID_TOOLBAR_HELP					212

/************************************************************************/
/*	
	+-------------------+-----------------------------+
	|[Data Source]:.....|AddressFile..IODevice........|
	|...................|.......\...../...............|
	|...................|........\.../................|
	|[Data Maintaining]:|........Model ---> EntryItem.|
	|...................|........//..\................|
	|...................|.......//....\...............|
	|[Data Processing]:.|ListView ----> Dialog........|
	+-------------------+-----------------------------+
*/
/************************************************************************/

// For status bar
static UINT indicators[]={ID_INDICATOR_REC, ID_INDICATOR_NUM };

class AddressBook;
/*! 
 * @brief:CAddressBookApplicationDlg dialog
 * @auth:Yuri Yang<yuri.yang@liteon.com>
 * @data:2016-7-15
 */
class CAddressBookApplicationDlg : public CDialog
{
// Construction
public:
	CAddressBookApplicationDlg(CWnd* pParent = NULL);	// standard constructor
	~CAddressBookApplicationDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_APP };

	// list view handle
	HWND addressView() const { return m_addressView.m_hWnd; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	/*! create toolbar, list view, buttons,status bar etc.*/
	void setupUi();
	void initView();
	/*just enable or disabled button or other controls.*/
	void updateUi(bool enabled);

public:
	/*! new a contact to model*/
	void addContact();
	/*! edit current selected contact and submit to model.*/
	void editContact();
	/*! remove a contact from model*/
	void deleteContact();

	bool newFile();
	bool openFile();
	bool saveFile();
	bool saveAsFile();
	
	bool connectToDevice();
	bool loadFromDevice();
	bool saveToDevice();

	void updateCount();

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMDblClickAddressview(NMHDR *pNMHDR, LRESULT *pResult);//double clicked address view

	// Toolbar button event
	afx_msg void OnToolbarButtonClicked(UINT nID);
	afx_msg void OnUpdateUI(CCmdUI *pCmdUI);
	// Contact button events
	afx_msg void OnDlgButtonClicked(UINT nID);
	afx_msg LRESULT OnItemSelectionChanged(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	ProgressBarDlg m_progressDlg;

private:
	AddressBook *m_addressBook;	// data core manager

	CDlgAnchor m_dlgAnchor;	// window resizing manager
	CImageList m_toolbarImages;
	CImageList m_toolbarImagesDis;
	
	// UI data controls
	CToolBarCtrl m_toolbarCtrl;
	CToolBar m_toolbar;
	AddressListView m_addressView;
	CStatusBar  m_statusBar;
	CComboBox m_speedCombox;
	CEdit m_nameEdit;
	CEdit m_faxNumberEdit;
	CEdit m_companyEdit;
	CEdit m_departmentEdit;
	CButton m_ecmCheckbox;
	CButton m_addButton;
	CButton m_editButton;
	CButton m_deleteButton;

	// editor
	bool isEditMode;
	bool m_isChanged;
	CString m_titleSymbol;
	CString m_windowTitle;
	CString m_currentFilepath;
	CString m_currentFilename;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
