#include "stdafx.h"
#include "HWinThread.h"

//#include "UserKTS.h"
//
//#if defined PTR_DB
//extern CUserKTSApp
//#else
//extern CWinApp
//#endif
//theApp;

IMPLEMENT_DYNCREATE(HWinThread, CWinThread)

HWinThread::HWinThread (void) : CWinThread (),
                                m_dwThreadIdResponce (0x0) {
    m_idExitCodeThread = WAIT_IO_COMPLETION;

	m_tmSpanMaxDiffLastResponce = 0;
	setLastResponce ();

    m_ptrmapInData = new CMapWordToPtr ();

    m_bAutoDelete = false;
}

HWinThread::~HWinThread (void) {
    BYTE *pData = NULL;
    POSITION pos;
    WORD key;
    
    if (pos = m_ptrmapInData->GetStartPosition ()) {
        while (pos) {
            m_ptrmapInData->GetNextAssoc(pos, key, (void *&) pData);
            //if (pData && pos)
            if (pData)
                delete pData;
            else
                ;
        }
        
        m_ptrmapInData->RemoveAll ();
    }
    else
        ;

    delete m_ptrmapInData;
    m_ptrmapInData = 0x0;

	/*FILELOG_VARIABLE
    FILELOG_IF_WRITE (HDEBUG)
	strLog.format (_T ("%S '%S', ID = %i"), _T ("ДЕструктор потока"), WC_HSTRING (HString (GetRuntimeClass ()->m_lpszClassName)), m_nThreadID);
	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    FILELOG_ENDIF*/
}

BOOL HWinThread::InitInstance () {
	// TODO:  perform and per-thread initialization here	
	BOOL bRes = CWinThread::InitInstance();

    //return bRes;
    return TRUE;
}

int HWinThread::ExitInstance () {
	// TODO:  perform any per-thread cleanup here	
	BOOL bRes =  CWinThread::ExitInstance();

    return bRes;
}

BEGIN_MESSAGE_MAP(HWinThread, CWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_LIVE, &OnLive)
    ON_THREAD_MESSAGE (WM_USER_QUERY_THREAD_ID, &OnQueryThreadID)
    ON_THREAD_MESSAGE (WM_USER_INIT_ID_THREAD, &OnInitThreadIdResponce)
    ON_THREAD_MESSAGE (WM_USER_RESET_THREAD_RESPONCE, &OnResetThreadResponce)
END_MESSAGE_MAP()

// HThreadDirWatched message handlers
//int HWinThread::Run()
//{
//    return m_idExitCodeThread;
//}

afx_msg void HWinThread::OnQueryThreadID (WPARAM wp, LPARAM) {
    ::PostThreadMessage (wp, WM_USER_INIT_ID_THREAD, m_nThreadID, 0);
}

afx_msg void HWinThread::OnInitThreadIdResponce (WPARAM wp, LPARAM ) {
    m_dwThreadIdResponce = (DWORD) wp;
}

int HWinThread::DelData (int id) {
    BYTE *pPreviousData = 0x0;

    if (m_ptrmapInData->Lookup (id, (void *&) pPreviousData) == TRUE) {
        delete pPreviousData;
        m_ptrmapInData->RemoveKey (id);
    }
    else
        ;

    return m_ptrmapInData->GetSize ();
}

int HWinThread::AddData (int id, void *data, long len) {
    BYTE *pInData = DEBUG_NEW BYTE [len + sizeof (long)];
    
    memset (pInData, 0x0, len + sizeof (long));

    *(long *) pInData = len;
    memcpy (pInData + sizeof (long), data, len);

    DelData (id);

    m_ptrmapInData->SetAt (id, pInData);

    return m_ptrmapInData->GetSize ();
}

int HWinThread::AddData (int id, HString &str) {
    return AddData (id, (BYTE *) str.GetBuffer (), str.length () * sizeof (TCHAR) + sizeof (TCHAR));
}

BYTE *HWinThread::GetDataOfId (int id) {
    BYTE *pData = 0x0;
    try {
        m_ptrmapInData->Lookup (id, (void *&)pData);

        if (! (pData == 0x0))
            pData += sizeof (long);
        else
            ;
    }
    catch (CException *err) {
        err->Delete ();
    }

    return pData;
}

long HWinThread::GetLengthDataOfId (int id) {
    BYTE *pData = NULL;
    try {
        m_ptrmapInData->Lookup (id, (void *&)pData);
    }
    catch (CException *err) {
        err->Delete ();
    }

    return *((long *) pData);
}

HString HWinThread::stringData (BYTE *data) {
    int iPos = -1, iPosParametr = -1;
    HString strRes, strTemp;
	while (* (TCHAR *) data)
	{
	    strRes.appendChar (* (TCHAR *) data);
	    data += sizeof (TCHAR);
	}

	return strRes;
}

BOOL HWinThread::IsLive (void) {
	BOOL bRes = FALSE;
	if (! (m_tmSpanMaxDiffLastResponce == 0))
		if ((getDiffLastResponce () < m_tmSpanMaxDiffLastResponce) && (true))
			bRes = TRUE;
		else ;
	else
		bRes = TRUE;

	return bRes;
}

BOOL HWinThread::QueryThreadIdResponce (int indx) {
    if ((! (AfxGetApp ()->m_nThreadID == m_nThreadID)) && (! (indx < 0)))
        return ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_QUERY_THREAD_ID, m_nThreadID, indx);
    else
        return FALSE;
}

afx_msg void HWinThread::OnResetThreadResponce (WPARAM , LPARAM ) {
    m_dwThreadIdResponce = 0x0;
}