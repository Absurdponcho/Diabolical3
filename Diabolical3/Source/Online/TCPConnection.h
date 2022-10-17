#pragma once
#include "Thread/Thread.h"
#include "Thread/GameThread.h"
#include "Online/Socket/DSocket.h"
#include "NetTypes.h"
#include "Thread/ThreadsafeContainer.h"
#include "Types/DMemory.h"

class DTCPConnection
{
public:
	DTCPConnection(DUniquePtr<DSocket>& NewSocket, DString IncomingIP);
	virtual ~DTCPConnection();

	const DString& GetIP() const { return RemoteIP; } 
	void QueueTCPSendBuffer(DUniquePtr<DNetBuffer>&& Buffer);

	void SendPing();

	DAction<DVector<uint8_t>> OnCompletePacketReceived;

protected:

	// Using a unique_ptr to heap allocated buffers. saves us from doing HUGE buffer copies. Thread will have full ownership.
	// during mutex lock, only ptr needs to be swapped so its much better than copying buffers
	DThreadsafeContainer<DVector<DUniquePtr<DNetBuffer>>> TCPSendBuffers;

	bool PopTCPSendBuffer(DUniquePtr<DNetBuffer>& Buffer);
	void PopCompletePacket(uint32_t Length);

	bool bMustClose = false;
	void TCPSend();
	void TCPReceive();

	DString RemoteIP = "";

	DUniquePtr<DSocket> TCPSocket;
	DUniquePtr<DThread> TCPSendThread;
	DUniquePtr<DThread> TCPReceiveThread;

	DVector<uint8_t> ReceiveBuffer;
};