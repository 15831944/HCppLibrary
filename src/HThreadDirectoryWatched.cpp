#include "stdafx.h"
#include "HThreadDirectoryWatched.h"

EXT_DECLARE_HTHREAD_FILELOG

IMPLEMENT_DYNCREATE(HThreadDirectoryWatched, HWinThread)

HThreadDirectoryWatched::HThreadDirectoryWatched (void) {
    FILELOG_VARIABLE
    FILELOG_IF_WRITE (THREAD_WATCH_DIRECTORY)
    strLog.format (_T ("%S"), _T ("Конструктор потока 'HThreadDirectoryWatched'"));
    	
	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_WATCH_DIRECTORY);

	/*ptrMsgFileLog = DEBUG_NEW MESSAGE_FILELOG;
	TRY { ptrMsgFileLog->mes = HString (strLog.GetBuffer ()); }
	CATCH_ALL (err) { ptrMsgFileLog->mes = "Неизвестное сообщение???"; THROW (); }
	END_CATCH_ALL
    ptrMsgFileLog->idThread = idCurrentThread;
    memset (&ptrMsgFileLog->stmOccurred, 0x0, sizeof (SYSTEMTIME));
    CTime::GetCurrentTime ().GetAsSystemTime (ptrMsgFileLog->stmOccurred);
    ptrMsgFileLog->type = THREAD_WATCH_DIRECTORY;
    ::PostThreadMessage (PTR_THREAD_FILELOG->m_nThreadID, WM_APP_FILELOGMSG_WITHDT, (WPARAM) ptrMsgFileLog, 0);*/
    
	FILELOG_ENDIF

    m_arHandlesRun [BREAK_FORCE] = CreateEvent (NULL, true, false, NULL); //Stop Event
    
    m_pOverlappedDir = DEBUG_NEW OVERLAPPED;
    m_pOverlappedDir ->hEvent = CreateEvent (NULL, false, false, NULL); //DirectoryWatched
    m_arHandlesRun [WATCHED] = m_pOverlappedDir ->hEvent;
    ////DuplicateHandle (AfxGetApp ()->m_hInstance, m_pOverlappedDir ->hEvent, AfxGetApp ()->m_hInstance, m_arHandlesRun [1], ...);
    //if (DuplicateHandle (AfxGetApp ()->m_hThread, m_pOverlappedDir ->hEvent, AfxGetApp ()->m_hThread, &m_arHandlesRun [1], NULL, NULL, NULL))
    //{
    //    SetEvent (m_arHandlesRun [0]);
    //    
    //    m_idExitCodeThread = -1;
    //}
    //else
        /*m_idExitCodeThread = WAIT_IO_COMPLETION;*/
    
    m_arHandlesRun [DIRECTORY] = NULL;
    
    m_iSizeDataWatched = 4096;
    m_lpDataDirWatched = DEBUG_NEW BYTE [m_iSizeDataWatched + 1];
}

//В 'stdafx.h'
//#define CLOSE_HANDLE(hndl) if (hndl == NULL) ; else CloseHandle (hndl);

HThreadDirectoryWatched::~HThreadDirectoryWatched (void) {
	TRY { CLOSE_HANDLE (m_arHandlesRun [DIRECTORY]); }
	CATCH_ALL (err) {
		THROW (err);
	}
	END_CATCH_ALL

    //CLOSE_HANDLE (m_arHandlesRun [WATCHED]); //DirectoryWatched
    CLOSE_HANDLE (m_pOverlappedDir->hEvent); //DirectoryWatched
    delete m_pOverlappedDir;
    
    CLOSE_HANDLE (m_arHandlesRun [BREAK_FORCE]); //Stop Event

    delete m_lpDataDirWatched;

	FILELOG_VARIABLE
    FILELOG_IF_WRITE (HDEBUG)
    //strLog.format (_T ("%S"), _T ("ДЕструктор потока 'HThreadDirectoryWatched'"));
	strLog.format (_T ("%S '%S'"), _T ("ДЕструктор потока"), WC_HSTRING (HString (GetRuntimeClass ()->m_lpszClassName))); //, m_nThreadID);
    //FILELOG_WRITE_WITHDT (strLog.GetBuffer (), THREAD_WATCH_DIRECTORY);
	FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    FILELOG_ENDIF
}

BOOL HThreadDirectoryWatched::InitInstance () {
	// TODO:  perform and per-thread initialization here	
	return HWinThread::InitInstance ();
}

int HThreadDirectoryWatched::ExitInstance () {
	// TODO:  perform any per-thread cleanup here	
	return HWinThread::ExitInstance ();
}

BEGIN_MESSAGE_MAP(HThreadDirectoryWatched, HWinThread)
	ON_THREAD_MESSAGE (WM_USER_THREAD_RESUME, &HThreadDirectoryWatched::OnResumeRun)
END_MESSAGE_MAP()

//int HThreadDirectoryWatched::Run ()
afx_msg void HThreadDirectoryWatched::OnResumeRun (WPARAM wp, LPARAM lp) {
    FILELOG_VARIABLE

    DWORD idExitCodeThread = m_idExitCodeThread;
    //CWinApp *theApp = ::AfxGetApp ();

    //Здесь обман 'link.exe', который возвращает error - ::Run () must return a value
    //Но НЕОБХодимо завершить ПОТОК вызовом 'AfxEndThread'
    //с этим КОДом имеем лишь: warning - not all paths return a value
    if (! (idExitCodeThread == WAIT_IO_COMPLETION)) {
        idExitCodeThread = -1;
        //return idExitCodeThread;
    }
    else
        ;

    DWORD iRes;//Результат поиска изменений в папке (количество возвращённых BYTE)
	DWORD iParam; //Переменная для параметров наблюдения за каталогом
	HString strCurDir; //Выбранный каталог
	int iCountRec; //Количество записей в изменившемся каталоге;
	
	strCurDir.free ();
	strCurDir = stringData (GetDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED));

	HANDLE hEvtDirectoryWatched = *(HANDLE *) GetDataOfId (HANDLE_EVENT_DIRECTORY_WATCHED);
	DWORD idThreadResponse = 0x0;
	//CWnd *pWnd = 0x0;
	HWND hWnd = 0x0;

	/*if ((CWnd *) GetDataOfId (PTR_WND_RESPONSE))
		pWnd = (CWnd *) GetDataOfId (PTR_WND_RESPONSE);*/
	if ((HWND *) GetDataOfId (HWND_RESPONSE))
		hWnd = *(HWND *) GetDataOfId (HWND_RESPONSE);
	else
		;
	
	if ((DWORD *) GetDataOfId (INT_IDTHREAD_RESPONSE))
		idThreadResponse = *(DWORD *) GetDataOfId (INT_IDTHREAD_RESPONSE);
	else
		;
		
	//Описатель каталога
	m_arHandlesRun [DIRECTORY] = CreateFile (WC_HSTRING (strCurDir),
	                    GENERIC_READ,
	                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	                    NULL,
	                    OPEN_EXISTING,
	                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED ,
	                    0);

	idExitCodeThread = STILL_ACTIVE;

    //Если ошиблись...
    if (m_arHandlesRun [DIRECTORY] == INVALID_HANDLE_VALUE) {
		idExitCodeThread = GetLastError ();

        strLog.format ("Ошибка: 'CreateFile...HThreadDirectoryWatched' для dir = %S", WC_HSTRING (strCurDir));
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);
        strLog.free ();
    }
    else {
        iParam = 0;
	    //Для получения уведомления об изменении имени
	    iParam |= FILE_NOTIFY_CHANGE_FILE_NAME;
	    //Для получения уведомления об изменении содержания
	    iParam |= FILE_NOTIFY_CHANGE_LAST_WRITE;
	    //Для получения уведомления об изменении размера
	    //iParam |= FILE_NOTIFY_CHANGE_SIZE;
    }

    strCurDir.free ();
        
    if (idExitCodeThread == STILL_ACTIVE) {
        if (! (hWnd == 0x0))
		    PostMessage (hWnd, WM_USER_ENTER_DIRECTORY_WATCHED, (WPARAM) 0x0, 0x0);
	    else
		    if (idThreadResponse)
                ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_ENTER_DIRECTORY_WATCHED, 0x0, 0x0);
		    else
			    ;

        while (idExitCodeThread == STILL_ACTIVE) {
            switch (WaitForSingleObject (hEvtDirectoryWatched, INFINITE)) {
			    case WAIT_ABANDONED:
			    case WAIT_TIMEOUT:
                    idExitCodeThread = WAIT_TIMEOUT;
				    break;
			    case WAIT_OBJECT_0:
                default:
                    ;
            }
            //Очищаем память перед записью в нее (на всякий случай) ???
            memset (m_lpDataDirWatched, 0x0, m_iSizeDataWatched);
            //Теперь мы будем ждать пока чего-нибудь в интересующем нас каталоге
            //изменится или произойдет ошибка (и мы выйдем из цикла)
            //FILE_NOTIFY_CHANGE - это список флагов - о них ВЫШе.
	        if (! ReadDirectoryChangesW (m_arHandlesRun [DIRECTORY], m_lpDataDirWatched, m_iSizeDataWatched, true, iParam, &iRes, m_pOverlappedDir, NULL)) {
			    //!!! Завершение ВЕЧНОГо циклА !!!
                iRes = GetLastError ();
                idExitCodeThread = iRes;
            }
            else {
			    //http://www.delphimaster.net/view/4-1124470423/all
                switch (WaitForMultipleObjects (2, m_arHandlesRun, false, INFINITE)) {
                    case WAIT_ABANDONED:
		            case WAIT_TIMEOUT:
			            break;
                    case WAIT_OBJECT_0 + 0: //Stop Event
                        idExitCodeThread = ERR_BREAK;
                        break;
                    case WAIT_OBJECT_0 + 1: //HANDLE of OVERLAPPED
                        if (! (GetOverlappedResult (m_arHandlesRun [DIRECTORY], m_pOverlappedDir, &iRes, false))) {
                            iRes = GetLastError ();
                            idExitCodeThread = iRes;
                        }
                        else {
                            if (iRes > 0) {
                                //strLog.format ("'WaitFor...m_hEvtDirectoryWatched' для 'fThreadDirectoryWatched' id = %i", GetCurrentThreadId ());
                                //FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), LEVEL_INFO_3);
                                //strLog.free ();
                                /*
                                //Сюда мы попадаем, если функция выполнилась успешно и количество переданных BYTE > 0
                                //и 'lpData' указывает на одну или несколько структур FILE_NOTIFY_INFORMATION
                                */
		                        switch (WaitForSingleObject (hEvtDirectoryWatched, INFINITE)) {
			                        case WAIT_ABANDONED:
			                        case WAIT_TIMEOUT:
				                        break;
			                        case WAIT_OBJECT_0:
			                            /*strLog.format ("'ResetEvent...m_hEvtDirectoryWatched' для 'fThreadDirectoryWatched' id = %i", GetCurrentThreadId ());
                                        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), LEVEL_INFO_3);
                                        strLog.free ();*/
    			                
				                        ResetEvent (hEvtDirectoryWatched);

									    /*strLog.format ("HThreadDirectoryWatched отправлено = %i байт", iRes);
									    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);
									    strLog.free ();*/

									    /*if (pWnd)
										    pWnd->PostMessage (WM_USER_MESSAGE_DIR_WATCH, (WPARAM) &m_nThreadID, (LPARAM) iRes);*/
									    if (hWnd)
										    SendMessage (hWnd, WM_USER_MESSAGE_DIR_WATCH, (WPARAM) &m_nThreadID, (LPARAM) iRes);
									    else
										    if (idThreadResponse)
											    ::PostThreadMessage (idThreadResponse, WM_USER_MESSAGE_DIR_WATCH, (WPARAM) &m_nThreadID, (LPARAM) iRes);
										    else
											    ;
				                        break;
			                        default:
				                        ;
		                        }
                            }
                            else
                                ; //Нет байт для обработки ???
                        }
                        break;
                    default:
                        ;
                }
            }
        }
    }
    else
        ; //Ошибка при старте функционала потока

    /*if (pWnd)
		pWnd->PostMessage (WM_USER_EXIT_DIRECTORY_WATCHED, (WPARAM) &m_nThreadID, 0x0);*/
	if (hWnd)
		PostMessage (hWnd, WM_USER_LEAVE_DIRECTORY_WATCHED, (WPARAM) &m_nThreadID, 0x0);
	else
		if (idThreadResponse)
			//::PostThreadMessage (idThreadResponse, WM_USER_EXIT_DIRECTORY_WATCHED, (WPARAM) &m_nThreadID, 0x0);
            ::PostThreadMessage (AfxGetApp ()->m_nThreadID, WM_USER_LEAVE_DIRECTORY_WATCHED, (WPARAM) &m_nThreadID, 0x0);
		else
			;

	CLOSE_HANDLE (m_arHandlesRun [DIRECTORY]);
    
    idExitCodeThread = 0x0;

	//Вариант №1
	//return idExitCodeThread;
    //Вариант №2
	//AfxEndThread (idExitCodeThread, m_bAutoDelete);
}