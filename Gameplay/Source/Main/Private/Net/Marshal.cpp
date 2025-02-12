#include "Marshal.h"

#include "LuaManger.h"

FMarshal::FMarshal()
{
	m_bUsering = false;
	m_nPos = 0;
	int32 nLen = 0;
	WriteBytes(&nLen, 4);
}

FMarshal::FMarshal(bool bUseing)
{
	m_bUsering = bUseing;
	m_nPos = 0;
	int32 nLen = 0;
	WriteBytes(&nLen, 4);
}



void FMarshal::WriteBytes(const void* d, int n)
{
	if(m_nPos + n > m_bytes.Num())
		m_bytes.Append((uint8*)d,m_nPos + n - m_bytes.Num());
	uint8* pData = m_bytes.GetData();
	FMemory::Memcpy(pData + m_nPos,d,n );
	m_nPos += n;
}

void FMarshal::Pack()
{
	int32 nSize = FEndianOpt::HostToNetwork(m_nPos);
	uint8* pData = m_bytes.GetData();
	FMemory::Memcpy(pData, &nSize, 4);
	if(m_bytes.Num() > m_nPos * 2)
	{
		int nNum = m_bytes.Num();
		m_bytes.RemoveAt(m_nPos, nNum - m_nPos);
	}
	
}

void FMarshal::RemoveLen(int nLen)
{
	if(nLen < 0)
		return;
	if(nLen >= m_bytes.Num())
		m_bytes.Empty();
	else
		m_bytes.RemoveAt(0, nLen);
}

void FMarshal::WriteStr(const char* pBuffer,int len)
{
	if(pBuffer == nullptr)
		len = 0;
	this->Write<uint32>(len);
	if (len > 0)
		WriteBytes(pBuffer, len);
}

void FMarshal::WriteStr(std::string& value)
{
	uint32 len = static_cast<uint32>(value.size());
	this->Write(len);
	if (len > 0)
		WriteBytes(value.data(), len);
}

void FMarshal::WriteStr(const FString& str)
{
	const char* pBuffer = TCHAR_TO_UTF8(*str);
	WriteStr(pBuffer, strlen(pBuffer));
}


void FMarshal::WriteOctets(const FMarshal* const oc)
{
	if(oc == nullptr)
		return;
	Write<uint32>(oc->Length());
	WriteBytes(oc->m_bytes.GetData(), oc->Length());
}

void FMarshal::CopyBinary(const char* pBuffer, int len)
{
	WriteBytes(pBuffer, len);
}


void FMarshal::SetUseing(bool bValue)
{
	FPlatformAtomics::InterlockedExchange(&m_bUsering, bValue?1:0);	
}

void FMarshal::ResetPos(int nPos)
{
	if(nPos < 0)
		m_nPos = 4;
	else
		m_nPos = nPos;
}

void FMarshal::RemoveNotUser()
{
	if(m_bytes.Num() > m_nPos)
		m_bytes.RemoveAt(m_nPos, m_bytes.Num() - m_nPos );
}
