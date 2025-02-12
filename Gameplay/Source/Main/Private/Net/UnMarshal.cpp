#include "UnMarshal.h"

#include <stdexcept>

#include "LuaManger.h"

FUnMarshal::FUnMarshal()
{
	m_nPos = 0;
	m_RealSize = 0;
	m_bUsering = false;
}
FUnMarshal::FUnMarshal(const TArray<uint8>&data,bool bMove)
{
	m_bUsering = false;
	m_RealSize = 0;
	AttachArray(data,bMove, data.Num());
}

FUnMarshal::FUnMarshal(uint8* buffer, int Len)
{
	m_nPos = 0;
	m_RealSize = 0;
	PushBuffer(buffer,Len);
}

void FUnMarshal::AttachArray(const TArray<uint8>&data, bool bMove, int Len)
{
	m_nPos = 0;
	if(bMove)
	{
		m_bytes = std::move(data);
		m_RealSize = m_bytes.Num();
	}
	else
	{
		if(Len <= 0 || Len > data.Num())
			Len = data.Num();
		int nNowBytesNum = m_bytes.Num();
		if(nNowBytesNum < Len)
			m_bytes.Append(data.GetData(), Len - nNowBytesNum);
		else if(nNowBytesNum >= Len*2)
		{
			m_bytes.RemoveAt(Len, nNowBytesNum - Len);
		} 
		FMemory::Memcpy(m_bytes.GetData(), data.GetData(),Len);	
		if(Len > 0)
			m_RealSize = Len;
		else
			m_RealSize = m_bytes.Num();
	}
}

void FUnMarshal::PushBuffer(uint8* buffer, int Len, bool bAppend)
{
	if(buffer == nullptr || Len <= 0)
	{
		m_nPos = m_RealSize;
		return;
	}
	if(bAppend)
	{
		m_bytes.Append(buffer, Len);
		FMemory::Memcpy(m_bytes.GetData() + m_RealSize, buffer, Len);
		m_RealSize += Len;
	}
	else
	{
		int nNowBytesNum = m_bytes.Num();
		if(nNowBytesNum < Len)
			m_bytes.Append(buffer, Len - nNowBytesNum);
		else if(nNowBytesNum >= Len*2)
		{
			m_bytes.RemoveAt(Len, nNowBytesNum - Len);
		}
		FMemory::Memcpy(m_bytes.GetData(), buffer, Len);
		m_RealSize = Len;
		m_nPos = 0;
	}
}

uint8* FUnMarshal::Consume(size_t size,bool& bResult)
{
	if(m_nPos + size <= m_RealSize)
	{
		uint8* ptr = m_bytes.GetData() + m_nPos;
		m_nPos += size;
		bResult = true;
		return ptr;
	}
	bResult = false;
	UE_LOG(LogTemp, Error, TEXT("[msg]unmarshal FUnMarshal::Consume out of range pos=%d,size=%lld,len=%d"), m_nPos, size, m_RealSize);
	return nullptr;
}

void FUnMarshal::ReadStr(std::string& oav,bool& bResult)
{
	int len = Read<uint32>(bResult);
	if (len > 0)
	{
		oav.resize(len);
		ReadBytes(&oav.front(), len, bResult);
	}
}
void FUnMarshal::ReadBytes(void* oav, int len,bool& bResult)
{
	uint8* ptr = Consume(len, bResult);
	if (bResult && len > 0)
		::memcpy(oav, ptr, len);
}

void FUnMarshal::SetUseing(bool bValue)
{
	FPlatformAtomics::InterlockedExchange(&m_bUsering, bValue?1:0);	
}

void FUnMarshal::ResetPos(int nPos)
{
	if(nPos < 0)
		m_nPos = 0;
	else
		m_nPos = nPos;
}