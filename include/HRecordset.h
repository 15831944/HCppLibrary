#pragma once
//#include "stdafx.h"

BOOL FreeMemoryDBObject (CObject *&);

class HRecordset : public CRecordset
{
    DECLARE_DYNAMIC(HRecordset)
public:
    HRecordset(CDatabase* pDatabase = NULL);
    ~HRecordset(void);

    HString GetNameTable (void);

    int SeekRSById (int ); //По 'ID' записи
    BOOL SeekRSTemplateToItem (int ); //По 'ID' записи
    int GetMaxValue (TCHAR *);
    
    //Параметры: имя поля, его значение по которЫМ выбирается ЗАПИСь для обновления
    void AddNew (void);
	int AddNewRecordSpaced (void);
	int AddNewRecordSpaced (int );
    void Edit (TCHAR *, COleVariant &);
    BOOL Update (void);
    
    CDBVariant GetFieldValue (TCHAR *);
    void GetFieldValue (TCHAR *, CDBVariant &);
    void GetFieldValue (TCHAR *, COleDateTime &);
    //DBVT_NULL
    void GetFieldValue (TCHAR *, VARIANT_BOOL &); //DBVT_BOOL
    void GetFieldValue (TCHAR *, unsigned char &); //DBVT_UCHAR
    //void GetFieldValue (TCHAR *, short &); //DBVT_SHORT
    int GetFieldValue (TCHAR *, int &); //DBVT_SHORT, DBVT_BOOL ???
    void GetFieldValue (TCHAR *, long &); //DBVT_LONG
    void GetFieldValue (TCHAR *, float &); //DBVT_SINGLE
    void GetFieldValue (TCHAR *, double &); //DBVT_DOUBLE
    void GetFieldValue (TCHAR *, TIMESTAMP_STRUCT &); //DBVT_DATE
    int GetFieldValue (TCHAR *, HString &); //DBVT_STRING
    void GetFieldValue (TCHAR *, CString &); //DBVT_STRING
    void GetFieldValue (TCHAR *, TCHAR *); //DBVT_STRING
    void GetFieldValue (TCHAR *, char *); //DBVT_STRING
    void GetFieldValue (TCHAR *, CLongBinary *); //DBVT_BINARY
#ifdef _UNICODE
    void GetFieldValue (TCHAR *, CStringA &); //DBVT_ASTRING
#endif
#ifdef _MBCS
    void GetFieldValue (TCHAR *, CStringW &); //DBVT_WSTRING
#endif
    int SetFieldValue (TCHAR *, COleVariant &);
    int SetFieldValue (TCHAR *, COleDateTime &);
    int SetFieldValue (TCHAR *, VARIANT_BOOL);
    //int SetFieldValue (TCHAR *, short);
    int SetFieldValue (TCHAR *, int);
    int SetFieldValue (TCHAR *, long);
    int SetFieldValue (TCHAR *, TIMESTAMP_STRUCT);
    int SetFieldValue (TCHAR *, TCHAR *); //DBVT_STRING
    int SetFieldValue (TCHAR *, HString &); //DBVT_STRING
    int SetFieldValue (TCHAR *, CString &); //DBVT_STRING
    
private:
    HString m_strSQLOfChange; //Для 'AddNew' и 'Edit'
    CMapStringToPtr m_mapVariantDataOfChange;
    CMapStringToPtr m_mapVariantFilter;
    
    static HString stringHOleVariant (COleVariant &oleData);
    static CString stringMFCOleVariant (COleVariant &oleData);
};

#define COUNT_RECORD(rs,cnt) { \
								CDBVariant dbVarBookmark; rs->GetBookmark (dbVarBookmark); \
								while (! (rs->IsEOF ())) { rs->MoveNext ();	cnt ++;	} \
								pRS->SetBookmark (dbVarBookmark); \
							}