#include "Sdk/SdkInterface.h"

#include "SteamSdkInterface.h"
FSdkInterface* FSdkInterface::m_SdkInterface = nullptr;
FSdkInterface* FSdkInterface::GetIns()
{
	if(m_SdkInterface == nullptr)
	{
		m_SdkInterface = new FSteamSdkInterface();
	}
	
	return m_SdkInterface;
}

void FSdkInterface::SetSdkInterface(FSdkInterface* pIns)
{
	m_SdkInterface = pIns;
}
