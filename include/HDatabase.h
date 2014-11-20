#pragma once
#include "hrecordset.h"

// HDatabase command target

//Microsoft Access: Driver={Microsoft Access Driver (*.mdb)};DBQ=physical path to .mdb file
//SQL Server: DRIVER={SQL Server};SERVER=path to server
//Oracle: DRIVER={Microsoft ODBC for Oracle};SERVER=path to server
//Microsoft Excel: Driver={Microsoft Excel Driver (*.xls)};DBQ=physical path to .xls file; DriverID=278
//Microsoft Excel 97: Driver={Microsoft Excel Driver (*.xls)};DBQ=physical path to .xls file;DriverID=790
//Paradox: Driver={Microsoft Paradox Driver (*.db)};DBQ=physical path to .db file;DriverID=26
//Text: Driver={Microsoft Text Driver (*.txt;*.csv)};DefaultDir=physical path to .txt file
//Microsoft Visual FoxPro® (with a database container): Driver={Microsoft Visual FoxPro Driver};SourceType=DBC;SourceDb=physical path to .dbc file
//Microsoft Visual FoxPro (without a database container): Driver={Microsoft Visual FoxPro Driver};SourceType=DBF;SourceDb=physical path to .dbf file

class HDatabase : public CDatabase
{
    //DECLARE_DYNAMIC (HDatabase)
	DECLARE_DYNCREATE (HDatabase)
public:
	enum DRIVER_DB {NOT_DRIVERS = -1, MS_ACCESS, MS_DBASE_ODBC, DRIVER_DB_COUNT};

	HDatabase ();
	virtual ~HDatabase();
	
	int Connect (HString &, DRIVER_DB);
	void Disconnect (void);
	
	int IsUse (void);
	
	HRecordset *GetRecordset (CObject *, HString &);
	void ReleaseRS (CObject *, HRecordset *, BOOL = true, BOOL = true);
	
	//enum ID_TABLES {TABLE_TEMPLATE, TABLE_TIME, TABLE_SOURCE, TABLE_NULL};
		
	//А если все RS ГУРТом нападут на свою БД ???
	//int SetFieldValue (CRecordset *, TCHAR *, COleVariant );
	//CDBVariant GetFieldValue (CRecordset *, TCHAR *);
	
	//static CString QueryDatabase (CDatabase *, TCHAR *);
	static HString QueryDatabase (CDatabase *, TCHAR *);
	static CStringA QueryDatabaseA (char *, char *) { CStringA strARes; return strARes; }
	
	static void InfoConfigDatabase (void);
	
protected:

private:
    HANDLE m_hEvtDatabase;
    CMapPtrToPtr m_pmapDBRS;
    CMapPtrToPtr m_pmapHandleEventRS;

	DRIVER_DB m_typeDriverDB;
	CStringA m_arstrDriverDB [DRIVER_DB_COUNT];
	CStringA m_arstrConnect [DRIVER_DB_COUNT];
    
    void LockDatabase (void);
    void UnlockDatabase (void);

    void FreeMap (void);

    int OpenRS (HRecordset *, TCHAR *);
        
    CString m_strSuccessCompletedConnection;
};

#define HRECORDSET(obj) (HRecordset *) obj