#pragma once
#include "Thread/Thread.h"
#include "Thread/GameThread.h"
#include "Types/DMemory.h"
#include "Types/BitStream.h"

class DSocket;

class DClient
{
public:
	DClient();
	~DClient();
	void AsyncConnect(const DString& Address, int Port);

	// Callback on non-game thread
	DAction<bool> OnConnect; // bool = success

	void SendRPC(uint64_t ObjectNetworkId, DBitStream&& BitStream);

protected:
	DUniquePtr<class DTCPConnection> TcpConnection;
	void TCPRun();

	bool bMustClose = false;
	bool bIsConnected = false;

	bool bPendingConnect = false;
	DString PendingAddress = ""; 
	int PendingPort = 0;

	DUniquePtr<DSocket> TCPSocket;
	DUniquePtr<DThread> TCPThread;

	void HandleTCPPacket(DVector<uint8_t> Payload);
};

