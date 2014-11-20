#pragma once

#include <afxsock.h>            // расширения сокетов MFC

// HWinApp

class HWinApp : public CWinApp
{
	DECLARE_DYNCREATE(HWinApp)

protected:
	HWinApp();           // защищенный конструктор, используемый при динамическом создании
	virtual ~HWinApp();

public:
	virtual BOOL InitInstance ();
	virtual int ExitInstance ();

    virtual int Run () { return /*CWinApp*/CWinThread::Run (); }

	int TerminateHThread (CObject *);

    static void ReadVersion (_TCHAR * , int , const _TCHAR * , const _TCHAR * );
    static int GetProductAndVersion (HString &, HString &);
    static HString ResourceVersion (const _TCHAR * );
    
    HString GetHostName (void) { return m_strHostName; }
	HString GetAppDirectory (void) { return m_strAppDirectory; }
	HString GetAppName (void) { return m_strAppName; }
	
	CTime GetTimeStart (void) { return m_tmStart; }

// Implementation
	DECLARE_MESSAGE_MAP()
	
protected:
    HString m_strHostName;
	HString m_strAppDirectory;
	HString m_strAppName;
	
	CTime m_tmStart;
    
private:
};

#define DECLARE_WSA WSADATA g_wsaData; HOSTENT *g_pHostEnt = 0x0; HString g_strPathApp;
#define EXT_DECLARE_WSA extern WSADATA g_wsaData; extern HOSTENT *g_pHostEnt; extern HString g_strPathApp;
#define WSA_PATHAPP(val) val.setLength (MAX_PATH); GetModuleFileName (::AfxGetApp ()->m_hInstance, WC_HSTRING (val), MAX_PATH);	val.actualizeLength ();
//Initialize\implement Winsock
#define WSA_IMPLEMENT \
						if (WSAStartup (MAKEWORD(2, 2), &g_wsaData) == 0) { \
							g_pHostEnt = gethostbyname ("localhost"); \
						    \
							if (WSACleanup () != 0) { \
								delete g_pHostEnt; \
								g_pHostEnt = 0x0; \
							} \
							else WSA_PATHAPP (g_strPathApp); \
						} \
						else ;
#define WSA_FREE g_strPathApp.free ();

#define WSA_IS_INIT g_pHostEnt == 0x0 ? 0 : 1
#define WSA_HOSTNAME(val) val = HString (g_pHostEnt->h_name); val = val.left (val.find (0, '.'));
#define WSA_APPDIRECTORY(val) val = g_strPathApp.left (g_strPathApp.reverseFind ('\\') - 1);
#define WSA_APPNAME(val) val = g_strPathApp.right (g_strPathApp.length () - g_strPathApp.reverseFind ('\\'));

#define CLOSE_HANDLE(h) { \
                            if (h == NULL) \
                                ; /*return 0;*/ \
                            else { \
                                if (CloseHandle (h)) { \
                                    h = NULL; \
                                    /*return 0;*/ \
                                } \
                                else \
                                    ; /*return GetLastError ();*/  \
                            } \
                        }

#define TIMEOUT_FORCED_TERMINATE 6 /*в секундах*/
#define DELETE_HTHREAD(ptr,type) { ((type *) ptr)->Delete ();  delete ((type *) ptr); }

#define IS_RUN_REMAINING (MODE_APP & HWinAppKTS::ID_MODE_APP::MODE_LOCAL) && \
						(WORK_SECONDS > UPDATE_SECONDS) && \
						(! (WORK_SECONDS == 0))

#define FREE_PTR(ptr) if (ptr) { delete ptr; ptr = NULL; } else ;

#define MEMORY_LEAKED(msg)   memStateEnd.Checkpoint (); \
                        if (memStateDiff.Difference (memStateBegin, memStateEnd) && (memStateDiff.m_lTotalCount > 4096)) \
                        { \
                            TRACE (_T ("::OnCommandThreadDirWatch - ")); \
                            TRACE (msg); \
                            TRACE (_T (" - Memory leaked!\n")); \
                            memStateDiff.DumpStatistics(); \
                        } \
                        else \
                            ;


