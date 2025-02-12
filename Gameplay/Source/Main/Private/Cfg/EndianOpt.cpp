#include "EndianOpt.h"

bool FEndianOpt::m_defaultLittleEndian = false;
bool FEndianOpt::m_bLittleEndian = true;

void FEndianOpt::Init()
{
	int i = 1;   
	char *p = (char *)&i;   
	if(*p == 1)     
		m_bLittleEndian = true;
	else
		m_bLittleEndian = false;
}

void FEndianOpt::CopyBuffer(uint8* pDest, uint8* pBuffer, int nLen)
{
	if(pDest == nullptr || pBuffer == nullptr)
		return;
	if(nLen == 1)
		FMemory::Memcpy(pDest, pBuffer, nLen);
	else
	{
		if(m_defaultLittleEndian == m_bLittleEndian)
		{
			FMemory::Memcpy(pDest, pBuffer,nLen);
		}
		else
		{
			//小端大端互转
			for(uint8 i = 0; i < nLen; i++)
			{
				FMemory::Memcpy(pDest + i , pBuffer + (nLen - 1 -i),1);
			}
		}
	}
}

void FEndianOpt::ReverseBuffer(uint8* pDest, uint8* pBuffer, int nLen)
{
	if(pDest == nullptr || pBuffer == nullptr)
		return;
	for(uint8 i = 0; i < nLen; i++)
	{
		FMemory::Memcpy(pDest + i , pBuffer + (nLen - 1 -i),1);
	}
}