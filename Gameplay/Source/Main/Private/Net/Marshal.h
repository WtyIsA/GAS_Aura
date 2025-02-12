#pragma once
#include <string>

#include "Cfg/EndianOpt.h"

class FMarshal
{
	FMarshal();
 	FMarshal(bool bUseing);
	~FMarshal(){};
public:
	friend class CMessagePool;
	template <typename _Ntype> void Write(_Ntype value)
	{
		_Ntype nv = FEndianOpt::HostToNetwork(value);
		WriteBytes(&nv, sizeof(nv));
	}
	
	void	WriteStr(std::string& value);
	void	WriteStr(const FString& str);
	void	WriteStr(const char* pBuffer,int len);
	void	WriteOctets(const FMarshal* const oc);
	void	CopyBinary(const char* pBuffer,int len);
	
	void	Pack();
	uint8*	GetData(){return m_bytes.GetData();}
	int		Length()const{return m_nPos;}
	void	RemoveLen(int nLen);
	TArray<uint8>& GetBytes(){return m_bytes;}

	bool	IsUsing()const {return m_bUsering > 0;}
	void	SetUseing(bool bValue);
	void	ResetPos(int nPos = 4);
	void	RemoveNotUser();
	
protected:	
	void WriteBytes(const void* d, int n);
private:
	TArray<uint8>	m_bytes;
	int				m_nPos;
	int8			m_bUsering;
};