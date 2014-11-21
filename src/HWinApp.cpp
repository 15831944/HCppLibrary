// HWinApp.cpp: файл реализации
//

#include "stdafx.h"

// HWinApp

//DECLARE_HA2KW
//DECLARE_HFILELOG
DECLARE_HTHREAD_FILELOG
DECLARE_WSA

#ifdef _DEBUG
    CMemoryState g_memStateBegin, g_memStateEnd, g_memStateDiff;
    int g_iCountFileInfo = 0;
#endif

IMPLEMENT_DYNCREATE(HWinApp, CWinApp)

BEGIN_MESSAGE_MAP(HWinApp, CWinApp)
END_MESSAGE_MAP()

HWinApp::HWinApp () {
	FILELOG_VARIABLE

    WSA_IMPLEMENT

    if (WSA_IS_INIT) {
		m_strHostName = HString (g_pHostEnt->h_name); m_strHostName = m_strHostName.left (m_strHostName.find (0, '.'));
		//WSA_HOSTNAME (m_strHostName)
		WSA_APPDIRECTORY (m_strAppDirectory)
		WSA_APPNAME (m_strAppName)

		m_tmStart = CTime::GetCurrentTime ();
	}
	else {
		//TRACE ("WSAStartup не может быть запущена. Код возврата: %d\n", iRes);
	}
}

HWinApp::~HWinApp () {
	m_strHostName.free ();
	m_strAppDirectory.free ();
	m_strAppName.free ();

	FILELOG_VARIABLE

	strLog.format (_T ("Дестркутор обЪекта приложения %S!"), _T ("HWinApp"));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HCOMMON);

	DELETE_HTHREAD_FILELOG
}

BOOL HWinApp::InitInstance (void) {
	// TODO: выполните здесь индивидуальную инициализацию потоков
#ifdef _DEBUG
	g_memStateBegin.Checkpoint ();
#endif

    /*FILELOG_VARIABLE
	if (IS_INIT_HTHREAD_FILELOG) {*/
		CWinApp::InitInstance ();

		// InitCommonControlsEx() is required on Windows XP if an application
		// manifest specifies use of ComCtl32.dll version 6 or later to enable
		// visual styles.  Otherwise, any window creation will fail.
		INITCOMMONCONTROLSEX InitCtrls;
		InitCtrls.dwSize = sizeof(InitCtrls);
		// Set this to include all the common control classes you want to use
		// in your application.
		InitCtrls.dwICC = ICC_WIN95_CLASSES | ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&InitCtrls);

		AfxEnableControlContainer();

		EnableTaskbarInteraction (FALSE);

		if (! AfxSocketInit ()) {
			/*strLog.format (_T ("Ошибка при вызове 'AfxSocketInit ()' = %S"), IDP_SOCKETS_INIT_FAILED);
			FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);*/
			return FALSE;
		}
		else
			;

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		// of your final executable, you should remove from the following
		// the specific initialization routines you do not need
		// Change the registry key under which our settings are stored
		// TODO: You should modify this string to be something appropriate
		// such as the name of your company or organization
    //}
    //else
		//Код завершения потока отличен от АКТИВного (выполняющегося) потока
        //return FALSE; 
	
	//Не удалось получить код завершения потока для ведения ЛОГ-файлА
        
#ifdef _DEBUG
    g_memStateEnd.Checkpoint ();
    if (g_memStateDiff.Difference (g_memStateBegin, g_memStateEnd)) {
        //g_memStateDiff.DumpAllObjectsSince ();
        g_memStateDiff.DumpStatistics ();
    }
    else
        return FALSE;

#endif   
    //AfxDaoTerm ();
	
	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return TRUE;
}

int HWinApp::ExitInstance (void) {
	// TODO: выполните здесь любую индивидуальную очистку потоков
    WSA_FREE

    int iRes = CWinApp::ExitInstance ();

	return iRes;
}

int HWinApp::TerminateHThread (CObject *obj) {
    int iRes = 0; //Success
    FILELOG_VARIABLE
    
#if defined _WORK_THREAD
    HThreadDescription *pThread = obj;
#else
    HWinThread *pThread = (HWinThread *) obj;
#endif
    
    GetExitCodeThread (pThread->m_hThread, &pThread->m_idExitCodeThread);
    if (pThread->m_idExitCodeThread == STILL_ACTIVE) {
        //iRes = TerminateThread (pThread->m_hThread, 0x0);

		FILELOG_IF_WRITE (HCOMMON)
		strLog.format (_T ("'TerminateHThread' - ПОТОК ID = %i активен..."), pThread->m_nThreadID);
		FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HCOMMON);
		FILELOG_ENDIF

        pThread->Stop ();
		::PostThreadMessage (pThread->m_nThreadID, WM_QUIT, 0, 0);
		Sleep (666);

		FILELOG_IF_WRITE (HDEBUG)
		strLog.format (_T ("'TerminateHThread'  - команда на завершившение ПОТОКа ID = %i"), pThread->m_nThreadID);
		FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
		FILELOG_ENDIF

		/*HANDLE hEventTimeout = CreateEvent (NULL, true, false);
		WaitForSingleObject (hEventTimeout, INFINITE) {}*/
		CTime tmEntry = CTime::GetCurrentTime ();
		GetExitCodeThread (pThread->m_hThread, &pThread->m_idExitCodeThread);

		while (pThread->m_idExitCodeThread == STILL_ACTIVE) {
			FILELOG_IF_WRITE (HDEBUG)
			strLog.format (_T ("'TerminateHThread' - ПОТОК ID = %i активен...(ожидание %i сек.)"), pThread->m_nThreadID, (CTime::GetCurrentTime () - tmEntry).GetTotalSeconds ());
			FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
			FILELOG_ENDIF

			if ((CTime::GetCurrentTime () - tmEntry).GetTotalSeconds () > TIMEOUT_FORCED_TERMINATE) {
				FILELOG_IF_WRITE (HDEBUG)
				strLog.format (_T ("'TerminateHThread' - timeout ожидания завершения ПОТОКа с ID = %i..."), pThread->m_nThreadID);
				FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
				FILELOG_ENDIF

				iRes = TerminateThread (pThread->m_hThread, WAIT_ABANDONED);

				FILELOG_IF_WRITE (HDEBUG)
				strLog.format (_T ("'TerminateHThread' - TerminateThread ПОТОКа с ID = %i = %i"), pThread->m_nThreadID, iRes);
				FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDEBUG);
				FILELOG_ENDIF

				if (iRes == 1)
					pThread->m_idExitCodeThread = WAIT_IO_COMPLETION;
				else
					pThread->m_idExitCodeThread = WAIT_ABANDONED;

				break;
			}
			else
				Sleep (666);

			GetExitCodeThread (pThread->m_hThread, &pThread->m_idExitCodeThread);
		}

        //strLog.format (_T ("Остановлен ПОТОК: %i, %S"), pThread->m_nThreadID, WC_HSTRING (pThread->stringData (pThread->GetDataOfId (HThreadDirectoryWatched::PATH_DIRECTORY_WATCHED))));
        //Не у каждого потока есть ::PATH_DIRECTORY_WATCHED
        strLog.format (_T ("Остановлен ПОТОК: %i"), pThread->m_nThreadID);

        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDEBUG);
    }
    else
        ;
    
    CLOSE_HANDLE (pThread->m_hThread)
    pThread->m_nThreadID = 0x0;

    return iRes;
}

int HWinApp::GetProductAndVersion (HString &strProductName, HString &strProductVersion) {
    // get the filename of the executable containing the version resource
    TCHAR szFilename [MAX_PATH + 1] = {0};
    if (GetModuleFileName (NULL, szFilename, MAX_PATH) == 0) {
        TRACE ("GetModuleFileName failed with error %d\n", GetLastError ());
        return -1;
    }
    else
        ;

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
    if (dwSize == 0) {
        TRACE ("GetFileVersionInfoSize failed with error %d\n", GetLastError ());
        return -1;
    }
    else
        ;

    std::vector<BYTE> data(dwSize);

    // load the version info
    if (! GetFileVersionInfo (szFilename, NULL, dwSize, &data [0])) {
        TRACE ("GetFileVersionInfo failed with error %d\n", GetLastError ());
        return -1;
    }
    else
        ;

    // get the name and version strings
    LPVOID pvProductName = NULL;
    unsigned int iProductNameLen = 0;
    LPVOID pvProductVersion = NULL;
    unsigned int iProductVersionLen = 0;

    /*WCHAR fileEntry [MAX_PATH + 1];
    _swprintf (fileEntry, L"\\VarFileInfo\\Translation");
    retVal = VerQueryValueW (&data [0], fileEntry, &retbuf, (UINT*) &vLen);
    if (retVal && vLen == 4) {
        memcpy (&langD, retbuf, 4);            
        _swprintf (fileEntry, L"\\StringFileInfo\\%02X%02X%02X%02X\\%s", (langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24, (langD & 0xff0000) >> 16, entry);            
    }
    else 
        _swprintf (fileEntry, L"\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID (), entry);*/

    // replace "040904e4" with the language ID of your resources
    if (! VerQueryValue (&data [0], _T ("\\StringFileInfo\\040004e3\\ProductName"), &pvProductName, &iProductNameLen) ||
        !VerQueryValue (&data [0], _T ("\\StringFileInfo\\040004e3\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
        TRACE("Can't obtain ProductName and ProductVersion from resources\n");
        return -1;
    }
    else
        ;

    strProductName = (LPCTSTR) pvProductName; //iProductNameLen
    strProductVersion = (LPCTSTR) pvProductVersion; //iProductVersionLen

    return 0;
}

void HWinApp::ReadVersion (_TCHAR * szVer, int cbVer, const _TCHAR * szAppName, const _TCHAR * szEntry) {
    _tcscpy (szVer, _T ("\n"));
    HRSRC hVersion = FindResource (NULL, MAKEINTRESOURCE (VS_VERSION_INFO), RT_VERSION);
    if (hVersion) {
        HGLOBAL hGlobal = LoadResource (NULL, hVersion); 
        if (hGlobal != NULL) {
            LPVOID versionInfo  = LockResource (hGlobal);
            if (versionInfo != NULL) {
                DWORD vLen, langD;
                BOOL retVal;

                LPVOID retbuf = NULL;

#ifdef _UNICODE
                const _TCHAR * entry = szEntry;
#else     
                WCHAR entry [64]; //I assume here that length of szEntry string is less than 64
                MultiByteToWideChar (CP_ACP, 0, szEntry, -1, entry, 64);
#endif

                WCHAR fileEntry [1024];

                _swprintf (fileEntry, L"\\VarFileInfo\\Translation");
                retVal = VerQueryValueW (versionInfo, fileEntry, &retbuf, (UINT*) &vLen);
                if (retVal && vLen == 4) {
                    memcpy (&langD, retbuf, 4);            
                    _swprintf (fileEntry, L"\\StringFileInfo\\%02X%02X%02X%02X\\%s", (langD & 0xff00) >> 8, langD & 0xff, (langD & 0xff000000) >> 24, (langD & 0xff0000) >> 16, entry);            
                }
                else 
                    _swprintf (fileEntry, L"\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID (), entry);

                retVal = VerQueryValueW (versionInfo, fileEntry, &retbuf, (UINT*) &vLen);
                if (retVal) {
#ifdef _UNICODE
                    _sntprintf (szVer, cbVer-1, _T(" %s\n"), (_TCHAR*) retbuf);
                    szVer [cbVer-1] = '\0';
#else
                    szVer[0] = '\0';
                    int n = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)retbuf, -1, szVer+1, cbVer-2, NULL, NULL);
                    if (n > 0) {
                        szVer[0] = ' ';
                        szVer[n] = '\n';
                        szVer[n+1] = '\0';
                    }
                    else
                        ;
#endif
                }
            }
    
            FreeResource (hGlobal);
        }
    }
}

HString HWinApp::ResourceVersion (const _TCHAR *key) {
    HString strRes;
    _TCHAR strVer [MAX_PATH + 1], strModuleFileName [MAX_PATH + 1];
    GetModuleFileName (AfxGetApp ()->m_hInstance, strModuleFileName, MAX_PATH);

    ReadVersion (strVer, MAX_PATH, strModuleFileName, key);

    strRes = strVer;

    return strRes;
}

HString HWinApp::parseParam (HString par) {
    HString strRes
        , strCmdLine = m_lpCmdLine;
    int posPar = strCmdLine.find (0, par);
    BOOL bParLast = FALSE
        , bParValueQuoted = FALSE;

    if (! (posPar < 0)) {
        posPar += (par.length () + 1);
        //Определить параметр крайний
        if (strCmdLine.find (posPar, '/') < 0) {
            bParLast = TRUE;
        } else {
        }

        //Определить значение параметра в кавычках
        if (strCmdLine.GetAt (posPar) == '"') {
            bParValueQuoted = TRUE;
        } else {
        }

        if (bParLast == TRUE)
            strRes = strCmdLine.mid (posPar);
        else {
            if (bParValueQuoted == TRUE)
                strRes = strCmdLine.mid (posPar + 1, strCmdLine.find (posPar + 1, '"') - (posPar + 1));
            else
                strRes = strCmdLine.mid (posPar, strCmdLine.find (posPar, ' ') - posPar);
        }
    } else {
    }

    return strRes;
}