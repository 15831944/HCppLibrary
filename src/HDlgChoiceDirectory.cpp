#include "stdafx.h"

//#include "resource.h"
#include <direct.h>

#include "HDlgChoiceDirectory.h"

//#if ! defined _WIN32
#ifndef x64
    #define GWL_HINSTANCE GWLP_HINSTANCE
#else
#endif

// HDlgChoiceDirectory dialog

IMPLEMENT_DYNAMIC(HDlgChoiceDirectory, HDlg)

HDlgChoiceDirectory::HDlgChoiceDirectory (CWnd* pParent) : HDlg (pParent), /*CDialog (HDlgChoiceDirectory::IDD, pParent),*/
																	//m_lpszTemplateName (_T ("")),
																	//m_hDialogTemplate (0x0),
																	m_strPath (_T ("")),
																	m_ptrTreeCtrlDirectory (0x0) {
}

//extern BOOL CALLBACK deleteChildWindows (HWND , LPARAM );

HDlgChoiceDirectory::~HDlgChoiceDirectory (void) {
    m_strPath.free ();

	delete m_ptrTreeCtrlDirectory;
	//EnumChildWindows (this->m_hWnd, deleteChildWindows, 0x0);

	/*CWnd *ptrChildWnd = 0x0;
	ptrChildWnd = CWnd::FromHandle (::GetDlgItem (this->m_hWnd, IDOK));
	delete ptrChildWnd;
	ptrChildWnd = CWnd::FromHandle (::GetDlgItem (this->m_hWnd, IDCANCEL));
	delete ptrChildWnd;*/

	// Освобождение использованной памяти
}

void HDlgChoiceDirectory::DoDataExchange (CDataExchange* pDX) {
    HDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREECTRL_DIRECTORY, *(CWnd *) m_ptrTreeCtrlDirectory);
}


BEGIN_MESSAGE_MAP(HDlgChoiceDirectory, HDlg)
	ON_BN_CLICKED(IDOK, &HDlgChoiceDirectory::OnBnClickedOk)
//  ON_NOTIFY(TVN_SINGLEEXPAND, IDC_TREE_DIRECTORY, &HDlgChoiceDirectory::OnTvnSingleExpandTreeDirectory)
//  ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_DIRECTORY, &HDlgChoiceDirectory::OnTvnItemexpandedTreeDirectory)
    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREECTRL_DIRECTORY, &HDlgChoiceDirectory::OnTvnItemexpandingTreeDirectory)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREECTRL_DIRECTORY, &HDlgChoiceDirectory::OnTvnSelchangedTreeDirectory)
END_MESSAGE_MAP()

// HDlgChoiceDirectory message handlers

BOOL HDlgChoiceDirectory::OnInitDialog () {
    SetWindowText (_T ("Выбор директории...: отмена <ESC>"));

    ModifyStyle (WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 0);

	CButton *ptrButton = 0x0;
	HWND hWndButton;
	m_ptrTreeCtrlDirectory = DEBUG_NEW CTreeCtrl ();

	UINT styleTreeCtrl = WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | WS_HSCROLL  | WS_VSCROLL | WS_TABSTOP | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		styleButton = WS_VISIBLE | /*WS_BORDER |*/ WS_OVERLAPPED | WS_CHILD | WS_TABSTOP;
	
	CRect rcDlgClient, rcTreeCtrl, rcButton;
    this->GetClientRect (&rcDlgClient);
	rcTreeCtrl.left = DLG_MARGIN_ITEM; rcTreeCtrl.top = DLG_MARGIN_ITEM; rcTreeCtrl.right = rcDlgClient.Width () - DLG_MARGIN_ITEM; rcTreeCtrl.bottom = rcDlgClient.Height () - 2 * DLG_MARGIN_ITEM - DLG_HEIGHT_ITEM;

	if (m_ptrTreeCtrlDirectory->Create (styleTreeCtrl, rcTreeCtrl, this, IDC_TREECTRL_DIRECTORY)) {
		//ptrButton = DEBUG_NEW CButton ();
		rcButton.left = rcDlgClient.Width () - 2 * DLG_MARGIN_ITEM - 2 * 95; rcButton.top = rcDlgClient.Height () - DLG_MARGIN_ITEM - DLG_HEIGHT_ITEM;
		//if (ptrButton->Create (_T ("Отмена"), styleButton, rcButton, this, IDCANCEL)) {
		hWndButton = CreateWindow (_T ("BUTTON"), _T ("OK"), styleButton, rcButton.left, rcButton.top, 95, DLG_HEIGHT_ITEM , m_hWnd, (HMENU) IDOK, (HINSTANCE) (LONG_PTR) GetWindowLong (m_hWnd, GWL_HINSTANCE), NULL);
		if (hWndButton) {
			::SendDlgItemMessage (m_hWnd, IDOK, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);
			//ptrButton = DEBUG_NEW CButton ();
			rcButton.left += (DLG_MARGIN_ITEM + 95);
			//if (ptrButton->Create (_T ("OK"), styleButton, rcButton, this, IDOK)) {
			hWndButton = CreateWindow (_T ("BUTTON"), _T ("Отмена"), styleButton, rcButton.left, rcButton.top, 95, DLG_HEIGHT_ITEM , m_hWnd, (HMENU) IDCANCEL, (HINSTANCE) (LONG_PTR) GetWindowLong (m_hWnd, GWL_HINSTANCE), NULL);
			if (hWndButton) {
				::SendDlgItemMessage (m_hWnd, IDCANCEL, WM_SETFONT, (WPARAM) GetStockObject (DEFAULT_GUI_FONT), TRUE);

				m_tvInsert.hParent = NULL;
				m_tvInsert.hInsertAfter = NULL;
				m_tvInsert.item.cchTextMax = MAX_PATH + 1;
				m_tvInsert.item.mask = TVIF_TEXT;
				m_tvInsert.hInsertAfter = TVI_FIRST;
				m_tvInsert.item.pszText = _T("Мой компьютер");
			    
				m_hRoot = m_ptrTreeCtrlDirectory->InsertItem (&m_tvInsert);
				SetChildItems (m_hRoot, NULL);
				m_ptrTreeCtrlDirectory->SelectItem(m_hRoot);
			    
				//Уже инициализировали до вызова 'DoModal'
				//this->GetParent ()->GetDlgItem (IDC_EDIT_PATH)->GetWindowText (m_strPath);
				
				//ExpandCurrent (m_hRoot, m_strPath);
			}
			else
				; // Не создана 'IDOK'
		}
		else
			; // Не создана 'IDCANCEL'
	}
	else
	    return false;
	
	HDlg::OnInitDialog ();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void HDlgChoiceDirectory::OnBnClickedOk ()
{
	// TODO: Add your control notification handler code here
    m_strPath = GetFullPath (m_ptrTreeCtrlDirectory->GetSelectedItem ());

	HDlg::OnOK ();
}

int HDlgChoiceDirectory::SetChildItems(HTREEITEM hItem, LPCTSTR path)
{
    CFileFind finder;
    CString strItem;
    TVINSERTSTRUCT tvInsert;
   
    tvInsert.hParent = hItem;
    tvInsert.hInsertAfter = NULL;
    tvInsert.item.mask = TVIF_TEXT;
    
    //if (hItem == m_ptrTreeCtrlDirectory->GetRootItem ())
    if (hItem == m_hRoot) {
        TCHAR g_szDrvMsg[] = _T("A:");
        ULONG uDriveMask = _getdrives();
        while (uDriveMask) {
            tvInsert.hParent = hItem;
            
            if (uDriveMask & 1) {
                //strItem = CString (_T("[-")) + CString (g_szDrvMsg) + CString (_T("-]"));
                strItem = CString (CString (g_szDrvMsg));
			    tvInsert.item.pszText = strItem.GetBuffer ();
			    m_ptrTreeCtrlDirectory->InsertItem (&tvInsert);
			    //SetChildItems (m_ptrTreeCtrlDirectory->InsertItem (&tvInsert), g_szDrvMsg);
		    }
			else
				;

            ++g_szDrvMsg[0];
            uDriveMask >>= 1;
        }
    }
    else {
        // build a string with wildcards
        CString strWildcard = CString(path) + CString (_T("\\"));
        strWildcard += _T("*.*");

        // start working for files
        finder.Close ();
        BOOL bWorking = finder.FindFile(strWildcard);

        while (bWorking) {
            bWorking = finder.FindNextFile();

            // skip . and .. files; otherwise, we'd
            // recur infinitely!

            if (finder.IsDots())
                 continue;
			else
				;

            // if it's a directory, recursively search it
            if (finder.IsDirectory() && ! finder.IsCompressed () && ! finder.IsHidden ()) {
                strItem = CString (finder.GetFileName ());
                tvInsert.item.pszText = strItem.GetBuffer ();
                 
                m_ptrTreeCtrlDirectory->InsertItem (&tvInsert);
                //SetChildItems (m_ptrTreeCtrlDirectory->InsertItem (&tvInsert), finder.GetfilePath ());
            }
			else
				;
        }

        finder.Close();
    }

    return 0;
}

//void HDlgChoiceDirectory::OnTvnSingleExpandTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

//void HDlgChoiceDirectory::OnTvnItemexpandedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
//{
//    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//    // TODO: Add your control notification handler code here
//    *pResult = 0;
//}

void HDlgChoiceDirectory::OnTvnItemexpandingTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    
    // TODO: Add your control notification handler code here
    UINT stateItem = TVIS_EXPANDED;
    stateItem |= TVIS_SELECTED;
    
    stateItem = TVIS_EXPANDED;
    stateItem &= TVIS_SELECTED;
    
    HTREEITEM hChildItem = m_ptrTreeCtrlDirectory->GetChildItem (pNMTreeView->itemNew.hItem),
            hGrandChild, hDelItem;
    
    while (hChildItem != NULL)
    {
        if (pNMTreeView->itemNew.state != 96)
        {
            SetChildItems (hChildItem, WC_HSTRING (GetFullPath (hChildItem)));
        }
        else
        {
            hGrandChild = m_ptrTreeCtrlDirectory->GetChildItem (hChildItem);
            
            while (hGrandChild != NULL)
            {
                hDelItem = hGrandChild;
                hGrandChild = m_ptrTreeCtrlDirectory->GetNextItem(hGrandChild, TVGN_NEXT);
                m_ptrTreeCtrlDirectory->DeleteItem (hDelItem);
            }
        }
        
        hChildItem = m_ptrTreeCtrlDirectory->GetNextItem(hChildItem, TVGN_NEXT);
    }

    *pResult = 0;
}

HString HDlgChoiceDirectory::GetFullPath (HTREEITEM hItem) {
    HString path;
    path.free ();
    
    HTREEITEM hParentItem = m_ptrTreeCtrlDirectory->GetParentItem (hItem);
    path = m_ptrTreeCtrlDirectory->GetItemText (hItem);
            
    //while (hParentItem != NULL && hParentItem != m_ptrTreeCtrlDirectory->GetRootItem ())
    while (hParentItem != NULL && hParentItem != m_hRoot) {
        path.insert (0, _T ("\\"));
        path.insert (0, m_ptrTreeCtrlDirectory->GetItemText (hParentItem).GetBuffer ());
        
        hParentItem = m_ptrTreeCtrlDirectory->GetParentItem (hParentItem);
    }

    //return path + _T("\\");
    return path;
}

void HDlgChoiceDirectory::ExpandCurrent(HTREEITEM hItem, HString & path) {
    HTREEITEM hItemUpLevel = hItem, hChildItem;
    
    m_ptrTreeCtrlDirectory->Expand (hItemUpLevel, TVE_EXPAND);
    hChildItem = m_ptrTreeCtrlDirectory->GetChildItem (hItemUpLevel);
    
	int curPos = 0;
    HString resToken = path.tokenize (_T ("\\"), curPos), strTemp;
    if (resToken.isUses () == 1) {
       while (hChildItem != NULL)
       {
            //SetChildItems (hChildItem, GetFullPath (hChildItem));
            if (resToken.compareNoCase (m_ptrTreeCtrlDirectory->GetItemText (hChildItem).GetBuffer ()) == 0) {
                ExpandCurrent (hChildItem, m_strPath.right (m_strPath.length () - (GetFullPath (hChildItem)).length () - 1));
            }
			else
				;

            hChildItem = m_ptrTreeCtrlDirectory->GetNextItem (hChildItem, TVGN_NEXT);
       }
       
       resToken = m_strPath.tokenize(_T("\\"), curPos);
    }
    else
        m_ptrTreeCtrlDirectory->SelectItem (hItemUpLevel);
}

void HDlgChoiceDirectory::OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}
