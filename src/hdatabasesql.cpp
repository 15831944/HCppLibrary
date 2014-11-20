#include "stdafx.h"

#include <hdatabasesql.h>

IMPLEMENT_DYNCREATE (HDatabaseSQL, CDatabase)

EXT_DECLARE_HTHREAD_FILELOG

HDatabaseSQL::HDatabaseSQL (void) {
    m_strAConnectDB.Empty ();
}

HDatabaseSQL::~HDatabaseSQL (void) {
    m_strAConnectDB.Empty ();
    m_strAConnectDB.ReleaseBuffer ();
}

void HDatabaseSQL::InfoCallSQLFunction (SQLHANDLE h, SQLSMALLINT ht, SQLSMALLINT res) {
    FILELOG_VARIABLE
    BYTE state [SQL_SQLSTATE_SIZE + 1], errmessage [SQL_MAX_MESSAGE_LENGTH];
    int numrecord = 0, errcode = -1, lenerrmessage = -1;

    if (res == SQL_SUCCESS)
        ;
    else {
        FILELOG_IF_WRITE (HERROR)
        if (res == SQL_ERROR) {
            //strLog.Format (_T ("%s"), L"Error in " L#res L"\n");
            strLog.format ("Ошибка в дескрипторе %i", ht);
            FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        }
        else {
            numrecord = 0;
            do {
                if (numrecord) {
                    strLog.format ("%s", MB_HSTRING (HString ((char *) errmessage)));
                    FILELOG_WRITE (MB_HSTRING (strLog), HERROR);
                }
                else
                    ;

                memset (state, 0x0, SQL_SQLSTATE_SIZE + 1);
                memset (errmessage, 0x0, SQL_MAX_MESSAGE_LENGTH);
                errcode = 0, lenerrmessage = 0;
            }
            while (SQLGetDiagRecA (ht, h, ++numrecord, state, (SQLINTEGER *) &errcode, errmessage, SQL_MAX_MESSAGE_LENGTH, (SQLSMALLINT *) &lenerrmessage) == SQL_SUCCESS);
        }

        FILELOG_ENDIF
    }
}

short HDatabaseSQL::GetTimeOut (CStringA &con) {
    short shRes = -1;
    CStringA strARes;

    int iPos = con.Find ("timeout=", 0);
    if (iPos > -1) {
        iPos += strlen ("timeout=");
        while ((con.GetAt (iPos) >= '0') && (con.GetAt (iPos) <= '9'))
        //while (! (con.GetAt (iPos) == ';'))
            strARes.AppendChar (con.GetAt (iPos ++));

        con.Delete (iPos - (strlen ("timeout=") + strARes.GetLength () + 1), strlen ("timeout=") + strARes.GetLength () + 1);
        shRes = atoi (strARes.GetBuffer ());
    }
    else
        ;

    return shRes;
}

int HDatabaseSQL::Connect (CStringA con) {
    int iRes = 0; //Success
    FILELOG_VARIABLE

    FILELOG_IF_WRITE (HDEBUG) //HDATA_COMMON
    strLog.format ("Вход в 'HData::Connect': %s", MB_HSTRING (HString (con.GetBuffer ())));
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG); //HDATA_COMMON
    FILELOG_ENDIF

    SQLRETURN sqlRes;
    CStringA strAConnectDB;

    con.MakeLower (); //???

    //strAConnectDB.Format ("DSN=UserKTS-data;SERVER=%s;DATABASE=e6work;UID=sa;PWD=energ6_sa;Trusted_Connection=no;Network=DBMSSOCN;", con);
    strAConnectDB.Format ("DSN=UserKTS-data;%s;Trusted_Connection=no;", con); //Network in [DBMSSOCN, DBNPNTW]

    //Создаём DSN
    TCHAR *strNewUserDSN = _T ("DSN=UserKTS-data\0Description=DB Source UserKTS"); //\0SERVER=KTS62\0DATABASE=e6work\0UID=sa\0PWD=energ6_sa\0Network=DBMSSOCN\0StatsLog_On=Yes");
    //_Отладка
    sqlRes = SQLConfigDataSource (NULL, ODBC_ADD_DSN, _T ("SQL Server"), strNewUserDSN);
    if (! sqlRes)
        //INFO_CONFIG_DATABASE
        //HDatabase::InfoConfigDatabase ();
        ;
    else
        ;
    FILELOG_IF_WRITE (HDEBUG) //HDATA_COMMON
    strLog.format ("Строка соединения для 'OpenEx': %s", MB_HSTRING (HString (strAConnectDB.GetBuffer ())));
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG); //HDATA_COMMON
    FILELOG_ENDIF

    try { iRes = OpenEx (CString (strAConnectDB).GetBuffer (), CDatabase::noOdbcDialog); }
    catch (CDBException *err) {
        TRACE (err->m_strError);
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("HData::Connect: %s", MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HERROR);
        FILELOG_ENDIF

		err->Delete ();
    }

    if (iRes == TRUE)
        iRes = 0; //TRUE изменим на SUCCESS
    else
        iRes = -1; //FALSE изменим на ОШИБКу

    if (iRes == 0) {
#ifdef _MBCS    
        m_strAConnectDB = m_pDB->GetConnect ();
#else
        m_strAConnectDB = _WC_TO_MB (GetConnect ());
#endif

        //Установим 'TimeOut' для запросов
        short shTimeOut = -1;
        //ИзвлечЁм 'TimeOut' (ВЫРЕЖем, если есть)
        shTimeOut = GetTimeOut (con /*m_strAConnectDB*/);
        if (shTimeOut > 0)
            SetQueryTimeout (shTimeOut);
	    else
		    ;
    }
    else
        ;
    //Удаляем DSN
    if (! SQLConfigDataSource (NULL, ODBC_REMOVE_DSN, _T ("SQL Server"), strNewUserDSN))
        //INFO_CONFIG_DATABASE
        //HDatabase::InfoConfigDatabase ();
        ;
    else
        ; //Всё в ПОРЯДКе - временный АЛИАС удалён

    FILELOG_IF_WRITE (HDEBUG) //HDATA_COMMON
    strLog.format ("Выход из 'HData::Connect' с кодом %i!", iRes);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG); //HDATA_COMMON
    FILELOG_ENDIF
        
    return iRes;
}

char *HDatabaseSQL::GetParametrConnectDB (int i) {
    //char *data [] = {"SERVER", "NETWORK", "DATABASE", "UID", "PWD", "TIMEOUT", ""};
    //int i = 0;
    //while (strlen (data [i]))
    //    ptr.push_back (data [i ++]);
        
    vector <char *> data;
    data.push_back ("SERVER");
    data.push_back ("NETWORK");
    data.push_back ("DATABASE");
    data.push_back ("UID");
    data.push_back ("PWD");
    data.push_back ("TIMEOUT");
    //data.push_back ("");
    if (i < data.size ())
        return data.at (i);
    else
        return NULL;
}

CRecordset *HDatabaseSQL::GetRecordset (void) {
    CRecordset *pRSRes = 0x0;
    
    if (IsConnectDB ())
        pRSRes = DEBUG_NEW CRecordset (this);
    else
        ;

    return pRSRes;
}