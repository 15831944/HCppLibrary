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
    HFileInfo (char *); // Создаём объект
    HFileInfo (WCHAR *); // Создаём объект
    HFileInfo (HANDLE , char *, WIN32_FIND_DATA *); // Создаём объект
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
    int Execute (); // Открывает файл в программе, связанной с ним
    char * VersionString (); // Версия файла. Строка пустая, если строка не найдена
	char * FileType (); // Тип файла
	char * DosFileName (); // Имя файла в DOS
    char * OS (); // Операционная система
    int Path (HString &); // Путь файла
    int Path (char *); // Путь файла
    int FileName (HString &); // Имя файла
    int FileName (char *); // Имя файла
    int name (HString &); // Имя файла без расширения
    int name (char *); // Имя файла без расширения
    int FileExt (HString &); // Расширение файла
    int FileExt (char *); // Расширение файла
    int FileExtAsNumber (); // Расширение файла
    long int FileSize (); // Размер файла
    
	BOOL isDebugBuild (); // True если флаг отладки (debug build) установлен
    BOOL isPreRelease (); // True если флаг prerelease установлен
    BOOL isPrivateBuild (); // True если флаг private установлен
    BOOL isSpecialBuild (); // True если флаг special build установлен
    BOOL  isDirectory (); // True если файл является директорией
    BOOL  isHidden (); // True если файл является скрытым (hidden)
    BOOL  isSystemFile (); // True если файл является системным
    BOOL  isVolumeId (); // True если файл является меткой тома
    BOOL  isArchive (); // True если файл является архивом
    FILETIME ftmCreationTime (); // Время создания файла
    FILETIME ftmLastAccessed (); // Время последнего доступа к файлу
    FILETIME ftmLastWritten (); // Время последней записи в файл
    time_t tmtCreationTime (); // Время создания файла
    time_t tmtLastAccessed (); // Время последнего доступа к файлу
    time_t tmtLastWritten (); // Время последней записи в файл
    CTime tmCreationTime (); // Время создания файла
    CTime tmLastAccessed (); // Время последнего доступа к файлу
    CTime tmLastWritten (); // Время последней записи в файл
    SYSTEMTIME stmCreationTime (); // Время создания файла
    SYSTEMTIME stmLastAccessed (); // Время последнего доступа к файлу
    SYSTEMTIME stmLastWritten (); // Время последней записи в файл
};

#define HFILEINFO(ptr, path) { \
	if (! (ptr == NULL)) { \
		if (AfxIsMemoryBlock (ptr, sizeof (HFileInfo)) == 1) \
			delete ptr; \
		else \
			/*ОШИБКА ПАМЯТИ???*/; \
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
		ptr = NULL; /*ОШИБКА ПАМЯТИ???*/ \
}
