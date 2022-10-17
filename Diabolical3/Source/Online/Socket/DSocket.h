#pragma once
#include "Types/DString.h"
#include "Types/DMemory.h"
#include "DiabolicalEngine.h"

class DSocket
{
public:
	virtual ~DSocket() {};

	virtual void SetRecieveTimeoutMillis(uint32_t Millis) = 0;
	virtual void SetSendTimeoutMillis(uint32_t Millis) = 0;
	virtual bool CreateTCP() = 0;
	virtual bool CreateUDP() = 0;
	virtual bool Connect(DString Address, int Port) = 0;
	virtual int Close() = 0;
	virtual bool Send(const char* Buffer, int Count) = 0;
	virtual bool Receive(char* Buffer, int BufferLength, int& RecievedBytes) = 0;
	virtual bool StartListening() = 0;
	virtual bool Bind(DString LocalAddress, int Port) = 0;
	virtual bool AcceptConnection(DUniquePtr<DSocket>& NewSocket, DString* IncomingAddress = nullptr) = 0;
	static void InitSockets();
	static DUniquePtr<DSocket> CreateSocket();

};

#if PLATFORM_WINDOWS
#include "DSocketWindows.h"
#endif