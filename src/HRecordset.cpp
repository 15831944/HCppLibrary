#include "stdafx.h"
#include "HRecordset.h"

IMPLEMENT_DYNAMIC (HRecordset, CRecordset)

//#if defined PTR_DB
//#include "UserKTS.h"
//extern CUserKTSApp
//#else
//extern CWinApp
//#endif
//theApp;

extern BOOL FreeMemoryDBObject (CObject *&);

#if defined _HTHREAD_FILELOG
EXT_DECLARE_HTHREAD_FILELOG
#else
#endif

HRecordset::HRecordset (CDatabase* pDatabase) : CRecordset (pDatabase) {
}

HRecordset::~HRecordset (void) {
    POSITION pos;
    CString key;
    COleVariant *poleVarData;
    
    pos = m_mapVariantDataOfChange.GetStartPosition ();
    while (pos) {
        m_mapVariantDataOfChange.GetNextAssoc (pos, key, (void *&) poleVarData);
        delete (COleVariant *) poleVarData;
    }
    m_mapVariantDataOfChange.RemoveAll ();
    
    pos = m_mapVariantFilter.GetStartPosition ();
    while (pos) {
        m_mapVariantFilter.GetNextAssoc (pos, key, (void *&) poleVarData);
        delete (COleVariant *) poleVarData;
    }
    m_mapVariantFilter.RemoveAll ();
    
    m_strSQLOfChange.free ();
}

int HRecordset::GetMaxValue (TCHAR *nameField) {
#if defined _HTHREAD_FILELOG
    FILELOG_VARIABLE
#else
#endif
    int iRes = -1;
    HRecordset *pRS = DEBUG_NEW HRecordset (m_pDatabase);
    
    for (int i = 0; i < m_nFields; i ++)
        TRACE ((TCHAR *) m_rgFieldInfos [i].m_pvDataCache);
    
    try { pRS->Open (CRecordset::dynaset, _T ("SELECT MAX (ID) as MaxId FROM TEMPLATE")); }
    catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("%s", MB_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF
#else
#endif
		err->Delete ();
    }
    pRS->GetFieldValue (_T ("MaxId"), iRes);
    FreeMemoryDBObject ((CObject *&) pRS);
        
    return iRes;
}

//#define CRECORDSET_GETFIELDVALUE(field, data) ((CRecordset *) this)->GetFieldValue (field, data);
#define CRECORDSET_GETFIELDVALUE(field, data) ASSERT (this); TRY { CRecordset::GetFieldValue (field, data); }	CATCH_ALL (err) { THROW (err); } END_CATCH_ALL

void HRecordset::GetFieldValue (TCHAR *nameField, CDBVariant &val) {
    //((CRecordset *) this)->GetFieldValue (nameField, val);
	//TRY { CRecordset::GetFieldValue (nameField, val); }	CATCH_ALL (err) { THROW (err); } END_CATCH_ALL
	CRECORDSET_GETFIELDVALUE (nameField, val)
}

void HRecordset::GetFieldValue (TCHAR *nameField, COleDateTime &val) {
    CDBVariant dbvarData;
	CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val.SetDateTime (dbvarData.m_pdate->year, dbvarData.m_pdate->month, dbvarData.m_pdate->day, dbvarData.m_pdate->hour, dbvarData.m_pdate->minute, dbvarData.m_pdate->second);
}

CDBVariant HRecordset::GetFieldValue (TCHAR *nameField) {
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    
    switch (dbvarData.m_dwType)
    {
        case DBVT_WSTRING:
        case DBVT_ASTRING:
        case DBVT_STRING:
            break;
        default:
            ;
    }
    
    return dbvarData;
}

// BOOL == int ???
void HRecordset::GetFieldValue (TCHAR *nameField, VARIANT_BOOL &val)  //DBVT_BOOL
{
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = dbvarData.m_boolVal;
}

void HRecordset::GetFieldValue (TCHAR *nameField, unsigned char &val) {//DBVT_UCHAR
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = dbvarData.m_chVal;
}

/*
void HRecordset::GetFieldValue (TCHAR *nameField, short &val) {//DBVT_SHORT
    CDBVariant dbvarData;
    ((CRecordset *) this)->GetFieldValue (nameField, dbvarData);
    val = dbvarData.m_iVal;
}
*/

int HRecordset::GetFieldValue (TCHAR *nameField, int &val) {//DBVT_SHORT
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)

	/*FILELOG_IF_WRITE (HDEBUG)
	strLog.format ("HRecordset::GetFieldValue (параметр = %S), значение = %i", nameField, dbvarData.m_iVal);
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
	FILELOG_ENDIF*/

    val = dbvarData.m_iVal;

	return 0; //Success
}

void HRecordset::GetFieldValue (TCHAR *nameField, long &val) {//DBVT_LONG
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = dbvarData.m_lVal;
}

void HRecordset::GetFieldValue (TCHAR *nameField, float &val) {//DBVT_SINGLE
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = dbvarData.m_fltVal;
}

void HRecordset::GetFieldValue (TCHAR *nameField, double &val) //DBVT_DOUBLE
{
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = dbvarData.m_dblVal;
}

void HRecordset::GetFieldValue (TCHAR *nameField, TIMESTAMP_STRUCT &val) //DBVT_DATE
{
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val = *dbvarData.m_pdate;
}

int HRecordset::GetFieldValue (TCHAR *nameField, HString &val) {//DBVT_STRING
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif

    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)

	/*FILELOG_IF_WRITE (HRECORDSET)
	strLog.format ("HRecordset::GetFieldValue (параметр = %S)", nameField);
	FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HRECORDSET);
	FILELOG_ENDIF*/

	if (AfxIsValidString (dbvarData.m_pstring->GetBuffer ())) {
		HString::STRING_TYPE type = val.type ();
		val = HString (dbvarData.m_pstring->GetBuffer ());
		if (! (type == HString::EMPTYSTR))
			val.to (type);
		else
			;
	}
	else
		;

	return val.length ();
}

void HRecordset::GetFieldValue (TCHAR *nameField, CString &val) {//DBVT_STRING
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val.Format (_T ("%s"), dbvarData.m_pstring->GetBuffer ());
}

void HRecordset::GetFieldValue (TCHAR *nameField, TCHAR *pVal) //DBVT_STRING
{
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    if (dbvarData.m_dwType == DBVT_STRING || dbvarData.m_dwType == DBVT_WSTRING)
    {
        pVal = DEBUG_NEW TCHAR [dbvarData.m_pstring->GetLength () + 1];
        memset (pVal, 0x0, dbvarData.m_pstring->GetLength () + 1);
        memcpy (pVal, dbvarData.m_pstring->GetBuffer (), dbvarData.m_pstring->GetLength ());
    }
    else
        pVal = NULL; //Ошибка в типе данных
}

void HRecordset::GetFieldValue (TCHAR *nameField, char *pVal) {//DBVT_STRING
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    if (dbvarData.m_dwType == DBVT_STRING || dbvarData.m_dwType == DBVT_ASTRING) {
    }
    else
        pVal = NULL; //Ошибка в типе данных
}

void HRecordset::GetFieldValue (TCHAR *nameField, CLongBinary *pval) {//DBVT_BINARY
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    pval = dbvarData.m_pbinary;
}

#ifdef _UNICODE
void HRecordset::GetFieldValue (TCHAR *nameField, CStringA &val) {//DBVT_ASTRING
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val.Format ("%s", _WC_TO_MB (*dbvarData.m_pstring));
}
#endif

#ifdef _MBCS
void HRecordset::GetFieldValue (TCHAR *nameField, CStringW &val) {//DBVT_WSTRING
    CDBVariant dbvarData;
    CRECORDSET_GETFIELDVALUE (nameField, dbvarData)
    val.Format (_T ("%s"), dbvarData.m_pstring->GetBuffer ());
}
#endif

HString HRecordset::GetNameTable (void)
{
    int curPos = 0;
    HString strRes;
    
    if (IsOpen ())
    {
        strRes = this->GetSQL ().Tokenize (_T (" "), curPos).GetBuffer ();
        while (! (strRes.compareNoCase (_T ("FROM")) == 0))
            strRes = this->GetSQL ().Tokenize (_T (" "), curPos);
        //Теперь НАСТОЯЩее имя
        strRes = this->GetSQL ().Tokenize (_T (" "), curPos).GetBuffer ();
    }
    else
        ;
        
    return strRes;
}

HString HRecordset::stringHOleVariant (COleVariant &oleData) {
    HString strRes;
    COleDateTime oleDateTime;
    
    switch (oleData.vt)
    {
        case VT_BOOL:
            strRes.format ("%i", oleData.boolVal);
        case VT_I2:
            strRes.format ("%i", oleData.iVal);
        case VT_I4:
        case VT_INT:
            strRes.format ("%i", oleData.iVal);
            break;
        case VT_DATE:
            // ???
            strRes.append (_T ("'"));
            //oleDateTime = COleDateTime (oleData.date);
            strRes.append (COleDateTime (oleData.date).Format (VAR_TIMEVALUEONLY).GetBuffer ());
            strRes.append (_T ("'"));
            break;
        case VT_BSTR:
            strRes.append (_T ("'"));
            strRes.append (oleData.bstrVal);
            strRes.append (_T ("'"));
            break;
        case VT_R4:
            strRes.format ("%f", oleData.fltVal);
            break;
        case VT_R8:
            strRes.format ("%f", oleData.dblVal);
            break;   
        default:
            ;
    }
    
    return strRes;
}

CString HRecordset::stringMFCOleVariant (COleVariant &oleData) {
    CString strRes;
    COleDateTime oleDateTime;
    
    switch (oleData.vt)
    {
        case VT_BOOL:
            strRes.Format (_T ("%i"), oleData.boolVal);
        case VT_I2:
            strRes.Format (_T ("%i"), oleData.iVal);
        case VT_I4:
        case VT_INT:
            strRes.Format (_T ("%i"), oleData.iVal);
            break;
        case VT_DATE:
            // ???
            strRes.Append (_T ("'"));
            //oleDateTime = COleDateTime (oleData.date);
            strRes.Append (COleDateTime (oleData.date).Format (VAR_TIMEVALUEONLY));
            strRes.Append (_T ("'"));
            break;
        case VT_BSTR:
            strRes.Append (_T ("'"));
            strRes.Append (oleData.bstrVal);
            strRes.Append (_T ("'"));
            break;
        case VT_R4:
            strRes.Format (_T ("%f"), oleData.fltVal);
            break;
        case VT_R8:
            strRes.Format (_T ("%f"), oleData.dblVal);
            break;   
        default:
            ;
    }
    
    return strRes;
}

void HRecordset::AddNew (void) {
    m_strSQLOfChange = _T ("INSERT INTO");
}

void HRecordset::Edit (TCHAR *key, COleVariant &val) {
    COleVariant *poleVariant = DEBUG_NEW COleVariant (val);
    m_mapVariantFilter.SetAt (CString (key), poleVariant);
    
    m_strSQLOfChange = _T ("UPDATE");
}

BOOL HRecordset::Update (void) {
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
    HString strInstance, strValue, strFilter;
    POSITION pos;
    CString key;
    COleVariant *poleVarData;
    
    m_strSQLOfChange.appendChar (' ');
    m_strSQLOfChange.append (GetNameTable ());
    
    //if ((m_strSQLOfChange.FindOneOf (_T ("UPDATE")) > -1) && (m_mapVariantFilter.GetSize () > 0))
    if ((m_strSQLOfChange.find (0, _T ("UPDATE")) > -1) && (m_mapVariantFilter.GetSize () > 0)) {
        strInstance = _T ("SET");
    
        pos = m_mapVariantDataOfChange.GetStartPosition ();
        while (pos)
        {
            m_mapVariantDataOfChange.GetNextAssoc (pos, key, (void *&) poleVarData);
            
            //if (poleVarData && pos)
            if (poleVarData)
            {
                strInstance.appendChar (' ');
                strInstance.append (key.GetBuffer ());
                strInstance.append (_T ("="));
                
                strValue = stringHOleVariant (*poleVarData);
                
                strInstance.append (strValue);
            }
            
            if (pos)
                strInstance.append (_T (","));
        }
        
        m_strSQLOfChange.appendChar (' ');
        m_strSQLOfChange.append (strInstance);
        
        strFilter = _T ("WHERE");
        
        pos = m_mapVariantFilter.GetStartPosition ();
        while (pos)
        {
            m_mapVariantFilter.GetNextAssoc (pos, key, (void *&) poleVarData);
            
            //if (poleVarData && pos)
            if (poleVarData)
            {
                strFilter.appendChar (' ');
                strFilter.append (key.GetBuffer ());
                strFilter.appendChar ('=');
                
                strValue = stringHOleVariant (*poleVarData);
                
                strFilter.append (strValue);
            }
        }
        
        m_strSQLOfChange.appendChar (' ');
        m_strSQLOfChange.append (strFilter);
    }
    else
        //if ((m_strSQLOfChange.FindOneOf (_T ("INSERT")) > -1) && (m_mapVariantFilter.GetSize () == 0))
        if ((m_strSQLOfChange.find (0, _T ("INSERT")) > -1) && (m_mapVariantFilter.GetSize () == 0))
        {
            pos = m_mapVariantDataOfChange.GetStartPosition ();
            
            if (pos)
                strInstance.append (_T (" ("));
            else
                return false; //НЕТ значений НИ для ОДНОго ПОЛя (самих ПОЛей ТОЖЕ нет)
            
            while (pos)
            {
                m_mapVariantDataOfChange.GetNextAssoc (pos, key, (void *&) poleVarData);
                
                if (poleVarData)
                    strInstance.append (key.GetBuffer ());
                    
                if (pos)
                    strInstance.appendChar (',');
            }
            
            strInstance.append (_T (") VALUES ("));
            
            //Оба 'while' Можно Было Бы СОВМЕСТИТь
            pos = m_mapVariantDataOfChange.GetStartPosition ();
            while (pos)
            {
                m_mapVariantDataOfChange.GetNextAssoc (pos, key, (void *&) poleVarData);
                
                if (poleVarData)
                    strInstance.append (stringHOleVariant (*poleVarData));
                
                if (pos)
                    strInstance.appendChar (',');
            }
            
            strInstance.appendChar (')');
            
            m_strSQLOfChange.appendChar (' ');
            m_strSQLOfChange.append (strInstance);
        }
        else
            return false;

    try { this->m_pDatabase->ExecuteSQL (m_strSQLOfChange.GetBuffer ()); }
    catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HERROR)
        strLog.format (_T ("%S"), WC_HSTRING (HString (err->m_strError)));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR)
        FILELOG_ENDIF
        strLog.free ();
#else
#endif

		err->Delete ();
        //throw (err->m_strError);
    }
    
    pos = m_mapVariantDataOfChange.GetStartPosition ();
    while (pos)
    {
        m_mapVariantDataOfChange.GetNextAssoc (pos, key, (void *&) poleVarData);
        delete (COleVariant *) poleVarData;
    }
    m_mapVariantDataOfChange.RemoveAll ();
    
    pos = m_mapVariantFilter.GetStartPosition ();
    while (pos)
    {
        m_mapVariantFilter.GetNextAssoc (pos, key, (void *&) poleVarData);
        delete (COleVariant *) poleVarData;
    }
    m_mapVariantFilter.RemoveAll ();
    
    m_strSQLOfChange.free ();
    
    try { Requery (); }
    catch (CDBException *err) {
#if defined _HTHREAD_FILELOG
        FILELOG_IF_WRITE (HERROR)
        strLog.format (_T ("%S"), WC_HSTRING (HString (err->m_strError.GetBuffer ())));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR)
        FILELOG_ENDIF
        strLog.free ();
#else
#endif        
        err->Delete ();
		//throw (err->m_strError);
    }
    
    return true; //Success
}

int HRecordset::SetFieldValue (TCHAR *nameField, COleVariant &val)
{
    COleVariant *poleVariant = NULL;
    if (m_mapVariantDataOfChange.Lookup (nameField, (void *&) poleVariant))
        if (poleVariant)
        {
            delete poleVariant;
            m_mapVariantDataOfChange.RemoveKey (nameField);
        }
        
    poleVariant = DEBUG_NEW COleVariant (val);
    m_mapVariantDataOfChange.SetAt (CString (nameField), poleVariant);
    
    return m_mapVariantDataOfChange.GetSize ();
}

int HRecordset::SetFieldValue (TCHAR *nameField, VARIANT_BOOL val)
{
    COleVariant oleData;
    oleData = val;
    return SetFieldValue (nameField, oleData);
}

int HRecordset::SetFieldValue (TCHAR *nameField, int val)
{
    COleVariant oleData;
    oleData = (long) val;
    return SetFieldValue (nameField, oleData);
}
/*
int HRecordset::SetFieldValue (TCHAR *nameField, short val)
{
    COleVariant oleData;
    oleData = val;
    return SetFieldValue (nameField, oleData);
}
*/
int HRecordset::SetFieldValue (TCHAR *nameField, long val)
{
    COleVariant oleData;
    oleData = val;
    return SetFieldValue (nameField, oleData);
}

int HRecordset::SetFieldValue (TCHAR *nameField, COleDateTime &val)
{
    COleVariant *poleVariant = DEBUG_NEW COleVariant (val);
    m_mapVariantDataOfChange.SetAt (CString (nameField), poleVariant);
    
    return m_mapVariantDataOfChange.GetSize ();
}

int HRecordset::SetFieldValue (TCHAR *nameField, TIMESTAMP_STRUCT val)
{
    COleVariant oleData;
    //oleData.Attach ((COleVariant &) val);
    CTime tm (val.year, val.month, val.day, val.hour, val.minute, val.second);
    oleData.Detach ();
    oleData.Clear ();
    oleData.ChangeType (VT_DATE);
    oleData.date = tm.GetTime ();
    return SetFieldValue (nameField, oleData);
}

int HRecordset::SetFieldValue (TCHAR *nameField, TCHAR *val) //DBVT_STRING
{
    COleVariant oleData;
    oleData = val;
    return SetFieldValue (nameField, oleData);
}

int HRecordset::SetFieldValue (TCHAR *nameField, HString &val) //DBVT_STRING
{
    COleVariant oleData;
	CString strVal (val.GetBuffer ());
    oleData = strVal;
    return SetFieldValue (nameField, oleData);
}

int HRecordset::SetFieldValue (TCHAR *nameField, CString &val) //DBVT_STRING
{
    COleVariant oleData;
    oleData = val;
    return SetFieldValue (nameField, oleData);
}

//По 'ID' записи
BOOL HRecordset::SeekRSTemplateToItem (int id) {
    CString strLog;
    //TCHAR nameRS [] = _T (TABLENAME_TEMPLATE);
    int dbId;

    ASSERT (this);
    
    MoveFirst ();
    while (! IsEOF ()) {
        GetFieldValue (_T("ID"), dbId);
        if (dbId == id)
            return true;
        else
            ;

        MoveNext ();
    }
    
    //strLog.Format (_T (""));
	//FILELOG_WRITE_WITHDT (strLog.GetBuffer (), LEVEL_INFO_2);
	
	return false;
}

int HRecordset::SeekRSById (int ) { //По 'ID' записи
    return 0;
}

int HRecordset::AddNewRecordSpaced (void) {
	int i = -1, recCount = 0 /*GetRowsetSize ()*/ /*GetRecordCount ()*/, fieldCount = /*CRecordset::GetBoundFieldIndex ()*/GetODBCFieldCount ();
	HString strTemp;
	COleVariant data;
	CODBCFieldInfo odbcFileInfo;

	AddNew ();

	for (i = 0; i < fieldCount; i ++) {
		GetODBCFieldInfo (i, odbcFileInfo);

		//if (odbcFileInfo.m_nSQLType == строка)
		strTemp.setLength (odbcFileInfo.m_nPrecision, 0x20);
		data = (COleVariant) CString (strTemp.GetBuffer ());
		SetFieldValue (odbcFileInfo.m_strName.GetBuffer (), data);
	}

	Update ();

	return recCount;
}

int HRecordset::AddNewRecordSpaced (int num) {
	int i = -1, recCount = num /*GetRowsetSize ()*/ /*GetRecordCount ()*/, fieldCount = /*CRecordset::GetBoundFieldIndex ()*/GetODBCFieldCount ();
	HString strTemp;
	COleVariant data;
	CODBCFieldInfo odbcFileInfo;

	AddNew ();

	data = (COleVariant ) (LONG) (++ recCount);
	SetFieldValue (_T ("NUMBER_LINE"), data);

	for (i = 0; i < fieldCount; i ++) {
		GetODBCFieldInfo (i, odbcFileInfo);
		if (! (odbcFileInfo.m_strName.CompareNoCase (_T ("NUMBER_LINE")) == 0)) {
			strTemp.setLength (odbcFileInfo.m_nPrecision, 0x20);
			data = (COleVariant) CString (strTemp.GetBuffer ());
			SetFieldValue (odbcFileInfo.m_strName.GetBuffer (), data);
		}
		else
			;
	}

	Update ();

	return recCount;
}