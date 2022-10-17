#include "DiabolicalEngine.h"
#include "DSocketWindows.h"

#if PLATFORM_WINDOWS
#include "Logging/Logging.h"
#include "DSocket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

union SocketAddressUnion
{
	struct
	{
		uint8_t b1, b2, b3, b4;
	} UnionBytes;
	struct
	{
		uint16_t w1, w2;
	} UnionShorts;
	uint32_t Address;
};

struct SocketAddress
{
	short Family;
	unsigned short Port;
	SocketAddressUnion InAddress;
	char Zero[8];
};

int DSocketWindows::Close()
{
	LOG("Socket closed");
	int Status = 0;
	Status = shutdown(Socket, SD_BOTH);
	if (Status == 0) { Status = closesocket(Socket); }	
	Socket = 0;
	return Status;
}

bool DSocketWindows::CreateTCP()
{
	Check(!Socket);
	if (Socket) return false;

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	Check(Socket != INVALID_SOCKET);
	if (Socket) return true;
	return false;
}

bool DSocketWindows::CreateUDP()
{
	Check(!Socket);
	if (Socket) return false;

	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	Check(Socket != INVALID_SOCKET);
	if (Socket) return true;
	return false;
}

bool DSocketWindows::Connect(DString Address, int Port)
{
	SocketAddress Server;
	Server.InAddress.Address = inet_addr(*Address);
	if (Server.InAddress.Address == INADDR_NONE)
	{
		struct hostent* remoteHost = gethostbyname(*Address);
		if (!remoteHost)
		{
			LOG_ERR(DString::Format("Could not resolve remote host (%s)", *Address));
			return false;
		}
		if (remoteHost->h_addrtype != AF_INET)
		{
			LOG_ERR(DString::Format("Tried to connect to remote host (%s) which is non-IPv4", *Address));
			return false;
		}
		Server.InAddress.Address = *(uint32_t*)remoteHost->h_addr_list[0];
	}
	Server.Family = AF_INET;
	Server.Port = htons(Port);

	int ConnectionStatus = connect(Socket, (struct sockaddr*)&Server, sizeof(Server));
	if (ConnectionStatus != 0)
	{
		LOG_WARN("Could not connect to remote address");
		return false;
	}

	return true;
}

void DSocket::InitSockets()
{
	WSADATA wsa;
	int Sock = WSAStartup(MAKEWORD(2, 2), &wsa);
	Check(Sock == 0);
	if (Sock != 0)
	{
		LOG_ERR("WinSocket Not Initialized");
		abort();
	}
	LOG("WinSocket Initialized");
}

bool DSocketWindows::Send(const char* Buffer, int Count)
{
	int SendVal = send(Socket, Buffer, Count, 0);
	if (SendVal != SOCKET_ERROR) return true;
	return false;
}

bool DSocketWindows::Receive(char* Buffer, int BufferLength, int& RecievedBytes)
{
	RecievedBytes = recv(Socket, Buffer, BufferLength, 0);
	if (RecievedBytes < 0 || RecievedBytes == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

void DSocketWindows::SetRecieveTimeoutMillis(uint32_t Millis)
{
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&Millis, sizeof(int));
}

void DSocketWindows::SetSendTimeoutMillis(uint32_t Millis)
{
	setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (char*)&Millis, sizeof(int));
}

bool DSocketWindows::StartListening()
{
	if (listen(Socket, SOMAXCONN) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DSocketWindows::Bind(DString LocalAddress, int Port)
{
	SocketAddress Server;
	Server.InAddress.Address = inet_addr(*LocalAddress);
	if (Server.InAddress.Address == INADDR_NONE)
	{
		LOG_ERR(DString::Format("Could not bind to local address %s", *LocalAddress));
		return false;
	}
	Server.Family = AF_INET;
	Server.Port = htons(Port);

	if (bind(Socket, (struct sockaddr*)&Server, sizeof(Server)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DSocketWindows::AcceptConnection(DUniquePtr<DSocket>& NewSocket, DString* IncomingAddress)
{
	SocketAddress SAddress;
	int AddrSize = sizeof(SAddress);
	uint64_t NewConnection = accept(Socket, (struct sockaddr*)&SAddress, &AddrSize);

	if (NewConnection == INVALID_SOCKET)
	{
		return false;
	}

	NewSocket = DUniquePtr<DSocket>(new DSocketWindows(NewConnection));

	if (IncomingAddress)
	{
		char* ip = inet_ntoa(*(in_addr*)&SAddress.InAddress);
		if (ip)
		{
			*IncomingAddress = DString(ip);
		}
	}

	return true;
}

DUniquePtr<DSocket> DSocket::CreateSocket()
{
	return DUniquePtr<DSocket>(new DSocketWindows);
}

#endif
