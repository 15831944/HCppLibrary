#pragma once
//#include "stdafx.h"

#if defined _CONSOLE
#include <iostream>
#else
#endif

//using namespace std;

//#ifdef  __cplusplus
//#define _ADDRESSOF(v)   ( &reinterpret_cast<const char &>(v) )
//#else
//#define _ADDRESSOF(v)   ( &(v) )
//#endif

//#define _INTSIZEOF(n)   ((sizeof (n) + sizeof (int) - 1) & ~(sizeof (int) - 1))

#define hva_start(ap,v)  (ap = (char *) (&reinterpret_cast <const char &> (v)) + ((sizeof (v) + sizeof (int) - 1) & ~(sizeof (int) - 1)))
#define hva_arg(ap,t)    (*(t *)((ap += ((sizeof (t) + sizeof (int) - 1) & ~(sizeof (int) - 1))) - ((sizeof (t) + sizeof (int) - 1) & ~(sizeof (int) - 1))))
#define hva_end(ap)      (ap = (char *)0)

union VAR_ARGS
{
    DWORD i;
	DWORDLONG l;
    float f;
	double d;
    char c;
    void *ptr;
};

class HString : public CObject
{
public:
    enum STRING_TYPE {EMPTYSTR = 0, MBSTR, WCSTR, UNICODESTR};

    HString (STRING_TYPE type = EMPTYSTR)  : CObject (), m_pData (NULL), m_length (0), m_strType (type) {
        if (type == EMPTYSTR)
            ;
        else
            init (type);
    }
    HString (BYTE *, STRING_TYPE);
	HString (char *src) : CObject (), m_pData (NULL), m_length (0), m_strType (MBSTR) { init (src); }
    HString (LPCSTR src) : CObject (), m_pData (NULL), m_length (0), m_strType (MBSTR) { init (src); }
#if defined _UNICODE
	HString (TCHAR *src) : CObject (), m_pData (NULL), m_length (0), m_strType (WCSTR) { init (src); }
    HString (LPCTSTR src) : CObject (), m_pData (NULL), m_length (0), m_strType (WCSTR) { init (src); }
#else
#endif
    
    ~HString () { free (); }
    
    HString (const HString& src) : CObject (), m_pData (NULL), m_length (0), m_strType (EMPTYSTR) 
    {
        switch (src.m_strType)
        {
            case MBSTR:
                init ((char *) src.m_pData);
                break;
            case WCSTR:
                init ((TCHAR *) src.m_pData);
                break;
            default:
                ;                
        }
    }
	//Инициализация строки с помощью оператора '='
    HString &operator = (const HString &src)
    {
        switch (src.m_strType)
        {
            case MBSTR:
                init ((char *) src.m_pData);
                break;
            case WCSTR:
                init ((TCHAR *) src.m_pData);
                break;
            default:
                ;                
        }
        
        return *this;
    }
    HString &operator = (const char *src)
    {
        init ((char *) src);
        
        return *this;
    }
#if defined _UNICODE
    HString &operator = (const TCHAR *src)
    {
        init ((TCHAR *) src);
        
        return *this;
    }
#else
#endif
    //Сложение строк
    HString operator + (const HString &src)
    {
        //Вариант №1
        //HString strRes;
        //strRes = dup ();
        //strRes.append (src.m_pData, src.m_strType, strRes.type ());

        //return strRes;
        
        //Вариант №2
        append (src.m_pData, src.m_strType, type ());
        
        return *this;
    }
    HString operator + (const char *src) {
        append ((BYTE *) src, MBSTR, type ());
        
        return *this;
    }
#if defined _UNICODE
    HString operator + (const TCHAR *src)
    {
        append ((BYTE *) src, WCSTR, type ());
        
        return *this;
    }
#else
#endif
    HString operator += (const HString &src)
    {
        //Вариант №1
        //HString strRes;
        //strRes = dup ();
        //strRes.append (src.m_pData, src.m_strType, strRes.type ());

        //return strRes;
        
        //Вариант №2
        append (src.m_pData, src.m_strType, type ());
        
        return *this;
    }
    HString operator += (const char *src)
    {
        append ((BYTE *) src, MBSTR, type ());
        
        return *this;
    }
#if defined _UNICODE
    HString operator += (const TCHAR *src)
    {
        append ((BYTE *) src, WCSTR, type ());
        
        return *this;
    }
#else
#endif

//#if defined _UNICODE
//TCHAR
//#else
char
//#endif
    operator [] (int pos)
    {
        if (m_strType == MBSTR)
            return *(((char *) m_pData) + pos);
        else
            if (m_strType == WCSTR)
                return *(((TCHAR *) m_pData) + pos);
            else
                return -1;
    }
    
    int init (STRING_TYPE type)
    {
        switch (type)
        {
            case MBSTR:
                init ("");
                break;
            case WCSTR:
                init (_T (""));
                break;
            default:
                ;
        }
        
        return m_length;
    }
	int init (char *, STRING_TYPE = MBSTR);
    int init (LPCSTR, STRING_TYPE = MBSTR);
#if defined _UNICODE
    int init (TCHAR *, STRING_TYPE = WCSTR);
	int init (LPCTSTR, STRING_TYPE = WCSTR);
#else
#endif

	//Установка длины строки с символом заполнения 
    int setLength (int, char = 0x0);
#if defined _UNICODE
    //int setLength (int, TCHAR = 0x0);
#else
#endif
    int actualizeLength (void); //или 'trimRight' ???

    int free (BOOL = true, STRING_TYPE = EMPTYSTR);
    
    int szElement (void)
    {
        int iRes = -1;
    
        switch (type ())
        {
            case MBSTR:
                iRes = sizeof (char);
                break;
            case WCSTR:
                iRes = sizeof (TCHAR);
                break;
            default:
                ;
        }
        
        return iRes;
    }
    
    int isUses (void) { if ((! (m_pData == NULL)) && (! (m_length == 0x0)) && (! (m_strType == EMPTYSTR))) return 1; else return 0; }
    int isEmpty (void) { if ((! (m_pData == NULL)) && (m_length == 0x0) && (! (m_strType == EMPTYSTR))) return 1; else return 0; }
    int isNull (void) { if ((m_pData == NULL) && (m_length == 0x0) && (m_strType == EMPTYSTR)) return 1; else return 0; }
    int isFree (void) { if ((m_pData == NULL) || (m_length == 0x0)) return 1; else return 0; }
    
    void format (char *, ...);
#if defined _UNICODE
    void format (TCHAR *, ...);
#else
#endif

    inline int length (void) { return m_length; }
    static int _len (BYTE *src, STRING_TYPE type)
    {
        int len = -1;
        switch (type)
        {
            case MBSTR:
                len = _mbslen ((unsigned char *) src);
                break;
            case WCSTR:
                len = wcslen ((TCHAR *) src);
                break;
            default:
                ;
        }
        
        return len;
    }
    //Возврщение 
    inline STRING_TYPE type (void) { return m_strType; }
    //Возврщение кода символа, находящегося на позиции 'pos'
    int codeAt (int pos)
    {
        int iRes = -1;

        switch (type ())
        {
            case MBSTR:
                iRes = (int) *(((unsigned char *) m_pData) + pos);
                    iRes --;
                break;
            case WCSTR:
                iRes = (int) *(((TCHAR *) m_pData) + pos);
                break;
            default:
                ;
        }
        
        return iRes;
    }

    //Добавление В КОНЕЦ к имеющейся строке другую. 2-ой параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
    int append (HString &, STRING_TYPE = EMPTYSTR);
    int append (char *, STRING_TYPE = EMPTYSTR);
#if defined _UNICODE
    int append (TCHAR *, STRING_TYPE = EMPTYSTR);
#else
#endif

    //Вставить в имеющююся строкУ другую. 3-ий параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
    int insert (int , HString &, STRING_TYPE = EMPTYSTR);
    int insert (int , char *, STRING_TYPE = EMPTYSTR);
#if defined _UNICODE
    int insert (int , TCHAR *, STRING_TYPE = EMPTYSTR);
#else
#endif

    //Вырезать количество 'c' элементов, начиная с 'pos' (уменьшить длину на 'c')
    int cut (int , int );
    
    //Вырезать количество 'c' элементов, начиная с 'pos' (возвратить новую строку)
    HString right (int );
    HString left (int );
    HString mid (int , int = -1);
    
    int upr (void);
    int lwr (void);
    
    int trim (void);
    int trimLeft (void);
    int trimRight (void);
    
    //Сравнение БЕз учЁтА регистра симВОЛов
    int compareNoCase (HString &);
    int compareNoCase (char *);
#if defined _UNICODE
    int compareNoCase (TCHAR *);
#else
#endif
    
    //Сравнение с учЁтом регистра симВОЛов
    int compareWithCase (HString &);
    int compareWithCase (char *);
#if defined _UNICODE
    int compareWithCase (TCHAR *);
#else
#endif
    
    HString tokenize (HString &, int &);
    HString tokenize (char *, int &);
#if defined _UNICODE
    HString tokenize (TCHAR *, int &);
#else
#endif
    
    //Дублирование строкИ
    HString dup () { return HString (*this); }
    BYTE *dup (BYTE *src)
    {
        switch (type ())
        {
            case MBSTR:
                return (BYTE *) _mbsdup ((const unsigned char *) src);
                break;
            case WCSTR:
                return (BYTE *) _wcsdup ((TCHAR *) src);
                break;
            default:
                return  NULL;
        }
    }
        
    //Замена подстроки
    int replace (BYTE *, STRING_TYPE , BYTE *, STRING_TYPE );
    int replace (char *, char *);
#if defined _UNICODE
    int replace (char *, TCHAR *);
    int replace (TCHAR *, TCHAR *);
    int replace (TCHAR *, char *);
#else
#endif
    
    //Проверка вхождения сироки в строку
    int indexOf (HString &src) { return find (0, src); }
    int indexOf (char *src) { return find (0, src); }
#if defined _UNICODE
    int indexOf (TCHAR *src) { return find (0, src); }
#else
#endif
    //Поиск СТРОКи, начинаЯ с символа с номерОМ
    static int HString::find (int , BYTE *, STRING_TYPE , BYTE *, STRING_TYPE );
    //Поиск СТРОКи, начинаЯ с символа с номерОМ
    int find (int, BYTE *, STRING_TYPE);
    int find (int, HString &);
    //Поиск СТРОКи, начинаЯ с символа с номерОМ
    int find (int, char *);
#if defined _UNICODE
    int find (int, TCHAR *);
#else
#endif
    //Поиск строки, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
    void find (int, char *, int &);
#if defined _UNICODE
    void find (int, TCHAR *, int &);
#else
#endif
    //Поиск СИМВОЛа, начинаЯ с символа с номерОМ
    int find (int , char );
#if defined _UNICODE
    int find (int , TCHAR );
#else
#endif
    //Поиск СИМВОЛа, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
    void find (int , char , int &);
#if defined _UNICODE
    void find (int , TCHAR , int &);
#else
#endif
    int reverse (void);
    static BYTE *reverse (BYTE *, STRING_TYPE, BOOL = false);

    //Поиск СТРОКи, начинаЯ с окончания строкИ
    int reverseFind (BYTE *);
    //Поиск СТРОКи, начинаЯ с окончания строкИ
    int reverseFind (char *);
#if defined _UNICODE
    int reverseFind (TCHAR *);
#else
#endif
    //Поиск СИМВОЛа, начинаЯ с окончания строкИ
    int reverseFind (char );
#if defined _UNICODE
    int reverseFind (TCHAR );
#else
#endif
    
    //Добавление символа к строке
    int appendChar (char);
#if defined _UNICODE
    int appendChar (TCHAR);
#else
#endif
    //Добавление цифры к строке
    int appendNumber (DWORD);
	int appendNumber (DWORDLONG);
    int appendNumber (float);
    int appendNumber (double);
    
    int number (void);
#if defined _UNICODE
    static int number (TCHAR *);
    static TCHAR *number (int);
#else
     static int number (char *);
     static char *number (int);
#endif
    //Возврат LP(W)STR. Необходимо приведение типА СЛЕВа
    inline BYTE *getData () { return m_pData; }
    char *getMBData (void) { if (! (m_strType == MBSTR)) to (MBSTR); else ; return (char *) m_pData; }
    TCHAR *getWCData (void) { if (! (m_strType == WCSTR)) to (WCSTR); else ; return (TCHAR *) m_pData; }
    char getMBElement (int p) { if (! (m_strType == MBSTR)) to (MBSTR); else ; if ((p > -1) && (p < m_length)) return *(((char *) m_pData) + p); else return -1; }
    TCHAR getWCElement (int p) { if (! (m_strType == WCSTR)) to (WCSTR); else ; if ((p > -1) && (p < m_length)) return *(((TCHAR *) m_pData) + p); else return -1; }
    
#if defined _MBCS
    char *GetBuffer () { return getMBData (); }
    char GetAt (int pos) { return getMBElement (pos); }
#else
#if defined _UNICODE
    TCHAR *GetBuffer () { return getWCData (); }
    TCHAR GetAt (int pos) { return getWCElement (pos); }
#else
#endif
#endif
    
    static BYTE *to (BYTE *, int , STRING_TYPE = EMPTYSTR, STRING_TYPE = EMPTYSTR, BOOL = false);
    int to (STRING_TYPE = EMPTYSTR);

	static CStringA wctomb (CStringW wcstr) {
        CStringA strARes;
        BOOL bSetCharDefault = false;
        int length_wcstr = wcstr.GetLength ();
        strARes.GetBufferSetLength (length_wcstr);
        WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK, wcstr.GetBuffer (), length_wcstr, strARes.GetBuffer (), length_wcstr, NULL, &bSetCharDefault);
        
        return strARes;
    }
    
    static CStringW mbtowc (CStringA mbstr) {
        CStringW strWRes;
        BOOL bSetCharDefault = false;
        int length_mbstr = mbstr.GetLength ();
        strWRes.GetBufferSetLength (length_mbstr);
        MultiByteToWideChar (CP_ACP, WC_COMPOSITECHECK, mbstr.GetBuffer (), length_mbstr, strWRes.GetBuffer (), length_mbstr);
        
        return strWRes;
    }
    
private:
	BYTE *m_pData;
    //std::auto_ptr <BYTE> m_pData;
    STRING_TYPE m_strType;
    int m_length;

    STRING_TYPE getType (BYTE *);

    int append (BYTE *, STRING_TYPE , STRING_TYPE);
    int insert (int , BYTE *, STRING_TYPE , STRING_TYPE);
    
    BYTE *elementToPtr (BYTE *ptrElement) {
        BYTE *ptrRes = NULL;
    
        if (szElement () > 0)
        {
            ptrRes = DEBUG_NEW BYTE [2 * szElement ()];
            memset (ptrRes, 0x0, 2 * szElement ());
            memcpy (ptrRes, ptrElement, szElement ());
        }
        else
            ;
            
        return ptrRes;
    }

    DECLARE_DYNCREATE (HString)
};

#define HDEBUG_NEW(ptr, type, count) { \
								if (! (ptr == NULL)) { TRY { delete ptr; } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL; ptr = NULL; } else ; \
								while (ptr == NULL) { TRY { ptr = DEBUG_NEW type [count]; } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL } \
								if (AfxIsValidString ((LPCSTR) ptr, sizeof (type) * count)) \
									memset (ptr, 0x0, count * sizeof (type)); \
								else throw -1; \
								}

#define MFC_STRING(src) CString (src.GetBuffer ())

#define HSTRING(src) HString ((BYTE *) src)
//Вариант №1
//#define MB_HSTRING(src) ((char *) src.getData ())
//#define WC_HSTRING(src) ((TCHAR *) src.getData ())
//Вариант №2
#define MB_HSTRING(src) src.getMBData ()
#define WC_HSTRING(src) src.getWCData ()
//Вариант №3
//#define MB_HSTRING(src) (src.type () == HString::MBSTR ? (char *) src.getData () : (char *) HString::to (src.getData (), src.type (), HString::MBSTR))
//#define WC_HSTRING(src) (src.type () == HString::WCSTR ? (TCHAR *) src.getData () : (TCHAR *) HString::to (src.getData (), src.type (), HString::WCSTR))

#define MB_AT(src, pos) src.getMBElement (pos)
#define WC_AT(src, pos) src.getWCElement (pos)

#define _WC_TO_MB(str) HString::wctomb (str)
#define _MB_TO_WC(str) HString::mbtowc (str)

char *_mbsnew (char *);
TCHAR *_wcsnew (TCHAR *);

char *_mbsadd (char *, char *, BOOL = false);
TCHAR *_wcsadd (TCHAR *, TCHAR *, BOOL = false);

char *_mbsadddw (char *, DWORD, BOOL = false);
char *_mbsadddw (char *, DWORD, DWORD &, BOOL = false);

TCHAR *_wcsadddw (TCHAR *, DWORD, BOOL = false);
TCHAR *_wcsadddw (TCHAR *, DWORD, DWORD &, BOOL = false);