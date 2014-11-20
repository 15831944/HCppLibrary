#pragma once
#include <hstring.h>

//EXT_DECLARE_HFILELOG

class HFileInfo : public CObject
{
private:
	void SetInfo (HANDLE , char *, WIN32_FIND_DATA *);
	void SetInfo (HANDLE , WCHAR *, WIN32_FIND_DATA *);
protected:
	LPWIN32_FIND_DATA m_pstFD;
    LPWIN32_FILE_ATTRIBUTE_DATA m_pstFAD;
	LPBY_HANDLE_FILE_INFORMATION m_pstFI;
	HANDLE m_hFile;
    VS_FIXEDFILEINFO fVerBlk;
    HString m_strFileName;
    BOOL GetFileVersion();
    
    void InitializePtr (void);
public:
    HFileInfo (char *); // ������ ������
    HFileInfo (WCHAR *); // ������ ������
    HFileInfo (HANDLE , char *, WIN32_FIND_DATA *); // ������ ������
    ~HFileInfo ();
    int IsInfo (void)
    {
        if ((m_pstFD == NULL) || (m_pstFAD == NULL) || (m_pstFI == NULL))
            return FALSE;
        else
            return TRUE;
    }
    void CopyOf (HFileInfo *);
    void CopyTo (HFileInfo *);
    int Execute (); // ��������� ���� � ���������, ��������� � ���
    char * VersionString (); // ������ �����. ������ ������, ���� ������ �� �������
	char * FileType (); // ��� �����
	char * DosFileName (); // ��� ����� � DOS
    char * OS (); // ������������ �������
    int Path (HString &); // ���� �����
    int Path (char *); // ���� �����
    int FileName (HString &); // ��� �����
    int FileName (char *); // ��� �����
    int name (HString &); // ��� ����� ��� ����������
    int name (char *); // ��� ����� ��� ����������
    int FileExt (HString &); // ���������� �����
    int FileExt (char *); // ���������� �����
    int FileExtAsNumber (); // ���������� �����
    long int FileSize (); // ������ �����
    
	BOOL isDebugBuild (); // True ���� ���� ������� (debug build) ����������
    BOOL isPreRelease (); // True ���� ���� prerelease ����������
    BOOL isPrivateBuild (); // True ���� ���� private ����������
    BOOL isSpecialBuild (); // True ���� ���� special build ����������
    BOOL  isDirectory (); // True ���� ���� �������� �����������
    BOOL  isHidden (); // True ���� ���� �������� ������� (hidden)
    BOOL  isSystemFile (); // True ���� ���� �������� ���������
    BOOL  isVolumeId (); // True ���� ���� �������� ������ ����
    BOOL  isArchive (); // True ���� ���� �������� �������
    FILETIME ftmCreationTime (); // ����� �������� �����
    FILETIME ftmLastAccessed (); // ����� ���������� ������� � �����
    FILETIME ftmLastWritten (); // ����� ��������� ������ � ����
    time_t tmtCreationTime (); // ����� �������� �����
    time_t tmtLastAccessed (); // ����� ���������� ������� � �����
    time_t tmtLastWritten (); // ����� ��������� ������ � ����
    CTime tmCreationTime (); // ����� �������� �����
    CTime tmLastAccessed (); // ����� ���������� ������� � �����
    CTime tmLastWritten (); // ����� ��������� ������ � ����
    SYSTEMTIME stmCreationTime (); // ����� �������� �����
    SYSTEMTIME stmLastAccessed (); // ����� ���������� ������� � �����
    SYSTEMTIME stmLastWritten (); // ����� ��������� ������ � ����
};

#define HFILEINFO(ptr, path) { \
	if (! (ptr == NULL)) { \
		if (AfxIsMemoryBlock (ptr, sizeof (HFileInfo)) == 1) \
			delete ptr; \
		else \
			/*������ ������???*/; \
		ptr = NULL; \
	} \
	else \
		; \
	while (ptr == NULL) { ptr = DEBUG_NEW HFileInfo (path); } \
	if (AfxIsMemoryBlock (ptr, sizeof (HFileInfo)) == 1) { \
		if (ptr->IsInfo ()) \
			; \
		else { \
			delete ptr; \
			ptr = NULL; \
		} \
	} \
	else \
		ptr = NULL; /*������ ������???*/ \
}
