#pragma once

#if PLATFORM_WINDOWS
#include "DSocket.h"

class DSocketWindows : public DSocket
{
public:
	virtual ~DSocketWindows() {};
	DSocketWindows() : Socket(0) {}
	DSocketWindows(uint64_t EstablishedConnection) : Socket(EstablishedConnection) {}

	virtual void SetRecieveTimeoutMillis(uint32_t Millis) override;
	virtual void SetSendTimeoutMillis(uint32_t Millis) override;
	virtual bool CreateTCP() override;
	virtual bool CreateUDP() override;
	virtual bool Connect(DString Address, int Port) override;
	virtual int Close() override;
	virtual bool Send(const char* Buffer, int Count) override;
	virtual bool Receive(char* Buffer, int BufferLength, int& RecievedBytes) override;
	virtual bool StartListening() override;
	virtual bool Bind(DString LocalAddress, int Port) override;
	virtual bool AcceptConnection(DUniquePtr<DSocket>& NewSocket, DString* IncomingAddress = nullptr) override;

private:
	uint64_t Socket = 0;
};

#endif