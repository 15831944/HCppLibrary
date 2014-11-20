// HDaoDatabase.cpp : implementation file
//
#include "HDaoDatabase.h"

IMPLEMENT_DYNAMIC (HDaoDatabase, CDaoDatabase)

#include "stdafx.h"
#include "UserKTS.h"
#include "HDaoDatabase.h"
#include "HDaoRecordset.h"

#ifdef _DEBUG
    extern CMemoryState g_memState;
#endif

// HDaoDatabase

HDaoDatabase::HDaoDatabase()
{
    m_hEvtDatabase = CreateEvent (NULL, true, true, NULL);
    /*
    for (ID_TABLES currentId = ID_TABLES::TABLE_TEMPLATE; currentId < ID_TABLES::TABLE_NULL; currentId ++)
    {
        m_pmapDaoDBRS [currentId] = NULL;
    }
    */
}

HDaoDatabase::~HDaoDatabase()
{
    CString strLog;
    
    strLog.Format (_T ("Деструктор БД!"));
    PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);    

    CloseHandle (m_hEvtDatabase);
    
    FreeMap ();
}

void HDaoDatabase::FreeMap (void)
{
    POSITION pos;
    CString key;
    
    void *pDest = NULL, *pRefDef = NULL;
    HANDLE *pRefHandleEvent = NULL;
    HDaoRecordset *pRefRecordset = NULL;
    
    for (pos = m_pmapDaoDBRecordset.GetStartPosition (); pos != NULL;)
    {
        m_pmapDaoDBRecordset.GetNextAssoc (pos, (void *&) pRefRecordset, pDest);
        if (pRefRecordset)
            FreeMemoryDaoObject ((CObject *&) pRefRecordset);
            //pRefRecordset->Close ();
    }
    m_pmapDaoDBRecordset.RemoveAll ();
    
    for (pos = m_pmapHandleEvent.GetStartPosition (); pos != NULL;)
    {
        m_pmapHandleEvent.GetNextAssoc (pos, (void *&) pRefRecordset, (void *&) pRefHandleEvent);
        if (pRefHandleEvent)
        {
            CloseHandle (*pRefHandleEvent);
            delete pRefHandleEvent;
        }
    }
    m_pmapHandleEvent.RemoveAll ();
    
    for (pos = m_pmapDaoDBDef.GetStartPosition(); pos != NULL;)
    {
        m_pmapDaoDBDef.GetNextAssoc(pos, key, (void *&) pRefDef);
        if (pRefDef)
            FreeMemoryDaoObject ((CObject *&) pRefDef);
    }
    m_pmapDaoDBDef.RemoveAll ();
}

#define CAST(type, obj) (type *) obj

HDaoRecordset *HDaoDatabase::GetRecordset (CObject *some, CString nameDef)
{
    CString strLog;
    HDaoRecordset *pResRecordset = NULL; 
    CObject *pRefDef = NULL;
    HANDLE *pRefHandleEvent = NULL;
    
    //CAST(pResRecordset->GetRuntimeClass ()->m_lpszClassName, pResRecordset) = NULL;
    (HDaoRecordset *) pResRecordset = NULL;
    
    m_pmapDaoDBDef.Lookup (nameDef, (void *&) pRefDef);
    if (pRefDef == NULL)
    {
        if (nameDef.Find (_T ("query")) > -1)
        {
            pRefDef = DEBUG_NEW CDaoQueryDef (this);
            (DYNAMIC_DOWNCAST (CDaoQueryDef, pRefDef))->Open (nameDef);
            (DYNAMIC_DOWNCAST (CDaoQueryDef, pRefDef))->SetParamValueNull (_T("NAME_PARAM"));
        }
        else
        {
            pRefDef = DEBUG_NEW CDaoTableDef (this);
            (DYNAMIC_DOWNCAST (CDaoTableDef, pRefDef))->Open (nameDef);
        }
        m_pmapDaoDBDef.SetAt (nameDef, (void *&) pRefDef);
        
        pResRecordset = CreateRecordset (some, pRefDef);
    }
    else
    {
        POSITION pos;
        HDaoRecordset *keyRS;
        CObject *ptrSome;
    
        HDaoRecordset *pRefRecordset = NULL;
    
        //Поиск 'RecordSet', выделенного для ЭТОГо 'some' обЪекта
        //Но в самом обЪекте 'some' такая проверка УЖЕ произведена ??? т.е. в любом случае создаЁм новый 'RecordSet'
        for (pos = m_pmapDaoDBRecordset.GetStartPosition(); pos != NULL;)
        {
            m_pmapDaoDBRecordset.GetNextAssoc(pos, (void *&) keyRS, (void *&) ptrSome);
            if (((keyRS->m_pTableDef == pRefDef) || (keyRS->m_pQueryDef == pRefDef)) && (some == ptrSome))
                break;
        }
        
        if (nameDef.Find (_T ("query")) > -1)
        {
            if (! (DYNAMIC_DOWNCAST (CDaoQueryDef, pRefDef))->IsOpen ())
            {
                (DYNAMIC_DOWNCAST (CDaoQueryDef, pRefDef))->Open (nameDef);
                (DYNAMIC_DOWNCAST (CDaoQueryDef, pRefDef))->SetParamValueNull (_T("NAME_PARAM"));
            }
            else
                ;
        }
        else
        {
            if (! (DYNAMIC_DOWNCAST (CDaoTableDef, pRefDef))->IsOpen ())
                (DYNAMIC_DOWNCAST (CDaoTableDef, pRefDef))->Open (nameDef);
            else
                ;
        }
        //ЛИШНяя проверка !!! pos == NULL = true
        if (pos == NULL)
            pResRecordset = CreateRecordset (some, pRefDef);
        else
            pResRecordset = keyRS;
    }
        
    m_pmapHandleEvent.Lookup (pResRecordset, (void *&) pRefHandleEvent);
    if (! (pRefHandleEvent == NULL))
    {//При создании 'BUSY' !!!
        switch (WaitForSingleObject (*pRefHandleEvent, 0x0))
        {
            case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
		        //ТАК и д.б. !!!
			    break;
		    case WAIT_OBJECT_0:
		        //ERROR:
		        ResetEvent (*pRefHandleEvent);
		        break;
            default:
                ;
        }
    }
    else
        ; // Event create with RS !!!
    
    return pResRecordset;
}

#define NAME_DEF(rs) rs->m_pTableDef ? rs->m_pTableDef->GetName () : rs->m_pQueryDef->GetName ()
#define DUMP_TABLE_DEF(type, obj, context) (DYNAMIC_DOWNCAST (#type, obj))->Dump (context)

HDaoRecordset *HDaoDatabase::CreateRecordset (CObject *pDest, CObject *pDef)
{
    CString strLog;
    
    POSITION pos;
    
    HANDLE *pRefHandleEvent = NULL;
    HDaoRecordset *pRefRecordset = NULL;

#ifdef _DEBUG        
    //ASSERT_VALID ((DYNAMIC_DOWNCAST (CDaoTableDef, pDef)));
    pDef->AssertValid ();
#endif

    if (pRefRecordset == NULL)
        pRefRecordset = DEBUG_NEW HDaoRecordset (this);
        
    //strLog.Format (_T ("Создан обЪект для RS: %s."), pDef->GetName ());
    //PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);

#ifdef _DEBUG
    ASSERT_VALID ((DYNAMIC_DOWNCAST (HDaoRecordset, pRefRecordset)));
    pRefRecordset->AssertValid ();
#endif

    try
    {
        if (strcmp ((pDef->GetRuntimeClass ())->m_lpszClassName, "CDaoTableDef") == 0)
        {
            if ((DYNAMIC_DOWNCAST (CDaoTableDef, pDef))->IsOpen ())
                pRefRecordset->Open (DYNAMIC_DOWNCAST (CDaoTableDef, pDef), dbOpenTable, 0); 
            else
                ; //Д.б. открывать в 'GetRecordset' ???
        }
        else
            if (strcmp ((pDef->GetRuntimeClass ())->m_lpszClassName, "CDaoQueryDef") == 0)
                if ((DYNAMIC_DOWNCAST (CDaoQueryDef, pDef))->IsOpen ())
                    ((CDaoRecordset *) pRefRecordset)->Open (DYNAMIC_DOWNCAST (CDaoQueryDef, pDef), dbOpenDynaset, 0); 
                else
                    ; //Д.б. открывать в 'GetRecordset' ???
            else
                return NULL; //ERRROR ???
    }
	catch (CDaoException *err)
	{
		strLog.Format (_T ("%s"), err->m_pErrorInfo->m_strDescription);
		//MessageBox (strLog, _T("Ошибка!"), MB_ICONINFORMATION);
        PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
        
        return NULL;
	}
    
    if (pRefRecordset)
    {
        strLog.Format (_T ("Открыт RS: %s."), NAME_DEF (pRefRecordset));
        PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
    
        m_pmapDaoDBRecordset.SetAt (pRefRecordset, pDest);
        
        pRefHandleEvent = DEBUG_NEW HANDLE;
        *pRefHandleEvent = CreateEvent (NULL, true, false, NULL);
        m_pmapHandleEvent.SetAt (pRefRecordset, pRefHandleEvent);
        
        return pRefRecordset;
    }
    else
        return NULL;
}

void HDaoDatabase::ReleaseRS (HDaoRecordset *pRefRecordset, BOOL bClose, BOOL bDelete)
{
    CString strLog, nameDef;
    HANDLE *pRefHandleEvent = NULL;
    
    if (pRefRecordset->IsKindOf (RUNTIME_CLASS (HDaoRecordset)))
    {
        strLog.Format (_T ("Освобождаем RS: %s; close= %i; delete=%i."), NAME_DEF (pRefRecordset), bClose, bDelete);
        PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);

        m_pmapHandleEvent.Lookup (pRefRecordset, (void *&) pRefHandleEvent);
        if (pRefHandleEvent)
            SetEvent (*pRefHandleEvent);
        else
        {//ERROR: Д.б. ПАРА НЕ НУЛевых указателей ИЛИ Нет события для 'Recordset' ???        
            strLog.Format (_T ("Попытка освободить RS: %s; close= %i; delete=%i. Нет события для RS."), NAME_DEF (pRefRecordset), bClose, bDelete);
            PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
        }

        nameDef = NAME_DEF (pRefRecordset);
        
        if (bClose)
        {                
            strLog.Format (_T ("Закрываем RS %s."), nameDef);
            PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
            pRefRecordset->Close ();
            
            if (bDelete)
            {
                strLog.Format (_T ("Уничтожаем RS: %s."), nameDef);
                PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
                
                m_pmapDaoDBRecordset.RemoveKey (pRefRecordset);
                
                CloseHandle (*pRefHandleEvent);
                delete pRefHandleEvent;
                m_pmapHandleEvent.RemoveKey (pRefRecordset);
                
                FreeMemoryDaoObject ((CObject *&) pRefRecordset);
            }
        }
        else
            ; //НЕЛЬЗЯ уничтожить НЕ ЗАКРывая 'Recordset' ???
    }
    else
        ; //Не 'HDaoRecordset' ???
}

int HDaoDatabase::Connect (CString &bd)
{
    CString strLog;
    
    try { Open (bd, true, false); }
	catch (CDaoException *err)
	{
	    strLog.Format (_T("%s"), err->m_pErrorInfo->m_strDescription);
	    PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
	
	    return -1;
	}

    strLog.Format (_T("%s %s"), _T("Открыта БД:"), bd);
    PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
    
    return 0;
}

void HDaoDatabase::Disconnect (void)
{    
    CString strLog;

    if (IsOpen ())
    {
        FreeMap ();
    
        //TRACE (m_pDAODatabase);
    
        try { Close (); }
        catch (CDaoException *err)
        {
            strLog.Format (_T ("%s"), err->m_pErrorInfo->m_strDescription);
        }
        strLog.Format (_T("%s"), _T("Закрыли БД!"));
    }
    else
        strLog.Format (_T("%s"), _T("БД УЖЕ закрыта!"));

    PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
    strLog.ReleaseBuffer ();
}

#define REF_HANDLE_EVENT(rs) { \
                                HANDLE *pRefHandleEvent = NULL; \
	                            m_pmapHandleEvent.Lookup (rs, (void *&) pRefHandleEvent)); \
	                            return pRefHandleEvent; \
                            }

int HDaoDatabase::SetFieldValue (HDaoRecordset *pRS, TCHAR *nameField, COleVariant data)
{
    CString strLog;
    HANDLE *pRefHandleEvent = NULL;
    
    if (pRS->IsKindOf (RUNTIME_CLASS (HDaoRecordset)))
    {
	    m_pmapHandleEvent.Lookup (pRS, (void *&) pRefHandleEvent);

        switch (WaitForSingleObject (*pRefHandleEvent, 0x0))
	    {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
	            if (pRS->CanUpdate ())
                {
                    pRS->Edit ();
                    pRS->SetFieldValue (nameField, data);
                    pRS->Update ();
                    
                    return 0; //SUCCESS
                }
                else
                    ; // ERROR:
			    break;
		    case WAIT_OBJECT_0:
			    //ОБъЕКТ свободен ???
			    strLog.Format (_T("Записать ЗНАЧение в НЕинициализированной ТАБЛице=%s по полю=%s"), NAME_DEF (pRS), nameField);
                PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
                break;
		    default:
			    ;
        }
    }
    else    
        return -1;
}

COleVariant HDaoDatabase::GetFieldValue (HDaoRecordset *pRS, TCHAR *nameField)
{
    CString strLog;
    COleVariant oleVarRes ((LONG) 0x0);
    HANDLE *pRefHandleEvent = NULL;
    
    if (pRS->IsKindOf (RUNTIME_CLASS (HDaoRecordset)))
    {
	    m_pmapHandleEvent.Lookup (pRS, (void *&) pRefHandleEvent);
        switch (WaitForSingleObject (*pRefHandleEvent, 0x0))
	    {
		    case WAIT_ABANDONED:
		    case WAIT_TIMEOUT:
		        if (pRS)
		            pRS->GetFieldValue (nameField, oleVarRes);
			    break;
		    case WAIT_OBJECT_0:
			    //ОБъЕКТ свободен ???
			    strLog.Format (_T("Прочитать ЗНАЧение в НЕинициализированной ТАБЛице=%s по полю=%s"), NAME_DEF (pRS), nameField);
                PTR_FILELOG->WriteMessageWithDT (strLog.GetBuffer (), LEVEL_INFO_2);
                break;
		    default:
			    ;
        }
    }
    else
        ; //Не 'Recordset' ???
        
    return oleVarRes;
}

// HDaoDatabase member functions