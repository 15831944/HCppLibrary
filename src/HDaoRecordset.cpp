#include "stdafx.h"
#include "HDaoRecordset.h"

IMPLEMENT_DYNAMIC (HDaoRecordset, CDaoRecordset)

HDaoRecordset::HDaoRecordset(void)
{
}

HDaoRecordset::HDaoRecordset(CDaoDatabase* pDatabase) : CDaoRecordset (pDatabase)
{
}

HDaoRecordset::~HDaoRecordset(void)
{
}

void HDaoRecordset::Open (CDaoTableDef* pTableDef, int nOpenType, int nOptions)
{
	ASSERT_VALID(this);
	// Must pass valid, open TableDef
	ASSERT(pTableDef != NULL);
	if (!pTableDef->IsOpen())
		ThrowDaoException(AFX_DAO_ERROR_OBJECT_NOT_OPEN);
	m_pTableDef = pTableDef;

	// Re-Opening is invalid.
	if (IsOpen())
	{
		ASSERT(FALSE);
		return;
	}

	if (nOpenType == AFX_DAO_USE_DEFAULT_TYPE)
		m_nOpenType = m_nDefaultType;
	else
		m_nOpenType = nOpenType;

	// Cache parameters and initialize
	m_nOptions = nOptions;
	m_cbFixedLengthFields = 0;

	// Share the same database object
	m_pDatabase = m_pTableDef->m_pDatabase;

	// Open the DAO recordset (implicit MoveFirst)
	
	DAO_CHECK (m_pTableDef->m_pDAOTableDef->OpenRecordset (COleVariant ((long) m_nOpenType), COleVariant ((long) m_nOptions), &m_pDAORecordset));
    
    //AfxAssertValidObject(m_pTableDef, "HDaoRecordset", 51);
    //m_pTableDef->AssertValid ();
    //int iRes = m_pTableDef->m_pDAOTableDef->OpenRecordset(COleVariant((long)m_nOpenType), COleVariant((long)m_nOptions), &m_pDAORecordset);
		
	m_bOpen = TRUE;

	// Add the recordset to map of Open CDaoRecordsets
	m_pDatabase->m_mapRecordsets.SetAt(this, this);

	TRY
	{
		BindFields();
		GetDataAndFixupNulls();
		SetCursorAttributes();
	}
	CATCH_ALL(e)
	{
		Close();
		THROW_LAST();
	}
	END_CATCH_ALL
}

BOOL HDaoRecordset::SeekRSTemplateToItem (int id) //По 'ID' записи
{
    CString strLog;
    TCHAR nameRS [] = _T ("Template");

    if (m_pTableDef->GetName ())
    {
        MoveFirst ();
        while (! IsEOF ())
        {
            if (GetFieldValue (_T("ID")).intVal == id)
                return true;

            MoveNext ();
        }
    }
    else
        ; //Не ТАБЛица 'Template'
    //strLog.Format (_T (""));
	//PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
	
	return false;
}