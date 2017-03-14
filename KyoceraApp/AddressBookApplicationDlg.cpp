
// AddressBookApplicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddressBookApplication.h"
#include "AddressBookApplicationDlg.h"
#include "AddGroupDlg.h"
#include <list>
#include <AddressBook.h>
#include <EntryItem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAddressBookApplicationDlg dialog


CAddressBookApplicationDlg::CAddressBookApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddressBookApplicationDlg::IDD, pParent),
	m_currentFilepath(_T("")), m_currentFilename(_T("Untitled")), m_titleSymbol(_T("*")),
	isEditMode(false), m_isChanged(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_addressBook = new AddressBook;
}


CAddressBookApplicationDlg::~CAddressBookApplicationDlg()
{
	delete m_addressBook;
}

void CAddressBookApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADDRESSVIEW, m_addressView);
	DDX_Control(pDX, IDC_COMBO_SPEED, m_speedCombox);
	DDX_Control(pDX, IDC_EDIT_NAME, m_nameEdit);
	DDX_Control(pDX, IDC_EDIT_FAXNUMBER, m_faxNumberEdit);
	DDX_Control(pDX, IDC_EDIT_COMPANY, m_companyEdit);
	DDX_Control(pDX, IDC_EDIT_DEPARTMENT, m_departmentEdit);
	DDX_Control(pDX, IDC_CHECK_ECM, m_ecmCheckbox);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_addButton);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_editButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_deleteButton);
}

BEGIN_MESSAGE_MAP(CAddressBookApplicationDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_LIST_EVENT_ITEM_SELECTIONCHANGED, &CAddressBookApplicationDlg::OnItemSelectionChanged)

	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ADDRESSVIEW, &CAddressBookApplicationDlg::OnNMDblClickAddressview)
	ON_COMMAND_RANGE(IDC_BUTTON_ADD, IDC_BUTTON_CANCEL, &CAddressBookApplicationDlg::OnDlgButtonClicked)
	ON_COMMAND_RANGE(ID_TOOLBAR_NEW, ID_TOOLBAR_HELP, &CAddressBookApplicationDlg::OnToolbarButtonClicked)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLBAR_NEW, ID_TOOLBAR_HELP, &CAddressBookApplicationDlg::OnUpdateUI)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAddressBookApplicationDlg message handlers

BOOL CAddressBookApplicationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
/*
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
*/
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	setupUi();
	
	m_windowTitle.LoadString(IDS_APP_WINDOWTITLE);
	this->SetWindowText(  m_currentFilename + _T(" - ") + m_windowTitle );

	// connect to device
	SetTimer(TIMER_DEVICE_CNNT, 800, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddressBookApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CAddressBookApplicationDlg::OnPaint()
{
	if (IsIconic())
	{	
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
	
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAddressBookApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAddressBookApplicationDlg::OnSize(UINT nType, int cx, int cy)
{	
	CDialog::OnSize(nType, cx, cy);

	// redraw contact group box control
	if( GetDlgItem(IDC_STATIC_CONTACT) )
	{
		CRect rect;
		GetDlgItem(IDC_STATIC_CONTACT)->GetClientRect(&rect);
		GetDlgItem(IDC_STATIC_CONTACT)->InvalidateRect(rect);
	}

	// auto resizing....
	m_dlgAnchor.OnSize(TRUE);
}

void CAddressBookApplicationDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// fixed/limit mini size for the window 
	lpMMI->ptMinTrackSize.x = 590;
	lpMMI->ptMinTrackSize.y = 750;

	// fixed max size for the window
	//lpMMI->ptMaxTrackSize.x = 1366;
	//lpMMI->ptMaxTrackSize.y = 750;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

/*!
 @brief: Create Dialog's child controls
 @auth:Yuri Yang@liteon.com 2016-6-16
*/
void CAddressBookApplicationDlg::setupUi()
{
	m_progressDlg.Create(IDD_DIALOG_PROGRESS, this);

	/*! Anchor initialize */
	m_dlgAnchor.Init(this->m_hWnd);

	/*! create and initialize address listview control*/
	initView();

	/*! initial speed drop list control*/
	for (int i = 0; i < m_speedCombox.GetCount(); ++i)
	{
		m_speedCombox.DeleteString(i);
	}
	m_speedCombox.InsertString(0, _T("33600 bps"));
	m_speedCombox.InsertString(1, _T("14400 bps"));
	m_speedCombox.InsertString(2, _T("9600 bps"));
	m_speedCombox.SetCurSel(0);

	/*! creat a toolbar control*/
	if ( !m_toolbarCtrl.Create(WS_CHILD | WS_VISIBLE  | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | TBSTYLE_TOOLTIPS
		| CBRS_SIZE_DYNAMIC | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, CRect(0,0,470,42),this,0) ) // | CBRS_GRIPPER
	{ 
		TRACE0("Cannot create toolbar.\n"); 
		return ;
	}
	// load button image
	m_toolbarImages.Create(32, 32, ILC_COLORDDB | ILC_MASK, 0, 11);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_TOOLBAR);
	m_toolbarImages.Add(&bmp, RGB(0, 0, 0)); // black mask
	bmp.DeleteObject();
	m_toolbarImagesDis.Create(32,32, ILC_COLORDDB | ILC_MASK, 0, 11);
	bmp.LoadBitmap(IDB_BITMAP_TOOLBAR_DISABLED);
	m_toolbarImagesDis.Add(&bmp, RGB(0, 0, 0));
	bmp.DeleteObject();
	m_toolbarCtrl.SetImageList(&m_toolbarImages);
	m_toolbarCtrl.SetDisabledImageList(&m_toolbarImagesDis);
	const int		buttonCount = 11;
	TBBUTTON		tbbutton[buttonCount] = {0};
	const CSize		button_size(32,32);      
	const CSize		bitmap_size(32,32);
	for(int i = 0; i < buttonCount; ++i)
	{
		tbbutton[i].iBitmap = i;
		tbbutton[i].fsState = TBSTATE_ENABLED;
		tbbutton[i].fsStyle = TBSTYLE_BUTTON;
		tbbutton[i].idCommand = ID_TOOLBAR_NEW + i;
		tbbutton[i].dwData = i;
		//tbbutton[i].iString = m_toolbarCtrl.AddString(IDS_TOOLBAR_NEW + i);
	}
	m_toolbarCtrl.SetButtonSize(button_size);
	m_toolbarCtrl.SetBitmapSize( bitmap_size);
	m_toolbarCtrl.AddButtons(buttonCount,tbbutton);
	// insert separator button
	TBBUTTON sepButton;
	sepButton.idCommand = 0;
	sepButton.fsStyle = TBSTYLE_SEP;
	sepButton.fsState = TBSTATE_ENABLED;
	sepButton.iString = 0;
	sepButton.iBitmap = 0;
	sepButton.dwData = 0;
	m_toolbarCtrl.InsertButton(4, &sepButton);
	m_toolbarCtrl.InsertButton(7, &sepButton);
	m_toolbarCtrl.InsertButton(12, &sepButton);
	m_toolbarCtrl.InsertButton(14, &sepButton);
	//m_toolbarCtrl.SetOwner(this);
	m_toolbarCtrl.EnableButton(ID_TOOLBAR_LOADFROMDEVICE, 0);
	m_toolbarCtrl.EnableButton(ID_TOOLBAR_SAVETODEVICE, 0);
	m_toolbarCtrl.EnableButton(ID_TOOLBAR_EDITGROUP, 0);
	m_toolbarCtrl.EnableButton(ID_TOOLBAR_BLOCKNUMBERS, 0);

// 	m_toolbarCtrl.SetAnchorHighlight();
// 	m_toolbarCtrl.SetInsertMarkColor(COLORREF(RGB(233,4,3)));
// 	m_toolbarCtrl.MarkButton(ID_TOOLBAR_NEW);
	
	// horizontal line in the toolbar control
	CRect rect;
	GetDlgItem(IDC_STATIC_HORIZONTAL_LINE)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_HORIZONTAL_LINE)->MoveWindow(-8, 42, 10000, 2, TRUE);

	/*! create status bar control*/
	if( !m_statusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM) )
	{
		TRACE0("Cannot create statusbar.\n");
		return;
	}
	m_statusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	CRect client_rect;
	GetClientRect(client_rect);
	m_statusBar.MoveWindow(0, client_rect.bottom - 20, client_rect.right, 20);
	m_statusBar.SetPaneInfo(0, m_statusBar.GetDlgCtrlID(), SBPS_STRETCH, client_rect.right - 60); // SBPS_STRETCH Automatic stretch pane
	m_statusBar.SetPaneInfo(1, m_statusBar.GetDlgCtrlID(), SBPS_NORMAL,  30);
	m_statusBar.SetPaneText(0, _T("Ready"));
	//m_statusBar.SetPaneText(1, _T("NUM"));

	// initialize controls status value
	m_ecmCheckbox.SetCheck(1);

	/*! anchor all controls */
	m_dlgAnchor.Add(m_toolbarCtrl.m_hWnd, ANCHOR_TOPLEFT);
	m_dlgAnchor.Add(IDC_STATIC_HORIZONTAL_LINE, ANCHOR_TOPLEFT);
	//m_dlgAnchor.Add(IDC_STATIC_VIEW_PARENT, ANCHOR_ALL);
	m_dlgAnchor.Add(m_addressView, ANCHOR_ALL);
	m_dlgAnchor.Add(m_statusBar.m_hWnd, ANCHOR_BOTTOMLEFT | ANCHOR_BOTTOMRIGHT);

	m_dlgAnchor.Add(m_addButton, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_editButton, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_deleteButton, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_BUTTON_OK, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_BUTTON_CANCEL, ANCHOR_BOTTOMLEFT);

	m_dlgAnchor.Add(m_nameEdit, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_faxNumberEdit, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_companyEdit, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_departmentEdit, ANCHOR_BOTTOMLEFT);

	m_dlgAnchor.Add(m_speedCombox, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(m_ecmCheckbox, ANCHOR_BOTTOMLEFT);

	m_dlgAnchor.Add(IDC_STATIC_CONTACT, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_NAME, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_FAXNUMBER, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_COMPANY, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_DEPARTMENT, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_SPEED, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_INFO1, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_INFO2, ANCHOR_BOTTOMLEFT);
	m_dlgAnchor.Add(IDC_STATIC_ITEM_COUNT, ANCHOR_BOTTOMRIGHT);
}

void CAddressBookApplicationDlg::initView()
{
	// set style for address listview
	m_addressView.initialize();
	m_addressView.SetParent(this);

	// set model header that apply to address listview header
	m_addressBook->setModelHeader("No.", ABE::Role_Id);
	m_addressBook->setModelHeader("Type", ABE::Role_Type);
	m_addressBook->setModelHeader("Name", ABE::Role_Name);
	m_addressBook->setModelHeader("Source", ABE::Role_Source);
	m_addressBook->setModelHeader("FAX number", ABE::Role_Fax);
	m_addressBook->setModelHeader("Company", ABE::Role_Company);
	m_addressBook->setModelHeader("Department", ABE::Role_Department);
	m_addressBook->setModelHeader("ECM", ABE::Role_ECM);
	m_addressBook->setModelHeader("FAX communication speed", ABE::Role_Speed);

	AbstractModel *model = m_addressBook->sourceModel();
	// must set model to address list view
	m_addressView.setEntryModel(model);//;
}

void CAddressBookApplicationDlg::updateUi(bool enabled)
{
	CString empty = _T("");
	m_nameEdit.SetWindowText(empty);
	m_faxNumberEdit.SetWindowText(empty);
	m_companyEdit.SetWindowText(empty);
	m_departmentEdit.SetWindowText(empty);
	m_speedCombox.SetCurSel(0);
	m_ecmCheckbox.SetCheck(1);

	GetDlgItem(IDC_BUTTON_OK)->EnableWindow(enabled);// testing set true always
	GetDlgItem(IDC_BUTTON_CANCEL)->EnableWindow(enabled);
	m_addressView.EnableWindow(enabled);
	m_nameEdit.EnableWindow(enabled);
	m_nameEdit.SetFocus();
	m_faxNumberEdit.EnableWindow(enabled);
	m_companyEdit.EnableWindow(enabled);
	m_departmentEdit.EnableWindow(enabled);
	m_speedCombox.EnableWindow(enabled);
	m_ecmCheckbox.EnableWindow(enabled);

	m_addButton.EnableWindow(enabled);
	m_editButton.EnableWindow(enabled);
	m_deleteButton.EnableWindow(enabled);

	GetDlgItem(IDC_STATIC_NAME)->EnableWindow(enabled);
	GetDlgItem(IDC_STATIC_FAXNUMBER)->EnableWindow(enabled);
	GetDlgItem(IDC_STATIC_COMPANY)->EnableWindow(enabled);
	GetDlgItem(IDC_STATIC_DEPARTMENT)->EnableWindow(enabled);
	GetDlgItem(IDC_STATIC_SPEED)->EnableWindow(enabled);
}
/*!
 @brief: Set tooltip/clicked event for toolbar button
 @auth:Yuri Yang@liteon.com 2016-6-21
*/
BOOL CAddressBookApplicationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// set toolbar buttons tooltips
	TOOLTIPTEXT *tooltip = (TOOLTIPTEXT*)lParam;
	if ( tooltip->hdr.code == TTN_NEEDTEXT)
	{
		CString strTooltip;
		switch( tooltip->hdr.idFrom )
		{
			case ID_TOOLBAR_NEW:
				strTooltip.LoadString(IDS_TOOLBAR_NEW);
				break;
			case  ID_TOOLBAR_OPEN:
				strTooltip.LoadString(IDS_TOOLBAR_OPEN);
				break;
			case ID_TOOLBAR_SAVE:
				strTooltip.LoadString(IDS_TOOLBAR_SAVE);
				break;
			case ID_TOOLBAR_SAVEAS:
				strTooltip.LoadString(IDS_TOOLBAR_SAVEAS);
				break;
			case ID_TOOLBAR_LOADFROMDEVICE:
				strTooltip.LoadString(IDS_TOOLBAR_LOADFROMDEVICE);
				break;
			case ID_TOOLBAR_SAVETODEVICE:
				strTooltip.LoadString(IDS_TOOLBAR_SAVETODEVICE);
				break;
			case ID_TOOLBAR_ADDGROUP:
				strTooltip.LoadString(IDS_TOOLBAR_ADDGROUP);
				break;
			case ID_TOOLBAR_EDITGROUP:
				strTooltip.LoadString(IDS_TOOLBAR_EDITGROUP);
				break;
			case ID_TOOLBAR_DELETEDUPLICATES:
				strTooltip.LoadString(IDS_TOOLBAR_DELETEDUPLICATES);
				break;
			case ID_TOOLBAR_BLOCKNUMBERS:
				strTooltip.LoadString(IDS_TOOLBAR_BLOCKNUMBERS);
				break;
			case ID_TOOLBAR_HELP:
				strTooltip.LoadString(IDS_TOOLBAR_HELP);
				break;
				break;
			default:
				return CDialog::OnNotify(wParam, lParam, pResult);
		}

#ifdef _UNICODE
				wmemset( tooltip->szText, 0, strTooltip.GetLength()+1 );
				wmemcpy( tooltip->szText, strTooltip.GetBuffer(strTooltip.GetLength()), strTooltip.GetLength() );
#else
				int len = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)strTooltip, -1, NULL, NULL, 0, NULL);
				WideCharToMultiByte(CP_ACP, 0, (wchar_t*)strTooltip, strTooltip.GetLength(), tooltip->szText, len, 0, NULL);
#endif
		return TRUE;
	}
	// set status bar messages
	TBNOTIFY *btn = (TBNOTIFY*)lParam;
	if (btn->hdr.code == TBN_HOTITEMCHANGE)//HICF_ENTERING
	{
		switch(btn->tbButton.iBitmap)
		{
			case ID_TOOLBAR_NEW:
				m_statusBar.SetPaneText(0, _T("Create a new address book."));
				break;
			case ID_TOOLBAR_OPEN:
				m_statusBar.SetPaneText(0, _T("Open an existing address book."));
				break;
			case ID_TOOLBAR_SAVE:
				m_statusBar.SetPaneText(0, _T("Save address book."));
				break;
			case ID_TOOLBAR_SAVEAS:
				m_statusBar.SetPaneText(0, _T("Save address book to a new file."));
				break;
			case ID_TOOLBAR_LOADFROMDEVICE:
				m_statusBar.SetPaneText(0, _T("Load address book from device."));
				break;
			case ID_TOOLBAR_SAVETODEVICE:
				m_statusBar.SetPaneText(0, _T("Save address book to device."));
				break;
			case ID_TOOLBAR_ADDGROUP:
				m_statusBar.SetPaneText(0, _T("Add an address book group."));
				break;
			case ID_TOOLBAR_EDITGROUP:
				m_statusBar.SetPaneText(0, _T("Edit an address book group."));
				break;
			case ID_TOOLBAR_DELETEDUPLICATES:
				m_statusBar.SetPaneText(0, _T("Delete all duplicates entries."));
				break;
			case ID_TOOLBAR_BLOCKNUMBERS:
				m_statusBar.SetPaneText(0, _T("Set up number blocking."));
				break;
			case ID_TOOLBAR_HELP:
				m_statusBar.SetPaneText(0, _T("Help"));
				break;
			default:
				m_statusBar.SetPaneText(0, _T("Ready"));
				break;
		}
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}

/*! 
 @brief:Set background color for edit/static control when set it disabled
 @auth:Yuri Yang@liteon.com  2016-6-21
*/
HBRUSH CAddressBookApplicationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (nCtlColor) //handling each control
	{  
	// process my edit controls by ID.
	case CTLCOLOR_STATIC:
		{
			switch(pWnd->GetDlgCtrlID())
			{
			case IDC_EDIT_NAME:
			case IDC_EDIT_FAXNUMBER:
			case IDC_EDIT_COMPANY:
			case IDC_EDIT_DEPARTMENT:
			case IDC_COMBO_SPEED:
				{
					// edit controls background color(it's working when set disabled or enabled)
					COLORREF clr(RGB(255,255,255));
					//pDC->SetTextColor(clr);   
					pDC->SetBkColor(clr);
					pDC->SetBkMode(TRANSPARENT); 
					hbr = ::CreateSolidBrush(clr);
				}
				break;
			case IDC_STATIC_INFO1:
			case IDC_STATIC_INFO2:
			case IDC_STATIC_ITEM_COUNT:
				{
					COLORREF clr(RGB(115,115,115));
					//COLORREF winClr = (COLORREF)GetSysColor(COLOR_WINDOW);
					pDC->SetTextColor(clr);
				}
				break;
			case IDC_STATIC_ADDRESSBOOK:
				{
					COLORREF clr(RGB(0,70,213));
					pDC->SetTextColor(clr);
				}
				break;
			default:
				break;  
			}
		};
		break;

	default:  
		break;  
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CAddressBookApplicationDlg::OnNMDblClickAddressview(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int row = pNMItemActivate->iItem;
	
	if( row > -1 && m_addressBook->entryType(row) == ABE::GroupType)
	{
		EntryItem *item = m_addressBook->entryOf(row);
		AddGroupDlg dlg(item, this);
		dlg.setModel(m_addressBook->sourceModel());
		INT_PTR r = dlg.DoModal();
		if (r == IDOK && dlg.hasChanged())
		{
			m_isChanged = true;
			this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
		}
	}
}

/*! 
 @brief:Create a new contact
 @auth:Yuri Yang@liteon.com  2016-7-5
*/
void CAddressBookApplicationDlg::addContact()
{
	CString name;
	CString source = _T("HOST");
	CString fax;
	CString company;
	CString department;
	m_nameEdit.GetWindowText(name);
	m_faxNumberEdit.GetWindowText(fax);
	m_companyEdit.GetWindowText(company);
	m_departmentEdit.GetWindowText(department);
	CString strECM;
	m_ecmCheckbox.GetCheck() == 1 ? strECM = _T("On") : strECM = _T("Off");
	CString strSpeed;
	m_speedCombox.GetWindowText(strSpeed);

	EntryItem *item = new EntryItem;

	int count = m_addressBook->size();
	// auto order id
	// eg: current cur_id = 3, model exists id 3 but lost 4, then cur_id = 4.
	// too slowly too bad
	int id = 1;
	while ( m_addressBook->hasId(id, ABE::ContactType) )
		++id;

	char buff[10];
	sprintf_s(buff, "%d", id);
	item->setData( buff, ABE::Role_Id );


	item->setType( ABE::ContactType );
	item->setData( Util::WToA(name), ABE::Role_Name );
	item->setData( Util::WToA(source), ABE::Role_Source );
	item->setData( Util::WToA(fax), ABE::Role_Fax );
	item->setData( Util::WToA(company), ABE::Role_Company );
	item->setData( Util::WToA(department), ABE::Role_Department );
	item->setData( Util::WToA(strECM), ABE::Role_ECM );
	item->setData( Util::WToA(strSpeed), ABE::Role_Speed );
	m_addressBook->appendEntry(item);
	{
		updateCount();
		::SendMessage(m_addressView, WM_ADDRBOOK_ADD, count, 0);
	}
}

/*! 
 @brief:Edit current selected contact
 @auth:Yuri Yang@liteon.com  2016-7-5
*/
void CAddressBookApplicationDlg::editContact()
{
	CString name;
	CString source = _T("HOST");
	CString fax;
	CString company;
	CString department;
	m_nameEdit.GetWindowText(name);
	m_faxNumberEdit.GetWindowText(fax);
	m_companyEdit.GetWindowText(company);
	m_departmentEdit.GetWindowText(department);
	CString strECM;
	m_ecmCheckbox.GetCheck() == 1 ? strECM = _T("On") : strECM = _T("Off");
	CString strSpeed;
	m_speedCombox.GetWindowText(strSpeed);

	POSITION pos = m_addressView.GetFirstSelectedItemPosition();
	if (pos)
	{
		int row = m_addressView.GetNextSelectedItem(pos);
		EntryItem *item = m_addressBook->entryOf(row);
		if( item )
		{
			item->setData( Util::WToA(name), ABE::Role_Name );
			item->setData( Util::WToA(source), ABE::Role_Source );
			item->setData( Util::WToA(fax), ABE::Role_Fax );
			item->setData( Util::WToA(company), ABE::Role_Company );
			item->setData( Util::WToA(department), ABE::Role_Department );
			item->setData(Util::WToA(strECM), ABE::Role_ECM );
			item->setData(Util::WToA(strSpeed), ABE::Role_Speed);
			::SendMessage(m_addressView, WM_ADDRBOOK_EDIT, row, ABE::ContactType);
		}
	}
}

/*! 
 @brief:delete current selected contact(s),delete contact from tail to head.
 @auth:Yuri Yang@liteon.com  2016-7-5
*/
void CAddressBookApplicationDlg::deleteContact()
{
	POSITION pos = m_addressView.GetFirstSelectedItemPosition();
	list<int> indexes;
	while(pos)
	{
		// collect all item indexes that would be removed.
		int idx = m_addressView.GetNextSelectedItem(pos);
		indexes.push_back(idx);
	}

	while( !indexes.empty() )
	{
		// we remove item from  end to font.
		// if remove item from font to end, the item index would be changed
		int index = indexes.back();
		if( m_addressBook->removeAt(index) )
		{		
			indexes.pop_back();
			updateCount();
			::SendMessage(m_addressView, WM_ADDRBOOK_DELETE, index, 0);
		}
	}
}

bool CAddressBookApplicationDlg::newFile()
{
	if( m_isChanged )
	{
		CString info;
		info.Format(_T("Do you want to save the changes to \"%s\"?"), m_currentFilename);
		int ret =  MessageBox(info, _T(""), MB_YESNOCANCEL);
		if( ret == IDYES)
		{
			if ( this->saveFile() )
				m_isChanged = false;
			else
				return false;
		}
		else if(ret == IDCANCEL)
		{
			return false;
		}
	}
	m_currentFilepath.Empty();
	m_currentFilename = _T("Untitled");
	this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );
	m_editButton.EnableWindow(false);
	m_deleteButton.EnableWindow(false);
	m_addressBook->removeAll();
	updateCount();
	::SendMessage(m_addressView, WM_ADDRBOOK_CLEAR, 0, 0);

	return true;
}

bool CAddressBookApplicationDlg::openFile()
{
	if( m_isChanged )
	{
		CString info;
		info.Format(_T("Do you want to save the changes to \"%s\"?"), m_currentFilename);
		int ret =  MessageBox(info, _T(""), MB_YESNOCANCEL);
		if( ret == IDYES)
		{
			if ( this->saveFile() )
				m_isChanged = false;
			else
				return false;
		}
		else if(ret == IDCANCEL)
		{
			// do nothing
			return false;
		}
	}

	CString ext = _T("Address Book Files (*.fdt)|*.fdt|All Files (*.*)|*.*||");
	CFileDialog openFileDlg(TRUE, ext, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, ext, this);
	if( IDOK != openFileDlg.DoModal() )
		return false;
	m_currentFilepath = openFileDlg.GetPathName();
	m_currentFilename = openFileDlg.GetFileName();

	m_addressBook->removeAll();
	::SendMessage(m_addressView, WM_ADDRBOOK_RERLOAD, 0, 0);

	bool opened = m_addressBook->openBook(Util::WToA(m_currentFilepath));
	return opened;
}

bool CAddressBookApplicationDlg::saveFile()
{
	if( m_currentFilepath.IsEmpty())
	{
		CString ext = _T("Address Book Files (*.fdt)|*.fdt|All Files (*.*)|*.*||");
		// Get file path
		CFileDialog saveFileDlg(FALSE, ext, m_currentFilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, ext, this);
		if (IDOK != saveFileDlg.DoModal())
		{
			return false;
		}
		m_currentFilepath = saveFileDlg.GetPathName();
		m_currentFilename = saveFileDlg.GetFileName();
	}

	bool saved = m_addressBook->saveBook( Util::WToA(m_currentFilepath) );
	return saved;
}

bool CAddressBookApplicationDlg::saveAsFile()
{
	CString ext = _T("Address Book Files (*.fdt)|*.fdt|All Files (*.*)|*.*||");
	CFileDialog saveFileDlg(FALSE, ext, m_currentFilename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, ext, this);
	if (IDOK != saveFileDlg.DoModal())
	{
		return false;
	}
	CString tempPath = m_currentFilepath;
	CString tempName = m_currentFilename;

	m_currentFilepath = saveFileDlg.GetPathName();
	m_currentFilename = saveFileDlg.GetFileName();

	bool saved = m_addressBook->saveAsBook( Util::WToA(m_currentFilepath) );
	return saved;
}

void CAddressBookApplicationDlg::OnToolbarButtonClicked(UINT nID)
{
	CString ext = _T("Address Book Files (*.fdt)|*.fdt|All Files (*.*)|*.*||");

	//int position = nID - ID_TOOLBAR_NEW;
	switch( nID )
	{
	case ID_TOOLBAR_NEW:
		{
			newFile();
		}
		break;
	case ID_TOOLBAR_OPEN:
		{
			if (this->openFile())
			{
				this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );

				updateCount();
				::SendMessage(m_addressView, WM_ADDRBOOK_RERLOAD, 0, 0);
			}
		}
		break;

	case ID_TOOLBAR_SAVE:
		{
			if (this->saveFile())
			{
				m_isChanged = false;
				this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );
			}
		}
		break;
	case ID_TOOLBAR_SAVEAS:
		{
			if( this->saveAsFile() )
			{
				m_isChanged = false;
				this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );
			}
		}
		break;
	case ID_TOOLBAR_LOADFROMDEVICE:
		{
			// Connection is interrupted, throw a error!!!
			// and set a timer to listen for connection.
			if( !m_addressBook->isConnected())
			{
				if( !connectToDevice())
				{
					CString str;
					str.LoadString(IDS_ERROR_CONNECT);
					MessageBox(str, _T(""), MB_OK | MB_ICONERROR);
					SetTimer(TIMER_DEVICE_CNNT, 10000, 0);
					return;
				}
			}

			//check file is saved?
			if (m_isChanged)
			{
				CString info;
				info.LoadString(IDS_DEVICE_LOAD);
				int ret =  MessageBox(info, _T(""), MB_YESNO);
				if( ret == IDNO)
				{
					if ( !this->saveFile() )
						return;
				}

				m_isChanged = false;
			}

			// reset address book file
			if( newFile() )
			{
				m_progressDlg.centerWindow(this);
				m_progressDlg.ShowWindow(SW_SHOW);
				m_progressDlg.setProgressPos(0);
				CString str;
				str.LoadString(IDS_PROGRESS_RETRIEVING);
				m_progressDlg.setStatusText(str);

				m_progressDlg.setProgressPos(30);
				m_progressDlg.setStepText(_T("1/3"));
				if (loadFromDevice())
				{
					m_progressDlg.setStepText(_T("2/3"));

					int percent = 0;
					int total = m_addressBook->count();
					for (int i = 0; i < total; ++i)
					{
						percent = (i+1) * 100 / total;
						m_progressDlg.setProgressPos(percent);

						updateCount();
						::SendMessage(m_addressView, WM_ADDRBOOK_ADD, i, 0);
					}

					m_isChanged = false;
					this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );
					m_addressBook->disconnectDevice();

					m_progressDlg.setStepText(_T("3/3"));
					m_progressDlg.setProgressPos(100);
					SetTimer(TIMER_PROGRESS_HIDE, 400, 0);
				}
			}
		}
		break;
	case ID_TOOLBAR_SAVETODEVICE:
		{
			// Connection is interrupted, throw a error!!!
			// and set a timer to listen for connection.
			if( !m_addressBook->isConnected())
			{
				if( !connectToDevice())
				{
					CString str;
					str.LoadString(IDS_ERROR_CONNECT);
					MessageBox(str, _T(""), MB_OK | MB_ICONERROR);
					SetTimer(TIMER_DEVICE_CNNT, 10000, 0);
					return;
				}
			}

			m_progressDlg.centerWindow(this);
			m_progressDlg.ShowWindow(SW_SHOW);
			m_progressDlg.setProgressPos(0);
			CString str;
			str.LoadString(IDS_PROGRESS_SAVING);
			m_progressDlg.setStatusText(str);


			if (saveToDevice())
			{
				CString str;
				str.LoadString(IDS_PROGRESS_RETRIEVING);

				m_progressDlg.setStatusText(str);
				m_progressDlg.setProgressPos(60);
				m_progressDlg.setStepText(_T("2/3"));

				m_isChanged = false;
				this->SetWindowText( m_currentFilename + _T(" - ") + m_windowTitle );
				m_addressBook->disconnectDevice();

				m_progressDlg.setStepText(_T("3/3"));
				m_progressDlg.setProgressPos(100);
				SetTimer(TIMER_PROGRESS_HIDE, 400, 0);
			}
		}
		break;
	case ID_TOOLBAR_ADDGROUP:
		{
			AddGroupDlg dlg(this);
			dlg.setModel(m_addressBook->sourceModel());
			INT_PTR ret = dlg.DoModal();
			//if( ret == IDOK )
			bool ok = dlg.hasChanged();
			if( ok )
			{
				m_isChanged = ok;
				this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
			}
		}
		break;
	case ID_TOOLBAR_EDITGROUP:
		{
			POSITION pos = m_addressView.GetFirstSelectedItemPosition();
			int row = m_addressView.GetNextSelectedItem(pos);

			EntryItem *item = m_addressBook->entryOf(row);
			if( item )
			{
				AddGroupDlg dlg(item, this);
				dlg.setModel(m_addressBook->sourceModel());
				INT_PTR r = dlg.DoModal();
				if (r == IDOK && dlg.hasChanged())
				{
					m_isChanged = true;
					this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
				}
			}
		}
		break;
	case ID_TOOLBAR_DELETEDUPLICATES:
		{
			//deleteDuplicates();
			int count = m_addressBook->removeDuplicates();
			if (count > 0)
			{
				updateCount();
				::SendMessage(m_addressView, WM_ADDRBOOK_RERLOAD, 0, 0);
				m_isChanged = true;
				this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
			}
		}
		break;
	case ID_TOOLBAR_BLOCKNUMBERS:
		{
			BlockedNumberDlg blockedNumberDlg;
			blockedNumberDlg.DoModal();
		}
		break;
	case ID_TOOLBAR_HELP:
		{

		}
		break;

	default:break;
	}
}

// no working
void CAddressBookApplicationDlg::OnUpdateUI(CCmdUI *pCmdUI)
{
	MessageBox(_T("aaa"), _T("sss"));

	switch( pCmdUI->m_nID )
	{
	case ID_TOOLBAR_NEW:
		{
			MessageBox(_T("aaa"), _T("sss"));
		}
		break;
	}
	
}

void CAddressBookApplicationDlg::OnDlgButtonClicked(UINT nID)
{
	switch( nID )
	{
	case IDC_BUTTON_ADD:
		{
			isEditMode = false;
			bool enabled = TRUE;
			updateUi(enabled);
			m_addButton.EnableWindow(!enabled);
			m_editButton.EnableWindow(!enabled);
			m_deleteButton.EnableWindow(!enabled);
			m_addressView.EnableWindow(!enabled);
		}
		break;

	case IDC_BUTTON_EDIT:
		{
			POSITION pos = m_addressView.GetFirstSelectedItemPosition();
			int row = m_addressView.GetNextSelectedItem(pos);

			EntryItem *item = m_addressBook->entryOf(row);
			if (!item)
				return;

			if( item->type() == ABE::GroupType)
			{
				AddGroupDlg dlg(item, this);
				dlg.setModel(m_addressBook->sourceModel());
				INT_PTR r = dlg.DoModal();
				if (r == IDOK && dlg.hasChanged())
				{
					m_isChanged = true;
					this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
				}
			}
			else
			{
				isEditMode = true;
				bool enabled = TRUE;
				updateUi(enabled);
				m_addressView.EnableWindow(!enabled);
				m_addButton.EnableWindow(!enabled);
				m_editButton.EnableWindow(!enabled);
				m_deleteButton.EnableWindow(!enabled);

				m_nameEdit.SetWindowText( m_addressView.GetItemText(row, ABE::Role_Name) );
				m_faxNumberEdit.SetWindowText(m_addressView.GetItemText(row, ABE::Role_Fax));
				m_companyEdit.SetWindowText(m_addressView.GetItemText(row, ABE::Role_Company));
				m_departmentEdit.SetWindowText(m_addressView.GetItemText(row, ABE::Role_Department));
				m_speedCombox.SetWindowText(m_addressView.GetItemText(row, ABE::Role_Speed));
				m_addressView.GetItemText(row, ABE::Role_ECM) == CString(_T("On")) ? m_ecmCheckbox.SetCheck(1) : m_ecmCheckbox.SetCheck(0);
			}
		}
		break;

	case IDC_BUTTON_DELETE:
		{
			deleteContact();
			if ( m_addressBook->isEmpty() )
			{
				m_editButton.EnableWindow(FALSE);
				m_deleteButton.EnableWindow(FALSE);
			}
			m_isChanged = true;
			this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
		}
		break;

	case IDC_BUTTON_OK:
		{
			//test larg data
// 			for (int i = 0; i < 300; ++i)
// 			{
// 				addContact();
// 			}
			isEditMode ? editContact() : addContact();

			m_isChanged = true;
			isEditMode = false;

			bool enabled = FALSE;
			updateUi(enabled);
			m_addButton.EnableWindow(!enabled);
			m_addressView.EnableWindow(!enabled);

			//int index = m_addressView.GetSelectionMark();
			//if ( index != -1 )
			{
				m_editButton.EnableWindow(!enabled);
				m_deleteButton.EnableWindow(!enabled);
			}

			this->SetWindowText( m_titleSymbol + m_currentFilename + _T(" - ") + m_windowTitle );
		}
		break;

	case IDC_BUTTON_CANCEL:
		{
			isEditMode = false;
			bool enabled = FALSE;
			updateUi(enabled);

			m_addButton.EnableWindow(!enabled);
			m_addressView.EnableWindow(!enabled);
			if ( m_addressView.GetSelectedCount() > 0)
			{
				m_editButton.EnableWindow(!enabled);
				m_deleteButton.EnableWindow(!enabled);
			}
		}
		break;

	default:break;
	}
}

void CAddressBookApplicationDlg::OnClose()
{
	if( m_isChanged )
	{
		CString info;
		info.Format(_T("Do you want to save the changes to \"%s\"?"), m_currentFilename);
		int ret =  MessageBox(info, _T(""), MB_YESNOCANCEL);
		if( ret == IDYES)
		{
			if ( this->saveFile() )
				m_isChanged = false;
		}
		else if(ret == IDCANCEL)
		{
			// do nothing
			return;
		}
	}
	CDialog::OnClose();
}

LRESULT CAddressBookApplicationDlg::OnItemSelectionChanged(WPARAM wParam, LPARAM lParam)
{
	int idx = (int)wParam;
	if (idx < 0)
	{
		m_toolbarCtrl.EnableButton(ID_TOOLBAR_EDITGROUP, FALSE);
		m_editButton.EnableWindow(FALSE);
		m_deleteButton.EnableWindow(FALSE);
	}
	else
	{
		// maybe the index item has been deleted?
		m_addressBook->entryType(idx) == ABE::GroupType ? m_toolbarCtrl.EnableButton(ID_TOOLBAR_EDITGROUP): m_toolbarCtrl.EnableButton(ID_TOOLBAR_EDITGROUP, FALSE);

		m_editButton.EnableWindow(TRUE);
		m_deleteButton.EnableWindow(TRUE);
	}

	return 0;
}

BOOL CAddressBookApplicationDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE )
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CAddressBookApplicationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	switch( nIDEvent )
	{
	case TIMER_DEVICE_CNNT:
		{
			if (connectToDevice())
			{
#ifdef _DEBUG
				m_statusBar.SetPaneText(0, _T("connect to the device succeed!"));
#endif
				m_toolbarCtrl.EnableButton(ID_TOOLBAR_LOADFROMDEVICE);
				m_toolbarCtrl.EnableButton(ID_TOOLBAR_SAVETODEVICE);
				m_toolbarCtrl.EnableButton(ID_TOOLBAR_BLOCKNUMBERS);
				KillTimer(TIMER_DEVICE_CNNT);
				m_addressBook->disconnectDevice();
			}
			else
			{
#ifdef _DEBUG
				m_statusBar.SetPaneText(0, _T("Retry after 10 seconds to connect to the device..."));
				SetTimer(TIMER_DEVICE_CNNT, 10000, 0);
#else
				KillTimer(nIDEvent);
#endif
			}
		}
		break;

	case TIMER_PROGRESS_HIDE:
		{
			m_progressDlg.ShowWindow(SW_HIDE);
			KillTimer(TIMER_PROGRESS_HIDE);
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

bool CAddressBookApplicationDlg::connectToDevice()
{
	const string printerName = "Dell Color MFP E525w Fax";
	return m_addressBook->connectToDevice(printerName);
}

bool __stdcall readCallbackFn(void *buffer, DWORD size, int type, void *arg)
{
	AbstractModel *model =  (AbstractModel*)arg;
	if( !model )
		return false;

	CAddressBookApplicationDlg *dlg = (CAddressBookApplicationDlg*)theApp.m_pMainWnd;
	if (!dlg)
	{
		return false;
	}

	int total = model->rowCount();
	if ( total <= 0 )
		return false;

	dlg->m_progressDlg.setStepText(_T("2/3"));
	int oldPos = dlg->m_progressDlg.progressPos();
	int percent = 0;
	for (int i = 0; i < total; ++i)
	{
		percent = (i+1) * 300 / total;
		dlg->m_progressDlg.setProgressPos(percent+oldPos);

		dlg->updateCount();
		::SendMessage(dlg->addressView(), WM_ADDRBOOK_ADD, i, 0);
	}

	return true;
}

bool CAddressBookApplicationDlg::loadFromDevice()
{
	return m_addressBook->loadFromDevice();
}

bool __stdcall writeCallbackFn(void *data, DWORD size, int type, void *arg)
{
	AbstractModel *model =  (AbstractModel*)arg;
	if( !model )
		return false;

	CAddressBookApplicationDlg *dlg = (CAddressBookApplicationDlg*)theApp.m_pMainWnd;
	if (!dlg)
		return false;

	CString str;
	str.LoadString(IDS_PROGRESS_RETRIEVING);
	dlg->m_progressDlg.setStatusText(str);
	dlg->m_progressDlg.setProgressPos(60);
	dlg->m_progressDlg.setStepText(_T("2/3"));

	return true;
}

bool CAddressBookApplicationDlg::saveToDevice()
{
	return m_addressBook->saveToDevice();
}

void CAddressBookApplicationDlg::updateCount()
{
	CString totalStr;
	totalStr.Format(_T("items:%d"),  m_addressBook->size());
	GetDlgItem(IDC_STATIC_ITEM_COUNT)->SetWindowText(totalStr);
}

