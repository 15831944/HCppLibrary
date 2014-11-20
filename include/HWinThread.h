#pragma once
#include "afxwin.h"
//#include <hstring.h>

class HWinThread : public CWinThread {
	DECLARE_DYNCREATE(HWinThread)

protected:
    // protected constructor used by dynamic creation
    DWORD m_dwThreadIdResponce; //ThreadControl, ThreadGUIControl
    BOOL QueryThreadIdResponce (int ); //Аргумент индекс требуемого потока

    virtual PTR_HANDLER_ONTHREADMESSAGE OnResetThreadResponce;

public:
    HWinThread ();
    virtual ~HWinThread ();

    enum ID_DATAITEM {HTREEITEM_EMPTY = 666};

	virtual BOOL InitInstance();
	virtual int ExitInstance();

    //virtual int Run (void) /*PURE*/;
	virtual void Stop (void) /*PURE;*/ {}
	virtual afx_msg void OnResumeRun (WPARAM , LPARAM ) /*PURE;*/ {}
    afx_msg void OnLive (WPARAM , LPARAM ) { setLastResponce (); }
    PTR_HANDLER_ONTHREADMESSAGE OnQueryThreadID;
    PTR_HANDLER_ONTHREADMESSAGE OnInitThreadIdResponce;

    inline DWORD GetThreadIDResponce (void) { return m_dwThreadIdResponce; }

	inline void setLastResponce (void) {
		CTime::GetCurrentTime ().GetAsSystemTime (m_stmLastResponce);
	}
	inline void setMaxDiffLastResponce (LONGLONG diff) {
		m_tmSpanMaxDiffLastResponce = CTimeSpan (diff);
	}
	CTimeSpan getDiffLastResponce (void) {
		return CTime::GetCurrentTime () - CTime (m_stmLastResponce);
	}
	BOOL IsLive (void);
    
    //HANDLE hThread;
    //DWORD idThread;
	DWORD m_idExitCodeThread;
	
    int DelData (int);
	int AddData (int , void *, long );
	int AddData (int, HString &);
	
    HString stringDataOfId (int id) { return stringData (GetDataOfId (id)); }
	static HString stringData (BYTE *);
	//BYTE *GetDataOfNumber (int );
	long GetLengthDataOfId (int );
	BYTE *GetDataOfId (int );
	
private:
    //void *m_pInData;
    CMapWordToPtr *m_ptrmapInData;
	SYSTEMTIME m_stmLastResponce;
	CTimeSpan m_tmSpanMaxDiffLastResponce;

protected:
	DECLARE_MESSAGE_MAP()
};
