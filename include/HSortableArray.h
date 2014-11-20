#pragma once
#include "afxcoll.h"

class HSortableArray : public CObArray
{
public:
	HSortableArray(void);

	void Sort(int(*CompareFunc)(CObject* pFirst, CObject* pSecond));
	void Sort(int iStartPos, int iElements, int(*CompareFunc)(CObject* pFirst, CObject* pSecond));

public:
	~HSortableArray(void);
};

template < class TYPE > class HTypedSortableArray : public HSortableArray
{
public:
	// Accessing elements
	TYPE GetAt(int nIndex) const
	{ return (TYPE)HSortableArray::GetAt(nIndex); }
	TYPE& ElementAt(int nIndex)
	{ return (TYPE&)HSortableArray::ElementAt(nIndex); }
	void SetAt(int nIndex, TYPE ptr)
	{ HSortableArray::SetAt(nIndex, ptr); }
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, TYPE newElement)
	{ HSortableArray::SetAtGrow(nIndex, newElement); }
	int Add(TYPE newElement)
	{ return HSortableArray::Add(newElement); }
	int Append(const CTypedPtrArray< HSortableArray, TYPE >& src)
	{ return HSortableArray::Append(src); }
	void Copy(const CTypedPtrArray< HSortableArray, TYPE >& src)
	{ HSortableArray::Copy(src); }
	
	// Operations that move elements around
	void InsertAt(int nIndex, TYPE newElement, int nCount = 1)
	{ HSortableArray::InsertAt(nIndex, newElement, nCount); }
	void InsertAt(int nStartIndex, HTypedSortableArray< TYPE >* pNewArray)
	{ HSortableArray::InsertAt(nStartIndex, pNewArray); }
	
	// overloaded operator helpers
	TYPE operator[](int nIndex) const
	{ return (TYPE)HSortableArray::operator[](nIndex); }
	TYPE& operator[](int nIndex)
	{ return (TYPE&)HSortableArray::operator[](nIndex); }
	
	void Sort( int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj) )
	{ HSortableArray::Sort((int(*)(CObject*,CObject*))CompareFunc); }
	void Sort( int iStartPos, int iElements, int(*CompareFunc)(TYPE pFirstObj, TYPE pSecondObj) )
	{ HSortableArray::Sort(iStartPos, iElements, (int(*)(CObject*,CObject*))CompareFunc); }
};
