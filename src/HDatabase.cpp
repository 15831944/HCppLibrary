#include "stdafx.h"
// HDatabase.cpp : implementation file
//
#include "HDatabase.h"
#include "HDatabaseSQL.h"

//IMPLEMENT_DYNAMIC (HDatabase, CDatabase)
IMPLEMENT_DYNCREATE (HDatabase, CDatabase)

#if defined _HTHREAD_FILELOG
EXT_DECLARE_HTHREAD_FILELOG
#else
#endif

#ifdef _DEBUG
extern CMemoryState g_memStateDiff,
                    g_memStateBegin,
                    g_memStateEnd;
#endif

// HDatabase

HDatabase::HDatabase () : CDatabase (), m_typeDriverDB (NOT_DRIVERS) {
    m_hEvtDatabase = CreateEvent (NULL, true, true, NULL);
    /*
    for (ID_TABLES currentId = ID_TABLES::TABLE_TEMPLATE; currentId < ID_TABLES::TABLE_NULL; currentId ++)
    {
        m_pmapDBRS [currentId] = NULL;
    }
    */

	m_arstrDriverDB [MS_ACCESS] = "Microsoft Access Driver (*.mdb)";
	m_arstrDriverDB [MS_DBASE_ODBC] = "Microsoft dBASE Driver (*.dbf)";
	
	m_arstrConnect [MS_ACCESS] = "DBQ";
	m_arstrConnect [MS_DBASE_ODBC] = "DriverID=277;Dbq";
}

HDatabase::~HDatabase() {
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
    
    FILELOG_IF_WRITE (HCOMMON)
    strLog.format ("%s", "Деструктор БД!");
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON);
    FILELOG_ENDIF
#else
#endif
    FreeMap ();

    CLOSE_HANDLE (m_hEvtDatabase)
}

void HDatabase::FreeMap (void) {
    POSITION pos;
    HString key;
    
    void *pDest = NULL, *pRefDef = NULL;
    HANDLE *pRefHandleEvent = NULL;
    HRecordset *pRefRecordset = NULL;
    
    if (pos = m_pmapDBRS.GetStartPosition ()) {
        while (pos)
        {
            m_pmapDBRS.GetNextAssoc (pos, (void *&) pRefRecordset, pDest);
            //if (pRefRecordset && pos)
            if (pRefRecordset)
                FreeMemoryDBObject ((CObject *&) pRefRecordset);
                //pRefRecordset->Close ();
        }
        m_pmapDBRS.RemoveAll ();
    }
    else
        ;
    
    if (pos = m_pmapHandleEventRS.GetStartPosition ()) {
        while (pos) {
            m_pmapHandleEventRS.GetNextAssoc (pos, (void *&) pRefRecordset, (void *&) pRefHandleEvent);
            //if (pRefHandleEvent && pos)
            if (pRefHandleEvent)
            {
                CLOSE_HANDLE (*pRefHandleEvent);
                delete pRefHandleEvent;
            }
        }
        m_pmapHandleEventRS.RemoveAll ();
    }
    else
        ;
}

#define CAST(type, obj) (type *) obj

HRecordset *HDatabase::GetRecordset (CObject *some, HString &nameDef) {
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif

	HRecordset *pResRecordset = NULL; 
    HANDLE *pRefHandleEvent = NULL;
    CObject *pRefObject;
    POSITION pos;
    
    if (! IsUse ())
        LockDatabase ();
	else
		;
    
#if defined _HTHREAD_FILELOG
    FILELOG_IF_WRITE (HDATABASE)
    strLog.format (_T ("Запрошен RS: %S"), WC_HSTRING (nameDef));
    FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE);
    FILELOG_ENDIF
#else
#endif
    
    if (pos = m_pmapDBRS.GetStartPosition ()) {
        while (pos) {
            m_pmapDBRS.GetNextAssoc (pos, (void *&) pResRecordset, (void *&) pRefObject);
            if (pRefObject && pos) {
                if (pRefObject == some) {
                    if ((HRECORDSET (pResRecordset))->GetNameTable ().compareNoCase (nameDef) == 0) {
                        m_pmapHandleEventRS.Lookup (pResRecordset, (void *&) pRefHandleEvent);
                        if (pRefHandleEvent) {
#if defined _HTHREAD_FILELOG
                            FILELOG_IF_WRITE (HDATABASE)
                            strLog.format (_T ("'WaitFor...Recordset' для 'HDatabase::GetRecordset' от %S"), WC_HSTRING (HString (some->GetRuntimeClass ()->m_lpszClassName)));
                            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE);
                            FILELOG_ENDIF
#else
#endif                            
                            switch (WaitForSingleObject (*pRefHandleEvent, INFINITE))
                            {
                                case WAIT_ABANDONED:
	                            case WAIT_TIMEOUT:
		                            break;
	                            case WAIT_OBJECT_0:
	                                //Состояние RS ЗАНЯТ
		                            ResetEvent (*pRefHandleEvent);
	                            default:
		                            ;
                            }
#if defined _HTHREAD_FILELOG
                            FILELOG_IF_WRITE (HDATABASE)
                            strLog.format (_T ("Возвращён открытый RS: %S"), WC_HSTRING (nameDef));
                            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE);
                            FILELOG_ENDIF
#else
#endif
                            if (! pResRecordset->IsOpen ())
                                if (OpenRS (pResRecordset, nameDef.GetBuffer ()) == 0)
                                    return pResRecordset;
                                else
                                    return NULL;
                            else
                                return pResRecordset;
                        }
                        else
                            ; //Нет события для RS ???
                    }
                    else
                        ; //ИМя таблицы запрашиваемого RS не совпадает с именем таблицы текущего RS
                }
                else
                    ; //ОбЪект, запрашивающий RS не совпадает с "владельцем" текущего RS
            }
            else
                ; //Не найден в 'Map' ОбЪект ??? ЗНАЧИТ pos == NULL ???
        }
    }
    else
        ;
    
    pResRecordset = DEBUG_NEW HRecordset (this);
    
    if (OpenRS (pResRecordset, nameDef.GetBuffer ()) == 0) {
        m_pmapDBRS.SetAt (pResRecordset, some);
        pRefHandleEvent = DEBUG_NEW HANDLE;
        *pRefHandleEvent = CreateEvent (NULL, true, false, nameDef.GetBuffer ());
        m_pmapHandleEventRS.SetAt (pResRecordset, pRefHandleEvent);
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HDATABASE)
        strLog.format (_T ("Открыт RS %S"), WC_HSTRING (nameDef));
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE)
        FILELOG_ENDIF
#else
#endif
        return pResRecordset;
    }
    else
        return NULL;
}

int HDatabase::OpenRS (HRecordset *pRS, TCHAR *nameDef) {
	int iRes = 0; //Success
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
    HString strSQL;

    UINT typeOpen; typeOpen = CRecordset::dynaset;
	//strSQL = _T ("SELECT * FROM ");
	/*strSQL.appendChar ('[');
	strSQL.append (nameDef);
	strSQL.appendChar (']');*/

	strSQL.format (_T ("SELECT * FROM [%S]"), nameDef);
	//strSQL.format (_T ("SELECT * FROM %S"), nameDef);

	/*switch (m_typeDriverDB) {
		case MS_ACCESS:
			typeOpen = CRecordset::dynaset;
			strSQL.appendChar ('[');
			strSQL.append (nameDef);
			strSQL.appendChar (']');
			break;
		case MS_DBASE_ODBC:
			typeOpen = CRecordset::snapshot;
			strSQL.append (nameDef);
			break;
		default:
			;
	};*/

    try { pRS->Open (typeOpen, strSQL.GetBuffer (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); }
    catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HERROR)
        strLog.format (_T ("%S"), WC_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR)
        FILELOG_ENDIF
#else
#endif
		err->Delete ();
        
        iRes = -1;
    }
    
    return iRes;
}

void HDatabase::ReleaseRS (CObject * some, HRecordset *pRefRecordset, BOOL bClose, BOOL bDelete) {
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
	HString nameDef;
    CObject *pRefObject = NULL;
    HANDLE *pRefHandle = NULL;

    if (pRefRecordset)
    {
        m_pmapDBRS.Lookup (pRefRecordset, (void *&) pRefObject);
        if (pRefObject)
        {
            if (pRefObject == some)
            {
                m_pmapHandleEventRS.Lookup (pRefRecordset, (void *&) pRefHandle);
                if (pRefHandle)
                {
                    /*strLog.Format (_T ("'WaitFor...Recordset' для 'HDatabase::GetRecordset'"));
                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_3);*/

                    //Проверим состояние СОБЫТИя
                    switch (WaitForSingleObject (*pRefHandle, 0x0))
                    {
                        case WAIT_ABANDONED:
                        case WAIT_TIMEOUT:
	                        break;
                        case WAIT_OBJECT_0:
                        default:
	                        ;
                    }
                    //Но в ЛЮБом случае ОСВОБОДим
                    SetEvent (*pRefHandle);

                    nameDef = pRefRecordset->GetNameTable ();
#if defined _HTHREAD_FILELOG
                    FILELOG_IF_WRITE (HDATABASE)
                    strLog.format (_T ("Освобождён RS: %S"), WC_HSTRING (nameDef));
	                FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE);
	                FILELOG_ENDIF
#else
#endif
                    if (bClose) {
                        pRefRecordset->Close ();
                        if (bDelete) {
                            CLOSE_HANDLE (*pRefHandle);
                            delete pRefHandle;
                            m_pmapHandleEventRS.RemoveKey (pRefRecordset);

                            m_pmapDBRS.RemoveKey (pRefRecordset);

                            FreeMemoryDBObject ((CObject *&) pRefRecordset);
#if defined _HTHREAD_FILELOG
                            FILELOG_IF_WRITE (HDATABASE)
                            strLog.format (_T ("Удалён RS: %S"), WC_HSTRING (nameDef));
                            FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HDATABASE);
                            FILELOG_ENDIF
#else
#endif
                        }
                        else
                            ; //Закрыли, НО не освободили память
                    }
                    else
                        if (bDelete)
                            ; //Как УДАЛить НЕ закрывая перед этим
                        else
                            ; //Ничего не делаем - оставляем открытым и свободным для использования RS
                }
                else
                    ; //Для RS нет события ???
            }
            else
                ; //ОбЪект передал СВОЙ RS, НО в ЭТОм 'MAPe' они НЕ РОДНые ???
        }
        else
            ; //RS есть, а обЪекта НЕТ ???
    }
    else
        ; //Передан пустой RS - УЖЕ ошибка ??? (хорошо, что 'some' не М.Б. == NULL)
        
    if (m_pmapDBRS.GetSize () == 0)
        UnlockDatabase ();
	else
		;
}

void HDatabase::LockDatabase (void)
{
    switch (WaitForSingleObject (m_hEvtDatabase, 0x0))
    {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            break;
        case WAIT_OBJECT_0:
            //Состояние DB ЗАНЯТа
            ResetEvent (m_hEvtDatabase);
        default:
            ;
    }
}

void HDatabase::UnlockDatabase (void)
{
    switch (WaitForSingleObject (m_hEvtDatabase, 0x0))
    {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            //Состояние DB свободна
            SetEvent (m_hEvtDatabase);
            break;
        case WAIT_OBJECT_0:
        default:
            ;
    }
}

int HDatabase::IsUse (void) {
    int iRes = FALSE;

    switch (WaitForSingleObject (m_hEvtDatabase, 0x0))
    {
        case WAIT_ABANDONED:
        case WAIT_TIMEOUT:
            iRes = TRUE;
            break;
        case WAIT_OBJECT_0:
            break;
        default:
            ;
    }

    return iRes;
}

void HDatabase::InfoConfigDatabase (void) {
    char state [SQL_SQLSTATE_SIZE + 1], errmessage [SQL_MAX_MESSAGE_LENGTH];
    int numrecord = 0, errcode = -1, lenerrmessage = -1;

    do {
        memset (state, 0x0, SQL_SQLSTATE_SIZE + 1);
        memset (errmessage, 0x0, SQL_MAX_MESSAGE_LENGTH);
        errcode = 0, lenerrmessage = 0;
    }
    while (SQLInstallerError (++numrecord, (DWORD *) &errcode,
#ifdef _MBCS
    (LPSTR) (CString (errmessage)).GetBuffer (), 
#else
    (LPWSTR) (CStringW (errmessage)).GetBuffer (), 
#endif
    SQL_MAX_MESSAGE_LENGTH, NULL));
}

int HDatabase::Connect (HString &pathDB, DRIVER_DB drv_db) {
    int iRes = -1;
	m_typeDriverDB = drv_db;
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
	HString strNameModule = AfxGetAppName (),
			strNewUserDSN,
			driverDB = m_arstrDriverDB [m_typeDriverDB],
            strAConnectDB, strANameServer, strANameDSN, strADirectoryDB, strANameFileDB;

    //strANameServer = "NE1150";
	strANameDSN.append (MB_HSTRING (strNameModule)); strANameDSN.append ("-main");

    //strNewUserDSN.format (_T ("DSN=UserKTS-main\0Description=DB\0Option=UserKTS"));
	//strNewUserDSN.format (_T ("DSN=%s\0DBQ=%s\\%s\0"), MB_HSTRING (strANameDSN), MB_HSTRING (strADefaultDir), MB_HSTRING (strANameFileDB));

	switch (m_typeDriverDB) {
		case MS_ACCESS:
			strADirectoryDB = MB_HSTRING (pathDB.left (pathDB.reverseFind ('\\')));
			strANameFileDB = MB_HSTRING (pathDB.right (pathDB.length () - (pathDB.reverseFind ('\\') + 1)));
			strNewUserDSN.format (_T ("DSN=%s\0%s=%s\\%s\0"), MB_HSTRING (strANameDSN), m_arstrConnect [m_typeDriverDB].GetBuffer (), MB_HSTRING (strADirectoryDB), MB_HSTRING (strANameFileDB));
			break;
		case MS_DBASE_ODBC:
			strADirectoryDB = pathDB;
			strNewUserDSN.format (_T ("DSN=%s\0%s=%s\0"), MB_HSTRING (strANameDSN), m_arstrConnect [m_typeDriverDB].GetBuffer (), MB_HSTRING (strADirectoryDB));
			break;
		default:
			;
	}

    //strNameDriver = _T ("Microsoft Access Driver (*.mdb, *.accdb)");
    //strNameDriver = _T ("Microsoft Access Driver (*.mdb)");

    int iCOUNT_ATTEMPT = 6,
        iResSQL = FALSE,
        iInstanceDSN = FALSE;
    while ((iCOUNT_ATTEMPT > 0) && (! (iRes == TRUE))) {
        //if ((iResSQL == FALSE) || (iInstanceDSN == FALSE)) {
        if (iInstanceDSN == FALSE) {
            try { iResSQL = SQLConfigDataSource (NULL, ODBC_ADD_DSN, WC_HSTRING (driverDB), WC_HSTRING (strNewUserDSN)); }
            catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("Исключение (ODBC_ADD_DSN): SQLConfigDataSource (Драйвер:=%S\nНовыйДСН:=%S)", WC_HSTRING (driverDB), WC_HSTRING (strNewUserDSN));
                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
                FILELOG_ENDIF
#else
#endif
                //Принудительно установим значение результата завершения операции
                iResSQL = FALSE;

                err->Delete ();
            }

            iInstanceDSN = iResSQL;

#if defined _HTHREAD_FILELOG
            FILELOG_IF_WRITE (HCOMMON)
            strLog.format ("Драйвер:=%S=\nНовыйДСН:=%S=", WC_HSTRING (driverDB), WC_HSTRING (strNewUserDSN));
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON);
            FILELOG_ENDIF
#else
#endif
        }
        else {
            //INFO_CONFIG_DATABASE
            //HDatabase::InfoConfigDatabase ();

#if defined _HTHREAD_FILELOG
            FILELOG_IF_WRITE (HERROR)
            strLog.format ("Декрементация числа попыток создания НовыйДСН... iCOUNT_ATTEMPT:=%i", iCOUNT_ATTEMPT);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
            FILELOG_ENDIF
#else
#endif

            iCOUNT_ATTEMPT --;
            continue;
        }

        if ((iResSQL == TRUE) && (iInstanceDSN == TRUE)) {
            //strAConnectDB.Format ("DSN=%s;DBQ=%s\\%s;StatsLog_On=Yes", strANameDSN, strADefaultDir, strANameFileDB);
            //strAConnectDB.format ("DSN=%s;DBQ=%s\\%s;", MB_HSTRING (strANameDSN), MB_HSTRING (strADefaultDir), MB_HSTRING (strANameFileDB));

            //strAConnectDB.format ("DSN=%s", MB_HSTRING (strANameDSN));

			switch (m_typeDriverDB) {
				case MS_ACCESS:
					strAConnectDB.format ("DSN=%s;%s=%s\\%s;", MB_HSTRING (strANameDSN), m_arstrConnect [m_typeDriverDB].GetBuffer (), MB_HSTRING (strADirectoryDB), MB_HSTRING (strANameFileDB));
					break;
				case MS_DBASE_ODBC:
					strAConnectDB.format ("DSN=%s;%s=%s;", MB_HSTRING (strANameDSN), m_arstrConnect [m_typeDriverDB].GetBuffer (), MB_HSTRING (strADirectoryDB));
					break;
				default:
					;
			}

#if defined _HTHREAD_FILELOG
            FILELOG_IF_WRITE (HCOMMON)
            strLog.format ("Строка соединения:=%s=", MB_HSTRING (strAConnectDB));
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON);
            FILELOG_ENDIF
#else

#endif
            try { iResSQL = OpenEx (strAConnectDB.GetBuffer (), CDatabase::noOdbcDialog); }
            catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
                FILELOG_IF_WRITE (HERROR)
                strLog.format (_T ("Исключение при попытке №%i: ОШИБКА (%S)"), (6 - iCOUNT_ATTEMPT) + 1, WC_HSTRING (HString (err->m_strError)));
                FILELOG_WRITE_WITHDT (WC_HSTRING (strLog), HERROR);
                FILELOG_ENDIF
#else
#endif

                //return -1;
                iCOUNT_ATTEMPT --;

				err->Delete ();

                //Принудительно установим значение результата завершения операции
                iResSQL = FALSE;

                Sleep (666);
            }

            iRes = iResSQL;

            try { iResSQL = SQLConfigDataSource (NULL, ODBC_REMOVE_DSN, driverDB.GetBuffer (), strNewUserDSN.GetBuffer ()); }
            catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
                FILELOG_IF_WRITE (HERROR)
                strLog.format ("Исключение (ODBC_REMOVE_DSN): SQLConfigDataSource (Драйвер:=%S\nНовыйДСН:=%S)", WC_HSTRING (driverDB), WC_HSTRING (strNewUserDSN));
                FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
                FILELOG_ENDIF
#else
#endif
                //Принудительно установим значение результата завершения операции
                iResSQL = FALSE;

                err->Delete ();
            }

            if (iResSQL == TRUE) {
                //Всё в ПОРЯДКе - временный АЛИАС удалён
                iInstanceDSN = FALSE;
            }
            else {
                //Не был удален, но м.б. его и не было ???
                //Какого рода и с каким № брошено исключение
                //iAddDSN = FALSE;

                //INFO_CONFIG_DATABASE
                //HDatabase::InfoConfigDatabase ();
            }
        }
        else
            ;
    }

    if (iRes == TRUE) {
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HCOMMON)
        strLog.format ("Открыта БД: %s\\%s", MB_HSTRING (strADirectoryDB), MB_HSTRING (strANameFileDB));
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON);
        FILELOG_ENDIF
#else
#endif

        return 0;
    }
    else
        return -1;
}

void HDatabase::Disconnect (void) {
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
    if (IsOpen ()) {    
        try { Close (); }
        catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
            strLog = HString (err->m_strError.GetBuffer ());
#else
#endif
			err->Delete ();
        }

#if defined _HTHREAD_FILELOG
        if (strLog.isUses () == 1)
            ; //strLog.format ("%s", "Ошибка при попытке закрыть БД!");
		else {
            strLog.format ("%s", "Закрыли БД!");
		}
#else
#endif
    }
	else {
#if defined _HTHREAD_FILELOG
        strLog.format ("%s", "БД УЖЕ закрыта!");
#else
#endif
	}

#if defined _HTHREAD_FILELOG
    FILELOG_IF_WRITE (HCOMMON)
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HCOMMON);
    FILELOG_ENDIF
#else
#endif
}

#define REF_HANDLE_EVENT(rs) { \
                                HANDLE *pRefHandleEvent = NULL; \
	                            m_pmapHandleEventRS.Lookup (rs, (void *&) pRefHandleEvent)); \
	                            return pRefHandleEvent; \
                            }

//А если все RS ГУРТом нападут (Set и Get VALUe) на свою БД ???
/*
int HDatabase::SetFieldValue (CRecordset *pRS, TCHAR *nameField, COleVariant data)
{
    CString strLog;
    HANDLE *pRefHandleEvent = NULL;
    
    if (pRS->IsKindOf (RUNTIME_CLASS (CRecordset)))
    {
	    m_pmapHandleEventRS.Lookup (pRS, (void *&) pRefHandleEvent);

        switch (WaitForSingleObject (*pRefHandleEvent, 0x0))
	    {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
	            if (pRS->CanUpdate ())
                {                    
                    return 0; //SUCCESS
                }
                else
                    ; // ERROR:
			    break;
		    case WAIT_OBJECT_0:
			    //ОБъЕКТ свободен ???
			    strLog.Format (_T("Записать ЗНАЧение по запросу=%s"), pRS->GetSQL ());
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
                break;
		    default:
			    ;
        }
    }
    else    
        return -1;
}

CDBVariant HDatabase::GetFieldValue (CRecordset *pRS, TCHAR *nameField)
{
    CString strLog;
    CDBVariant dbVarRes;
    HANDLE *pRefHandleEvent = NULL;
    
    if (pRS->IsKindOf (RUNTIME_CLASS (CRecordset)))
    {
	    m_pmapHandleEventRS.Lookup (pRS, (void *&) pRefHandleEvent);
        switch (WaitForSingleObject (*pRefHandleEvent, 0x0))
	    {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
		        if (pRS)
		            pRS->GetFieldValue (nameField, dbVarRes);
			    break;
		    case WAIT_OBJECT_0:
			    //ОБъЕКТ свободен ???
			    strLog.Format (_T("ЗНАЧение по запросу=%s"), pRS->GetSQL ());
                FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
                break;
		    default:
			    ;
        }
    }
    else
        ; //Не 'Recordset' ???
        
    return dbVarRes;
}
*/

HString HDatabase::QueryDatabase (CDatabase *pDB, TCHAR *strQuery) {
    HString strRes, strData;
    CDBVariant dbvarData;
    int iRes = -1;
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
    CRecordset *pRS = NULL;
    pRS = DEBUG_NEW CRecordset (pDB);
    
    if (pDB->IsOpen ()) {
        if (pRS) {
            try { pRS->Open (AFX_DB_USE_DEFAULT_TYPE, strQuery); }
            catch (CDBException *err) {
                //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
#if defined _HTHREAD_FILELOG
                FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
#else
#endif
				err->Delete ();
            }
            
            if (pRS->IsOpen ()) {
                if (pRS->GetRowsetSize ()) {
                    if (pRS->CanScroll ()) {
                        try { pRS->Move (1, SQL_FETCH_FIRST); }
                        catch (CDBException *err) {
                            //MessageBox (err->m_strError.GetBuffer (), _T("Ошибка!"), MB_ICONINFORMATION);
#if defined _HTHREAD_FILELOG
                            FILELOG_WRITE_WITHDT (err->m_strError.GetBuffer (), HERROR);
#else
#endif
							err->Delete ();
                        }
                    }
                    else
                        ;
                    
                    while (! pRS->IsEOF ()) {
						//ВНИМАНие! ТОЛЬКо ОДНа записЬ
                        pRS->GetFieldValue ((short ) 0, dbvarData);
                        if (dbvarData.m_dwType == 4)
                            ;
                        else
                            ;
                            
                        iRes = dbvarData.m_iVal;

                        pRS->MoveNext ();
                    }
                }
                else
                    ; //Нет СТРОК ???
            }
            else
                ; //RS НЕ ОТКРыт 
                
            FreeMemoryDBObject ((CObject *&) pRS);
        }
        else
            ; //Не создан RS
    }
    else
        ; //Нет соединения с БД

	return strRes;
}

BOOL FreeMemoryDBObject(CObject *&obj) {
	//CString className (obj->GetRuntimeClass ()->m_lpszClassName);
	//* obj->GetRuntimeClass ()->m_lpszClassName

	BOOL bOpen = false;
	
	if (obj)
	{
#ifdef PTR_DAO_DB
	    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoDatabase") == 0)
		    bOpen = (DYNAMIC_DOWNCAST (CDaoDatabase, obj))->IsOpen ();
        else
		    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoDatabase") == 0)
		        bOpen = (DYNAMIC_DOWNCAST (HDaoDatabase, obj))->IsOpen ();
            else
#endif
                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDatabase") == 0)
		            bOpen = (DYNAMIC_DOWNCAST (CDatabase, obj))->IsOpen ();
                else
#ifdef PTR_DB
                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabase") == 0)
		                bOpen = (DYNAMIC_DOWNCAST (HDatabase, obj))->IsOpen ();
                    else
		                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabaseSQL") == 0)
		                    bOpen = (DYNAMIC_DOWNCAST (HDatabaseSQL, obj))->IsOpen ();
                        else
#endif
                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CRecordset") == 0)
		                        bOpen = (DYNAMIC_DOWNCAST (CRecordset, obj))->IsOpen ();
                            else
#ifdef PTR_DB
		                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HRecordset") == 0)
		                            bOpen = (DYNAMIC_DOWNCAST (HRecordset, obj))->IsOpen ();
                                else
#endif
#ifdef PTR_DAO_DB
                                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoRecordset") == 0)
		                                bOpen = (DYNAMIC_DOWNCAST (CDaoRecordset, obj))->IsOpen ();
                                    else
		                                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoRecordset") == 0)
		                                    bOpen = (DYNAMIC_DOWNCAST (HDaoRecordset, obj))->IsOpen ();
                                        else
		                                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoTableDef") == 0)
		                                        bOpen = (DYNAMIC_DOWNCAST (CDaoTableDef, obj))->IsOpen ();
                                            else
		                                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoQueryDef") == 0)
		                                            bOpen = (DYNAMIC_DOWNCAST (CDaoQueryDef, obj))->IsOpen ();
                                                else
#endif
                                                    return false;

		if (bOpen) {
#ifdef PTR_DAO_DB
			if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoDatabase") == 0)
                (DYNAMIC_DOWNCAST (CDaoDatabase, obj))->Close ();
            else
		        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoDatabase") == 0)
		            (DYNAMIC_DOWNCAST (HDaoDatabase, obj))->Close ();
                else
#endif
                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDatabase") == 0)
		                (DYNAMIC_DOWNCAST (CDatabase, obj))->Close ();
                    else
#ifdef PTR_DB
		                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabase") == 0)
		                    (DYNAMIC_DOWNCAST (HDatabase, obj))->Close ();
                        else
                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabaseSQL") == 0)
		                        (DYNAMIC_DOWNCAST (HDatabaseSQL, obj))->Close ();
                            else
#endif
                                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CRecordset") == 0)
		                            (DYNAMIC_DOWNCAST (CRecordset, obj))->Close ();
                                else
#ifdef PTR_DB
		                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HRecordset") == 0)
		                                (DYNAMIC_DOWNCAST (HRecordset, obj))->Close ();
                                    else
#endif
#ifdef PTR_DAO_DB
                                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoRecordset") == 0)
		                                    (DYNAMIC_DOWNCAST (CDaoRecordset, obj))->Close ();
                                        else
		                                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoRecordset") == 0)
		                                        (DYNAMIC_DOWNCAST (HDaoRecordset, obj))->Close ();
                                            else
		                                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoTableDef") == 0)
		                                            (DYNAMIC_DOWNCAST (CDaoTableDef, obj))->Close ();
                                                else
		                                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoQueryDef") == 0)
		                                                (DYNAMIC_DOWNCAST (CDaoQueryDef, obj))->Close ();
                                                    else
#endif
                                                        return false;
		}
		
#ifdef PTR_DAO_DB
		if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoDatabase") == 0)
            delete (DYNAMIC_DOWNCAST (CDaoDatabase, obj));
        else
	        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoDatabase") == 0)
	            delete (DYNAMIC_DOWNCAST (HDaoDatabase, obj));
            else
#endif
                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDatabase") == 0)
	                delete (DYNAMIC_DOWNCAST (CDatabase, obj));
                else
#ifdef PTR_DB
	                if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabase") == 0)
	                    delete (DYNAMIC_DOWNCAST (HDatabase, obj));
                    else
                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDatabaseSQL") == 0)
	                        delete (DYNAMIC_DOWNCAST (HDatabaseSQL, obj));
                        else
#endif
                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CRecordset") == 0)
	                            delete (DYNAMIC_DOWNCAST (CRecordset, obj));
                            else
#ifdef PTR_DB
	                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HRecordset") == 0)
	                                delete (DYNAMIC_DOWNCAST (HRecordset, obj));
                                else
#endif
#ifdef PTR_DAO_DB
                                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoRecordset") == 0)
	                                    delete (DYNAMIC_DOWNCAST (CDaoRecordset, obj));
                                    else
	                                    if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "HDaoRecordset") == 0)
	                                        delete (DYNAMIC_DOWNCAST (HDaoRecordset, obj));
                                        else
	                                        if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoTableDef") == 0)
	                                            delete (DYNAMIC_DOWNCAST (CDaoTableDef, obj));
                                            else
	                                            if (strcmp ((obj->GetRuntimeClass ())->m_lpszClassName, "CDaoQueryDef") == 0)
	                                                delete (DYNAMIC_DOWNCAST (CDaoQueryDef, obj));
                                                else
#endif
                                                    return false;
		
	    obj = NULL;

		return true;
	}
	
	return false;
}