// HDlg.cpp: ���� ����������
//

#include "stdafx.h"
#include <AfxPriv.h>
#include "HDlg.h"

#if defined _DEBUG
//#include "resource.h"
#else
#endif

extern BOOL CALLBACK fWndEnumProcDeleteChildWindows (HWND , LPARAM );

// ���������� ���� HDlg

IMPLEMENT_DYNAMIC(HDlg, CDialogEx)

HDlg::HDlg (CWnd* pParent /*=NULL*/) :   /*CDialogEx (HDlg::IDD, pParent)*/CDialogEx (),
													m_ptrDataExchange (0x0),
                                                    m_iStyle (0x0),
                                                    m_hLocalMemory (0x0),
                                                    m_ptrDlgItem (0x0) {
    if (pParent) {
        m_hIcon = pParent->GetIcon (false);
        m_iStyle |= HDLG_CHILD;
    }
    else
/*#if defined _DEBUG
        m_hIcon = LoadIcon (AfxGetApp ()->m_hInstance, MAKEINTRESOURCE (IDI_ICON_H_48))
#else
#endif*/
        ;
}

HDlg::~HDlg () {
    // ������������ �������������� ������ 
	LocalUnlock (m_hLocalMemory);
	LocalFree (m_hLocalMemory);
}

void HDlg::DoDataExchange (CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	m_ptrDataExchange = pDX;
}

BEGIN_MESSAGE_MAP(HDlg, CDialogEx)
END_MESSAGE_MAP()

int HDlg::PreCreate (CRect &rcSize, int style) {
    // ��� ������ � ��������� ������ ������� � ������ ��� ��������� ������ ������ ���� ������������ � Unicode
	WCHAR szDlgTitle [] = L"������ ������...: ����� � <ESC>";
	WCHAR szDlgFontName [] = L"MS Shell Dlg";

	m_dlgMemory.style = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_DLGFRAME | WS_BORDER;
	m_dlgMemory.dwExtendedStyle = 0;
	m_dlgMemory.cdit = 0;
	m_dlgMemory.x = rcSize.left; m_dlgMemory.y = rcSize.top; m_dlgMemory.cx = rcSize.right; m_dlgMemory.cy = rcSize.bottom;

	if (DLGITEM_COUNT) {
		m_ptrDlgItem = DEBUG_NEW HDLGITEMTEMPLATE [DLGITEM_COUNT];

		if (DLGITEM_COUNT) {
			// ������������ ���������� � ������ �������� �������
			for (int i = 0; i < DLGITEM_COUNT; i++) {
			}
		}
		else
			;
	}
	else
		;

	// ������ ����� ���������� ������ ������, ����������� ��� ���������� ������� ����� �������, � �������������� ������ ��� ������ �������:
	// 1) ��� ����;
	// 2) ������;
	// 3) ��������� ������
	int nBufferSize = sizeof (DLGTEMPLATE) +				// ���������
					(2 * sizeof (WORD)) +					// ���� + �����
					sizeof (szDlgTitle) +					// ���������
					sizeof (WORD) + sizeof (szDlgFontName);	// �����

	// ������������ �� DWORD
	nBufferSize = (nBufferSize + 3) & ~3;

	if (DLGITEM_COUNT) {
		// ��������� ������� ��������� DLGITEMTEMPLATE � ���������� ���������
		for (int i = 0; i < DLGITEM_COUNT; i++) {
			int nItemLength = (sizeof (DLGITEMTEMPLATE) +   // ���������
								3 * sizeof (WORD) +			// 3 ������� ��� ������, ��������� � ������ �������� ����������
								(m_ptrDlgItem [i].m_strTitle.length () + 1) * sizeof (WCHAR)); // ������ ������ Unicode ��� ���������

			// ������������ �� DWORD
			if (i != 1)
				nItemLength = (nItemLength + 3) & ~3;
			else
				;

			nBufferSize += nItemLength;
		}
	}
	else
		; // ��� ��������� ���������� ��� ���������� � ������

	// �������� ������ ��� ������
	m_hLocalMemory = LocalAlloc (LHND, nBufferSize);

	if (m_hLocalMemory == NULL) {
		// ��������� ������ ������������� ������
	}
	else
		;

	BYTE *pBuffer = (BYTE *) LocalLock (m_hLocalMemory);

	if (pBuffer == NULL) {
		LocalFree (m_hLocalMemory);

		// ��������� ������ ���������� ������
	}
	else
		;

	BYTE *pDest = pBuffer;

	memcpy (pDest, &m_dlgMemory, sizeof (DLGTEMPLATE)); // ������������ ��������� DLGTEMPLATE � ������

	pDest += sizeof (DLGTEMPLATE);

	*(WORD *) pDest = 0; // ����, ���
	pDest += sizeof (WORD);
	*(WORD *) pDest = 0; // ���������� ����� �� ���������
	//pDest += 2 * sizeof (WORD);
	pDest += sizeof (WORD);

	memcpy (pDest, szDlgTitle, sizeof (szDlgTitle)); // ������������ ���������
	pDest += sizeof (szDlgTitle);

	*(WORD *) pDest = DLG_FONT_SIZE; // ������ ������
	pDest += sizeof (WORD);

	memcpy (pDest, szDlgFontName, sizeof (szDlgFontName)); // ������������ ��� ������
	pDest += sizeof (szDlgFontName);

	if (DLGITEM_COUNT) {
		// ������������ ���������� � ������ �������� �������
		for (int i = 0; i < DLGITEM_COUNT; i++) {
			// ������������ ������ �� DWORD
			pDest = (BYTE *) (((DWORD) pDest + 3) & ~3);

			memcpy (pDest, (void *) &m_ptrDlgItem [i].m_ctrItem,  sizeof (DLGITEMTEMPLATE)); 

			pDest += sizeof (DLGITEMTEMPLATE); 

			*(WORD *) pDest = 0xFFFF; // ���� ������������� �������  

			pDest += sizeof (WORD);

			*(WORD *) pDest = m_ptrDlgItem [i].m_ctrtype;
			
			pDest += sizeof (WORD);

			m_ptrDlgItem [i].m_strTitle.to (HString::WCSTR);
			// ����������� ��������� � Unicode ������ 
			int nChar = m_ptrDlgItem [i].m_strTitle.length () + 1;
			//WCHAR *pchCaption = 0x0;
			//pchCaption = new WCHAR [nChar]; 

			//// ����������� ������ ANSI � ������ Unicode 
			//nChar = MultiByteToWideChar (CP_ACP,					// ������� �������� ANSI 
			//							WC_COMPOSITECHECK | MB_PRECOMPOSED,
			//							MB_HSTRING (m_ptrDlgItem [i].m_strTitle),	// �������������� �������������� ������, ����� ������
			//							-1,							// �������������� ����������� �����
			//							pchCaption,					// ����� ������ ������ Unicode
			//							nChar);						// ������ ������ ������ Unicode
			
			ASSERT (nChar > 0);

			//memcpy (pDest, pchCaption, nChar * sizeof (WCHAR));  // ������������ ��������� �������� ����������
			memcpy (pDest, WC_HSTRING (m_ptrDlgItem [i].m_strTitle), nChar * sizeof (WCHAR));  // ������������ ��������� �������� ����������

			pDest += nChar * sizeof (WCHAR);
			//delete pchCaption; 

			*(WORD *) pDest = 0;

			pDest += sizeof (WORD);  
		}
	}
	else
		; // ��� ��������� ���������� ��� ���������� � ������

	*(WORD *) pDest = 0; // ???
	pDest += sizeof (WORD); 
	
	ASSERT ((pDest - pBuffer) == nBufferSize); // �������� �� ������� ���������� ������  

	// ����� ���������� ������ ���������� � ��� �������� � ��������� ����:  
		// ��������� DLGTEMPLATE .  
		// 0x0000 (WORD) � ��������� ������� ����  
		// 0x0000 (WORD) � ����� ����� ������� �� ���������  
		// ��������� � ������ Unicode  
		// 0x0000 � ������ ������������� ������  
		//"Courier New Cyr" � ��� ������������� ������  

	// ��������� DLGITEMTEMPLATE ��� ������������ ������ 
		// 0xFFFF � ���� ������������� �������� ����������  
		// 0x0081 � ������������� ������ Static  
		// ��������� � ������ Unicode ,  
		// 0x0000 � �������������� ������ ��� 
	// ��������� 'DLGITEMTEMPLATE ��� �������������� ������  
		// OxFFFF � ���� ������������� �������� ����������  
		// 0x0082 � ������������� ������ Edit  
		// ��������� � ������ Unicode  
		// 0x0000 � �������������� ������ ��� 

	// �������� ������� "���� �������"
	//CDialog dig;
    
    if (style)
        m_iStyle |= style;
    else
        ;

    int iRes = -1; //��������� ��������
	// �������� ���� Windows ����� ������� 
	if (m_iStyle && HDLG_MODALES)
        iRes = InitModalIndirect ((DLGTEMPLATE *) pBuffer);
    else
        iRes = CreateIndirect ((DLGTEMPLATE *) pBuffer);

    return iRes;
}

//int HDlgChoiceDirectory::Create (CWnd* pWndParent) {
//	int iRes = -1;
//	UINT style = 0;
//	//style = WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU;
//	style = WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_POPUP | WS_CAPTION | WS_SYSMENU;
//	
//	CRect rcDlgClient, rcDlgChoiceDirectory;
//	if (pWndParent) {
//		//pWndParent->GetClientRect(&rcDlgClient);
//		//rcDlgChoiceDirectory.left = 10; rcDlgChoiceDirectory.top = 10; rcDlgChoiceDirectory.right = 0.35 * rcDlgChoiceDirectory.right; rcDlgChoiceDirectory.bottom = rcDlgClient.bottom - 25;
//	}
//	else
//		;
//
//	rcDlgChoiceDirectory.left = 0; rcDlgChoiceDirectory.top = 0; rcDlgChoiceDirectory.right = 183; rcDlgChoiceDirectory.bottom = 186;
//	
//	/*TRY { iRes = CreateEx (WS_EX_DLGMODALFRAME | WS_EX_OVERLAPPEDWINDOW, _T ("#32770"), _T ("����� ��������"), WS_VISIBLE, rcDlgChoiceDirectory, pWndParent, IDD); }
//	CATCH_ALL (err) {
//		iRes = GetLastError ();
//		THROW_LAST ();
//	}
//	END_CATCH_ALL*/
//
//	//iRes = CreateEx (WS_EX_DLGMODALFRAME | WS_EX_OVERLAPPEDWINDOW, _T ("#32770"), _T ("����� ��������"), WS_VISIBLE, rcDlgChoiceDirectory, pWndParent, IDD);
//	//iRes = CDialog::Create (IDD, pWndParent);
//	m_hWnd = CreateWindow (_T ("#32770"),
//						_T ("����� ��������"),
//						style, //WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_CAPTION | WS_SYSMENU,
//						rcDlgChoiceDirectory.left, rcDlgChoiceDirectory.top, rcDlgChoiceDirectory.Width (), rcDlgChoiceDirectory.Height (),
//						pWndParent->m_hWnd,
//						0x0, //(HMENU) IDD, //�� ����
//						(HINSTANCE) (LONG_PTR) GetWindowLong (pWndParent->m_hWnd, GWL_HINSTANCE),
//						NULL);
//
//	//if (iRes) {
//	if (m_hWnd) {
//		CFont *ptrFont = DEBUG_NEW CFont ();
//		ptrFont->CreatePointFont (8, _T ("MS Shell Dlg"));
//		CDialog::SetFont (/*GetStockObject (DEFAULT_GUI_FONT)*/ptrFont);
//
//        return true;
//	}
//	else {
//		iRes = GetLastError ();
//
//	    return false;
//	}
//}

// ����������� ��������� HDlg
BOOL HDlg::OnInitDialog (void) {
    CDialogEx::OnInitDialog ();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

INT_PTR HDlg::DoModal (void) {
    // can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		m_lpDialogTemplate != NULL);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
    
    if (m_iStyle && HDLG_CHILD)
        ;
    else
        hWndParent = 0x0;

	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
#ifndef _AFX_NO_OLE_SUPPORT
	CWnd* pMainWnd = NULL;
	BOOL bEnableMainWnd = FALSE;
#endif
	if (hWndParent && hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
#ifndef _AFX_NO_OLE_SUPPORT
		pMainWnd = AfxGetMainWnd();
		if (pMainWnd && pMainWnd->IsFrameWnd() && pMainWnd->IsWindowEnabled())
		{
			//
			// We are hosted by non-MFC container
			// 
			pMainWnd->EnableWindow(FALSE);
			bEnableMainWnd = TRUE;
		}
#endif
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
						CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		THROW_LAST ();
        //THROW (e)
		m_nModalResult = -1;
	}
	END_CATCH_ALL

#ifndef _AFX_NO_OLE_SUPPORT
	if (bEnableMainWnd)
		pMainWnd->EnableWindow(TRUE);
#endif
	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}

void HDlg::OnDestroy (void) {
	//EnumWindows (fWndEnumProcDeleteChildWindows, (LPARAM) GetSafeHwnd ());

	CDialog::OnDestroy ();
}