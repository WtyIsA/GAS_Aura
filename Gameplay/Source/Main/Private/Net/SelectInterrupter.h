#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"

class CSelectInterrupter
{
public:
	CSelectInterrupter();
	~CSelectInterrupter();
	bool	OpenInterrupter();
	uint64	GetSocket();
	void	RecvData();
	void	Interrupt();
private:
	void	Reset();

private:
	FSocket*	m_readSocket;
	FSocket*	m_wirteSocket;
};