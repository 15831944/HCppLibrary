#include "stdafx.h"
#include <Tlhelp32.h>

#ifdef PTR_DAO_DB
#include "HDaoDatabase.h"
#include "HDaoRecordset.h"
#endif

#if defined _HTHREAD_FILELOG
EXT_DECLARE_HTHREAD_FILELOG
#else
#endif

void FreeMemoryArrayString (CStringArray &);
void FreeMemoryString (CString &);

int FindDate (int, char * , int = 5); //По умолчанию поиск по укороченному варианту
int FindTime (int , char * , int = 5); //По умолчанию поиск по укороченному варианту
void parseTime (HString &, CTimeSpan &);
void parseTime (HString &, SYSTEMTIME &);
void parseTime (HString &, CTime &);
BOOL CheckDate (BYTE * ptr, int);
BOOL CheckTime (BYTE * ptr, int);

double roundToPrecision (double , int);

CTime roundDigitUnitsSecond (CTime& tm) { return tm; }
//Сброс единиц секунд, напр.: 23:48:5s = 23:48:50, где 's' кол-во секунд, ЕСТЕСТВЕННО от 0-9
CTime resetDigitUnitsSecond (CTime );

//Округление десятков секунд, напр.: 23:34:ss = 23:34:00, где 'ss' кол-во секунд, 0-30 И 23:34:ss = 23:34:01, где 'ss' кол-во секунд, 31-59
CTime roundDigitTensSecond (CTime& tm) { return tm; }
//Сброс десятков секунд, напр.: 23:34:ss = 23:34:00, где 'ss' кол-во секунд, ЕСТЕСТВЕННО от 0-59 
CTime resetDigitTensSecond (CTime );

CTime roundDigitUnitsMinute (CTime& tm) { return tm; }
//Сброс единиц минут, напр.: 23:4n:ss = 23:40:00, где 'n' кол-во минут, ЕСТЕСТВЕННО от 0-9 
CTime resetDigitUnitsMinute (CTime );

int randRange (int , int);

BOOL IsNumber(BYTE );

//Возвращает путь к указаноому во 2-м аргументе файле. Поиск начинается с каталога, указанного в 1-м аргументе
HString ReFindFile (HString &, HString &);

DWORD findIdProcess (CString , DWORD);
DWORD findIdThreadAppOfProcess (DWORD);

BOOL CALLBACK fWndEnumProcToLog (HWND , LPARAM );
BOOL CALLBACK fWndEnumProcCloseErr (HWND , LPARAM );

BOOL CALLBACK fWndEnumProcDeleteChildWindows (HWND , LPARAM );

#define HSTR(str) empty

//var enummonths=DEBUG_NEW Array("Январь", "Февраль", "Март", "Апрель", "Май", "Июнь","Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь","Декабрь");
//var daysOfWeek = DEBUG_NEW Array("Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс");

int lpdayOffset [] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
int dayOffset [] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

int yearOffset [] = {0, 365, 730, 1096};

int leapYear (int year) {
    //valid from 1901 to 2099
    return (year % 4 == 0);
}

int monthDuration (int year, int month) {
    int *Offsets;
	if (leapYear (year))
		Offsets = lpdayOffset;
	else
		Offsets = dayOffset;
    return Offsets [month] - Offsets [month - 1];
}

/*
int GetCountDayInYearMounth (int year, int month)
{
    /// declaration for days
    static size_t const days [] = {31, year % 4 ? 28 : 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        
    /// check month
    if (!(month < sizeof (days) / sizeof (days [0]))) {            
        /// throw error
        throw _T("Invalid month value!");
    }
	else
		;
    
    return days [month];
}
*/

int GetCountDayInYearMounth (int year, int month) {
	int iRes = (month != 2 ? ((month % 2) ^ (month > 7)) + 30 : (! (year % 400) || ! (year % 4) && (year % 25) ? 29 : 28));
	return iRes;
}

void IncreaseDate (ULONGLONG *fpDate, int fwDays) {
    char labDaysOfMonths [12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int lwDay, lwMonth, lwYear;
 
    lwYear  = ((long)(*fpDate)/(long)10000)-2000;
    lwMonth = ((long)(*fpDate)-(lwYear+2000)*(long)10000)/(long)100;
    lwDay   =  (long)(*fpDate)-(lwYear+2000)*(long)10000-lwMonth*100;
 
    if ((lwDay > 31) || (lwMonth > 12) || (lwYear > 99))
		return;
    if (! (lwYear & 3))
		labDaysOfMonths [1] = 29;
    else
		labDaysOfMonths [1] = 28;
		
	lwDay += fwDays;
    
	while (lwDay > labDaysOfMonths [lwMonth - 1]) {
        lwDay -= labDaysOfMonths [lwMonth - 1];
        lwMonth ++;
        if (lwMonth == 13) {
            lwMonth = 1;
            lwYear ++;
            if (lwYear == 100)
				lwYear = 0;
        }
        if (!(lwYear%3))
			labDaysOfMonths[1]=29;
        else
			labDaysOfMonths[1]=28;
    }

    *fpDate = (2000 + lwYear)*10000l + lwMonth*100l + lwDay;
}

BOOL IsCorrectSYSTEMTIME (SYSTEMTIME sttm) {
	BOOL bRes = true;

	if (sttm.wYear / 1000 > 0) {
		if ((sttm.wMonth > 0) && (sttm.wMonth < 13)) {
			if ((sttm.wDay > 0) && (sttm.wDay < GetCountDayInYearMounth (sttm.wYear, sttm.wMonth)))	{
			}
			else
				bRes = false;
		}
		else
			bRes = false;
	}
	else
		bRes = false;

	return bRes;
}

int CompareSYSTEMTIME (SYSTEMTIME sttm1, SYSTEMTIME sttm2) {
	int iRes = 0;
	CTime tm1 (sttm1), tm2 (sttm2);
	if (tm1 == tm2)	{
		if (! (sttm1.wMilliseconds == sttm2.wMilliseconds)) {
			if (sttm1.wMilliseconds > sttm2.wMilliseconds) iRes = 1;
			else iRes = 2;
		}
	}
	else {
		if (tm1 > tm2) iRes = 1;
		else iRes = 2;
	}

	return iRes;
}

BOOL IsNumber(BYTE ch) {
    if ((ch > 47) && (ch < 58))
        return true;
    else
        return false;
}
    
int FindDate (int iPos, char *str, int length) {
    int iRes = -1, i = iPos, l = strlen (str), j;
    BYTE *dt = DEBUG_NEW BYTE [length];
    
    if (l > (iPos + length)) {
        while (! str [i + length] == 0) {
            for (j = 0; j < length; j ++) {
                dt [j] = str [i + j];
            }
            
            if (! CheckDate (dt, length))
                i ++;
            else {
                //Только ЕСЛИ есть НЕ отображаемые символы в НАЧАЛе ИЛИ в КОНЦе
                if (i)
                    if ((str [i - 1] < 0x21) || (str [i + length] < 0x21))
                        break;
                    else
                        i ++;
                else
                    break;
            }
        }
        
        if ((i + length) < l)
            iRes = i;
        else
            ;
    }
    else
        ;
        
    delete dt;
    
    return iRes;
}

BOOL CheckDate (BYTE * ptr, int length) {
    BOOL bRes = true;
    int i = 0;

    while (bRes && i < length) {
        switch (i) {
            case 0:
            case 1:
            case 3:
            case 4:
            case 6:
            case 7:
                bRes = IsNumber (ptr [i]);
                break;
            case 2:
            case 5:
                if ((! (ptr [i] == '.')) && (! (ptr [i] == '/')) && (! (ptr [i] == '-')))
                    bRes = false;
                break;
            default:
                ;
        }
        
        i ++; 
    }
    
    return bRes;
}

int FindTime (int iPos, char *str, int length) {
    int iRes = -1, i = iPos, l = strlen (str), j;
    BYTE *dt = DEBUG_NEW BYTE [length];
    
    if (l > (iPos + length)) {
        while (! str [i + length] == 0) {
            for (j = 0; j < length; j ++) {
                dt [j] = str [i + j];
            }
            
            if (! CheckTime (dt, length))
                i ++;
            else {
                //Только ЕСЛИ есть НЕ отображаемые символы в НАЧАЛе ИЛИ в КОНЦе
                if (i)
                    if ((str [i - 1] < 0x21) || (str [i + length] < 0x21))
                        break;
                    else
                        i ++;
                else
                    break;
            }
        }
        
        if ((i + length) < l)
            iRes = i;
        else
            ;
    }
    else
        ;
        
    delete dt;

    return iRes;
}

BOOL CheckTime (BYTE * ptr, int length) {
    BOOL bRes = true;
    int i = 0;

    while (bRes && i < length) {
        switch (i) {
            case 0:
            case 1:
            case 3:
            case 4:
            case 6:
            case 7:
                bRes = IsNumber (ptr [i]);
                break;
            case 2:
            case 5:
                if (! (ptr [i] == ':'))
                    bRes = false;
                break;
            default:
                ;
        }
        
        i ++; 
    }
    
    return bRes;
}

void parseTime (HString &strTime, CTimeSpan &tmSpan) {
	int iPosPartTime = 0, iNumberPartTime = 0, iValuePartTime = -1;
	HString strPartTime = strTime.tokenize (_T (":"), iPosPartTime);

	while ((strPartTime.isUses () == 1) && (iNumberPartTime < 3)) { //В НАШем формаТе времени 3 часТи hh:nn:ss
        iValuePartTime = strPartTime.number ();

        switch (iNumberPartTime) {
            case 0: //ЧАСы
                tmSpan += iValuePartTime * 60 * 60;
                break;
            case 1: //МИНУТЫы
                tmSpan += iValuePartTime * 60;
                break;
            case 2: //СЕКУНДы
                tmSpan += iValuePartTime;
                break;
            default:
                ;
        }
    
        strPartTime = strTime.tokenize (_T (":"), iPosPartTime);
        iNumberPartTime ++;	      
    }
}

void parseTime (HString &strTime, SYSTEMTIME &stm) {
	int iPosPartTime = 0, iNumberPartTime = 0, iValuePartTime = -1;
	HString strPartTime = strTime.tokenize (_T (":"), iPosPartTime);

	while ((strPartTime.isUses () == 1) && (iNumberPartTime < 3)) { //В НАШем формаТе времени 3 часТи hh:nn:ss
        iValuePartTime = strPartTime.number ();

        switch (iNumberPartTime) {
            case 0: //ЧАСы
                stm.wHour = iValuePartTime;
                break;
            case 1: //МИНУТЫы
                stm.wMinute = iValuePartTime;
                break;
            case 2: //СЕКУНДы
                stm.wSecond = iValuePartTime;
                break;
            default:
                ;
        }
    
        strPartTime = strTime.tokenize (_T (":"), iPosPartTime);
        iNumberPartTime ++;
    }
}

void parseTime (HString &, CTime &) {
}

double roundToPrecision (double val, int precision) {
    double result;
     
    precision *= -1;
    result = LONGLONG ((val / pow (10.0, precision)) + 0.5) * pow (10.0, precision); 
    
    return result;
}

int randRange (int range_min, int range_max) {
   // Generate random numbers in the half-closed interval
   // [range_min, range_max). In other words,
   // range_min <= random number < range_max
   
   srand ((unsigned) time (NULL));

   return (int ) ((double) (((double) rand ()) / (RAND_MAX + 1)) * (range_max - range_min)) + range_min;
}

/*
static BOOL wctomb (CString strSrc, CStringA &strADest)
{
	int i, szSrc = strSrc.GetLength ();
	BYTE *pSrc = DEBUG_NEW BYTE [szSrc * sizeof (WCHAR) + 1];
	
	memset (pSrc, 0x0, szSrc * sizeof (WCHAR) + 1);
	memcpy (pSrc, strSrc.GetBuffer (), szSrc * sizeof (WCHAR));

	for (i = 0; i < szSrc; i ++)
	{
		if (*(pSrc + (i * sizeof (WCHAR)) + 1) == 4)
		{
			if ((*(pSrc + (i * sizeof (WCHAR)) + 0) < 0x50) && (*(pSrc + (i * sizeof (WCHAR)) + 0) > (0x50 - 32 * 2 - 1)))
				strADest.AppendChar (*(pSrc + (i * sizeof (WCHAR)) + 0) - 0x50);
			else
			{
				switch (*(pSrc + (i * sizeof (WCHAR)) + 0))
				{
					case 1:
						strADest.AppendChar (-88);
						break;
					case 81:
						strADest.AppendChar (-72);
						break;
				}
			}
		}
		else
			strADest.AppendChar (*(pSrc + (i * sizeof (WCHAR)) + 0));
	}

	delete pSrc;

	return true;
}
*/

CTime resetDigitUnitsMinute (CTime tm) {
    tm = resetDigitTensSecond (tm); //Десятки секунд
    tm -= ((tm.GetMinute () % 10) * 60); // ЕдиницЫ минут
    
    return tm;
}

CTime resetDigitTensSecond (CTime tm) {
    tm  = resetDigitUnitsSecond (tm); //Еденицы секунд
    tm -= (((int) (tm.GetSecond () / 10)) * 10); //Десятки секунд
    
    return tm;
}

CTime resetDigitUnitsSecond (CTime tm) {
    tm -= (tm.GetSecond () % 10); //Еденицы секунд
    
    return tm;
}

void FreeMemoryString (CString &str) {
	str.Delete (0, str.GetLength ());
	str.FreeExtra ();
	str.ReleaseBuffer ();
}

void FreeMemoryArrayString (CStringArray &arStr) {
    for (int i = 0; i < arStr.GetSize (); i ++)
	{
		FreeMemoryString ((CString &) arStr.GetAt (i));
	}
	arStr.RemoveAll ();
}

//Возвращает путь к указаноому во 2-м аргументе файле. Поиск начинается с каталога, указанного в 1-м аргументе
HString ReFindFile (HString &pathDB, HString &fileNameDB) {
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
	HString strWildcard = pathDB, findfileName, findfilePath, resFindfileDB;
	strWildcard.appendChar ('\\');
    strWildcard += _T ("*.*");

	CFileFind finder;
    // start working for files
    finder.Close ();
    BOOL bWorking = finder.FindFile (strWildcard.GetBuffer ());
#if defined _HTHREAD_FILELOG
    strLog.format (_T ("%S %S"), _T ("Поиск файла:"), WC_HSTRING (strWildcard));
    FILELOG_WRITE_WITHDT (strLog.GetBuffer (), HFIND_DB);
#else
#endif
	while (bWorking)
	{
		bWorking = finder.FindNextFileW ();

		if (finder.IsDots())
             continue;

        // if it's a directory, recursively search it
        if (finder.IsDirectory())
        {
			resFindfileDB = ReFindFile (HString (finder.GetFilePath ().GetBuffer ()), fileNameDB);
			
			//А этот блок ЗАЧЕМ??? - Рекурсия, однако!!!
			if (resFindfileDB.isUses () == 1) {
				//Прерывание рекурсии
				finder.Close ();
				
				break;
			}
			else
			    ;
		}
		else {
			findfileName = HString (finder.GetFilePath ().GetBuffer ());
			int right = findfileName.length ();
			right -= findfileName.reverseFind ('\\');
			right --;
			findfileName = findfileName.right (right);
			if (findfileName.compareNoCase (fileNameDB) == 0)
			{
				findfilePath = HString (finder.GetFilePath ().GetBuffer ());
				finder.Close ();
				
				resFindfileDB = findfilePath.left (findfilePath.length () - findfileName.length () - 1);
				//resFindfileDB = findfilePath;
				
				break;
			}
			else
			    ;
		}
	}

    strWildcard.free (); findfileName.free (); //findfilePath.free ();
#if defined _HTHREAD_FILELOG
	strLog.free ();
#else
#endif
    
    //if ((findfilePath.IsUses () == 1) && (resFindfileDB.IsUses () == 1))
    if (resFindfileDB.isUses () == 1)
        return resFindfileDB;
    else
    	return _T ("");
}

BOOL CALLBACK fWndEnumProcToLog (HWND hwnd, LPARAM lp) {
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
	WINDOWINFO wndInfo;
	HString wndText, wndClassName;

	//wndText.setLength (MAX_PATH);
	if (GetWindowTextLength (hwnd))
		wndText.setLength (GetWindowTextLength (hwnd));
	else
		;

	wndClassName.setLength (MAX_PATH);

	if (hwnd) {
		if (wndText.length ())
			GetWindowText (hwnd, wndText.GetBuffer (), wndText.length ());
		else
			wndText = _T ("БЕЗ ИМЕНи");

		::GetClassName (hwnd, wndClassName.GetBuffer (), MAX_PATH);
		wndClassName.actualizeLength ();

		::GetWindowInfo (hwnd, &wndInfo);
#if defined _HTHREAD_FILELOG
		strLog.format (_T ("Найдено ОКНО: %S, %S"), WC_HSTRING (wndText), WC_HSTRING (wndClassName));
		FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
#else
#endif
		if (wndText.find (0, (TCHAR *) lp) > -1) {
#if defined _HTHREAD_FILELOG
			strLog.free ();
#else
#endif
		}
		else
			;
	}
	else
		;

	return true;
}

BOOL CALLBACK fWndEnumProcCloseErr (HWND hwnd, LPARAM lp) {
#if defined _HTHREAD_FILELOG
	FILELOG_VARIABLE
#else
#endif
	WINDOWINFO *ptrWndInfo = DEBUG_NEW WINDOWINFO;
	HString wndText, wndClassName;

	//wndText.setLength (MAX_PATH);
	if (GetWindowTextLength (hwnd))
		wndText.setLength (GetWindowTextLength (hwnd));
	else
		;

	wndClassName.setLength (MAX_PATH);

	if (hwnd) {
		if (wndText.length ())
			GetWindowText (hwnd, wndText.GetBuffer (), wndText.length ());
		else
			wndText = _T ("БЕЗ ИМЕНи");

		::GetClassName (hwnd, wndClassName.GetBuffer (), MAX_PATH);
		wndClassName.actualizeLength ();

		::GetWindowInfo (hwnd, ptrWndInfo);

		int iPosError = wndText.find (0, "Ошибка"),
			iPosModule = wndText.find (0, (TCHAR *) lp),
			iPosClassName = wndClassName.find (0, "32770");

#if defined _DEBUG
//#if defined _HTHREAD_FILELOG
//		strLog.format (_T ("Найдено ОКНО: %S, %S (%i, %i, %i)"), WC_HSTRING (wndText), WC_HSTRING (wndClassName), iPosError, iPosModule, iPosClassName);
//		FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
//#else
//#endif

		if (iPosModule == 0)
			iPosModule = wndText.find (0, (TCHAR *) lp);
		else
			;
#else
#endif

		if ((iPosError > -1) && (iPosModule > -1) && (iPosClassName > -1)) {
			::PostMessage (hwnd, WM_CLOSE, 0, 0);
			//::PostMessage (hwnd, BM_CLICK, 0, 0);
//#if defined _HTHREAD_FILELOG
//			strLog.format (_T ("Закрыто ОКНО: %S, %S (%i, %i, %i)"), WC_HSTRING (wndText), WC_HSTRING (wndClassName), iPosError, iPosModule, iPosClassName);
//			FILELOG_WRITE_WITHDT (MB_HSTRING (strLog), HDEBUG);
//#else
//#endif
		}
		else
			;
	}
	else
		;

	wndClassName.free ();
	delete ptrWndInfo;

	return true;
}

//DWORD findTask (char *ExeFileName)
DWORD findIdProcess (CString ExeFileName, int iIdDupl) {
    DWORD idProcess = -1;
    BOOL ContinueLoop = true;
    HANDLE FSnapshotHandle;
    PROCESSENTRY32 FProcessEntry32;
    /*char *ptrExe = new char [strlen (ExeFileName) + 1];
    strcpy (ptrExe, ExeFileName);*/

    FSnapshotHandle = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
    FProcessEntry32.dwSize = sizeof (PROCESSENTRY32);
    
    ContinueLoop = Process32First (FSnapshotHandle, &FProcessEntry32);
    while (ContinueLoop) {
        //if (strcmp (strupr ((char *) FProcessEntry32.szExeFile), strupr (ExeFileName)) == 0)
        if ((ExeFileName.CollateNoCase (CString (FProcessEntry32.szExeFile)) == 0))
            if (! (FProcessEntry32.th32ProcessID == iIdDupl))
                idProcess = FProcessEntry32.th32ProcessID;
            else
                ;
        else
            ;
            
        ContinueLoop = Process32Next (FSnapshotHandle, &FProcessEntry32);
    }
 
    CLOSE_HANDLE (FSnapshotHandle);
    //delete ptrExe;
    
    return idProcess;
}

DWORD findIdThreadAppOfProcess (DWORD idOwnerProcess) {
    DWORD idThreadRes = -1;

    BOOL ContinueLoop = true;
    HANDLE FSnapshotHandle;
    THREADENTRY32 FThreadEntry32;
    /*char *ptrExe = new char [strlen (ExeFileName) + 1];
    strcpy (ptrExe, ExeFileName);*/

    FSnapshotHandle = CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD, 0);
    FThreadEntry32.dwSize = sizeof (THREADENTRY32);
    
    ContinueLoop = Thread32First (FSnapshotHandle, &FThreadEntry32);
    while (ContinueLoop) {
        if (FThreadEntry32.th32OwnerProcessID == idOwnerProcess) {
            idThreadRes = FThreadEntry32.th32ThreadID;
            break;
        }
        else
            ;
            
        ContinueLoop = Thread32Next (FSnapshotHandle, &FThreadEntry32);
    }
 
    CLOSE_HANDLE (FSnapshotHandle);

    return idThreadRes;
}

BOOL CALLBACK fWndEnumProcDeleteChildWindows (HWND hwnd, LPARAM lp) {
	CWnd *ptrChildWnd = CWnd::FromHandle (hwnd);
	HString strClassName = ptrChildWnd->GetRuntimeClass ()->m_lpszClassName;

	if ((strClassName.compareNoCase ("CTreeCtrl") == 0) || (strClassName.compareNoCase ("CButton") == 0)) {
		delete ptrChildWnd;
	}
	else
		;

	return true;
}