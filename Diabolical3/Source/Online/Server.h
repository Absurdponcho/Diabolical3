#pragma once
#include "Thread/Thread.h"
#include "Thread/GameThread.h"
#include "Online/Socket/DSocket.h"
#include "TCPConnection.h"
#include "Types/DVector.h"
#include "Types/DMemory.h"
#include "Types/Map.h"
#include "Types/BitStream.h"

class DSocket;

struct SServerClientData
{
	DUniquePtr<class DTCPConnection> TcpConnection;

	// Used by the server to track which objects are relevant to which clients
	DMap<uint64_t, DObjectPtr<class DObject>> RelevantNetObjects;
	DMap<uint64_t, DObjectPtr<class DObject>> RelevantNetSubObjects;

	DVector<uint64_t> OwnershipNetObjects;
	uint64_t ClientId = 0;
};

class DServer
{
public:
	DServer();
	~DServer();

	void AsyncListen(const DString& LocalAddress, int Port);

	// Callback on non-game thread
	DAction<SServerClientData*> OnConnection; // DSocket = new socket, DString = incoming IP

	DVector<SServerClientData>& GetClientData()
	{
		return ClientConnections;
	}

	void ClientInitializeDObject(SServerClientData& ClientData, DObjectPtr<class DObject> Test);
	void ClientRemoveDObject(SServerClientData& ClientData, uint64_t ObjectId);

	void ClientInitializeDObjectSub(SServerClientData& ClientData, DObjectPtr<class DObject> Test);
	void ClientRemoveDObjectSub(SServerClientData& ClientData, uint64_t ObjectId);

	void SendDirtySyncVars();
	void SendBroadcastRPC(uint64_t ObjectNetworkId, DBitStream&& BitStream);
	void SendRPC(DVector<uint64_t> Targets, uint64_t ObjectNetworkId, DBitStream&& BitStream);
	void SetNetOwnership(DVector<uint64_t> Targets, uint64_t ObjectNetworkId, bool bOwned);
	void EnsureSubObjectRelevancy();
	void EnsureSubObjectRelevancyForClient(SServerClientData& ClientData);
	void EnsureSubObjectRelevancyForObject(SServerClientData& ClientData, DObjectPtr<class DObject> Object);
	void EnsureSubObjectRelevancyForObjectAllClients(DObjectPtr<class DObject> Object);

	void Disconnect(uint64_t ClientId) {};

protected:
	DVector<SServerClientData> ClientConnections;

	void TCPRun();

	bool bMustClose = false;
	bool bIsBound = false;

	bool bPendingBind = false;
	DString PendingAddress = "";
	int PendingPort = 0;

	DUniquePtr<DSocket> TCPSocket;
	DUniquePtr<DThread> TCPThread;
	void HandleTCPPacket(uint64_t ClientId, DVector<uint8_t> Payload);

};
