#include "stdafx.h"
//#include <hstring.h>

IMPLEMENT_DYNAMIC (HString, CObject)

HString::HString (BYTE *src, STRING_TYPE newType) : CObject ()
{
    if (newType == EMPTYSTR)
        m_strType = getType (src);
    else
        m_strType = newType;
    
    switch (m_strType)
    {
        case MBSTR:
            m_pData = (BYTE *) _mbsnew ((char *) src);
            m_length = _mbslen (m_pData);
            //m_strType = MBSTR;
            break;
        case WCSTR:
            m_pData = (BYTE *) _wcsnew ((TCHAR *) src);
            m_length = wcslen ((TCHAR *) m_pData);
            //m_strType = WCSTR;
            break;
        case UNICODESTR:
            break;
        default:
            ;
    }
}

int HString::init (TCHAR *src, STRING_TYPE type)
{
    int iRes = 0; //Success

    if (free () == 0) {
        m_pData = (BYTE *) _wcsnew (src);
        m_length = wcslen ((TCHAR *) m_pData);
        m_strType = WCSTR;
    }
    else
    {
        m_length = 0;
        m_strType = EMPTYSTR;
    
        iRes = -1;
    }
    
    return iRes;
}

int HString::init (char *src, STRING_TYPE type)
{
    int iRes = 0; //Success

    if (free () == 0)
    {
        m_pData = (BYTE *) _mbsnew (src);
        m_length = _mbslen ((const unsigned char *) m_pData);
        m_strType = MBSTR;
    }
    else
    {
        m_length = 0;
        m_strType = EMPTYSTR;
    
        iRes = -1;
    }
    
    return iRes;
}

int HString::init (LPCSTR src, STRING_TYPE type) { return init ((char *) src, type); }

int HString::init (LPCTSTR src, STRING_TYPE type) { return init ((TCHAR *) src, type); }

int HString::setLength (int len, char val) {
    int iRes = 0; //Success
    BYTE *tmp = NULL;
	int i = -1;
    
    if (len > -1) {
        if (len == 0) {
            free (false);
            init (type ());
        }
        else
        {
            if (m_length == 0) {
				if (! (type () == EMPTYSTR)) {
					HDEBUG_NEW (m_pData, BYTE, (len + 1) * szElement ());
				}
                else {
#if defined _MBCS
                    HDEBUG_NEW (m_pData, BYTE, (len + 1) * sizeof (char));
					for (i = 0; i < len; i ++) { *(((unsigned char *) m_pData) + i) = val; } *(((unsigned char *) m_pData) + i) = 0x0;
                    m_strType = MBSTR;
                    
#else
#if defined _UNICODE
//#if defined _DEBUG
//					if (len == 52)
//						AfxDebugBreak ();
//					else
//						;
//#else
//#endif
					HDEBUG_NEW (m_pData, BYTE, (len  + 1) * sizeof (TCHAR));
					for (i = 0; i < len; i ++) { *(((TCHAR *) m_pData) + i) = val; } *(((TCHAR *) m_pData) + i) = 0x0;
                    m_strType = WCSTR;
#else
#endif
#endif
                }
                
                m_length = len + 1;
            }
            else {
                switch (type ()) {
                    case MBSTR:
                        tmp = (BYTE *) _mbsdup ((unsigned char *) m_pData);
                        break;
                    case WCSTR:
                        tmp = (BYTE *) _wcsdup ((TCHAR *) m_pData);
                        break;
                    default:
                        ;
                }

                HDEBUG_NEW (m_pData, BYTE, (len + 1) * szElement ());
                //memset (m_pData, 0x0, (len + 1) * szElement ());
				switch (type ()) {
                    case MBSTR:
						for (i = 0; i < len; i ++) { *(((unsigned char *) m_pData) + i) = val; } *(((unsigned char *) m_pData) + i) = 0x0;
                        break;
                    case WCSTR:
                        for (i = 0; i < len; i ++) { *(((TCHAR *) m_pData) + i) = val; } *(((TCHAR *) m_pData) + i) = 0x0;
                        break;
                    default:
                        ;
                }
                
                if (len > m_length)
                    memcpy (m_pData, tmp, m_length * szElement ());
                else
                    memcpy (m_pData, tmp, len * szElement ());
                    
                if (! (tmp == NULL))
                    delete [] tmp;
                else
                    ;

                m_length = len;
				iRes = m_length;
            }
        }
    }
    else
        iRes  = -1;
        
    return iRes;
}

int HString::actualizeLength (void)
{
    int iTmp = -1;
    BYTE *tmp = NULL;
    if (m_length)
    {
        tmp = dup (m_pData);

        switch (type ()) {
            case MBSTR:
                iTmp = _mbslen ((unsigned char *) tmp);
                break;
            case WCSTR:
                iTmp = wcslen ((TCHAR *) tmp);
                break;
            default:
                ;
        }
        
        free ();
        
        setLength (iTmp);
        memcpy (m_pData, tmp, m_length * szElement ());
        delete tmp;
    }
    else
        ;
    
    return m_length;
}

//Освобождение памяти (полное/НЕ полное - сохранить (изменить на НОВый) ТИП строКи)
int HString::free  (BOOL bFull, STRING_TYPE newType)
{
    if (! (m_pData == NULL)) {
        try { delete m_pData; }
        catch (CException *err) {
            m_length = -1;
        }
        
        m_pData = NULL;
    }
    else
        ;

    if (! bFull) {
        if (! (newType == EMPTYSTR)) {
            m_strType = newType;
        }
        else
            ;
    }
    else {
        m_strType = EMPTYSTR;
    }
    
    m_length = 0;
        
    return m_length;
}


//Определение типа строки в runtime
HString::STRING_TYPE HString::getType (BYTE *src)
{
    STRING_TYPE res = EMPTYSTR;
    unsigned short int i = 0;
    
    while (* (src + i)) { i ++; }
    
    return res;
}

HString HString::right (int len)
{ //len < m_length ???
	HString strRes;
    if ((len > m_length) || (len < 0))
        ;
    else
        strRes = HString (m_pData + (m_length - len) * szElement (), type ());

	return strRes;
}

HString HString::left (int len)
{ //len < m_length ???
    HString strRes;
	if ((len < 0) || (len > m_length))
		;
	else {
		BYTE *pData = NULL;
		HDEBUG_NEW (pData, BYTE, (len + 1) * szElement ());
		memcpy (pData, m_pData, len * szElement ());
    
		strRes = HString (pData, type ());
		delete [] pData;
	}
    
    return strRes;
}

HString HString::mid (int pos, int len) {
    HString strRes;
    BYTE *pData = NULL;

	if (len < 0)
		len = m_length - pos;
	else
		;

	if ((pos + len) < (m_length + 1)) {
		HDEBUG_NEW (pData, BYTE, (len + 1) * szElement ());
		memset (pData, 0x0, (len + 1) * szElement ());
		memcpy (pData, m_pData + pos * szElement (), len * szElement ());
	    
		strRes = HString (pData, type ());
		delete [] pData;
	}
	else
		;
    
    return strRes;
}

int HString::upr (void) {
    int iRes = 0;
    
    switch (type ())
    {
        case MBSTR:
            m_pData = (BYTE *) _mbsupr ((unsigned char *) m_pData);
            break;
        case WCSTR:
            m_pData = (BYTE *) _wcsupr ((TCHAR *) m_pData);
            break;
        default:
            ;
    }
    
    return iRes;
}

int HString::lwr (void)
{
    int iRes = 0;
    
    switch (type ()) {
        case MBSTR:
            m_pData = (BYTE *) _mbslwr ((unsigned char *) m_pData);
            break;
        case WCSTR:
            m_pData = (BYTE *) _wcslwr ((TCHAR *) m_pData);
            break;
        default:
            ;
    }
    
    return iRes;
}

int HString::trim (void) {
    int iRes = trimLeft ();
    if (iRes > -1)
        iRes += trimRight ();
    else
        ;
    
    return iRes;
}

int HString::trimLeft (void)
{
    int iRes = 0;

    if (m_length > 0)
    {
        switch (type ())
        {
            case MBSTR:
                while (((int) *(((unsigned char *) m_pData) + iRes) < 33) && (iRes < m_length))
                    iRes ++;
                break;
            case WCSTR:
                while (((int) *(((TCHAR *) m_pData) + iRes) < 33) && (iRes < m_length))
                    iRes ++;
                break;
            default:
                ;
        }
        
        //return iRes;
        iRes = cut (0, iRes);
    }
    else
        iRes = 0;
        
    return iRes;
}

int HString::trimRight (void)
{
    int iRes = m_length;

    if (iRes > 0) {
        switch (type ()) {
            case MBSTR:
                while ((int) *(((unsigned char *) m_pData) + (iRes - 1)) < 33)
                    iRes --;
                break;
            case WCSTR:
                while ((int) *(((TCHAR *) m_pData) + (iRes - 1)) < 33)
                    iRes --;
                break;
            default:
                ;
        }
        
        //return iRes;
        iRes = cut (iRes, m_length - iRes);
    }
    else
        iRes = 0;
        
    return iRes;
}

//Сравнение БЕз учЁтА регистра симВОЛов
int HString::compareNoCase (HString &src)
{
    int iRes = 0;
    
    BYTE *srcCopy = to (src.m_pData, src.length (), src.m_strType, type ());
    
    switch (type ())
    {
        case MBSTR:
            iRes =  _mbsicmp ((unsigned char *) m_pData, (const unsigned char *) srcCopy);
            break;
        case WCSTR:
            iRes =  _wcsicmp ((TCHAR *) m_pData, (TCHAR *) srcCopy);
            break;
        default:
            ;
    }

    delete [] srcCopy;
    
    return iRes;
}

int HString::compareNoCase (char *src)
{
    HString tmp (src);
    return compareNoCase (tmp);
}

int HString::compareNoCase (TCHAR *src)
{
    HString tmp (src);
    return compareNoCase (tmp);
}

//Сравнение с учЁтом регистра симВОЛов
int HString::compareWithCase (HString &src)
{
    int iRes = 0;
    
    BYTE *srcCopy = to (src.m_pData, src.length (), src.m_strType, type ());
    
    switch (type ())
    {
        case MBSTR:
            iRes =  _mbscmp ((unsigned char *) m_pData, (const unsigned char *) srcCopy);
            break;
        case WCSTR:
            iRes =  wcscmp ((TCHAR *) m_pData, (TCHAR *) srcCopy);
            break;
        default:
            ;
    }

    delete [] srcCopy;
    
    return iRes;
}

int HString::compareWithCase (char *src)
{
    HString tmp (src);
    return compareWithCase (tmp);
}

int HString::compareWithCase (TCHAR *src)
{
    HString tmp (src);
    return compareWithCase (tmp);
}

HString HString::tokenize (HString &token, int &pos)
{
    HString strRes;
    BYTE *tokenCopy = NULL,
         *thisCopy = NULL,
         *intermediate = NULL, *ret = NULL;

	if (pos < 0)
		pos = 0;
	else
		;

    if (pos < m_length) {
        tokenCopy = to (token.m_pData, token.length (), token.m_strType, type ());
        thisCopy = dup (m_pData);
        
        switch (type ())
        {
            case MBSTR:
                ret = (BYTE *) _mbstok_s (((unsigned char *) thisCopy) + pos, (unsigned char *) tokenCopy, &((unsigned char *) intermediate));
                break;
            case WCSTR:
                ret = (BYTE *) wcstok_s (((TCHAR *) thisCopy) + pos, (TCHAR *) tokenCopy, (TCHAR **) &intermediate);
                break;
            default:
                ;
        }
        
        pos = (intermediate - thisCopy) / szElement ();
        
		if (! (ret == 0x0)) {
			strRes = HString (ret, type ());
		}
		else
#if defined _DEBUG
			AfxDebugBreak ()
#else
#endif
			;
        
        delete [] tokenCopy;
        delete [] thisCopy;
    }
    else {
        pos = -1;
        strRes = "";
    }

    return strRes;
}

HString HString::tokenize (char *token, int &pos)
{
    return tokenize (HString (token), pos);
}

HString HString::tokenize (TCHAR *token, int &pos)
{
    return tokenize (HString (token), pos);
}

int HString::replace (BYTE *what, STRING_TYPE whatType, BYTE *than, STRING_TYPE thanType)
{
    int iRes = find (0, what, whatType);
    
    if (iRes > -1)
    {
		if (cut (iRes, _len (what, whatType)) > -1) {
            iRes = insert (iRes, than, thanType, EMPTYSTR);
        }
        else
            ;
    }
    else
        ;
        
    return iRes;
}

//Замена подстроки
int HString::replace (char *what, char *than)
{
    return replace ((BYTE *) what, MBSTR, (BYTE *) than, MBSTR);
}
//Замена подстроки
int HString::replace (char *what, TCHAR *than)
{
    return replace ((BYTE *) what, MBSTR, (BYTE *) than, WCSTR);
}
//Замена подстроки
int HString::replace (TCHAR *what, TCHAR *than)
{
    return replace ((BYTE *) what, WCSTR, (BYTE *) than, WCSTR);
}
//Замена подстроки
int HString::replace (TCHAR *what, char *than)
{
    return replace ((BYTE *) what, WCSTR, (BYTE *) than, MBSTR);
}

//Поиск СТРОКи, начинаЯ с символа с номерОМ
int HString::find (int posStart, BYTE *strIn, STRING_TYPE typeIn, BYTE *strMatch, STRING_TYPE typeMatch)
{
    int posRes = -1,
        szElement = -1;
    BOOL bDelete = false;
    
    BYTE *tmp = NULL, *ptrRes = NULL;
    
	if ((posStart > -1) && (! (strIn == 0x0)) && (! (strMatch == 0x0))) {
		if ((! (typeIn == typeMatch)) && (! (typeIn == EMPTYSTR)) && (! (typeMatch == EMPTYSTR)))
		{
			tmp = to ((BYTE *) strMatch, -1, typeMatch, typeIn);
			bDelete = true;
		}
		else
			tmp = strMatch;
	    
		switch (typeIn)
		{
			case MBSTR:
				ptrRes = (BYTE *) _mbsstr (((unsigned char *) strIn) + posStart, (unsigned char *) tmp);
				szElement = sizeof (char);
				break;
			case WCSTR:
				ptrRes = (BYTE *) wcsstr (((TCHAR *) strIn) + posStart, (TCHAR *) tmp);
				szElement = sizeof (TCHAR);
				break;
			default:
				;
		}
	    
		if (! (ptrRes == NULL))
			posRes = (ptrRes - strIn) / szElement;
		else
			;
	    
		if (bDelete)
			delete [] tmp;
		else
			;
	        
		tmp = NULL;
	}
	else
		;
    
    return posRes;
}
//Поиск СТРОКи, начинаЯ с символа с номерОМ
int HString::find (int posStart, HString &match) { return find (posStart, m_pData, type (), match.getData (), match.type ()); }
//Поиск СТРОКи, начинаЯ с символа с номерОМ
int HString::find (int posStart, BYTE *strIn, STRING_TYPE typeIn) { return find (posStart, m_pData, type (), strIn, typeIn); }
//Поиск СТРОКи, начинаЯ с символа с номерОМ
int HString::find (int posStart, char *strIn) { return find (posStart, (BYTE *) strIn, MBSTR); }
//Поиск СТРОКи, начинаЯ с символа с номерОМ
int HString::find (int posStart, TCHAR *strIn) { return find (posStart, (BYTE *) strIn, WCSTR); }

//Поиск строки, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
void HString::find (int posStart, char *strIn, int &posOut) { posOut = find (posStart, strIn); }

//Поиск строки, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
void HString::find (int posStart, TCHAR *strIn, int &posOut) { posOut = find (posStart, strIn); }

//Поиск СИМВОЛа, начинаЯ с символа с номерОМ
int HString::find (int posStart, char ch)
{
    int resPos = -1;
    BYTE *tmp = elementToPtr ((BYTE *) &ch);
    
    if (! (tmp == NULL))
    {
        //resPos = find (posStart, tmp, type ());
        resPos = find (posStart, m_pData, type (), tmp, type ());
        delete [] tmp;
    }
    else
        ;
    
    return resPos;
}
//Поиск СИМВОЛа, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
int HString::find (int posStart, TCHAR wch)
{
    int resPos = -1;
    BYTE *tmp = NULL;
    
    switch (type ())
    {
        case MBSTR:
            break;
        case WCSTR:
            tmp = elementToPtr ((BYTE *) &wch);
            break;
        default:
            ;
    }
    
    if (! (tmp == NULL))
    {
        //resPos = find (posStart, tmp, type ()); ТО ЖЕ самое
        //resPos = find (posStart, tmp, WCSTR); ТО ЖЕ самое
        resPos = find (posStart, m_pData, type (), tmp, type ());
        delete [] tmp;
    }
    else
        ;
    
    return resPos;
}
//Поиск СИМВОЛа, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
void HString::find (int posStart, char ch, int &posOut) { posOut = find (posStart, ch); }
//Поиск СИМВОЛа, начинаЯ с символа с номерОМ; рез-т возвращается в АРГумент
void HString::find (int posStart, TCHAR wch, int &posOut) { posOut = find (posStart, wch); }

//Обратный порядок символов
int HString::reverse (void)
{
    int iRes = -1;
    m_pData = reverse (m_pData, type (), true);

    if (! (m_pData == NULL))
        iRes = 0;
    else
    {
        //m_length = 0;
        //m_strType = EMPTYSTR;
    }
     
    return iRes;
}

//Обратный порядок символов
BYTE *HString::reverse (BYTE *srcIn, STRING_TYPE typeIn, BOOL bClear)
{
    BYTE *ptrRes = NULL;
    
    switch (typeIn)
    {
        case MBSTR:
            ptrRes = _mbsdup (srcIn);
            ptrRes = (BYTE *) _mbsrev ((unsigned char *) ptrRes);
            break;
        case WCSTR:
            ptrRes = (BYTE *) _wcsdup ((TCHAR *) srcIn);
            ptrRes = (BYTE *) _wcsrev ((TCHAR *) ptrRes);
            break;
        default:
            ;
    }
    
    if (bClear)
        delete [] srcIn;
    else
        ;
    
    return ptrRes;
}


int HString::reverseFind (BYTE *src)
{
    int iRes = -1;
    BYTE *tmp = reverse (m_pData, type ());
        
    iRes = find (0, tmp, type (), src, type ());
    delete [] tmp;
    
    return m_length - iRes - 1;
}

//Поиск СТРОКи, начинаЯ с окончания строкИ
int HString::reverseFind (char *srcIn)
{
    return reverseFind ((BYTE *) srcIn);
}
//Поиск СТРОКи, начинаЯ с окончания строкИ
int HString::reverseFind (TCHAR *srcIn)
{
    return reverseFind ((BYTE *) srcIn);
}
//Поиск СИМВОЛа, начинаЯ с окончания строкИ
int HString::reverseFind (char ch)
{
    int iRes = -1;
    BYTE *tmp = elementToPtr ((BYTE *) &ch);
    
    iRes = reverseFind (tmp);
    delete [] tmp;
    
    return iRes;
}
//Поиск СИМВОЛа, начинаЯ с окончания строкИ
int HString::reverseFind (TCHAR wch)
{
    int iRes = -1;
    BYTE *tmp = elementToPtr ((BYTE *) &wch);
    
    iRes = reverseFind (tmp);
    delete [] tmp;
    
    return iRes;
}

int HString::append (BYTE *srcIn, STRING_TYPE srcType, STRING_TYPE newType)
{
    int iRes = 0; //Success
    BYTE *src = NULL;
    
    if (isNull () == 1)
        init ("", newType);
    else
        ;
    
    //Приведение к ЕДИНомУ типУ - ??? в 'insert'
    if (newType == EMPTYSTR)
        ;
    else {
        if (! (m_strType == newType)) {
            to (newType);
        }
        else
            ;
    }
    
    src = to (srcIn, -1, srcType, m_strType);
    if (! (srcType == m_strType))
    {
        srcType = m_strType;
    }
    else
        ;
    
    switch (m_strType) //srcType == m_strType = TRUE
    {
        case MBSTR:
            m_pData = (BYTE *) _mbsadd ((char *) m_pData, (char *) src, true);
            m_length += _mbslen ((unsigned char *) src);
            break;
        case WCSTR:
            m_pData = (BYTE *) _wcsadd ((TCHAR *) m_pData, (TCHAR *) src, true);
            m_length += wcslen ((TCHAR *) src);
            break;;
        case UNICODESTR:
            break;
        default:
            ;
    }
    
    delete [] src;

    return m_length;
}

//Добавление к имеющейся строке другую. 2-ой параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::append (HString &src, STRING_TYPE type)
{
    return append (src.getData (), src.type (), type);
}

//Добавление к имеющейся строке другую. 2-ой параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::append (char *src, STRING_TYPE type)
{
    return append ((BYTE *) src, MBSTR, type);
}

//Добавление к имеющейся строке другую. 2-ой параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::append (TCHAR *src, STRING_TYPE type)
{
    return append ((BYTE *) src, WCSTR, type);
}

//Вставить в имеющююся строкУ другую
int HString::insert (int pos, BYTE *srcIn, STRING_TYPE srcType, STRING_TYPE newType)
{
    int iRes = 0; //Success
    BYTE *src = NULL, *tmp = NULL;
    
    //Приведение к ЕДИНомУ типУ - ??? в 'append'
    if (newType == EMPTYSTR)
    {//Оставляем исходный для текущей строки
        if (isNull () == 1)
        {
            switch (srcType)
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
        }
        else
            ;
    }
    else
    {
        if (! (m_strType == newType))
        {//Приводим к типУ, который указан в аргументе
            to (newType); //'m_strType' - изменяется внутри 'to'
        }
        else
            ;
    }
    //Статическая 'to' вернЁт либо дубликат, либо преобразованнУю к нужному ('m_strType') строку
    src = to (srcIn, -1, srcType, m_strType);
    if (! (srcType == m_strType))
    {
        srcType = m_strType;
    }
    else
        ;
    
    switch (m_strType) //srcType == m_strType = TRUE
    {
        case MBSTR:
            //Дубликат строки
            HDEBUG_NEW (tmp, BYTE, (m_length + 1) * sizeof (char));
            memcpy (tmp, m_pData, m_length * sizeof (char));
            //Память под новую строку
            HDEBUG_NEW (m_pData, BYTE, (m_length + _mbslen ((const unsigned char *) src) + 1) * sizeof (char));
            //Копирование до 'pos'
            memcpy (m_pData, tmp, pos * sizeof (char));
            //Собственно вставка
            memcpy (m_pData + pos * sizeof (char), src, _mbslen ((const unsigned char *) src) * sizeof (char));
            //Копирование после 'pos' (оставшаЯся часть)
            memcpy (m_pData + (pos + _mbslen ((const unsigned char *) src)) * sizeof (char), tmp + pos * sizeof (char), (m_length - pos) * sizeof (char));
            //Определение длины
            m_length += _mbslen ((const unsigned char *) src); //m_length = mb_len (m_pData);
            break;
        case WCSTR:
            //Дубликат строки
            HDEBUG_NEW (tmp, BYTE, (m_length + 1) * sizeof (TCHAR));
            memcpy (tmp, m_pData, m_length * sizeof (TCHAR));
            //Память под новую строку
            HDEBUG_NEW (m_pData, BYTE, (m_length + wcslen ((TCHAR *) src) + 1) * sizeof (TCHAR));
            memcpy (m_pData, tmp, pos * sizeof (TCHAR));
            memcpy (m_pData + pos * sizeof (TCHAR), src, wcslen ((TCHAR *) src) * sizeof (TCHAR));
            memcpy (m_pData + (pos + wcslen ((TCHAR *) src)) * sizeof (TCHAR), tmp + pos * sizeof (TCHAR), (m_length - pos) * sizeof (TCHAR));
            m_length += wcslen ((TCHAR *) src); //m_length = mb_len (m_pData);
            break;
        default:
            ;
    }
    
    delete [] src;
    delete [] tmp;

    return m_length;
}

//Вставить в имеющююся строкУ другую. 3-ий параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::insert (int pos, HString &src, STRING_TYPE type)
{
    return insert (pos, (BYTE *) src.getData (), src.type (), type);
}
//Вставить в имеющююся строкУ другую. 3-ий параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::insert (int pos, char *src, STRING_TYPE type)
{
    return insert (pos, (BYTE *) src, MBSTR, type);
}
//Вставить в имеющююся строкУ другую. 3-ий параметр определяет тип строки-результата (EMPTYSTR - оставить тип строки в исходном состоянии)
int HString::insert (int pos, TCHAR *src, STRING_TYPE type)
{
    return insert (pos, (BYTE *) src, WCSTR, type);
}

//Вырезать количество 'c' элементов, начиная с 'pos' (уменьшить длину на 'c')
int HString::cut (int pos, int c)
{
    int iRes = 0; //Success
    BYTE *tmp = NULL;

	//DEBUG Проверка условия
	/*if (((pos + c) < (m_length + 1)) && (pos > -1) && (c > 0))
		;
	else {
		iRes = 0;
	}*/

    //if (((pos + c) < (m_length + 1)) && (pos > -1) && (c > 0)) {
	//if (((pos + c) < (m_length + 1)) && (true) && (true)) {
	if (((pos + c) < (m_length + 1)) && (pos > -1) && (c > -1)) {
		if (c > 0) {
			tmp = dup (m_pData);
			if (! (setLength (m_length - c) == (m_length - c))) {
				memcpy (m_pData, tmp, pos * szElement ());
				memcpy (m_pData + pos * szElement (), tmp + (pos + c) * szElement (), (m_length - pos) * szElement ());
			}
			else {
				free ();
				iRes = -1;
			}
				
			delete [] tmp;
		}
		else
			;
        
        iRes = c;
    }
    else
        iRes = -1;
    
    return iRes;
}

//Добавление символа к строке
int HString::appendChar (char ch)
{
    int iRes = 0;
    
    BYTE *src = NULL;
	HDEBUG_NEW (src, BYTE, 2);
    memcpy (src, &ch, sizeof (char));
    iRes = append (src, MBSTR, EMPTYSTR);
    delete [] src;
    
	//iRes = append ((BYTE *) &ch, WCSTR, EMPTYSTR);

    return iRes;
}

//Добавление символа к строке
int HString::appendChar (TCHAR ch)
{
    int iRes = 0;
    
    BYTE *src = NULL;
	HDEBUG_NEW (src, BYTE,  2 * sizeof (TCHAR));
    memcpy (src, &ch, sizeof (TCHAR));
    iRes = append (src, WCSTR, EMPTYSTR);
    delete [] src;
    
	//iRes = append ((BYTE *) &ch, WCSTR, EMPTYSTR);

    return iRes;
}

//Добавление цифры к строке
int HString::appendNumber (DWORDLONG val) {
	return m_length;
}

//Добавление цифры к строке
int HString::appendNumber (DWORD val)
{
    DWORD retCnt = -1;
    switch (m_strType)
    {
        case MBSTR:
            //Вариант №1
            /*m_pData = (BYTE *) _mbsadddw ((char *) m_pData, val, true);
            m_length = _mbslen ((const unsigned char *) m_pData);*/
            //Вариант №2
            m_pData = (BYTE *) _mbsadddw ((char *) m_pData, val, retCnt, true);
            break;
        case WCSTR:
            //Вариант №1
            /*m_pData = (BYTE *) _wcsadddw ((TCHAR *) m_pData, val, true);
            m_length = wcslen ((TCHAR *) m_pData);*/
            //Вариант №2
            m_pData = (BYTE *) _wcsadddw ((TCHAR *) m_pData, val, retCnt, true);
            break;
        default:
#if defined _MBCS
			init ("");
			m_pData = (BYTE *) _mbsadddw ((char *)
#else
			init (_T (""));
			m_pData = (BYTE *) _wcsadddw ((TCHAR *)
#endif
			m_pData, val, retCnt, true);
            ;
    }
    //Вариант №2
    if (retCnt > 0)
        m_length += retCnt;
    else
        ; // ??? Ошибка
    
    return m_length;
}

//Добавление цифры к строке
int HString::appendNumber (float val) {
	/*DWORD retCnt = -1;
    switch (m_strType) {
        case MBSTR:
			break;
		case WCSTR:
			break;
		default:
			;
	}

	char *res = NULL, *tmp = NULL;
	TRY { tmp = DEBUG_NEW char [13]; }
	CATCH_ALL (err) { THROW (err); }
	END_CATCH_ALL
    _ftoa_s (src2,; tmp, 13, 10);
    pdwOutCnt = -1;
    
    res = _mbsadd (src1, tmp);
    if (! (res == NULL))
        pdwOutCnt = _mbslen ((unsigned char *) tmp);
    else
        ;
        
    delete [] tmp;
    
    if (bClear && (! (src1 == NULL))) delete src1;
    
    return res;*/

    return m_length;
}

//Добавление цифры к строке
int HString::appendNumber (double val)
{
    return m_length;
}

int HString::number (void)
{
    int iRes = -1;
    
    switch (type ())
    {
        case MBSTR:
            iRes = atoi ((char *) m_pData);
            break;
        case WCSTR:
            iRes = _wtoi ((TCHAR *) m_pData);
            break;
        default:
            ;
    }
    
    return iRes;
}

static int number (char *src) { return atoi (src); }

static int number (TCHAR *src) { return _wtoi (src); }
    
//http://www.ehow.com/how_2190103_use-variable-parameter-functions-c.html#ixzz24Fpuh7vU
void HString::format (char *frmt, ...)
{
    free (false, MBSTR);
    
    int i = -1;
    char * vl;
    hva_start (vl, frmt);
    
    VAR_ARGS any;
    
    /*any.i = hva_arg (vl, int);
    any.pmb = hva_arg (vl, char *);
    any.c = hva_arg (vl, char);
    any.i = hva_arg (vl, int);
    
    hva_start (vl, frmt);*/
    
    for (i = 0; frmt [i] != '\0'; ++i)
    {    
        if (frmt [i] == '%')
        {
            i ++;
        
            if (frmt [i] == 'i') {
                any.i = hva_arg (vl, int);
                appendNumber ((DWORD) any.i);
            }
            else
                if (frmt [i] == 'l') {
					any.l = hva_arg (vl, DWORDLONG);
					appendNumber ((DWORDLONG) any.l);
				}
				else
					if (frmt [i] == 'f') {
						any.i = hva_arg (vl, float);
						appendNumber ((DWORD) any.i);
					}
					else
						if (frmt [i] == 'c') {
							any.c = hva_arg (vl, char);
							appendChar (any.c);
						}
						else
							if (frmt [i] == 's') {
								any.ptr = hva_arg (vl, void *);
								append ((char *) any.ptr);
							}
							else
								if (frmt [i] == 'S') {
									any.ptr = hva_arg (vl, void *);
									append ((TCHAR *) any.ptr);
								}
								else {
									//throw CException (true);
								}
        }
        else
            appendChar (frmt [i]);
    }

    hva_end (vl);
}

//http://www.ehow.com/how_2190103_use-variable-parameter-functions-c.html#ixzz24Fpuh7vU
void HString::format (TCHAR *frmt, ...)
{
    free (false, WCSTR);
    
    int i = -1;
    char * vl;
    hva_start (vl, frmt);
    
    VAR_ARGS any;

    for (i = 0; frmt [i] != '\0'; ++i)
    {    
        if (frmt [i] == '%')
        {
            i ++;
        
            if (frmt [i] == 'i')
            {
                any.i = hva_arg (vl, int);
                appendNumber ((DWORD) any.i);
            }
            else
				if (frmt [i] == 'l')
				{
					any.l = hva_arg (vl, DWORDLONG);
					appendNumber ((DWORDLONG) any.l);
				}
				else
					if (frmt [i] == 'd')
					{
						any.d = hva_arg (vl, double);
						appendNumber (any.d);
					}
					else
						if (frmt [i] == 'f')
						{
							any.f = hva_arg (vl, float);
							appendNumber (any.f);
						}
						else
							if (frmt [i] == 'c')
							{
								any.c = hva_arg (vl, char);
								appendChar (any.c);
							}
							else
								if (frmt [i] == 's')
								{
									any.ptr = hva_arg (vl, void *);
									append ((char *) any.ptr);
								}
								else
									if (frmt [i] == 'S')
									{
										any.ptr = hva_arg (vl, void *);
										append ((TCHAR *) any.ptr);
									}
									else
									{
										//throw CException (true);
									}
        }
        else
            appendChar (frmt [i]);
    }

    hva_end (vl);
}

//Преобразование строк ИЗ 'MB' в 'WC' и ОБРАТНо
int HString::to (STRING_TYPE newType)
{
    int iRes = 0; //Success
    
    if (! (newType == EMPTYSTR))
        if (! (newType == m_strType))
        {
            m_pData = to (m_pData, m_length, m_strType, newType, true);
            if (! (m_pData == NULL))
                m_strType = newType;
            else
                iRes = -1;
        }
        else
            ;
    else
        ;

    return iRes;
}
//Статическая функция преобразования строки к типу (2-ой аргумент) с освобождением памяти исходной строки (3-ий аргумент)
BYTE *HString::to (BYTE *src, int len, STRING_TYPE type, STRING_TYPE newType, BOOL bClear)
{
    BYTE *pRes = NULL;
    int iRes = 0; //Success
    unsigned short szRes = 0, szSymbol = 0;
    BOOL bSetCharDefault = false;

    /*if ((newType == EMPTYSTR) || (type == newType))
        pRes = src;
    else
    {*/ 
		if (len < 0) {    
			switch (type) {
				case WCSTR:
					TRY { szRes = wcslen ((TCHAR *) src); } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL
					break;
				case MBSTR:
					TRY { szRes = _mbslen ((unsigned char *) src); } CATCH_ALL (err) { THROW (err); } END_CATCH_ALL
					break;
				default:
					;
			}
		}
		else
			szRes = len;

        switch (newType)
        {
            case WCSTR:
                szSymbol = sizeof (TCHAR);
                break;
            case MBSTR:
                szSymbol = sizeof (char);
                break;
            default:
                ;
        }
        
        szRes *= szSymbol;
        szRes += szSymbol; //Для '0x0'
        
        HDEBUG_NEW (pRes, BYTE, szRes);
        
        //int szMBSTR = (szRes - szSymbol) / szSymbol, szWCSTR = (szRes - szSymbol) / szSymbol;
        
        switch (newType)
        {
            case MBSTR:
                switch (type)
                {
                    case MBSTR:
                        memcpy (pRes, src, szRes - szSymbol);
                        break;
                    case WCSTR:
                        iRes = WideCharToMultiByte (CP_ACP, WC_COMPOSITECHECK, (TCHAR *) src, szRes - 1, (LPSTR ) pRes,  szRes - 1, NULL, &bSetCharDefault);
                        break;
                    default:
                        break;
                }
                break;
            case WCSTR:
                switch (type)
                {
                    case MBSTR:
                        if (MultiByteToWideChar (CP_ACP, MB_PRECOMPOSED, (LPCSTR ) src, (szRes - szSymbol) / szSymbol, (TCHAR *) pRes, (szRes - szSymbol) / szSymbol) == 0)
                        {
                            iRes = GetLastError ();
                            switch (iRes)
                            {
                                case ERROR_INSUFFICIENT_BUFFER:
                                    TRACE ("ERROR_INSUFFICIENT_BUFFER");
                                    break;
                                case ERROR_INVALID_FLAGS:
                                    TRACE ("ERROR_INVALID_FLAGS");
                                    break;
                                case ERROR_INVALID_PARAMETER:
                                    TRACE ("ERROR_INVALID_PARAMETER");
                                    break;
                                case ERROR_NO_UNICODE_TRANSLATION:
                                    TRACE ("ERROR_NO_UNICODE_TRANSLATION");
                                    break;
                                default:
                                    ;
                            }
                        }
                        break;
                    case WCSTR:
                        memcpy (pRes, src, szRes - szSymbol);
                        break;
                    default:
                        break;
                }
                break;
            default:
                ;
        }
        
#if defined _DEBUG
        //if (szRes == 104)
        //{
        //    TRACE ("Requirement 104 bytes???\n");
        //    if ((pRes [0] == 0x21) && (pRes [1] == 0x4) && (pRes [2] == 0x3e) && (pRes [3] == 0x4) && (pRes [4] == 0x3e) && (pRes [5] == 0x4))
        //        TRACE ("Requirement 104 bytes - РЕШЕНИЕ???\n");
        //    else
        //        ;
        //}
        //else
        //    ;
#endif
    
        if (bClear)
            delete [] src;
        else
            ;
    //}
    
    return pRes;
}

//Создание новоГо LPSTR
char *_mbsnew (char *src)
{
    char *pRes = NULL;
    
    if (src == 0x0)
        ; //AfxDebugBreak ();
	else {
#if defined _DEBUG
		if (_mbstrlen (src) == 0)
			; //AfxDebugBreak ();
		else
			;
#else
#endif
		HDEBUG_NEW (pRes, char, _mbstrlen (src) + 1);
		memcpy (pRes, src, _mbstrlen (src) * sizeof (char));

		pRes [_mbstrlen (src)] = 0x0;
	}
    
    return (char *) pRes;
}

//Создание новоГо LPWSTR
TCHAR *_wcsnew (TCHAR *src)
{
    TCHAR *pRes = NULL;
    
	if (src) {
		HDEBUG_NEW (pRes, TCHAR , wcslen (src) + 1);

		memcpy (pRes, src, wcslen (src) * sizeof (TCHAR));
    
		pRes [wcslen (src)] = 0x0;
	}
	else
		;
    
    return (TCHAR *) pRes;
}

//Слияние src1 и src2
char *_mbsadd (char *src1, char *src2, BOOL bClear)
{
    char *res = NULL;
    short szSrc1 = 0, szSrc2 = 0;
	
	if (! (src1 == 0x0))
        szSrc1 = _mbslen ((unsigned char *) src1);
	else
		;

	if (! (src2 == 0x0))
		szSrc2 = _mbslen ((unsigned char *) src2);
	else
		;
        
	if ((szSrc1 + szSrc2) > 0) {
        HDEBUG_NEW (res, char, (szSrc1 + szSrc2) + 1);

        if (! (src1 == NULL))
            memcpy (res, src1, szSrc1 * sizeof (char));
        else
            ;
            
        if (! (src2 == NULL))
			memcpy (res + szSrc1, src2, szSrc2 * sizeof (char));
		else
			;

        res [szSrc1 + szSrc2] = 0x0;
        
        if (bClear) {
            if (! (src1 == NULL))
                delete [] src1;
            else
                ;
        }
        else
            ;
	}
    else
        ;

    return res;
}

//Слияние src1 и src2
TCHAR *_wcsadd (TCHAR *src1, TCHAR *src2, BOOL bClear)
{
    TCHAR *res = NULL;
    short szSrc1 = 0, szSrc2 = 0;
	if (! (src1 == NULL))
        szSrc1 = wcslen ((TCHAR *) src1);
	else
		;
    
	if (! (src2 == NULL))
		szSrc2 = wcslen ((TCHAR *) src2);
	else
		;
        
//#if defined _DEBUG
//        if ((szSrc1 + szSrc2) == 60)
//        {
//            ATLTRACE2 (atlTraceGeneral, 0, "122 байта ??? = %s, Адрес = %i; %s, Адрес = %i", src1, src1, src2, src2);
//        }
//        else
//            ;
//#endif

	if ((szSrc1 + szSrc2) > 0) {
        HDEBUG_NEW (res, TCHAR, (szSrc1 + szSrc2) + 1);

        if (! (src1 == NULL))
            memcpy (res, src1, szSrc1 * sizeof (TCHAR));
        else
            ;

		if (! (src2 == NULL))
			memcpy (res + szSrc1, src2, szSrc2 * sizeof (TCHAR));
		else
			;

        res [szSrc1 + szSrc2] = 0x0;
        
//#if defined _DEBUG
//        if ((szSrc1 + szSrc2) == 60)
//        {
//            ATLTRACE2 (atlTraceGeneral, 0, "==");
//            int i = 0;
//            while (*(res + i))
//            {
//                ATLTRACE2 (atlTraceGeneral, 0, "%c", *(res + i));
//                i ++;
//            }
//            ATLTRACE2 (atlTraceGeneral, 0, ", Адрес = %i==\n", res);
//            
//
//            //AfxDebugBreak ();
//        }
//        else
//            ;
//#endif
        
        if (bClear) {
            if (! (src1 == NULL))
                delete [] src1;
            else
                ;
        }
        else
            ;
	}
    else
        ;

    return res;
}

//Добавление числа типа 'int' к строке
char *_mbsadddw (char *src1, DWORD src2, BOOL bClear)
{
    char *res = NULL, *tmp = DEBUG_NEW char [13];
    _itoa_s (src2, tmp, 13, 10);
    
    res = _mbsadd (src1, tmp);    
    delete [] tmp;
    
    if (bClear && (! (src1 == NULL))) delete src1; else ;
    
    return res;
}
//Добавление числа типа 'int' к строке (с возвратом числа добавленных символов в 'pdwOutCnt')
char *_mbsadddw (char *src1, DWORD src2, DWORD &pdwOutCnt, BOOL bClear)
{
	char *res = NULL, *tmp = NULL;
	TRY { tmp = DEBUG_NEW char [13]; }
	CATCH_ALL (err) { THROW (err); }
	END_CATCH_ALL
    _itoa_s (src2, tmp, 13, 10);
    pdwOutCnt = -1;
    
    res = _mbsadd (src1, tmp);
    if (! (res == NULL))
        pdwOutCnt = _mbslen ((unsigned char *) tmp);
    else
        ;
        
    delete [] tmp;
    
    if (bClear && (! (src1 == NULL))) delete src1;
    
    return res;
}
//Добавление числа типа 'int' к строке
TCHAR *_wcsadddw (TCHAR *src1, DWORD src2, BOOL bClear)
{
    TCHAR *res = NULL, *tmp = DEBUG_NEW TCHAR [13];
    _itow_s (src2, tmp, 13, 10);
    
    res = _wcsadd (src1, tmp);
    delete [] tmp;
    
    if (bClear && (! (src1 == NULL))) delete src1;
    
    return res;
}

//Добавление числа типа 'int' к строке (с возвратом числа добавленных символов в 'pdwOutCnt')
TCHAR *_wcsadddw (TCHAR *src1, DWORD src2, DWORD &pdwOutCnt, BOOL bClear)
{
    TCHAR *res = NULL, *tmp = DEBUG_NEW TCHAR [13];
    _itow_s (src2, tmp, 13, 10);
    pdwOutCnt = -1;
    
    res = _wcsadd (src1, tmp);
    if (! (res == NULL))
        pdwOutCnt = wcslen ((TCHAR *) tmp);
    else
        ;
        
    delete [] tmp;
    
    if (bClear && (! (src1 == NULL))) delete src1; else ;
    
    return res;
}