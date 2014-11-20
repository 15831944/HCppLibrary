#pragma once
#include "afxdialogex.h"

#define HDLG_MODALES 0x1
#define HDLG_CHILD 0x2

class HString;

#if ! defined IDD_HDLG
	#define IDD_HDLG WM_USER + 7667
#else
#endif

// диалоговое окно HDlg
#define DLG_FONT_SIZE 8
#define DLG_MARGIN_ITEM 8
#define DLG_HEIGHT_ITEM 3 * DLG_MARGIN_ITEM
#define DLGITEM_COUNT 0

struct HDLGITEMTEMPLATE {
	HString m_strTitle;
	WORD m_ctrtype;
	DLGITEMTEMPLATE m_ctrItem;
};

class HDlg : public CDialogEx {
	DECLARE_DYNAMIC(HDlg)

public:
	HDlg (CWnd* = 0x0);   // стандартный конструктор
	virtual ~HDlg ();

// Данные диалогового окна
	enum { IDD = IDD_HDLG };

    int PreCreate (CRect& = CRect (0, 0, 160, 180), int = 0);
    int IsModal (void) { if (m_iStyle && HDLG_MODALES) return 1; else return 0; }

protected:
    virtual BOOL OnInitDialog ();
	virtual void DoDataExchange (CDataExchange* pDX);    // DDX/DDV support

	void OnDestroy (void);

	HICON m_hIcon;
	CDataExchange *m_ptrDataExchange;
    int m_iStyle;

	DECLARE_MESSAGE_MAP()
private:
	HLOCAL m_hLocalMemory;
	DLGTEMPLATE m_dlgMemory;
	HDLGITEMTEMPLATE *m_ptrDlgItem;

public:
	INT_PTR DoModal (void);
};
