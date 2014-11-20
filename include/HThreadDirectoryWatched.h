#pragma once
//#include "d:\my project's\work's\c++\include\hwinthread.h"
#include <hwinthread.h>

class HThreadDirectoryWatched : public HWinThread {
    DECLARE_DYNCREATE(HThreadDirectoryWatched)
    
protected:
    // protected constructor used by dynamic creation
	
public:
    HThreadDirectoryWatched (void);
    virtual ~HThreadDirectoryWatched (void);
    
	enum ID_EVENT {BREAK_FORCE = 0, WATCHED, DIRECTORY};
    enum ID_DATAITEM {HTREEITEM_TEMPLATE = 666, PATH_DIRECTORY_WATCHED, HANDLE_EVENT_DIRECTORY_WATCHED, HWND_RESPONSE, INT_IDTHREAD_RESPONSE};
    
    BOOL InitInstance();
	int ExitInstance();
	
	void Stop (void) { SetEvent (m_arHandlesRun [0]); }

	afx_msg void OnResumeRun (WPARAM , LPARAM );
    //int Run();
    
    BYTE *m_lpDataDirWatched;
    void *getFNI (void) { return m_lpDataDirWatched; }

protected:
    int m_iSizeDataWatched;
    
    HANDLE m_arHandlesRun [3];
    LPOVERLAPPED m_pOverlappedDir;

	DECLARE_MESSAGE_MAP()
};
