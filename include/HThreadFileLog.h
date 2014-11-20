#pragma once
//#include "stdafx.h"
#include <hwinthread.h>
#include <hfilelog.h>

using namespace std;

#define _HTHREAD_FILELOG

#define WM_APP_MAXCOUNTRECORD       WM_APP + 679 //Контроль за значениями в 'stdafx.h'
#define WM_APP_LEVELOUTPUTINFO      WM_APP + 680
#define WM_APP_FILELOGMSG           WM_APP + 681 
#define WM_APP_FILELOGMSG_WITHDT    WM_APP + 682

class HThreadFileLog : public HWinThread //, public HFileLog
{
    DECLARE_DYNCREATE (HThreadFileLog)
public:
    HThreadFileLog (void);
    virtual ~HThreadFileLog (void);

    BOOL InitInstance();
	int ExitInstance();

	afx_msg void OnResumeRun (WPARAM , LPARAM ) {}
	//int Run();
    void Stop (void);

    //BOOL PreTranslateMessage (MSG *);

    inline HFileLog *GetPtrFileLog (void) { return m_ptrFileLog; }
	int IsValidHandleFileLog (void) { if (m_ptrFileLog) return m_ptrFileLog->IsValidHandle (); else return 0; }

    afx_msg void OnCommandWriteFileLog (WPARAM , LPARAM );
    afx_msg void OnCommandWriteWithDTFileLog (WPARAM , LPARAM );
    afx_msg void OnCommandMaxCountRecord (WPARAM , LPARAM );
    afx_msg void OnCommandLevelOutputInfo (WPARAM , LPARAM );
    
    int LockStreamMessageFileLog (DWORD = INFINITE);
    void UnlockStreamMessageFileLog (void);
    
    int push_backMessageFileLog (MESSAGE_FILELOG *);
    
protected:
	DECLARE_MESSAGE_MAP()

private:
    HFileLog *m_ptrFileLog;
    HANDLE m_hEventStop;
    HANDLE m_hEventMessageFileLog;
    
    int IsMessageFileLogWithDT (SYSTEMTIME &stm) { if ((stm.wYear == 0) && (stm.wMonth == 0) && (stm.wDay == 0) && (stm.wHour == 0) && (stm.wMinute == 0) && (stm.wSecond == 0) && (stm.wMilliseconds == 0)) return 0; else return 1; }
    
    int WriteMessages (void);
    //vector <MESSAGE_FILELOG *> m_vecMessageFileLog;
    CPtrArray m_vecMessageFileLog;
};

#define DECLARE_HTHREAD_FILELOG HThreadFileLog *g_ptrThreadFileLog = 0x0;
#define EXT_DECLARE_HTHREAD_FILELOG extern HThreadFileLog *g_ptrThreadFileLog;
#define PTR_HTHREAD_FILELOG g_ptrThreadFileLog

#define IMPLEMENT_HTHREAD_FILELOG { \
									PTR_HTHREAD_FILELOG = DEBUG_NEW HThreadFileLog (); \
									if (PTR_HTHREAD_FILELOG->IsValidHandleFileLog ()) { \
										if (PTR_HTHREAD_FILELOG->CreateThread (CREATE_SUSPENDED)) { \
											if (! (PTR_HTHREAD_FILELOG->ResumeThread () == (DWORD) -1)) { \
												FILELOG_WRITE ("Старт потока записи ЛОГ-файла...", HCOMMON); \
											} else ; \
										} else ; \
									} else ; \
								}

#define IS_INIT_HTHREAD_FILELOG (GetExitCodeThread (PTR_HTHREAD_FILELOG->m_hThread, &PTR_HTHREAD_FILELOG->m_idExitCodeThread)) && (PTR_HTHREAD_FILELOG->m_idExitCodeThread == STILL_ACTIVE) && (PTR_HTHREAD_FILELOG->IsValidHandleFileLog ())

#define DELETE_HTHREAD_FILELOG { \
									if (PTR_HTHREAD_FILELOG) { \
										PTR_HTHREAD_FILELOG->Stop (); \
										::PostThreadMessage (PTR_HTHREAD_FILELOG->m_nThreadID, WM_QUIT, 0, 0); \
										Sleep (666); \
 \
										CTime tmEntry = CTime::GetCurrentTime (); \
										GetExitCodeThread (PTR_HTHREAD_FILELOG->m_hThread, &PTR_HTHREAD_FILELOG->m_idExitCodeThread); \
 \
										while (PTR_HTHREAD_FILELOG->m_idExitCodeThread == STILL_ACTIVE) { \
											if ((CTime::GetCurrentTime () - tmEntry).GetTotalSeconds () > TIMEOUT_FORCED_TERMINATE) { \
												int iRes = TerminateThread (PTR_HTHREAD_FILELOG->m_hThread, WAIT_ABANDONED); \
 \
												if (iRes == 1) PTR_HTHREAD_FILELOG->m_idExitCodeThread = WAIT_IO_COMPLETION; \
												else PTR_HTHREAD_FILELOG->m_idExitCodeThread = WAIT_ABANDONED; \
 \
												break; \
											} \
											else Sleep (666); \
 \
											GetExitCodeThread (PTR_HTHREAD_FILELOG->m_hThread, &PTR_HTHREAD_FILELOG->m_idExitCodeThread); \
										} \
 \
										DELETE_HTHREAD (PTR_HTHREAD_FILELOG, HThreadFileLog) \
									} else ; \
								}
