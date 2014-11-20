#pragma once
#pragma once
#include "stdafx.h"

class HFileIni : public CObject
{
public:
	HFileIni(void);
	HFileIni(WCHAR *);
	HFileIni(char *);
	~HFileIni(void);

	int IsReasonably (void);

	CString GetParametrValue (TCHAR *);
	CString GetParametrName (int);
	int GetParametrsCount (void);
	
	//Для переноса в наследуемый КЛАСС ???
	void OutParametrToLog (void);
	void CorrectParametrValue (void);

protected:
	CArray <CStringA, CStringA> m_arstrAParametrName;
	CArray <CStringA, CStringA> m_arstrAParametrValue;
	
	int m_iReasonably;

	BOOL FindParametrBegin (CStringA &, int );
	BOOL IsParametrRemark (CStringA &, int );
	BOOL IsParametrPart (CStringA &, int );
	BOOL IsParametrNameDuplicate (CStringA &);
	int InitializeParametrName (CStringA &);
	int InitializeParametrValue (CStringA &);
	BOOL SetParametrValue (char *, CStringA &, CStringA &);

private:
};
