#pragma once
#include <string>

#include "Cfg/EndianOpt.h"

class FUnMarshal
{
	FUnMarshal();
 	FUnMarshal(uint8* buffer, int Len);	
 	FUnMarshal(const TArray<uint8>&data, bool bMove);
	~FUnMarshal(){}
public:
	friend class CMessagePool;
	void	PushBuffer(uint8* buffer, int Len, bool bAppend =false);
	void	AttachArray(const TArray<uint8>&data, bool bMove, int Len=0);
	//uint8	ReadByte(){return Sread<uint8>(Consume(1)); }
	
	template <typename _Ntype>  _Ntype Read(bool& bResult)
	{		
		return Sread<_Ntype>(Consume(sizeof(_Ntype), bResult));
	}
	
	void	ReadStr(std::string& oav,bool& bResult);
	void	ReadBytes(void* oav, int len,bool& bResult);
	int		Length()const{return m_RealSize;}
	int		GetArrayLen()const{return m_bytes.Num();}
	bool	IsUsing()const {return m_bUsering > 0;}
	void	SetUseing(bool bValue);
	void	ResetPos(int nPos = 0);
	uint8*	GetData(){return m_bytes.GetData();}
	int		GetPos() const {return  m_nPos;}
protected:
	uint8*	Consume(size_t size,bool& bResult);
	template <typename _Ntype> static _Ntype Sread(const void* ptr)
	{
		if(ptr == nullptr)
			return 0;
		_Ntype value;
		FMemory::Memcpy(&value, ptr, sizeof(value));
		return FEndianOpt::NetworkToHost(value);
	}
private:
	TArray<uint8>	m_bytes;
	int				m_nPos;
	int				m_RealSize;  //实际使用了的大小
	int8			m_bUsering;
};