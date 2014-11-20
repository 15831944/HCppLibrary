#pragma once
//#include "resource.h"

#include <HDlg.h>

#if ! defined IDD_DIALOG_DIRECTORY
	#define IDD_DIALOG_DIRECTORY WM_USER + 6667
#else
#endif
#if ! defined IDC_TREECTRL_DIRECTORY
	#define IDC_TREECTRL_DIRECTORY WM_USER + 6668
#else
#endif

// HDlgChoiceDirectory dialog

//http://www.realcoding.net/articles/glava-14-modalnye-i-nemodalnye-bloki-dialoga.html

class HDlgChoiceDirectory : public HDlg {
	DECLARE_DYNAMIC (HDlgChoiceDirectory)

public:
	HDlgChoiceDirectory (CWnd* = 0x0);   // standard constructor
	virtual ~HDlgChoiceDirectory ();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIRECTORY };

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	HTREEITEM m_hRoot, m_hClear;
	TVINSERTSTRUCT m_tvInsert;

	DECLARE_MESSAGE_MAP()
private:

public:
    CTreeCtrl *m_ptrTreeCtrlDirectory;
	HString m_strPath;
    
    afx_msg void OnBnClickedOk();
    int SetChildItems(HTREEITEM hItem, LPCTSTR path);
//    afx_msg void OnTvnSingleExpandTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
//    afx_msg void OnTvnItemexpandedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnItemexpandingTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
private:
    HString GetFullPath (HTREEITEM hItem);
public:
    void ExpandCurrent (HTREEITEM hItem, HString &path);
    afx_msg void OnTvnSelchangedTreeDirectory(NMHDR *pNMHDR, LRESULT *pResult);
};
