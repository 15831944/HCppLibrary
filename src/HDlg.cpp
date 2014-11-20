// HDlg.cpp: файл реализации
//

#include "stdafx.h"
#include <AfxPriv.h>
#include "HDlg.h"

#if defined _DEBUG
//#include "resource.h"
#else
#endif

extern BOOL CALLBACK fWndEnumProcDeleteChildWindows (HWND , LPARAM );

// диалоговое окно HDlg

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
    // Освобождение использованной памяти 
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
    // При работе с шаблонами блоков диалога в памяти все текстовые строки должны быть представлены в Unicode
	WCHAR szDlgTitle [] = L"Пустой диалог...: выход — <ESC>";
	WCHAR szDlgFontName [] = L"MS Shell Dlg";

	m_dlgMemory.style = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_DLGFRAME | WS_BORDER;
	m_dlgMemory.dwExtendedStyle = 0;
	m_dlgMemory.cdit = 0;
	m_dlgMemory.x = rcSize.left; m_dlgMemory.y = rcSize.top; m_dlgMemory.cx = rcSize.right; m_dlgMemory.cy = rcSize.bottom;

	if (DLGITEM_COUNT) {
		m_ptrDlgItem = DEBUG_NEW HDLGITEMTEMPLATE [DLGITEM_COUNT];

		if (DLGITEM_COUNT) {
			// Переписываем информацию о каждом элементе шаблона
			for (int i = 0; i < DLGITEM_COUNT; i++) {
			}
		}
		else
			;
	}
	else
		;

	// Прежде всего определяем размер памяти, необходимой для размещения шаблона блока диалога, и дополнительной памяти под четыре массива:
	// 1) для меню;
	// 2) класса;
	// 3) заголовка шрифта
	int nBufferSize = sizeof (DLGTEMPLATE) +				// структура
					(2 * sizeof (WORD)) +					// меню + класс
					sizeof (szDlgTitle) +					// заголовок
					sizeof (WORD) + sizeof (szDlgFontName);	// шрифт

	// Выравнивание по DWORD
	nBufferSize = (nBufferSize + 3) & ~3;

	if (DLGITEM_COUNT) {
		// Добавляем размеры структуры DLGITEMTEMPLATE и заголовков элементов
		for (int i = 0; i < DLGITEM_COUNT; i++) {
			int nItemLength = (sizeof (DLGITEMTEMPLATE) +   // структура
								3 * sizeof (WORD) +			// 3 массива для класса, заголовка и данных элемента управления
								(m_ptrDlgItem [i].m_strTitle.length () + 1) * sizeof (WCHAR)); // размер строки Unicode для заголовка

			// Выравнивание по DWORD
			if (i != 1)
				nItemLength = (nItemLength + 3) & ~3;
			else
				;

			nBufferSize += nItemLength;
		}
	}
	else
		; // Нет элементов управления для размещения в памяти

	// Выделяем память под шаблон
	m_hLocalMemory = LocalAlloc (LHND, nBufferSize);

	if (m_hLocalMemory == NULL) {
		// Обработка ошибки распределения памяти
	}
	else
		;

	BYTE *pBuffer = (BYTE *) LocalLock (m_hLocalMemory);

	if (pBuffer == NULL) {
		LocalFree (m_hLocalMemory);

		// Обработка ошибки блокировки памяти
	}
	else
		;

	BYTE *pDest = pBuffer;

	memcpy (pDest, &m_dlgMemory, sizeof (DLGTEMPLATE)); // Переписываем структуру DLGTEMPLATE в память

	pDest += sizeof (DLGTEMPLATE);

	*(WORD *) pDest = 0; // меню, нет
	pDest += sizeof (WORD);
	*(WORD *) pDest = 0; // используем класс по умолчанию
	//pDest += 2 * sizeof (WORD);
	pDest += sizeof (WORD);

	memcpy (pDest, szDlgTitle, sizeof (szDlgTitle)); // Переписываем заголовок
	pDest += sizeof (szDlgTitle);

	*(WORD *) pDest = DLG_FONT_SIZE; // размер шрифта
	pDest += sizeof (WORD);

	memcpy (pDest, szDlgFontName, sizeof (szDlgFontName)); // Переписываем имя шрифта
	pDest += sizeof (szDlgFontName);

	if (DLGITEM_COUNT) {
		// Переписываем информацию о каждом элементе шаблона
		for (int i = 0; i < DLGITEM_COUNT; i++) {
			// Выравнивание буфера по DWORD
			pDest = (BYTE *) (((DWORD) pDest + 3) & ~3);

			memcpy (pDest, (void *) &m_ptrDlgItem [i].m_ctrItem,  sizeof (DLGITEMTEMPLATE)); 

			pDest += sizeof (DLGITEMTEMPLATE); 

			*(WORD *) pDest = 0xFFFF; // есть идентификатор ресурса  

			pDest += sizeof (WORD);

			*(WORD *) pDest = m_ptrDlgItem [i].m_ctrtype;
			
			pDest += sizeof (WORD);

			m_ptrDlgItem [i].m_strTitle.to (HString::WCSTR);
			// Преобразуем заголовок в Unicode строку 
			int nChar = m_ptrDlgItem [i].m_strTitle.length () + 1;
			//WCHAR *pchCaption = 0x0;
			//pchCaption = new WCHAR [nChar]; 

			//// Преобразуем строку ANSI в строку Unicode 
			//nChar = MultiByteToWideChar (CP_ACP,					// кодовая страница ANSI 
			//							WC_COMPOSITECHECK | MB_PRECOMPOSED,
			//							MB_HSTRING (m_ptrDlgItem [i].m_strTitle),	// предварительно сформированная строка, адрес строки
			//							-1,							// автоматическое определение длины
			//							pchCaption,					// адрес буфера строки Unicode
			//							nChar);						// размер буфера строки Unicode
			
			ASSERT (nChar > 0);

			//memcpy (pDest, pchCaption, nChar * sizeof (WCHAR));  // Переписываем заголовок элемента управления
			memcpy (pDest, WC_HSTRING (m_ptrDlgItem [i].m_strTitle), nChar * sizeof (WCHAR));  // Переписываем заголовок элемента управления

			pDest += nChar * sizeof (WCHAR);
			//delete pchCaption; 

			*(WORD *) pDest = 0;

			pDest += sizeof (WORD);  
		}
	}
	else
		; // Нет элементов управления для размещения в памяти

	*(WORD *) pDest = 0; // ???
	pDest += sizeof (WORD); 
	
	ASSERT ((pDest - pBuffer) == nBufferSize); // Проверка на полноту заполнения буфера  

	// После заполнения буфера информация в нем хранится в следующем виде:  
		// Структура DLGTEMPLATE .  
		// 0x0000 (WORD) — индикатор наличия меню  
		// 0x0000 (WORD) — класс блока диалога по умолчанию  
		// Заголовок — строка Unicode  
		// 0x0000 — размер используемого шрифта  
		//"Courier New Cyr" — имя используемого шрифта  

	// Структура DLGITEMTEMPLATE для статического текста 
		// 0xFFFF — есть идентификатор элемента управления  
		// 0x0081 — идентификатор класса Static  
		// Заголовок — строка Unicode ,  
		// 0x0000 — дополнительных данных нет 
	// Структура 'DLGITEMTEMPLATE для редактируемого текста  
		// OxFFFF — есть идентификатор элемента управления  
		// 0x0082 — идентификатор класса Edit  
		// Заголовок — строка Unicode  
		// 0x0000 — дополнительных данных нет 

	// Создание объекта "блок диалога"
	//CDialog dig;
    
    if (style)
        m_iStyle |= style;
    else
        ;

    int iRes = -1; //Результат создания
	// Создание окна Windows блока диалога 
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
//	/*TRY { iRes = CreateEx (WS_EX_DLGMODALFRAME | WS_EX_OVERLAPPEDWINDOW, _T ("#32770"), _T ("Выбор каталога"), WS_VISIBLE, rcDlgChoiceDirectory, pWndParent, IDD); }
//	CATCH_ALL (err) {
//		iRes = GetLastError ();
//		THROW_LAST ();
//	}
//	END_CATCH_ALL*/
//
//	//iRes = CreateEx (WS_EX_DLGMODALFRAME | WS_EX_OVERLAPPEDWINDOW, _T ("#32770"), _T ("Выбор каталога"), WS_VISIBLE, rcDlgChoiceDirectory, pWndParent, IDD);
//	//iRes = CDialog::Create (IDD, pWndParent);
//	m_hWnd = CreateWindow (_T ("#32770"),
//						_T ("Выбор каталога"),
//						style, //WS_VISIBLE | WS_BORDER | WS_OVERLAPPED | WS_CHILD | DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | WS_CAPTION | WS_SYSMENU,
//						rcDlgChoiceDirectory.left, rcDlgChoiceDirectory.top, rcDlgChoiceDirectory.Width (), rcDlgChoiceDirectory.Height (),
//						pWndParent->m_hWnd,
//						0x0, //(HMENU) IDD, //Не МЕНю
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

// обработчики сообщений HDlg
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