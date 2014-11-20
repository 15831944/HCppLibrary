#include "stdafx.h"
#include "HThreadFileLog.h"

IMPLEMENT_DYNCREATE (HThreadFileLog, HWinThread)

HThreadFileLog::HThreadFileLog (void) : m_hEventStop (0x0), m_hEventMessageFileLog (0x0) {
    //try { m_ptrFileLog = new HFileLog (10000, ALL_MESSAGES); }
    try { m_ptrFileLog = DEBUG_NEW HFileLog (10000, TYPE_MESSAGE (HCOMMON) |
                                                TYPE_MESSAGE (HDEBUG) |
    //                                            //TYPE_MESSAGE (HDATA_COMMON) |
    //                                            //TYPE_MESSAGE (HDATA_ENERG5) |
    //                                            //TYPE_MESSAGE (HDATA_ENERG6) |
    //                                            //TYPE_MESSAGE (HDATA_TORNADO) |
    //                                            //TYPE_MESSAGE (HDATA_TORNADO1) |
    //                                            //TYPE_MESSAGE (HDATA_TORNADO6) |
    //                                            //TYPE_MESSAGE (HFIND_DB) |
    //                                            //TYPE_MESSAGE (HDATABASE) |
    //                                            //TYPE_MESSAGE (HRECORDSET) |
    //                                            //TYPE_MESSAGE (THREAD_WATCH_DIRECTORY) |
    //                                            //TYPE_MESSAGE (HANDLER_WATCH_DIRECTORY) |
    //                                            //TYPE_MESSAGE (THREAD_CREATE_TEMPLATE) |
    //                                            //TYPE_MESSAGE (THREAD_UNDO_CLEAR) |
    //                                            //TYPE_MESSAGE (OBJECT_TEMPLATE) |
    //                                            //TYPE_MESSAGE (CHECK_STATE_TEMPLATE) |
    //                                            //TYPE_MESSAGE (CONTENT_TEMPLATE) |
    //                                            //TYPE_MESSAGE (TREE_CTRL_TEMPLATE) |
    //                                            TYPE_MESSAGE (USER_KTS_DLG) |
                                                TYPE_MESSAGE (HERROR)); }
	catch (CFileException *err) {
		//MessageBox (NULL, _T("Обработка ЛОГ-файла завершена НЕ КОРРЕКТНО!"), _T("Ошибка!"), MB_ICONINFORMATION);
		delete m_ptrFileLog;
        m_ptrFileLog = NULL;

        err->Delete ();
    }
    
    /*m_idExitCodeThread = WAIT_IO_COMPLETION;*/

    if (m_ptrFileLog) {
        m_hEventStop = CreateEvent (NULL, true, false, NULL);
        m_hEventMessageFileLog = CreateEvent (NULL, true, true, NULL);
    
        if ((m_ptrFileLog->IsValidHandle () == 0) || (m_hEventStop == INVALID_HANDLE_VALUE) || (m_hEventMessageFileLog == INVALID_HANDLE_VALUE)) {
            delete m_ptrFileLog;
            m_ptrFileLog = NULL;
        
            m_hEventStop = 0x0;
            m_hEventMessageFileLog = 0x0;
        }
        else
            ;
    }
    else
        ;
}

HThreadFileLog::~HThreadFileLog (void) {
    FILELOG_VARIABLE

    ptrMsgFileLog = DEBUG_NEW MESSAGE_FILELOG;
    //ptrMsgFileLog->mes = strLog;
	ptrMsgFileLog->mes.format (_T ("%i - Дестркутор ПОТОКа лог-файла!"), idCurrentThread);
    memset (&ptrMsgFileLog->stmOccurred, 0x0, sizeof (SYSTEMTIME));
    //CTime::GetCurrentTime ().GetAsSystemTime (ptrMsgFileLog->stmOccurred);
    ptrMsgFileLog->type = HCOMMON;
    
    m_vecMessageFileLog.Add (ptrMsgFileLog);
    
    WriteMessages ();
    
    CLOSE_HANDLE (m_hEventStop);
    
    delete m_ptrFileLog;
}

BOOL HThreadFileLog::InitInstance () {
	WPARAM wParam;
    LPARAM lParam;
    wParam = MAKEWPARAM(0,0); // We can put whatever we
    lParam = MAKELPARAM(0,0); // want in wParam & lParam

    // Send the user-defined Thread Message
    // m_nThreadID is a member of CWinThread that holds the thread ID
	::PostThreadMessage (m_nThreadID, WM_USER + 2268, wParam, lParam);

	// TODO:  perform and per-thread initialization here	
	return TRUE;
}

int HThreadFileLog::ExitInstance () {
	// TODO:  perform any per-thread cleanup here	
	return HWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(HThreadFileLog, HWinThread)
    ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &HThreadFileLog::OnResumeRun)
	//ON_THREAD_MESSAGE (WM_APP_WRITEMESSAGE, OnCommandWriteFileLog)
    //ON_THREAD_MESSAGE (WM_APP_WRITEMESSAGE_WITHDT, OnCommandWriteWithDTFileLog)
    ON_THREAD_MESSAGE (WM_APP_MAXCOUNTRECORD, OnCommandMaxCountRecord)
    ON_THREAD_MESSAGE (WM_APP_LEVELOUTPUTINFO, OnCommandLevelOutputInfo)
    ON_THREAD_MESSAGE (WM_APP_FILELOGMSG, &HThreadFileLog::OnCommandWriteFileLog)
    ON_THREAD_MESSAGE (WM_APP_FILELOGMSG_WITHDT, &HThreadFileLog::OnCommandWriteWithDTFileLog)
END_MESSAGE_MAP()

//int HThreadFileLog::Run ()
//{
//    DWORD idExitCodeThread = m_idExitCodeThread;
//    
//    //Здесь обман 'link.exe', который возвращает error - ::Run () must return a value
//    //Но НЕОБХодимо завершить ПОТОК вызовом 'AfxEndThread'
//    //с этим КОДом имеем лишь: warning - not all paths return a value
//    if (! (idExitCodeThread == WAIT_IO_COMPLETION)) {
//        idExitCodeThread = -1;
//        return idExitCodeThread;
//    }
//    else
//        ;
//        
//    idExitCodeThread = STILL_ACTIVE;
//
//    while (idExitCodeThread == STILL_ACTIVE) {
//        switch (WaitForSingleObject (m_hEventStop, 0)) {
//            case WAIT_ABANDONED:
//            case WAIT_TIMEOUT:
//                break;
//            case WAIT_OBJECT_0:
//                idExitCodeThread = 0x0;
//                break;
//            default:
//                ;
//        }
//    
//        MSG msg;
//		while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
//        //while (::GetMessage (&msg, NULL, 0, 0)) { 
//            if (! (PumpMessage ())) { 
//                SetEvent (m_hEventStop);
//                //PostQuitMessage (0x0); 
//                break;
//            }
//            else
//                ; //PreTranslateMessage (&msg);
//        }
//
//        // let MFC do its idle processing
//        /*LONG lIdle = 0;
//        while (AfxGetApp ()->OnIdle (lIdle ++))
//            ;*/
//        // Perform some background processing here 
//        // using another call to OnIdle
//        //if (m_vecMessageFileLog.size ())
//        /*if (m_vecMessageFileLog.GetSize ())
//        {
//            //if (LockStreamMessageFileLog (INFINITE) == 1) {
//            if (LockStreamMessageFileLog (0) == 1) {
//                WriteMessages ();
//                
//                UnlockStreamMessageFileLog ();
//            }
//            else
//                ;
//        }
//        else
//            ;*/
//    }
//
//    idExitCodeThread = 0x0;
//	
//	//Вариант №1
//	return idExitCodeThread;
//    //Вариант №2
//	//AfxEndThread (idExitCodeThread, m_bAutoDelete);
//}

void HThreadFileLog::Stop (void)
{
    SetEvent (m_hEventStop);
}

//BOOL HThreadFileLog::PreTranslateMessage (MSG *msg)
//{
//    BOOL bRes = false;
//
//    switch (msg->message)
//    {
//        case WM_APP_FILELOGMSG:
//            bRes = true;
//            break;
//        case WM_APP_FILELOGMSG_WITHDT:
//            bRes = true;
//            break;
//        case WM_APP_MAXCOUNTRECORD:
//            bRes = true;
//            break;
//        default:
//            ;
//    }
//    
//    if (bRes)
//        return bRes;
//    else
//        return HWinThread::PreTranslateMessage (msg);
//}

afx_msg void HThreadFileLog::OnCommandWriteFileLog (WPARAM wp, LPARAM lp)
{
    int i = -1, iRes = 0; //Success
    //m_ptrFileLog->LockStream ();
    HString mes;
    mes.format ("%i - %s", ((MESSAGE_FILELOG *) wp)->idThread, MB_HSTRING (((MESSAGE_FILELOG *) wp)->mes));
    m_ptrFileLog->WriteMessage (mes, ((MESSAGE_FILELOG *) wp)->type);
    mes.free ();
    ((MESSAGE_FILELOG *) wp)->mes.free ();
    delete ((MESSAGE_FILELOG *) wp);
    //m_ptrFileLog->UnlockStream ();
}

afx_msg void HThreadFileLog::OnCommandWriteWithDTFileLog (WPARAM wp, LPARAM lp)
{
    int iLengthMsg = -1, iRes = 0; //Success
	HString mes;
	TYPE_FILELOG_MESSAGES typeMsg = HERROR;

	//m_ptrFileLog->LockStream ();

	if (wp) {
		TRY { iLengthMsg = ((MESSAGE_FILELOG *) wp)->mes.length (); }
		CATCH_ALL (err) {
			iLengthMsg = -1;
			THROW (err);
		}
		END_CATCH_ALL

		if (iLengthMsg > 0)
			mes.format ("%i - %s", ((MESSAGE_FILELOG *) wp)->idThread, MB_HSTRING (((MESSAGE_FILELOG *) wp)->mes));
		else
			mes.format ("%i - %s", ((MESSAGE_FILELOG *) wp)->idThread, "Неизвестное сообщение - поток");

		typeMsg = ((MESSAGE_FILELOG *) wp)->type;

		((MESSAGE_FILELOG *) wp)->mes.free ();
		delete ((MESSAGE_FILELOG *) wp);
	}
	else {
		mes.format ("%s - %s", "0000", "Неизвестное сообщение - поток");
	}

	m_ptrFileLog->WriteMessageWithDT (mes, typeMsg);
	mes.free ();

    //m_ptrFileLog->UnlockStream ();
}

afx_msg void HThreadFileLog::OnCommandMaxCountRecord (WPARAM wp, LPARAM lp)
{
    int i = -1, iRes = 0; //Success
    m_ptrFileLog->SetMaxCountRecord (* (int *) wp);
}

afx_msg void HThreadFileLog::OnCommandLevelOutputInfo (WPARAM wp, LPARAM lp)
{
    int i = -1, iRes = 0; //Success
    //m_ptrFileLog->SetLevelOutputInfo (* (int *) wp);
    //m_ptrFileLog->SetFilterMessages (* (DWORD32 *) wp);
}

int HThreadFileLog::LockStreamMessageFileLog (DWORD ms)
{
    int iRes = 0;

    switch (WaitForSingleObject (m_hEventMessageFileLog, ms)) {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            break;
        case WAIT_OBJECT_0:
            //Состояние: вектор с сообщениями занят
            ResetEvent (m_hEventMessageFileLog);
            
            iRes = 1;
        default:
            ;
    }
    
    return iRes;
}

void HThreadFileLog::UnlockStreamMessageFileLog (void)
{
    //Состояние: вектор с сообщениями свободен
    SetEvent (m_hEventMessageFileLog);
}

int HThreadFileLog::push_backMessageFileLog (MESSAGE_FILELOG *pMsgFileLog)
{
    m_vecMessageFileLog.Add (pMsgFileLog);
    
    return m_vecMessageFileLog.GetSize ();
}

int HThreadFileLog::WriteMessages (void)
{
    int iRes = m_vecMessageFileLog.GetSize ();

    MESSAGE_FILELOG *messageFileLogIt = NULL;
    //vector <MESSAGE_FILELOG *>::iterator messageFileLogIt;
    
    while (m_vecMessageFileLog.GetSize ())
    {
        //messageFileLogIt = m_vecMessageFileLog.begin ();
        messageFileLogIt = (MESSAGE_FILELOG *) m_vecMessageFileLog.GetAt (0);
        //if (IsMessageFileLogWithDT ((*messageFileLogIt)->stmOccurred) == 0)
        if (IsMessageFileLogWithDT (messageFileLogIt->stmOccurred) == 0)
            //m_ptrFileLog->WriteMessage ((*messageFileLogIt)->mes.getMBData (), (*messageFileLogIt)->level_info);
            m_ptrFileLog->WriteMessage (messageFileLogIt->mes.getMBData (), messageFileLogIt->type);
        else
            //m_ptrFileLog->WriteMessageWithDT ((*messageFileLogIt)->mes.getMBData (), (*messageFileLogIt)->level_info);
            m_ptrFileLog->WriteMessageWithDT (messageFileLogIt->mes.getMBData (), messageFileLogIt->type);
        
        //(*messageFileLogIt)->mes.free ();
        messageFileLogIt->mes.free ();
        //delete (*messageFileLogIt);
        delete messageFileLogIt;
        
        //m_vecMessageFileLog.erase (m_vecMessageFileLog.begin ());
        m_vecMessageFileLog.RemoveAt (0);
    }
    
    return iRes - m_vecMessageFileLog.GetSize ();
}