#pragma once

#if ! defined _HDATABASESQL_H_
#define _HDATABASESQL_H_

class HDatabaseSQL : public CDatabase {
    DECLARE_DYNCREATE (HDatabaseSQL)

public:
    HDatabaseSQL (void);
    virtual ~HDatabaseSQL (void);

    void InfoCallSQLFunction (SQLHANDLE h, SQLSMALLINT ht, SQLSMALLINT res);

    static char *GetParametrConnectDB (int );

    CRecordset *GetRecordset (void);
    int Connect (CStringA);
    int IsConnectDB (void) {
	    if (m_strAConnectDB.IsEmpty ())
	        return FALSE;
        else
            return TRUE;
	}

    CStringA m_strAConnectDB;

private:

protected:
    short GetTimeOut (CStringA &);
};

#define TRY_RS_OPEN(query) { \
\
                                FILELOG_VARIABLE \
                                try { iRes = (int) m_pRS->Open (CRecordset::dynaset,  CString (query).GetBuffer (), CRecordset::useBookmarks | CRecordset::noDirtyFieldCheck); } \
                                catch (CDBException *err) \
                                { \
                                    TRACE (err->m_strError); \
                                    FILELOG_IF_WRITE (HDATA_COMMON) \
                                    strLog.format ("Не выполнен запрос: %s.\n%s", MB_HSTRING (HString (query.GetBuffer ())), MB_HSTRING (HString (err->m_strError))); \
                                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_COMMON); \
                                    FILELOG_ENDIF \
                                    \
                                    err->Delete (); \
									iRes = -1; \
                                } \
                                if (iRes > -1) \
                                { \
                                    if (iRes == 1) \
                                        iRes = 0; \
                                    else \
                                        ; \
                                } \
                                else \
                                    ; \
                            }

#define TRY_RS_DATA(val)   { \
                            }
                            
#define TRY_DB_DROP(type, obj) { \
                                strAQuery.Append ("drop "); \
                                strAQuery.Append (type); \
                                strAQuery.AppendChar (0x20); \
                                strAQuery.Append (obj); \
                                try { m_pDBData->ExecuteSQL (CString (strAQuery).GetBuffer ()); } \
                                catch (CDBException *err) \
                                { \
                                    strLog.format ("Не выполнен запрос: %s\n%s", MB_HSTRING (HString (strAQuery.GetBuffer ())), MB_HSTRING (HString (err->m_strError.GetBuffer ()))); \
                                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HDATA_TORNADO); \
                                    \
                                    err->Delete (); \
                                    iRes = -1; \
                                } \
                            }

//Не использую (перенЁс в 'henergdata.h'), НО ОЧЕНЬ интересна строкА №6
#define INFO_CALL_SQLFUNCTION(plf, h, ht, res) { \
                                            if (res == SQL_SUCCESS) \
                                                ; \
                                            else { \
                                                if (res == SQL_ERROR) { \
                                                    strLog.Format (_T ("%s"), L"Error in " L#res L"\n"); \
                                                    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2); \
                                                } \
                                                else { \
                                                    numrecord = 0; \
                                                    do { \
                                                        if (numrecord) { \
                                                            strLog.Format (_T ("%s"), errmessage); \
                                                            if (numrecord == 1)FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2); \
                                                            else FILELOG_WRITE (strLog.GetBuffer (), LEVEL_INFO_2); \
                                                        } \
                                                        memset (state, 0x0, SQL_SQLSTATE_SIZE + 1); \
                                                        memset (errmessage, 0x0, SQL_MAX_MESSAGE_LENGTH); \
                                                        errcode = 0, lenerrmessage = 0; \
                                                    } \
                                                    while (SQLGetDiagRecA (ht, h, ++numrecord, state, (SQLINTEGER *) &errcode, errmessage, SQL_MAX_MESSAGE_LENGTH, (SQLSMALLINT *) &lenerrmessage) == SQL_SUCCESS); \
                                                } \
                                            } \
                                        }

#define INFO_CONFIG_DATABASE    { \
                                    numrecord = 0; \
                                    do { \
                                        memset (state, 0x0, SQL_SQLSTATE_SIZE + 1); \
                                        memset (errmessage, 0x0, SQL_MAX_MESSAGE_LENGTH); \
                                        errcode = 0, lenerrmessage = 0; \
                                    } \
                                    while (SQLInstallerError (++numrecord, (DWORD *) &errcode, (LPWSTR) errmessage, SQL_MAX_MESSAGE_LENGTH, NULL)); \
                                }

#else
#endif