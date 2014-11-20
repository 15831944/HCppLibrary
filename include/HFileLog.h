#pragma once
#include "identifiers.h"
//#include <hstring.h>

extern BOOL IsCorrectSYSTEMTIME (SYSTEMTIME);
extern int CompareSYSTEMTIME (SYSTEMTIME , SYSTEMTIME);
                    
#define TYPE_MESSAGE(type) 1 << (type - 1)

struct MESSAGE_FILELOG {
    SYSTEMTIME stmOccurred;
    HString mes;
    DWORD idThread;
    TYPE_FILELOG_MESSAGES type;
};

class HFileLog : public CObject
{
public:
    HFileLog (void);
    HFileLog (int, DWORD32);
    ~HFileLog (void);

    void LockStream (void);
    void UnlockStream (void);

	void SetMaxCountRecord (int n) { m_iMaxCountRec = n; }
	//void SetLevelOutputInfo (int l) { m_iLevelOutputInfo = l; }
	/*HString GetHostName (void) { return m_strHostName; }
	HString GetAppDirectory (void) { return m_strAppDirectory; }
	HString GetAppName (void) { return m_strAppName; }
	CTime GetTimeStart (void) { return m_tmStart; }*/
	
	void SetFilterMessages (TYPE_FILELOG_MESSAGES );
	void SetFilterMessages (DWORD32 );

	BOOL WriteMessage (LPSTR , TYPE_FILELOG_MESSAGES);
    BOOL WriteMessage (LPWSTR , TYPE_FILELOG_MESSAGES);
    BOOL WriteMessage (HString & , TYPE_FILELOG_MESSAGES);
	BOOL WriteMessageWithDT (LPSTR , TYPE_FILELOG_MESSAGES);
    BOOL WriteMessageWithDT (LPWSTR , TYPE_FILELOG_MESSAGES);
    BOOL WriteMessageWithDT (HString & , TYPE_FILELOG_MESSAGES);
    
    int IsWrite (TYPE_FILELOG_MESSAGES);
	int IsValidHandle (void) { if (m_fLog.m_hFile == CFile::hFileNull) return 0; else return 1; }
    
protected:
    /*CTime m_tmStart;*/

private:
    CFile m_fLog;
    //CEvent m_evWriteMessage;
    HANDLE m_hEvtWriteMessage;

    int m_iMaxCountRec, m_iCurCountRec
        ; //, m_iLevelOutputInfo;
    DWORD32 m_dw32FilterMessages;

#ifdef NUMBERED_RECORD
	int m_iLengthNumberRec;
#endif
    int FindFirstRec ();
    int FindLastRec ();

	HString GetNameFileError (HString &);

    HString GetDateTime (void);
	BOOL IsNumber (LPCSTR);
	SYSTEMTIME GetSYSTEMTIMERec (LPCSTR);

    CArray <BYTE> m_arbyteContentLog;
    int DeleteAdditionalRec (void);

    BOOL WriteMessage (LPSTR);
};

//#define DECLARE_HFILELOG HFileLog *g_ptrFileLog = NULL;
//#define HThreadFileLog *g_ptrThreadFileLog = 0x0;

//#define PTR_FILELOG g_ptrFileLog

//#define EXT_DECLARE_HFILELOG extern HFileLog *g_ptrFileLog;
#define EXT_DECLARE_HFILELOG 

#define FILELOG_VARIABLE \
                    HString strLog; \
                    DWORD idCurrentThread = GetCurrentThreadId (); \
                    MESSAGE_FILELOG *ptrMsgFileLog = 0x0; /*\
                    DWORD code;*/
                    

//#define FILELOG_WRITE(mes, lev) { \
//                                    g_ptrFileLog->LockStream (); \
//                                    g_ptrFileLog->WriteMessage (mes, lev); \
//                                    g_ptrFileLog->UnlockStream (); \
//                                }
//
//#define FILELOG_WRITE_WITHDT(mes, lev) { \
//                                    g_ptrFileLog->LockStream (); \
//                                    g_ptrFileLog->WriteMessageWithDT (mes, lev); \
//                                    g_ptrFileLog->UnlockStream (); \
//                                }

//#define FILELOG_WRITE(msg, lev) { \
//                                    int LEVEL_INFO = lev; \
//                                    ((CUserKTSApp *) AfxGetApp ())->postThreadMessage (msg, &LEVEL_INFO); \
//                                }

//#define FILELOG_WRITE_WITHDT(msg, lev) { \
//                                    int LEVEL_INFO = lev; \
//                                    ((CUserKTSApp *) AfxGetApp ())->postThreadMessageWithDT (msg, &LEVEL_INFO); \
//                                }

/*#define FILELOG_MSG(msg, lev) { \*/
#define FILELOG_WRITE(msg, lev) { \
                                    /*GetExitCodeThread (PTR_THREAD_FILELOG->m_hThread, &code);*/ \
                                    /*if (code == STILL_ACTIVE) {*/ \
                                    ptrMsgFileLog = DEBUG_NEW MESSAGE_FILELOG; \
									if (AfxIsValidString (msg)) { \
										TRY { ptrMsgFileLog->mes = HString (msg); } \
										CATCH_ALL (err) { \
											/*AfxDebugBreak ();*/ \
											ptrMsgFileLog->mes = "Неизвестное сообщение - TRY_код"; THROW (err); } \
										END_CATCH_ALL \
									} \
									else ptrMsgFileLog->mes = "Неизвестное сообщение - IsValid_код"; \
									/*if (ptrMsgFileLog->mes.length () > 0) ; else AfxDebugBreak ();*/ \
                                    ptrMsgFileLog->idThread = idCurrentThread; \
                                    memset (&ptrMsgFileLog->stmOccurred, 0x0, sizeof (SYSTEMTIME)); \
                                    /*CTime::GetCurrentTime ().GetAsSystemTime (ptrMsgFileLog->stmOccurred);*/ \
                                    ptrMsgFileLog->type = lev; \
                                    ::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_APP_FILELOGMSG, (WPARAM) ptrMsgFileLog, 0); \
                                    /*} else ;*/ \
								}

/*#define FILELOG_MSG_WITHDT(msg, lev) { \*/
#define FILELOG_WRITE_WITHDT(msg, lev) { \
                                    /*GetExitCodeThread (PTR_THREAD_FILELOG->m_hThread, &code);*/ \
                                    /*if (code == STILL_ACTIVE) {*/ \
                                    ptrMsgFileLog = DEBUG_NEW MESSAGE_FILELOG; \
									if (AfxIsValidString (msg)) { \
										TRY { ptrMsgFileLog->mes = HString (msg); } \
										CATCH_ALL (err) { \
											/*AfxDebugBreak ();*/ \
											ptrMsgFileLog->mes = "Неизвестное сообщение - TRY_код"; THROW (err); } \
										END_CATCH_ALL \
									} \
									else ptrMsgFileLog->mes = "Неизвестное сообщение - IsValid_код"; \
									/*if (ptrMsgFileLog->mes.length () > 0) ; else AfxDebugBreak ()*/; \
                                    ptrMsgFileLog->idThread = idCurrentThread; \
                                    memset (&ptrMsgFileLog->stmOccurred, 0x0, sizeof (SYSTEMTIME)); \
                                    CTime::GetCurrentTime ().GetAsSystemTime (ptrMsgFileLog->stmOccurred); \
                                    ptrMsgFileLog->type = lev; \
                                    ::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_APP_FILELOGMSG_WITHDT, (WPARAM) ptrMsgFileLog, 0); \
                                    /*} else ;*/ \
								}
                                    
//#define PTR_FILELOG theApp.GetPtrThreadFileLog ()->GetPtrFileLog ()

#define FILELOG_IF_WRITE(type) if (PTR_HTHREAD_FILELOG->GetPtrFileLog ()->IsWrite (type) == 1) {
#define FILELOG_ENDIF } else ; strLog.free ();

ENUM_INCREMENT (TYPE_FILELOG_MESSAGES);
ENUM_PLUS (TYPE_FILELOG_MESSAGES);