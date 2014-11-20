#include "stdafx.h"

extern CWinApp theApp;
extern void AbortExit (void);

HFileIni::HFileIni(void)
{
	m_iReasonably = -1;
}

HFileIni::HFileIni(TCHAR *pNameFile)
{
	m_iReasonably = -1;

	int iResponce = 0;

	unsigned long szFileOfRead; //������ ����� (� �������������� ������) ��� ������)
	TCHAR *bufferOfRead = 0x00; //����� ��� ������ �����
	CStringA strABufferOfRead, strATmp;
	CStringW strWTmp;

	HFileInfo *phfiTmp = NULL;
	//��������� ��� ������ ������ (���������)
    WIN32_FIND_DATA *lpFileData = DEBUG_NEW WIN32_FIND_DATA;
	//��������� ������
    HANDLE hFindFile, //��������� ��������� ����������
        hFileOfRead = 0,
        hTemplateFile = 0;
        
    //��������� ��� ������ 'ReadFile'
    OVERLAPPED gOverLapped;
    gOverLapped.Offset     = 0;
    gOverLapped.OffsetHigh = 0;
    gOverLapped.hEvent     = NULL;
    
    //��������� ��� ������ 'CreateFile'
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(LPSECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = false;

	hFindFile = FindFirstFile (pNameFile, lpFileData);
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
		phfiTmp = DEBUG_NEW HFileInfo (hFindFile, pNameFile, lpFileData);
        //��������� ���-����
		hFileOfRead = CreateFile(
				(LPCSTR) pNameFile,			// (LPCTSTR) pointer to name of the file
				GENERIC_READ,				// (DWORD) access (read-write) mode
				FILE_SHARE_READ,			// (DWORD) share mode
				&sa,						// (LPSECURITY_ATTRIBUTES) pointer to security attributes
				OPEN_EXISTING,				// (DWORD) how to create
				FILE_ATTRIBUTE_NORMAL,		// (DWORD) file attributes
				hTemplateFile 				// (HANDLE) handle to file with attributes to copy
		);

		FindClose (hFindFile);
	    hFindFile = 0x0;
		
		if (! (INVALID_HANDLE_VALUE == hFileOfRead))
		{
            szFileOfRead = phfiTmp->FileSize ();
		    
		    if (bufferOfRead)
		    {
		        delete bufferOfRead;
		        bufferOfRead = 0x00;
		    }
		        
		    bufferOfRead = DEBUG_NEW TCHAR [szFileOfRead];

		    iResponce = ReadFile (hFileOfRead, bufferOfRead, szFileOfRead, &szFileOfRead, &gOverLapped);
			
		    if (iResponce == 1)
		    {
			    strABufferOfRead = CString (bufferOfRead);
			    //������ ���-����� ��� �� �����
				/*
			    for (i = 0; i < szFileOfRead; i ++)
			    {
				    //cout << i << "-nd: " << char (* (bufferOfRead + i)) << endl;
				    if (((* (bufferOfRead + i)) == 13) && ((* (bufferOfRead + i + 1)) == 10))
				    {
					    arstrFileOfRead.Add (strTmp);
					    strTmp.Empty ();
					    //��� ��� � ������ ��� �� ����� 10, 13 � �.�.
					    i ++;
				    }
				    else
				    {
					    strTmp.AppendChar (* (bufferOfRead + i));
				    }
			    }
                */
                m_iReasonably = InitializeParametrName (strABufferOfRead);
				//����� �. �������� ������� ���������� �������� ��������� �� ���-����� ???
                //��� ���������� ������
                //���� ��� ���������� ������
				m_iReasonably = InitializeParametrValue (strABufferOfRead);
                				  	
			    if (m_iReasonably == 0)
					;
				else
				{//�� 'm_iReasonably' ��������� ��� ��������� ��� ������������� �������� ��������� ������
					strATmp = "������. ������������ ���������� INI-�����";
					PTR_FILELOG->WriteMessageWithDT (strATmp.GetBuffer (), LEVEL_INFO_1);
				}
		    }
			else
			{
				strATmp.Format ("������ ��� ������ ���-�����: %s", pNameFile);
				PTR_FILELOG->WriteMessageWithDT (strATmp.GetBuffer (), LEVEL_INFO_1);
			}
			    
		    //��������� ��������� ������������ ���-�����
		    CloseHandle (hFileOfRead);
		    hFileOfRead = 0x0;
		    
		    //����������� ����� ������ �� �����
		    delete bufferOfRead;
		    bufferOfRead = 0x00;
		}
		else
		{
			strATmp.Format ("������ ��� �������� ���-�����: %s", pNameFile);
			PTR_FILELOG->WriteMessageWithDT (strATmp.GetBuffer (), LEVEL_INFO_1);
		}
	}
	else
	{
		strATmp.Format ("���� %s ������������� �� ������", pNameFile);
		PTR_FILELOG->WriteMessageWithDT (strATmp.GetBuffer (), LEVEL_INFO_1);
	}

	strABufferOfRead.ReleaseBuffer ();
	strABufferOfRead.FreeExtra ();

	if (phfiTmp)
		delete phfiTmp;

	delete lpFileData;
}

HFileIni::HFileIni(WCHAR *nameFile)
{
}

HFileIni::~HFileIni(void)
{
	PTR_FILELOG->WriteMessageWithDT ("�������� ��'���� INI-�����...", LEVEL_INFO_0);
	
	int i;

	for (i = 0; i < m_arstrAParametrName.GetSize (); i ++)
	{
		m_arstrAParametrName.GetAt (i).Delete (0, m_arstrAParametrName.GetAt (i).GetLength ());
		m_arstrAParametrName.GetAt (i).FreeExtra ();
		m_arstrAParametrName.GetAt (i).ReleaseBuffer ();
	}
	m_arstrAParametrName.RemoveAll ();

	for (i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
	{
		m_arstrAParametrValue.GetAt (i).Delete (0, m_arstrAParametrValue.GetAt (i).GetLength ());
		m_arstrAParametrValue.GetAt (i).FreeExtra ();
		m_arstrAParametrValue.GetAt (i).ReleaseBuffer ();
	}
	m_arstrAParametrValue.RemoveAll ();
}

BOOL HFileIni::IsParametrNameDuplicate (CStringA &nameParametr)
{
	int i;
	for (i = 0; i < m_arstrAParametrName.GetSize (); i ++)
	{
		if (m_arstrAParametrName.GetAt (i).CollateNoCase (nameParametr) == 0)
			return true;
	}

	return false;
}

int HFileIni::FindParametrBegin (CStringA &strASource, int pos)
{
	int iRes = 0;

	if (pos == 0)
		;
	else
	{
		while (iRes == 0)
		{
			pos --;

			if ((strASource.GetAt (pos) == ';') || (strASource.GetAt (pos) < 0x21))
				iRes = pos + 1;
			else
			{
				if (pos > 1)
				{
					if (strASource.GetAt (pos - 1) == ';' && strASource.GetAt (pos) < 0x21)
						iRes = pos;
					else
					{
						if (pos > 2)
							if ((strASource.Mid (pos - 2, 3).CollateNoCase ("REM") == 0))
								iRes = pos + 1;
							else
							{
								if (pos > 3)
									if (strASource.Mid (pos - 3, 3).CollateNoCase ("REM") == 0)
									iRes = pos + 1;
							}
					}
				}
			}
		}
	}
            
    return iRes;
}

BOOL HFileIni::IsParametrRemark (CStringA &strASource, int pos)
{
    BOOL bRes = false;
	if (pos == 0)
		;
	else
	{
		pos --;
		while (strASource [pos] == 0x20)
			pos --;

		if (strASource.GetAt (pos) == ';')
			bRes = true;
		else
		{
			if (pos > 2)
				if (strASource.Mid (pos - 2, 3).CollateNoCase ("REM") == 0)
					bRes = true;
		}
	}
            
    return bRes;
}

BOOL HFileIni::IsParametrPart (CStringA &strASource, int pos)
{
	BOOL bRes = false;

	if (pos == 0)
		;
	else
	{
		pos --;

		if (strASource [pos] > 0x20)
		{
			if (! (strASource [pos] == ';'))
			{
				if (pos > 1)
					if (! (strASource.Mid (pos - 2, 3).CollateNoCase ("REM") == 0))
						bRes = true;
			}

		}
	}

	return bRes;
}

BOOL HFileIni::SetParametrValue (char *name, CStringA &strASource, CStringA &strADestination)
{
    BOOL bRes = false;
    CStringA strAParametr = CStringA (name);
    strAParametr.AppendChar ('=');
    int posParametr = strASource.Find (strAParametr);
    
    //���������� ��������������� ���������
    while (posParametr > 0)
    {
        if ((IsParametrRemark (strASource, posParametr)) || (IsParametrPart (strASource, posParametr)))
            posParametr = strASource.Find (strAParametr, posParametr + 1);
        else
            break;
    }

	//� ��� �������� �� ������� ���������� ���������� �������� ???
    
    if (posParametr > -1)
    {				        
        posParametr += strAParametr.GetLength ();

        while ((* (strASource.GetBuffer () + posParametr) != 13) &&
                (* (strASource.GetBuffer () + posParametr) != 10))
        {
	        strADestination.AppendChar (* (strASource.GetBuffer () + posParametr ++));
        }
        
        bRes = true;
    }
    else
        ;

	//�������� �� ������� ���������� ���������� �������� !!!
	posParametr = strASource.Find (strAParametr, posParametr + 1);
    
    //���������� ��������������� ���������
    while (posParametr > 0)
    {
        if ((IsParametrRemark (strASource, posParametr)) || (IsParametrPart (strASource, posParametr)))
            posParametr = strASource.Find (strAParametr, posParametr + 1);
        else
            break;
    }
    
    if (posParametr > -1)
    {
		bRes = false;
	}
        
    return bRes;
}

int HFileIni::InitializeParametrName (CStringA &strAData)
{
    CStringA strAParametr;
    
    char ch = '=';
    int posParametr = strAData.Find (ch, 0);
    
    //FindIdentifierParametr ???
    while (((posParametr - 1) > -1) && ((posParametr + 1) < strAData.GetLength ()))
    {
        if ((strAData.GetAt (posParametr - 1) == strAData.GetAt (posParametr)) || (strAData.GetAt (posParametr + 1) == strAData.GetAt (posParametr)))
            posParametr ++;
        else
            break;
        
        posParametr = strAData.Find (ch, posParametr);
    }
    
	while ((posParametr > -1) && (posParametr < strAData.GetLength ()))
	{
		posParametr = FindParametrBegin (strAData, posParametr);

		if (posParametr < strAData.Find ('=', posParametr))
			strAParametr = strAData.Mid (posParametr, strAData.Find ('=', posParametr) - posParametr);
			
		if ((strAParametr.GetLength () > 0) && (IsParametrNameDuplicate (strAParametr) == false))
			m_arstrAParametrName.Add (strAParametr);
		else
			;
	
		posParametr += strAParametr.GetLength ();
		strAParametr.Empty ();

		posParametr ++;
		posParametr = strAData.Find ('=', posParametr);
		
		//FindIdentifierParametr ???
        while (((posParametr - 1) > -1) && ((posParametr + 1) < strAData.GetLength ()))
        {
            if ((strAData.GetAt (posParametr - 1) == strAData.GetAt (posParametr)) || (strAData.GetAt (posParametr + 1) == strAData.GetAt (posParametr)))
                posParametr ++;
            else
                break;
            
            posParametr = strAData.Find (ch, posParametr);
        }
	}

	m_arstrAParametrName.Add ("");
    
    return m_arstrAParametrName.GetSize () - 1;
}

int HFileIni::InitializeParametrValue (CStringA &strAData)
{
    int i = 1;
    while (! m_arstrAParametrName [i - 1].IsEmpty ())
    {
        m_arstrAParametrValue.Add ("");
        if (! SetParametrValue (m_arstrAParametrName [i - 1].GetBuffer (), strAData, m_arstrAParametrValue.GetAt (i - 1)))
            return i;
            
        i ++;
    }
    
    return 0;
}

CString HFileIni::GetParametrValue (char *parametrName)
{
    CString strTmp;
    int i = 1;
    while (! m_arstrAParametrName [i - 1].IsEmpty ())
    {
        if (m_arstrAParametrName [i - 1].CollateNoCase (parametrName) == 0)
            break;
        
        i ++;
    }
    
    if (i > m_arstrAParametrValue.GetSize ())
    {
        strTmp.Format ("� ���-����� ����������� ��������: %s", parametrName);
        PTR_FILELOG->WriteMessageWithDT (strTmp.GetBuffer (), LEVEL_INFO_1);
        AbortExit (); //��� ��������� ������ ���������� ���������� ???
    }
    else
        return m_arstrAParametrValue [i - 1];
}

void HFileIni::CorrectParametrValue ()
{
	CStringA strATmp;
	CStringW strWTmp;

	PTR_FILELOG->WriteMessage ("������������� �������� ����������.", LEVEL_INFO_0);

    int i = 1;
    while (! m_arstrAParametrName [i - 1].IsEmpty ())
    {
		strATmp.Format ("������: %s%s", m_arstrAParametrName [i - 1], "...");
        //if (m_arstrAParametrName [i - 1].Find ("Dir") > -1)
        //if ((m_arstrAParametrName [i - 1].Find ("Dir") > -1) || (m_arstrAParametrName [i - 1].Find ("Database") > -1))
        if ((GetParametrValue (m_arstrAParametrName [i - 1].GetBuffer ()).Find (GetParametrValue ("NetSharedName")) > -1) && (! (m_arstrAParametrName [i - 1].CollateNoCase ("NetSharedName") == 0)))
        {
			strATmp.Format ("%s ������ %s �� %s", strATmp, GetParametrValue ("NetSharedName"), GetParametrValue ("NetSharedSource"));
            m_arstrAParametrValue [i - 1].Replace (GetParametrValue ("NetSharedName"), GetParametrValue ("NetSharedSource"));
        }
        else
            ;

		PTR_FILELOG->WriteMessage (strATmp.GetBuffer (), LEVEL_INFO_0);

        i ++;
    }
}

void HFileIni::OutParametrToLog (void)
{
	CString strTmp;
	for (int i = 0; i < m_arstrAParametrValue.GetSize (); i ++)
    {
        strTmp.Append (m_arstrAParametrName [i]);
		strTmp.AppendChar ('=');
		strTmp.Append (m_arstrAParametrValue [i]);

		PTR_FILELOG->WriteMessage (strTmp.GetBuffer (), LEVEL_INFO_0);

		strTmp.Empty ();
    }
}

int HFileIni::IsReasonably (void)
{
	return m_iReasonably;
}

int HFileIni::GetParametrsCount (void)
{
	return m_arstrAParametrValue.GetSize ();
}

CString HFileIni::GetParametrName (int n)
{
	//�������� CStringA, �� ������� CString ???
	return m_arstrAParametrName [n - 1];
}