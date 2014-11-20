#pragma once
//#include "stdafx.h"

#if ! defined _HA2KW_H_
#define _HA2KW_H_

class HA2KW : public CObject
{
private:
    unsigned char alt2_windows [256];
    unsigned char windows_alt2 [256];

    unsigned char alt2_koi8 [256];
    unsigned char koi8_alt2 [256];
	/*
	static const BYTE m_Alfabeta866 [] = "с©жгЄҐ­Јий§екдлўЇа®«¤¦нпзб¬ЁвмЎор‰–“Љ…Ќѓ™‡•љ”›‚ЂЏђЋ‹„†ќџ—‘Њ€’њЃћ",
                        m_Alfabeta1251 [] = "ёйцукенгшщзхъфывапролджэячсмитьбюЁЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
    */
protected:
    
public:
    //ncodein - тип кодировки исходного текста
	//1 - альтернативная #2
	//2 - KOI-8
	//3 - стандартная WINDOWS
	enum
	{
		CODE_RUSSIAN_ALTERNATIVE2 = 666,
		CODE_RUSSIAN_KOI8,
		CODE_RUSSIAN_WINDOWS
	};
    HA2KW(void);
    ~HA2KW(void);
    
    void Code (char *, int = CODE_RUSSIAN_WINDOWS, int = CODE_RUSSIAN_ALTERNATIVE2);
};

#define DECLARE_HA2KW HA2KW a2kw;
#define EXT_DECLARE_HA2KW extern HA2KW a2kw;
#define _WA2(src) a2kw.Code(src)
#define _A2W(src) a2kw.Code(src, CODE_RUSSIAN_ALTERNATIVE2, CODE_RUSSIAN_WINDOWS)
#define _A2K(src) a2kw.Code(src, CODE_RUSSIAN_ALTERNATIVE2, CODE_RUSSIAN_KOI8)
#define _KA2(src) a2kw.Code(src, CODE_RUSSIAN_KOI8, CODE_RUSSIAN_ALTERNATIVE2)
#define _KW(src) a2kw.Code(src, CODE_RUSSIAN_KOI8, CODE_RUSSIAN_WINDOWS)
#define _WK(src) a2kw.Code(src, CODE_RUSSIAN_WINDOWS, CODE_RUSSIAN_KOI8)

#else
#endif //_HA2KW_H_
