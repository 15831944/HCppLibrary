#pragma once
#include "afxdao.h"

class HDaoRecordset : public CDaoRecordset
{
    DECLARE_DYNAMIC(HDaoRecordset)
public:
    HDaoRecordset(void);
    HDaoRecordset(CDaoDatabase* pDatabase = NULL);
    ~HDaoRecordset(void);
    
    virtual void Open (CDaoTableDef* , int = dbOpenTable, int = 0);
    
    BOOL SeekRSTemplateToItem (int ); //По 'ID' записи
};
