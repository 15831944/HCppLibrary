#include "StdAfx.h"

#ifdef _CONSOLE
using namespace std;
#endif

EXT_DECLARE_WSA

HFileLog::HFileLog (void) {
}

HFileLog::HFileLog (int iMaxCountRec, DWORD32 dw32FilterMessages) {   
    CFileException *errFileOpen = NULL;
	HString strWTmp, strATmp, strPathApp, strNameFileError;
    int i, iRes = 0; //Success

    m_iCurCountRec = 0;
	m_iMaxCountRec = iMaxCountRec;
	//m_iLevelOutputInfo = iLevelInfo;
	SetFilterMessages (dw32FilterMessages);
    
    m_hEvtWriteMessage = CreateEvent (NULL, true, true, NULL);
    
    if ((m_hEvtWriteMessage == 0x0) || (m_hEvtWriteMessage == INVALID_HANDLE_VALUE))
        iRes = -1;
    else
        ;
    
	if (iRes == 0) {
		HString strHostName, strAppDirectory, strAppName;

		if (WSA_IS_INIT) {
			WSA_HOSTNAME (strHostName)
			WSA_APPDIRECTORY (strAppDirectory)
			WSA_APPNAME (strAppName)
		}
		else
			;

		strPathApp.append (strAppDirectory);
		strPathApp.append ("\\");
		strPathApp.append (strAppName);
		
		strPathApp.insert (strPathApp.reverseFind ('.'), HString (_T ("-")) + strHostName);
		strPathApp.replace (_T ("exe"), _T ("log"));

		//errFileOpen = NULL;

		try {
		   iRes = m_fLog.Open (strPathApp.GetBuffer (), CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate, errFileOpen);
		}
		catch (CFileException *err) {
		   //THROW (err);
           err->Delete ();
		}

        if (iRes == 0) {
            m_fLog.Abort ();
        }
        else
            ;

		if ((m_fLog.m_hFile == CFile::hFileNull) && /*(! (errFileOpen == NULL)) &&*/ (! (iRes == 1)))
			iRes = m_fLog.Open (strPathApp.GetBuffer (), CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate, errFileOpen);
		else
			;

		//if ((! (m_fLog.m_hFile == INVALID_HANDLE_VALUE)) && (err == NULL) && (iRes == 1))
		if ((! (m_fLog.m_hFile == CFile::hFileNull)) /*&& (errFileOpen == NULL)*/ && (iRes == 1)) {
			m_arbyteContentLog.SetSize (m_fLog.GetLength ());
			m_fLog.Read (m_arbyteContentLog.GetData (), m_fLog.GetLength ());

			for (i = 0; i < m_arbyteContentLog.GetSize (); i ++)
			{
				if (m_arbyteContentLog.GetAt (i) == 0xA)
				    m_iCurCountRec ++;
			}
			
			if (m_iCurCountRec > m_iMaxCountRec)
			{//НЕ ТЕСТировалось ???
				DeleteAdditionalRec ();
			}

			WriteMessageWithDT (_T("=============Запуск программы...============="), HCOMMON);
			//WriteMessageWithDT ("Запуск программы...");
	        
			strPathApp.free ();
		}
		else {
			//err = DEBUG_NEW CFileException (CFileException::invalidFile | CFileException::genericException);
			/*errFileOpen->ThrowErrno (EBADF);
			return;*/
		}
	}
	else
		; //Ошибка в определении имени ХОСТа (запуск WSA)
}

HFileLog::~HFileLog (void)
{
    //WriteMessageWithDT (_T("Exit programm..."));
	WriteMessageWithDT ("=============Выход из программы!=============", HCOMMON);

	m_arbyteContentLog.RemoveAll ();

	m_fLog.Close ();
	
	CLOSE_HANDLE (m_hEvtWriteMessage)
}

int HFileLog::DeleteAdditionalRec (void)
{
    int i = -1;
    
    if (m_iMaxCountRec > 0) {
        i = 0;
        
	    do
	    {//Удаление ЛИШНих записей
		    //m_arstrAContentLog.RemoveAt (m_arstrAContentLog.GetSize () - 1);
		    if (m_arbyteContentLog.GetSize () > i) {
		        if (m_arbyteContentLog.GetAt (i ++) == 0xA)
	                m_iCurCountRec --;
	            else
	                ;
            }
	        else
	            ;
	    }
	    while (m_iCurCountRec > m_iMaxCountRec);
    	
	    m_arbyteContentLog.RemoveAt (0, i);
    }
    else
        ;
	
	return i;
}

void HFileLog::LockStream (void)
{
    /*CString strLog;
    strLog.Format (_T ("'WaitFor...m_hEvtWriteMessage' для 'HFileLog::LockStream'"));
    WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_3);*/
    
    switch (WaitForSingleObject (m_hEvtWriteMessage, INFINITE))
    {
	    case WAIT_ABANDONED:
	    case WAIT_TIMEOUT:
		    break;
	    case WAIT_OBJECT_0:
	        //ЗАПРЕЩаем остальным РАБОТАть ???
		    ResetEvent (m_hEvtWriteMessage);
	    default:
		    ;
    }
}

void HFileLog::UnlockStream (void) {
    SetEvent (m_hEvtWriteMessage);
}

BOOL HFileLog::WriteMessage (LPSTR msg, TYPE_FILELOG_MESSAGES typeMsg) {
    BOOL bRes = false;
    
    if (IsWrite (typeMsg)) {	    
	    CStringA strMsg (msg);
	    bRes = WriteMessage (strMsg.GetBuffer ());
		//bRes = WriteMessage (msg);
	    
		strMsg.Empty ();
	    	    
	    bRes = true;
    }
    else
        bRes = false;
        
    return bRes;
}

BOOL HFileLog::WriteMessage (LPSTR msg) {
    int i;
    CStringA strAMsg;
    
    strAMsg.Append (msg);
	//delete msg;
	
    if ((strAMsg.Find ("DT") < 0) || (strAMsg.Find ("TM") < 0) || (strAMsg.Find ("MS") < 0))
    {//Достаточно ОДНОго условия !!!
	    i = GetDateTime ().length ();
	    i ++;
	    while (i --)
		    strAMsg.Insert (0, 0x20);
    }
	
    for (i = 0; i < strAMsg.GetLength (); i ++)
    {
        m_arbyteContentLog.Add (strAMsg.GetAt (i));
    }
    m_arbyteContentLog.Add (0xa);
	
    m_iCurCountRec ++;
                
    if (m_iCurCountRec > m_iMaxCountRec)
        DeleteAdditionalRec ();
	    
    m_fLog.SetLength (0);
    m_fLog.Write (m_arbyteContentLog.GetData (), m_arbyteContentLog.GetSize ());

#ifdef _CONSOLE
    //_WA2 (strAMsg.GetBuffer ());
    cout << strAMsg << endl;
#endif
	    
    strAMsg.ReleaseBuffer ();
        
    return true;
}

BOOL HFileLog::WriteMessage (LPWSTR wmsg, TYPE_FILELOG_MESSAGES typeMsg) {
    if (IsWrite (typeMsg)) {    
        CStringW strWMsg, strWTmp;
	    CStringA strAWrite;
	    BOOL bSetCharDefault = false;
     
        strWMsg.Append (wmsg);
	  
	    strAWrite = _WC_TO_MB (strWMsg);
	                
        /*
        BYTE *ptrMsg = DEBUG_NEW BYTE [szMsg + 1];
        BYTE *ptrWrite = DEBUG_NEW BYTE [szMsg / sizeof (WCHAR) + 2];
        memcpy (ptrMsg, strMsg.GetBuffer (), szMsg);
        ptrMsg [szMsg] = 0x0;
        for (i = 0; i < szMsg; i ++)
        {
            if (i % sizeof (WCHAR) == 0)
                memcpy ((ptrWrite + (i / sizeof (WCHAR))), (ptrMsg + i), 1);
        }
        
        ptrWrite [szMsg / sizeof (WCHAR) + 0] = 13;
        ptrWrite [szMsg / sizeof (WCHAR) + 1] = 0x0;
        
        delete ptrMsg;
        delete ptrWrite;
        */
            
        //fLog.Write (strAWrite.GetBuffer (), (szMsg / sizeof (WCHAR)));
	    WriteMessage (strAWrite.GetBuffer ());
        
        strWMsg.ReleaseBuffer ();
	    strAWrite.ReleaseBuffer ();
                
        return true;
    }
    else
        return false;
}

BOOL HFileLog::WriteMessage (HString &msg, TYPE_FILELOG_MESSAGES typeMsg) {
    switch (msg.type ()) {
        case HString::MBSTR:
            return WriteMessage (msg.getMBData (), typeMsg);
            //break;
        case HString::WCSTR:
            return WriteMessage (msg.getWCData (), typeMsg);
            //break;
        default:
            ;
    }
    
    return false;
}

BOOL HFileLog::WriteMessageWithDT (LPSTR msg, TYPE_FILELOG_MESSAGES typeMsg) {
    if (IsWrite (typeMsg)) {
        int i = 0;
	    BOOL bSetCharDefault = false;
        HString strAMsg;
        
        strAMsg = GetDateTime ();
        
        strAMsg.appendChar (' ');
            
        strAMsg.append (msg);
            
        WriteMessage (MB_HSTRING (strAMsg));
            
        return true;
    }
    else
        return false;
}

BOOL HFileLog::WriteMessageWithDT (LPWSTR msg, TYPE_FILELOG_MESSAGES typeMsg) {
    if (IsWrite (typeMsg)) {
        WCHAR wch;
        HString strWMsg;

        strWMsg = GetDateTime ();
        
        wch = 0x20;
        strWMsg.appendChar (wch);
            
        strWMsg.append (msg);
            
        WriteMessage (MB_HSTRING (strWMsg));
            
        return true;
    }
    else
        return false;
}

BOOL HFileLog::WriteMessageWithDT (HString &msg, TYPE_FILELOG_MESSAGES typeMsg) {
    switch (msg.type ()) {
        case HString::MBSTR:
            return WriteMessageWithDT (msg.getMBData (), typeMsg);
            //break;
        case HString::WCSTR:
            return WriteMessageWithDT (msg.getWCData (), typeMsg);
            //break;
        default:
            ;
    }
    
    return false;
}

int HFileLog::FindLastRec ()
{
    int iRes = 0;
	/*
	SYSTEMTIME sttm1, sttm2;
	int i;
    
    for (i = 1; i < m_arstrAContentLog.GetSize (); i ++)
    {
		sttm1 = GetSYSTEMTIMERec (m_arstrAContentLog.GetAt (i - 1));
		sttm2 = GetSYSTEMTIMERec (m_arstrAContentLog.GetAt (i));
		if (IsCorrectSYSTEMTIME (sttm1) && IsCorrectSYSTEMTIME (sttm2))
		{
			if (CompareSYSTEMTIME (sttm1, sttm2) == 1)
			{
				iRes = i;
				break;
			}
		}
		else
		{
			iRes = -1;
			break;
		}
    }

	if ((i == m_arstrAContentLog.GetSize ()) && (! (iRes == -1)))
		iRes = i;
    */
    return iRes;
}

SYSTEMTIME HFileLog::GetSYSTEMTIMERec (LPCSTR lprec)
{
	SYSTEMTIME sttm;
	HString strPartDT, rec (lprec);
	int iPosPartDT;
	BOOL bRes = true;

	memset (&sttm, 0x0, sizeof (SYSTEMTIME));

	iPosPartDT = rec.find (0, "DT");
#ifdef NUMBERED_RECORD
	if (iPosPartDT > 0)
#else
    if (iPosPartDT == 0)
#endif
	{
		iPosPartDT += 2;
		strPartDT = rec.mid (iPosPartDT, 2);
		if (IsNumber (MB_HSTRING (strPartDT)))
		{
			sttm.wDay = strPartDT.number ();

			iPosPartDT += 3;
			strPartDT = rec.mid (iPosPartDT, 2);
			if (IsNumber (MB_HSTRING (strPartDT)))
			{
				sttm.wMonth = strPartDT.number ();

				iPosPartDT += 3;
				strPartDT = rec.mid (iPosPartDT, 4);
				if (IsNumber (MB_HSTRING (strPartDT)))
				{
					sttm.wYear = strPartDT.number ();

					iPosPartDT = rec.find (0, "TM");
					if (iPosPartDT > 0)
					{
						iPosPartDT += 2;
						strPartDT = rec.mid (iPosPartDT, 2);
						if (IsNumber (MB_HSTRING (strPartDT)))
						{
							sttm.wHour = strPartDT.number ();

							iPosPartDT += 3;
							strPartDT = rec.mid (iPosPartDT, 2);
							if (IsNumber (MB_HSTRING (strPartDT)))
							{
								sttm.wMinute = strPartDT.number ();

								iPosPartDT += 3;
								strPartDT = rec.mid (iPosPartDT, 2);
								if (IsNumber (MB_HSTRING (strPartDT)))
								{
									sttm.wSecond = strPartDT.number ();

									iPosPartDT = rec.find (0, "MS");
									if (iPosPartDT > 0)
									{
										iPosPartDT += 2;
										strPartDT = rec.mid (iPosPartDT, 3);
										if (IsNumber (MB_HSTRING (strPartDT)))
											sttm.wMilliseconds = strPartDT.number ();
										else
											bRes = false;
									}
									else
										bRes = false;
								}
								else
									bRes = false;
							}
							else
								bRes = false;
						}
						else
							bRes = false;
					}
					else
						bRes = false;
				}
				else
					bRes = false;
			}
			else
				bRes = false;
		}
		else
			bRes = false;
	}
	else
		bRes = false;

	if (! bRes)
		memset (&sttm, 0x0, sizeof (SYSTEMTIME));

	return sttm;
}

BOOL HFileLog::IsNumber (LPCSTR lpstrNumber)
{
    HString strNumber (lpstrNumber);
    
	for (int i = 0; i < strNumber.length (); i ++)
	{
		if ((strNumber.GetAt (i) < 48) || (strNumber.GetAt (i) > 57))
			return false;
	}

	return true;
}

HString HFileLog::GetDateTime (void)
{
	int i;
	HString strWRes, strWms, strWTmp;

	SYSTEMTIME sttm;
    //GetSystemTime (&sttm);
	GetLocalTime (&sttm);
	CTime tm (sttm);

	//strWms.GetBufferSetLength (3);
	//wmemset (strWms.GetBuffer (), 0x0, strWms.GetLength ());
	strWms.format ("%i", sttm.wMilliseconds);
		
	i = 100;
	while (sttm.wMilliseconds / i == 0)
	{
		strWms.insert (0, "0"); //0x30
		i /= 10;
		if (i == 1)
			break;
	}
	
	strWTmp = HString (tm.Format (_T ("DT%d.%m.%YTM%H:%M:%S")).GetBuffer ());
    strWRes.append (strWTmp);
	strWRes.append ("MS");
	strWRes.append (strWms);

	strWms.free ();

	strWTmp.free ();

	return strWRes;
}

int HFileLog::IsWrite (TYPE_FILELOG_MESSAGES typeMsg)
{
    int iRes = 0; //Не записывать сообщение в лог-файл
    
    if (m_dw32FilterMessages & (1 << (typeMsg - 1)))
        iRes = 1;
    else
        iRes = 0;
    
    return iRes;
}

void HFileLog::SetFilterMessages (TYPE_FILELOG_MESSAGES typeMsg) {
    SetFilterMessages ((DWORD32) typeMsg);
}

void HFileLog::SetFilterMessages (DWORD32 filterMessages) {
    TYPE_FILELOG_MESSAGES typeMsg = NOT_MESSAGES;

    if ((! (filterMessages == NOT_MESSAGES)) && (! (filterMessages == ALL_MESSAGES))) {
        m_dw32FilterMessages = 0;
        for (++ typeMsg; typeMsg < ALL_MESSAGES; typeMsg ++) {
            //if (filterMessages & typeMsg) {
            if (filterMessages & (1 << (typeMsg - 1))) {
                m_dw32FilterMessages |= 1 << (typeMsg - 1);
            }
            else
                ;
        }
    
	    //m_dw32FilterMessages |= 1 << (filterMessages - 1);
	}
	else {
	    m_dw32FilterMessages = NOT_MESSAGES;
	    if (filterMessages == NOT_MESSAGES)
	        ;
	    else  {
	        for (++ typeMsg; typeMsg < ALL_MESSAGES; typeMsg ++) {
	            m_dw32FilterMessages |= 1 << (typeMsg - 1);
	        } 
        }
	}
}

HString HFileLog::GetNameFileError (HString &nameFileLog) {
	HString strRes = "", strExt, strWildcard = nameFileLog;
	int iRes = -1, iExt = -1;
	CFileFind ff;
	HFileInfo *pHFITmp = NULL;

	strWildcard += ".*";
	BOOL bFindFile = ff.FindFile (strWildcard.GetBuffer ());

	while (bFindFile) {
        bFindFile = ff.FindNextFile ();

        if (ff.IsDirectory ())
             continue;
		            
		HFILEINFO (pHFITmp, ff.GetFilePath ().GetBuffer ());
		if (pHFITmp == NULL) {
		    //По наЙденному файлУ не смогли получить информациЮ
		    iRes = -2;
		    break;
		}
		else
		    ;

		iExt = pHFITmp->FileExtAsNumber ();
		if (iRes < iExt)
			iRes = iExt;
		else
			;
	}

	//Найден ли хоть ОДИн файл
	//Прекращён ли while преждевременно
	switch (iRes) {
		case -1:
		case -2:
			iRes = 0;
			break;
		default:
			;
	}

	strExt.format ("%i", iRes);

	while (strRes.length () < 3)
		strExt.insert (0, "0");

	
	//strRes = 

	return strRes;
}
