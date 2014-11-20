#include "stdafx.h"

EXT_DECLARE_HTHREAD_FILELOG

#if (defined _DEBUG) && (defined USER_KTS)
extern int g_iCountFileInfo;
#endif

extern BOOL IsNumber(BYTE );

HFileInfo::HFileInfo (char *AFileName) {
    FILELOG_VARIABLE

    InitializePtr ();

	LPWIN32_FIND_DATA pwfd = DEBUG_NEW WIN32_FIND_DATA;

	HANDLE hFindFile = FindFirstFile (CString (AFileName).GetBuffer (), pwfd);
    if (hFindFile != INVALID_HANDLE_VALUE)
		SetInfo (hFindFile, AFileName, pwfd);
	else {
	    CStringA strATmp;
	    FILELOG_IF_WRITE (HERROR)
        strATmp.Format ("Конструктор 'HFileInfo (char *)'. Файл %s не НАЙДЕН!", AFileName);
        FILELOG_WRITE_WITHDT (strATmp.GetBuffer (), HERROR);
        FILELOG_ENDIF
        strATmp.Empty ();

        //FindClose (hFindFile);
        //delete pwfd;
        //
        //return ;
	}

	FindClose (hFindFile);
	delete pwfd;
}

HFileInfo::HFileInfo (WCHAR *FileName) {
    FILELOG_VARIABLE

    InitializePtr ();

	LPWIN32_FIND_DATA pwfd = DEBUG_NEW WIN32_FIND_DATA;

	HANDLE hFindFile = FindFirstFile (CString (FileName).GetBuffer (), pwfd);
    if (hFindFile != INVALID_HANDLE_VALUE)
		SetInfo (hFindFile, FileName, pwfd);
	else {
	    FILELOG_IF_WRITE (HERROR)
        strLog.format ("Конструктор 'HFileInfo (WCHAR *)'. Файл %s не НАЙДЕН!", MB_HSTRING (HString (FileName)));
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF

        //FindClose (hFindFile);
        //delete pwfd;
        //
        //return ;
	}

	FindClose (hFindFile);
	delete pwfd;
}

HFileInfo::HFileInfo (HANDLE hndl, char *AFileName, WIN32_FIND_DATA *wfd)
{
    FILELOG_VARIABLE

    InitializePtr ();

	if (hndl != INVALID_HANDLE_VALUE )
		SetInfo (hndl, AFileName, wfd);
	else {
        FILELOG_IF_WRITE (HERROR)
        strLog.format ("Конструктор 'HFileInfo (HANDLE, char *)'. Описатель для файла %s =%i!", MB_HSTRING (HString (AFileName)), hndl);
        FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HERROR);
        FILELOG_ENDIF
        
        //return ;
	}
}

void HFileInfo::InitializePtr (void)
{
    m_pstFD = NULL;
    m_pstFAD = NULL;
	m_pstFI = NULL;
	m_hFile = NULL;
}

void HFileInfo::SetInfo (HANDLE hndl, char *AFileName, WIN32_FIND_DATA *wfd)
{
	GET_FILEEX_INFO_LEVELS *fInfoLevelId = DEBUG_NEW GET_FILEEX_INFO_LEVELS;
	
	m_pstFD = DEBUG_NEW WIN32_FIND_DATA;
	m_pstFAD = DEBUG_NEW WIN32_FILE_ATTRIBUTE_DATA;
	m_pstFI = DEBUG_NEW BY_HANDLE_FILE_INFORMATION;
#if (defined _DEBUG) && (defined USER_KTS)
	g_iCountFileInfo ++;
#endif
	char pPath [MAX_PATH];
	memset (pPath, 0x0, MAX_PATH);
	memcpy (pPath, AFileName, strlen (AFileName));
	m_strFileName.free ();

	//Обрезаем
	int i, iTmp;
	for (i = 0; i < MAX_PATH; i ++)
    {
        if (pPath [i] == '\x0')
        {
            iTmp = i - 1;
            break;
        }
    }
	for (iTmp; iTmp > 0; iTmp --)
	{
		pPath [iTmp] = '\x0';
		if (pPath [iTmp - 1] == '\\')
		{
			break;
		}
	}
	for (i = 0; i < MAX_PATH; i ++)
    {
        pPath [iTmp + i] = wfd->cFileName [i];
        if (wfd->cFileName [i] == '\x0')
            break;
    }
	//memcpy (m_strFileName.GetBuffer (MAX_PATH), pPath, MAX_PATH);
	m_strFileName.init (pPath);
	/*
	m_hFile = CreateFile	(m_strFileName.GetBuffer(),	//LPCTSTR lpFileName,
							FILE_ALL_ACCESS,			//DWORD dwDesiredAccess, /GENERIC_READ/
							FILE_SHARE_READ,			//DWORD dwShareMode,
							NULL,						//LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							OPEN_EXISTING,				//DWORD dwCreationDisposition,
							FILE_ATTRIBUTE_NORMAL,		//DWORD dwFlagsAndAttributes,
							NULL						//HANDLE hTemplateFile
							);
    */
	m_hFile = hndl;
	
	memcpy (m_pstFD, wfd, sizeof (WIN32_FIND_DATA));
	
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
	}
	else
	{
		GetFileInformationByHandle (m_hFile, m_pstFI);
		GetFileAttributesEx (WC_HSTRING (m_strFileName), *fInfoLevelId, m_pstFAD);
	}
	
	delete fInfoLevelId;
}

void HFileInfo::SetInfo (HANDLE hndl, WCHAR *FileName, WIN32_FIND_DATA *wfd)
{
	CStringA AFileName = CStringA (FileName);
	
	SetInfo (hndl, AFileName.GetBuffer (), wfd);
}

HFileInfo::~HFileInfo ()
{
    delete m_pstFD;
    delete m_pstFAD;
    delete m_pstFI;
    
	m_strFileName.free ();
#if (defined _DEBUG) && (defined USER_KTS)
	g_iCountFileInfo --;
#endif
}

void HFileInfo::CopyOf (HFileInfo *src)
{
    memcpy (m_pstFD, src->m_pstFD, sizeof (WIN32_FIND_DATA));
    memcpy (m_pstFAD, src->m_pstFAD, sizeof (WIN32_FILE_ATTRIBUTE_DATA));
	memcpy (m_pstFI, src->m_pstFI, sizeof (BY_HANDLE_FILE_INFORMATION));
	m_hFile = src->m_hFile;
    memcpy (&fVerBlk, &src->fVerBlk, sizeof (VS_FIXEDFILEINFO));
    m_strFileName = src->m_strFileName;
}

void HFileInfo::CopyTo (HFileInfo *dest)
{
    memcpy (dest->m_pstFD, m_pstFD, sizeof (LPWIN32_FIND_DATA));
    memcpy (dest->m_pstFAD, m_pstFAD, sizeof (LPWIN32_FILE_ATTRIBUTE_DATA));
	memcpy (dest->m_pstFI, m_pstFI, sizeof (LPBY_HANDLE_FILE_INFORMATION));
	dest->m_hFile = m_hFile;
    memcpy (&dest->fVerBlk, &fVerBlk, sizeof (VS_FIXEDFILEINFO));
    dest->m_strFileName = m_strFileName;
}

BOOL HFileInfo::GetFileVersion()
{
	DWORD InfoSize, puLen;
	LPVOID Pt, InfoPtr;

	InfoSize = GetFileVersionInfoSize (WC_HSTRING (m_strFileName), &puLen );
	ZeroMemory( &fVerBlk, sizeof(VS_FIXEDFILEINFO));
	if (InfoSize > 0)
	{
		Pt = malloc (InfoSize);
		GetFileVersionInfo (WC_HSTRING (m_strFileName), 0, InfoSize, Pt);
		VerQueryValue(Pt, _T("\\"), (LPVOID *)&InfoPtr, (UINT *)&puLen);
		MoveMemory (InfoPtr, &fVerBlk, sizeof (VS_FIXEDFILEINFO));
		free (Pt);
		return true;
	}
	else
	{
		return false;
	}
}

char * HFileInfo::VersionString ()
{	
	CString strTemp;
	if (GetFileVersion ())
	{
		strTemp.Format(_T("%u.%u.%u.%u"),
						HIWORD(fVerBlk.dwProductVersionMS),
						LOWORD(fVerBlk.dwProductVersionMS),
						HIWORD(fVerBlk.dwProductVersionLS),
						LOWORD(fVerBlk.dwProductVersionLS));
	}
	else
	{
		strTemp = "";
	}

	return (char *)strTemp.GetBuffer ();
}

BOOL HFileInfo::isDebugBuild ()
{
	BOOL result = FALSE;
	if (GetFileVersion ())
	{
		if ((fVerBlk.dwFileFlagsMask && fVerBlk.dwFileFlags && VS_FF_DEBUG) != 0)
			result = TRUE;
	}
	return result;
}

BOOL HFileInfo::isPreRelease ()
{
	BOOL result = FALSE;
	if (GetFileVersion ())
	{
		if ((fVerBlk.dwFileFlagsMask && fVerBlk.dwFileFlags && VS_FF_PRERELEASE) != 0)
			result = TRUE;
	}
	return result;
}

BOOL HFileInfo::isPrivateBuild ()
{
	BOOL result = FALSE;
	if (GetFileVersion ())
	{
		if ((fVerBlk.dwFileFlagsMask && fVerBlk.dwFileFlags && VS_FF_PRIVATEBUILD) != 0)
			result = TRUE;
	}
	return result;
}

BOOL HFileInfo::isSpecialBuild ()
{
	BOOL result = FALSE;
	if (GetFileVersion ())
	{
		if ((fVerBlk.dwFileFlagsMask && fVerBlk.dwFileFlags && VS_FF_SPECIALBUILD) != 0)
			result = TRUE;
	}
	return result;
}

char * HFileInfo::OS ()
{
	char *result = DEBUG_NEW char [256];
	if (GetFileVersion ())
	{
		switch (fVerBlk.dwFileOS)
		{
			case VOS_DOS_WINDOWS16:	result = "MS-DOS or 16 bit Windows"; break;
			case VOS_DOS_WINDOWS32:	result = "32 bit Windows"; break;
			case VOS_OS216_PM16: result = "16 bit OS/2"; break;
			case VOS_OS232_PM32: result = "32 bit OS/2"; break;
			case VOS_NT_WINDOWS32: result = "Win32 or Windows NT"; break;
			default: result = "Unknown OS";
		}
	}
	else
		result = "";

	return result;
}

char * HFileInfo::FileType ()
{
	char *result = DEBUG_NEW char [256];
	if (GetFileVersion ())
	{
		switch (fVerBlk.dwFileType)
		{
			case VFT_APP: result = "Application"; break;
			case VFT_DLL: result = "Dynamic Link Library (DLL)"; break;
			case VFT_DRV: result = "Device Driver - ";
				switch (fVerBlk.dwFileSubtype)
				{
					case VFT2_DRV_PRINTER: strcpy_s (result, strlen ("Printer"), "Printer"); break;
					case VFT2_DRV_KEYBOARD:	strcpy_s (result, strlen ("Keyboard"), "Keyboard"); break;
					case VFT2_DRV_LANGUAGE:	strcpy_s (result, strlen ("Language"), "Language"); break;
					case VFT2_DRV_DISPLAY: strcpy_s (result, strlen ("Display"), "Display"); break;
					case VFT2_DRV_MOUSE: strcpy_s (result, strlen ("Mouse"), "Mouse"); break;
					case VFT2_DRV_NETWORK: strcpy_s (result, strlen ("Network"), "Network"); break;
					case VFT2_DRV_SYSTEM: strcpy_s (result, strlen ("System"), "System"); break;
					case VFT2_DRV_INSTALLABLE: strcpy_s (result, strlen ("Installable"), "Installable"); break;
					case VFT2_DRV_SOUND: strcpy_s (result, strlen ("Sound"), "Sound"); break;
					default:
						strcpy_s (result, strlen ("Unknown"), "Unknown");
				}
				break;
			case VFT_FONT: result = "Font File - ";
				switch (fVerBlk.dwFileSubtype)
				{
					case VFT2_FONT_RASTER: strcpy_s (result, strlen ("Raster"), "Raster"); break;
					case VFT2_FONT_VECTOR: strcpy_s (result, strlen ("Vector"), "Vector"); break;
					case VFT2_FONT_TRUETYPE: strcpy_s (result, strlen ("TrueType"), "TrueType"); break;
					default:
						strcpy (result, "Unknown");
				}
				break;
			case VFT_VXD: result = "Virtual Device"; break;
			case VFT_STATIC_LIB: result = "Static Link Library"; break;
			default:
				result = "Unknown File Type";
		}
	}

	return result;
}

int HFileInfo::Path (HString &str)
{
	int iPosFileName = m_strFileName.reverseFind ('\\'), i;
	iPosFileName ++;
	if (iPosFileName)
	{
	    i = 0;
        while (i < iPosFileName)
            str.appendChar (WC_AT (m_strFileName, i ++));
    	
	    return iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::Path (char *dest)
{
    int iPosFileName = m_strFileName.reverseFind ('\\');
	iPosFileName ++;
	if (iPosFileName)
	{
	    memcpy (dest, WC_HSTRING (m_strFileName), iPosFileName);
    	
	    * (dest + (iPosFileName)) = 0x0;
	    return iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::FileName (HString &str) // Имя файла
{
    int iPosFileName = m_strFileName.reverseFind ('\\'), i;
	iPosFileName ++;
	if (iPosFileName)
	{
	    i = iPosFileName;
        while (i < m_strFileName.length ())
            str.appendChar (WC_AT (m_strFileName, i ++));
        
        return m_strFileName.length () - iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::FileName (char *dest) // Имя файла
{
    int iPosFileName = m_strFileName.reverseFind ('\\');
	iPosFileName ++;
	if (iPosFileName)
	{
        memcpy (dest, WC_HSTRING (m_strFileName) + iPosFileName, m_strFileName.length () - iPosFileName);
        
        * (dest + (m_strFileName.length () - iPosFileName)) = 0x0;
        return m_strFileName.length () - iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::name (HString &str)
{
	int iPosFileName = m_strFileName.reverseFind ('\\'), iPosExt = m_strFileName.reverseFind ('.'), i;
	iPosFileName ++;
	if (iPosFileName)
	{
	    i = iPosFileName;
	    while (i < m_strFileName.length () - iPosExt)
	        str.appendChar (WC_AT (m_strFileName, i ++));
        
        return iPosExt - iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::name (char *dest)
{
	int iPosFileName = m_strFileName.reverseFind ('\\'), iPosExt = m_strFileName.reverseFind ('.');
	iPosFileName ++;
	if (iPosFileName)
	{
        memcpy (dest, WC_HSTRING (m_strFileName) + iPosFileName, iPosExt - iPosFileName);
        
        * (dest + (iPosExt - (iPosFileName + 1))) = 0x0;
        return iPosExt - iPosFileName;
    }
    else
        return 0;
}

int HFileInfo::FileExt (HString &str)
{
	int iPosExt = m_strFileName.reverseFind ('.'), i = 0;
	iPosExt ++;
	if (iPosExt)
	{
	    i = iPosExt;
	    while (i < m_strFileName.length ())
	        str.appendChar (WC_AT (m_strFileName, i ++));
                
        return  m_strFileName.length () - iPosExt;
    }
    else
        return 0;
}

int HFileInfo::FileExt (char *dest)
{
	int iPosExt = m_strFileName.reverseFind ('.');
	iPosExt ++;
	if (iPosExt)
	{
	    memcpy (dest, WC_HSTRING (m_strFileName) + iPosExt, m_strFileName.length () - iPosExt);
        
        * (dest + (m_strFileName.length () - iPosExt)) = 0x0;
        return  m_strFileName.length () - iPosExt;
    }
    else
        return 0;
}

//Расширение файла
int HFileInfo::FileExtAsNumber () {
    FILELOG_VARIABLE

    int iPosExt = m_strFileName.find (0, '.');
    HString strTmp = m_strFileName.right (m_strFileName.length () - iPosExt - 1);

    /*strLog.format ("'HFileInfo::FileExtAsNumber': ext = '%s', pos '.' = %i", MB_HSTRING (strTmp), iPosExt);
    FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);*/

    int iPos = 0, iRes = 0;
    while (iPos < strTmp.length ()) {
        if (! IsNumber (strTmp.GetAt (iPos))) {
            iRes = -1;
            break;
        }
        else
            ;

        iPos ++;
    }

    if (iRes == 0)
        return strTmp.number ();
    else
        return -1;
}

char * HFileInfo::DosFileName ()
{
	return (char *) m_pstFD->cAlternateFileName;
}

BOOL HFileInfo::isDirectory () {
	if (m_pstFD->dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL HFileInfo::isHidden () {
	if (m_pstFD->dwFileAttributes && FILE_ATTRIBUTE_HIDDEN == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL HFileInfo::isSystemFile () {
	if (m_pstFD->dwFileAttributes && FILE_ATTRIBUTE_SYSTEM == 0)
		return TRUE;
	else
		return FALSE;
}

BOOL HFileInfo::isVolumeId () {
	//if (m_pstFD->dwFileAttributes && faVolumeId == 0)
		//return TRUE;
	//else
		return FALSE;
}

BOOL HFileInfo::isArchive () {
	if (m_pstFD->dwFileAttributes && FILE_ATTRIBUTE_ARCHIVE == 0)
		return TRUE;
	else
		return FALSE;
}

long int HFileInfo::FileSize () {
	return (m_pstFD->nFileSizeHigh * (MAXDWORD + 1)) + m_pstFD->nFileSizeLow;
}

time_t HFileInfo::tmtCreationTime () {
	FILETIME LTime;
	//LPSYSTEMTIME SystemTime;
	time_t result;

	FileTimeToLocalFileTime (&m_pstFD->ftCreationTime, &LTime);
	//FileTimeToSystemTime (LTime, SystemTime );
	//result = SystemTimeToDateTime (SystemTime);

	//result = (m_pstFD->ftCreationTime.dwHighDateTime * (MAXDWORD + 1)) + m_pstFD->ftCreationTime.dwLowDateTime;
	result = (LTime.dwHighDateTime * (MAXDWORD + 1)) + LTime.dwLowDateTime;

	return result;
}

time_t HFileInfo::tmtLastAccessed () {
	FILETIME LTime;
	time_t result;

	FileTimeToLocalFileTime (&m_pstFD->ftLastAccessTime, &LTime);
	
	//result = (m_pstFD->ftLastAccessTime.dwHighDateTime * (MAXDWORD + 1)) + m_pstFD->ftLastAccessTime.dwLowDateTime;
	result = (LTime.dwHighDateTime * (MAXDWORD + 1)) + LTime.dwLowDateTime;

	return result;
}

time_t HFileInfo::tmtLastWritten () {
	FILETIME LTime;
	time_t result;

	FileTimeToLocalFileTime (&m_pstFD->ftLastWriteTime, &LTime);
	
	result = (m_pstFD->ftLastAccessTime.dwHighDateTime * (MAXDWORD + 1)) + m_pstFD->ftLastAccessTime.dwLowDateTime;
	//result = (LTime.dwHighDateTime * 0x100000000) + LTime.dwLowDateTime;
	
	return result;
}

 // Время создания файла
CTime HFileInfo::tmCreationTime () {
    FILETIME ft;
	SYSTEMTIME st;

	FileTimeToLocalFileTime (&m_pstFD->ftCreationTime, &ft);
	
	FileTimeToSystemTime (&ft, &st);

	return CTime (st);
}

// Время последнего доступа к файлу
CTime HFileInfo::tmLastAccessed () {
    FILETIME ft;
	SYSTEMTIME st;

	FileTimeToLocalFileTime (&m_pstFD->ftLastAccessTime, &ft);
	
	FileTimeToSystemTime (&ft, &st);

	return CTime (st);
}

// Время последней записи в файл
CTime HFileInfo::tmLastWritten () {
    FILETIME ft;
	SYSTEMTIME st;

	FileTimeToLocalFileTime (&m_pstFD->ftLastWriteTime, &ft);
	
	FileTimeToSystemTime (&ft, &st);

	return CTime (st);
}

SYSTEMTIME HFileInfo::stmCreationTime () {
	FILETIME LTime;
	SYSTEMTIME result;

	FileTimeToLocalFileTime (&m_pstFD->ftCreationTime, &LTime);
	
	FileTimeToSystemTime (&LTime, &result);

	return result;
}

SYSTEMTIME HFileInfo::stmLastAccessed () {
	FILETIME LTime;
	SYSTEMTIME result;

	FileTimeToLocalFileTime (&m_pstFD->ftLastAccessTime, &LTime);
	
	FileTimeToSystemTime (&LTime, &result);

	return result;
}

SYSTEMTIME HFileInfo::stmLastWritten () {
	FILETIME LTime;
	SYSTEMTIME result;

	FileTimeToLocalFileTime (&m_pstFD->ftLastWriteTime, &LTime);
	
	FileTimeToSystemTime (&LTime, &result);

	return result;
}

FILETIME HFileInfo::ftmCreationTime () {
	FILETIME result = m_pstFD->ftCreationTime;
	return result;
}

FILETIME HFileInfo::ftmLastAccessed () {
	FILETIME result = m_pstFD->ftLastAccessTime;
	return result;
}

FILETIME HFileInfo::ftmLastWritten () {
	FILETIME result = m_pstFD->ftLastWriteTime;
	return result;
}

int HFileInfo::Execute () {
	int result = 0;
	/*
	result = ::AfxGetApp ()->ShellExecute (theApp.Handle, "open", pChar(m_strFileName.GetData ()), "", pChar(Path), SW_SHOWDEFAULT);
  
	switch (result) of
	{
		case 0: raise Exception.Create ("Недостаточно памяти или ресурсов."); break;
		case ERROR_FILE_NOT_FOUND: raise Exception.Create ("Указанный файл не найден."); break;
		case ERROR_PATH_NOT_FOUND: raise Exception.Create ("Указанный путь не найден."); break;
		case ERROR_BAD_FORMAT: raise Exception.Create ("Ошибка файла .EXE (не -Win32 .EXE или ошибка в .EXE)."); break;
		case SE_ERR_ACCESSDENIED: raise Exception.Create ("Доступ к файлу запрещён."); break;
		case SE_ERR_ASSOCINCOMPLETE: raise Exception.Create ("The filename association is incomplete or invalid."); break;
		case SE_ERR_DDEBUSY: raise Exception.Create ("Транзакция DDE не может быть завершена, потому что другая транзакция DDE находится в процессе выполнения."); break;
		case SE_ERR_DDEFAIL: raise Exception.Create ("Ошибка транзакции DDE."); break;
		case SE_ERR_DDETIMEOUT: raise Exception.Create ("DDE транзакция не может быть завершина из-за тайм-аута."); break;
		case SE_ERR_DLLNOTFOUND: raise Exception.Create ("Указанная dynamic-link library не найдена."); break;
		case SE_ERR_NOASSOC: raise Exception.Create ("Не найдено приложение связанной с данным расширением файла."); break;
		case SE_ERR_OOM: raise Exception.Create ("Недостаточно памяти для завершения операции."); break;
		case SE_ERR_SHARE: raise Exception.Create ("Файл используется другим приложением."); break;
		default: ;
	}
	*/
	return result;
}