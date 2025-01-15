#pragma once

/*
* 一般来说，x86 系列 CPU 都是 little-endian 的字节序，PowerPC 通常是 big-endian，网络字节顺序也是 big-endian还有的CPU 能通过跳线来设置 CPU 工作于 Little endian 还是 Big endian 模式。

对于0x12345678的存储：

小端模式：（从低字节到高字节）
地位地址 0x78 0x56 0x34 0x12 高位地址

大端模式：（从高字节到低字节）
地位地址 0x12 0x34 0x56 0x78 高位地址
 */
class FEndianOpt
{
public:
	static void Init();
	template<typename Type>
	static void GetValueFromBuffer(uint8* pBuffer, Type& nValue)
	{
		uint8 nLen = sizeof(nValue);
		CopyBuffer((uint8*)&nValue, pBuffer, nLen);
	}

	static void CopyBuffer(uint8* pDest, uint8* pBuffer, int nLen);
	static void ReverseBuffer(uint8* pDest, uint8* pBuffer, int nLen);

	static bool m_bLittleEndian;
	static bool m_defaultLittleEndian;  //默认小端

	template<typename Type>
	static Type HostToNetwork(Type nValue)
	{
		if(!m_bLittleEndian)
			return nValue;
		uint8 nLen = sizeof(nValue);
		if(nLen == 1)
			return nValue;
		Type nRe;
		ReverseBuffer((uint8*)&nRe, (uint8*)&nValue, nLen);
		return nRe;
	}

	template<typename Type>
	static Type NetworkToHost(Type nValue)
	{
		if(!m_bLittleEndian)
			return nValue;
		uint8 nLen = sizeof(nValue);
		if(nLen == 1)
			return nValue;
		Type nRe;
		ReverseBuffer((uint8*)&nRe, (uint8*)&nValue, nLen);
		return nRe;
	}
};