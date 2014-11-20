#include "StdAfx.h"
#include "HSortableArray.h"

HSortableArray::HSortableArray(void)
{
}

HSortableArray::~HSortableArray(void)
{
}

#define STRIDE_FACTOR 3

void HSortableArray::Sort(int(*CompareFunc)(CObject* pFirst, CObject* pSecond))
{
	// CompareFunc is expected to return a positive integer if pFirstObj
	// should follow pSecondObj (is greater than)
	
	// Uses Shell Sort
	
	// Basically it does a bunch of smaller insertion sorts than insertion sorts the
	//  whole thing.  Insertion sorting is much faster on a list that is already
	//  mostly sorted.
	
	// ** NOTE:  Because GetSize() is called to retrieve the number of elements, you should
	//            call SetSize() with the number of valid elements.  An alternative is
	//            shown in the sort function below.
	
	ASSERT_VALID(this);
	
	BOOL bFound;
	int iElements = GetSize();
	int iInner,iOuter,iStride = 1;
	CObject *pTmp;
	
	while (iStride <= iElements)
		iStride = iStride * STRIDE_FACTOR + 1;
	
	while (iStride > (STRIDE_FACTOR - 1))
	{
		iStride = iStride / STRIDE_FACTOR;
		for (iOuter = iStride; iOuter < iElements; iOuter++)
		{
			bFound = 0;
			iInner = iOuter - iStride;
			while ((iInner >= 0) && !bFound)
			{
				if (CompareFunc(m_pData[iInner+iStride],m_pData[iInner]) < 0)
				{
					pTmp = m_pData[iInner+iStride];
					m_pData[iInner+iStride] = m_pData[iInner];
					m_pData[iInner] = pTmp;
					iInner -= iStride;
				}
				else
					bFound = 1;
			}
		}
	}
}

void HSortableArray::Sort(int iStartPos, int iElements, int(*CompareFunc)(CObject* pFirst, CObject* pSecond))
{
	// This variation allows you to sort only a portion of the array
	
	ASSERT_VALID(this);
	ASSERT( iStartPos >= 0 && iStartPos <= GetUpperBound() );
	ASSERT( GetSize() - iStartPos >= iElements );
	
	BOOL bFound;
	int iInner,iOuter,iStride = 1;
	CObject *pTmp;
	CObject **pData = &m_pData[iStartPos];
	
	while (iStride <= iElements)
		iStride = iStride * STRIDE_FACTOR + 1;
	
	while (iStride > (STRIDE_FACTOR - 1))
	{
		iStride = iStride / STRIDE_FACTOR;
		for (iOuter = iStride; iOuter < iElements; iOuter++)
		{
			bFound = 0;
			iInner = iOuter - iStride;
			while ((iInner >= 0) && !bFound)
			{
				if (CompareFunc(pData[iInner+iStride],pData[iInner]) < 0)
				{
					pTmp = pData[iInner+iStride];
					pData[iInner+iStride] = pData[iInner];
					pData[iInner] = pTmp;
					iInner -= iStride;
				}
				else
					bFound = 1;
			}
		}
	}
}


