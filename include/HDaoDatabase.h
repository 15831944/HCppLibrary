#pragma once
#include <afxdb.h>
#include "stdafx.h"
#include "HDaoRecordset.h"

// HDaoDatabase command target

extern BOOL FreeMemoryDaoObject (CObject *&);

class HDaoDatabase : public CDaoDatabase
{
    DECLARE_DYNAMIC (HDaoDatabase)
public:
	HDaoDatabase();
	virtual ~HDaoDatabase();
	
	int Connect (CString &);
	void FreeMap (void);
	void Disconnect (void);
	
	void ReleaseRS (HDaoRecordset *, BOOL = true, BOOL = false);
	
	//enum ID_TABLES {TABLE_TEMPLATE, TABLE_TIME, TABLE_SOURCE, TABLE_NULL};
	
	HDaoRecordset *GetRecordset (CObject *, CString );
	
	int SetFieldValue (HDaoRecordset *, TCHAR *, COleVariant );
	COleVariant GetFieldValue (HDaoRecordset *, TCHAR *);
	
	static void InfoConfigDatabase ()
	{
	    CString strLog;
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
	
protected:

private:
    HANDLE m_hEvtDatabase;
    void LockDatabase (void) { ResetEvent (m_hEvtDatabase); }
    
    CMapStringToPtr m_pmapDaoDBDef;
    CMapPtrToPtr m_pmapDaoDBRecordset;
    
    CMapPtrToPtr m_pmapHandleEvent;

    HDaoRecordset *CreateRecordset (CObject *, CObject *);
};

#define RECORDSET(obj) (HDaoRecordset *) obj

