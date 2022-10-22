#include "Server.h"
#include "Check.h"
#include "World/Object.h"
#include "Types/BitStream.h"

DServer::DServer()
	:	TCPSocket(DSocket::CreateSocket()),
		TCPThread(std::make_unique<DThread>([=]() { TCPRun(); }))
{
	TCPSocket->CreateTCP();
}

DServer::~DServer()
{
	bMustClose = true;
	TCPThread->Join();
	TCPSocket->Close();
}

void DServer::AsyncListen(const DString& LocalAddress, int Port)
{
	PendingAddress = LocalAddress;
	PendingPort = Port;
	bPendingBind = true;
}

void DServer::TCPRun()
{
	while (!bMustClose)
	{
		if (!bIsBound)
		{
			if (bPendingBind)
			{
				bool bBound = TCPSocket->Bind(PendingAddress, PendingPort);
				if (!bBound)
				{
					LOG_ERR(DString::Format("Could not bind to %s:%i", *PendingAddress, PendingPort));
					bPendingBind = false;
					continue;
				}
				bool bListen = TCPSocket->StartListening();
				if (!bListen)
				{
					LOG_ERR(DString::Format("Could not listen on %s:%i", *PendingAddress, PendingPort));
					bPendingBind = false;
					continue;
				}
				bIsBound = true;

				bPendingBind = false;
			}

			if (!bIsBound)
			{
				DThread::Sleep(100);
			}
		}
		else
		{
			DUniquePtr<DSocket> NewConnection;
			DString IncomingIP = "";
			if (TCPSocket->AcceptConnection(NewConnection, &IncomingIP))
			{
				SServerClientData ServerClientData;
				ServerClientData.TcpConnection = std::make_unique<DTCPConnection>(NewConnection, IncomingIP);

				static uint64_t ClientIds = 1;
				ServerClientData.ClientId = ClientIds++;

				uint64_t ClientId = ServerClientData.ClientId;
				ServerClientData.TcpConnection->OnCompletePacketReceived = [this, ClientId](DVector<uint8_t> Payload)
				{
					this->HandleTCPPacket(ClientId, Payload);
				};

				ClientConnections.PushBack(std::move(ServerClientData));
				OnConnection.Invoke(&ClientConnections[ClientConnections.Size() - 1]);
			}
		}
	}
}

void DServer::HandleTCPPacket(uint64_t ClientId, DVector<uint8_t> Payload)
{
	DBitStream PayloadStream = DBitStream(std::move(Payload));

	DGameThread::Invoke(DAction<DBitStream>([ClientId](DBitStream BitStream)
		{
			uint8_t ReadCommand = 0;
			if (!BitStream.ReadByte(ReadCommand))
			{
				return;
			}
			ENetCommand Command = (ENetCommand)ReadCommand;

			switch (Command)
			{
			case ENetCommand::RPC:
				DEngine::ObjectManager->ProcessRPCPacketServer(ClientId, BitStream);
				break;
			default:
				Check(false);
			}
		}, PayloadStream));

}

void DServer::ClientInitializeDObject(SServerClientData& ClientData, DObjectPtr<class DObject> Test)
{
	if (ClientData.RelevantNetSubObjects.Contains(Test->GetNetworkId()))
	{
		ClientData.RelevantNetSubObjects.Remove(Test->GetNetworkId());
	}

	if (ClientData.RelevantNetObjects.Contains(Test->GetNetworkId()))
	{
		//LOG(DString::Format("Client already has been send relevant net object ID %i", Test.Get()->GetNetworkId()));
		return;
	}

	ClientData.RelevantNetObjects[Test->GetNetworkId()] = Test;
	
	DBitStream OutStream;
	OutStream.AddByte((uint8_t)ENetCommand::CreateObject);
	OutStream.AddPackedUInt(std::hash<DString>()(Test->GetRegisteredObject()->GetObjectName()));
	OutStream.AddPackedUInt(Test->GetNetworkId());

	DVector<DSyncVarBase*> SyncVars;
	Test.Get()->RegisterSyncVars(SyncVars);
	for (DSyncVarBase* SyncVar : SyncVars)
	{
		SyncVar->Serialize(OutStream);
		SyncVar->SetNotDirtyForConnection(ClientData.ClientId);
	}

	DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStream.GetByteCount());
	memcpy(Buffer->Buffer, OutStream.GetData(), OutStream.GetByteCount());

	ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
	//LOG(DString::Format("Server: Sent object Id %i", Test->GetNetworkId()));

	EnsureSubObjectRelevancyForObject(ClientData, Test);
}

void DServer::ClientRemoveDObject(SServerClientData& ClientData, uint64_t ObjectId)
{
	if (!ClientData.RelevantNetObjects.Contains(ObjectId))
	{
		//LOG(DString::Format("Client already has been send relevant net object ID %i", Test.Get()->GetNetworkId()));
		return;
	}

	ClientData.RelevantNetObjects.Remove(ObjectId);
	
	DBitStream OutStream;
	OutStream.AddByte((uint8_t)ENetCommand::RemoveObject);
	OutStream.AddPackedUInt(ObjectId);
	DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStream.GetByteCount());
	memcpy(Buffer->Buffer, OutStream.GetData(), OutStream.GetByteCount());

	ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
	LOG(DString::Format("Server: Remove object Id %i", ObjectId));

	EnsureSubObjectRelevancyForClient(ClientData);
}

void DServer::ClientInitializeDObjectSub(SServerClientData& ClientData, DObjectPtr<class DObject> Test)
{
	if (ClientData.RelevantNetObjects.Contains(Test->GetNetworkId()))
	{
		//LOG(DString::Format("Client already has been send relevant net object ID %i", Test.Get()->GetNetworkId()));
		return;
	}

	ClientData.RelevantNetSubObjects[Test->GetNetworkId()] = Test;

	DBitStream OutStream;
	OutStream.AddByte((uint8_t)ENetCommand::CreateObject);
	OutStream.AddPackedUInt(std::hash<DString>()(Test->GetRegisteredObject()->GetObjectName()));
	OutStream.AddPackedUInt(Test->GetNetworkId());

	DVector<DSyncVarBase*> SyncVars;
	Test->RegisterSyncVars(SyncVars);
	for (DSyncVarBase* SyncVar : SyncVars)
	{
		SyncVar->Serialize(OutStream);
		SyncVar->SetNotDirtyForConnection(ClientData.ClientId);
	}

	DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStream.GetByteCount());
	memcpy(Buffer->Buffer, OutStream.GetData(), OutStream.GetByteCount());

	ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));

	//LOG(DString::Format("Server: Sent object Id %i sub", Test->GetNetworkId()));
}

void DServer::ClientRemoveDObjectSub(SServerClientData& ClientData, uint64_t ObjectId)
{
	if (!ClientData.RelevantNetObjects.Contains(ObjectId) && !ClientData.RelevantNetSubObjects.Contains(ObjectId))
	{
		//LOG(DString::Format("Client already has been send relevant net object ID %i", Test.Get()->GetNetworkId()));
		return;
	}

	ClientData.RelevantNetSubObjects.Remove(ObjectId);

	if (!ClientData.RelevantNetObjects.Contains(ObjectId))
	{

		DBitStream OutStream;
		OutStream.AddByte((uint8_t)ENetCommand::RemoveObject);
		OutStream.AddPackedUInt(ObjectId);
		DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStream.GetByteCount());
		memcpy(Buffer->Buffer, OutStream.GetData(), OutStream.GetByteCount());

		ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));

		LOG(DString::Format("Server: Remove object Id %i sub", ObjectId));
		EnsureSubObjectRelevancyForClient(ClientData);
	}
}

void DServer::SendDirtySyncVars()
{
	DVector<uint64_t> RemoveList;
	DMap<DSyncVarBase*, int> DirtyList;

	// Go through each client to collect relevant data
	for (SServerClientData& ClientData : ClientConnections)
	{
		DBitStream OutStreamHeader;
		OutStreamHeader.AddByte((uint8_t)ENetCommand::SetSyncVar);

		uint64_t ObjectCount = 0;
		DBitStream OutStreamPayload;
		DVector<uint64_t> SyncVarCounts;

		auto Send = [&](std::pair<uint64_t, DObjectPtr<DObject>> pair) mutable
		{
			DVector<DSyncVarBase*> SyncVars;
			pair.second->RegisterSyncVars(SyncVars);
			uint64_t SyncVarIndex = 0;
			bool bAddedObjectId = false;
			uint64_t SyncVarCount = 0;
			for (DSyncVarBase* SyncVar : SyncVars)
			{
				if (SyncVar->IsDirty() && SyncVar->IsDirtyForConnection(ClientData.ClientId))
				{
					if (!bAddedObjectId)
					{
						// If we have any syncvars, need to add the network Id of the object.
						bAddedObjectId = true;
						OutStreamPayload.AddPackedUInt(pair.first);
						ObjectCount++;
					}

					// If syncvar is dirty, add the index and serialize the data. Add the syncvar to the dirty list to set not dirty later.
					OutStreamPayload.AddPackedUInt(SyncVarIndex);
					SyncVar->Serialize(OutStreamPayload);
					DirtyList[SyncVar] ++;
					SyncVarCount++;
					SyncVar->SetNotDirtyForConnection(ClientData.ClientId);
				}
				SyncVarIndex++;
			}

			if (SyncVarCount > 0)
			{
				// If we had any syncvars then add the amount we are sending.
				SyncVarCounts.Add(SyncVarCount);
			}
		};

		// Go through the relevant objects for this client and collect dirty sync vars
		for (auto& pair : ClientData.RelevantNetObjects)
		{
			if (!pair.second.IsValid())
			{
				// Object is no longer relevant. remove it.
				RemoveList.Add(pair.first);
				continue;
			}

			Send(pair);
		}
		for (auto& pair : ClientData.RelevantNetSubObjects)
		{
			if (!pair.second.IsValid())
			{
				// Object is no longer relevant. remove it.
				RemoveList.Add(pair.first);
				continue;
			}

			Send(pair);
		}

		if (ObjectCount > 0)
		{
			OutStreamHeader.AddPackedUInt(ObjectCount);

			uint64_t TotalSyncVarsSent = 0;
			for (uint64_t SyncVarCount : SyncVarCounts)
			{
				OutStreamHeader.AddPackedUInt(SyncVarCount);
				TotalSyncVarsSent += SyncVarCount;
			}

			DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStreamHeader.GetByteCount() + (uint32_t)OutStreamPayload.GetByteCount());
			memcpy(Buffer->Buffer, OutStreamHeader.GetData(), OutStreamHeader.GetByteCount());
			memcpy(Buffer->Buffer + OutStreamHeader.GetByteCount(), OutStreamPayload.GetData(), OutStreamPayload.GetByteCount());

			ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
			//LOG(DString::Format("Sent %i total dirty syncvars for %i objects to client Id %i", TotalSyncVarsSent, ObjectCount, ClientData.ClientId));
		}
	}

	for (auto& Pair : DirtyList)
	{
		Pair.first->MarkNotDirty();
	}
}

void DServer::SendBroadcastRPC(uint64_t ObjectNetworkId, DBitStream&& OutStreamPayload)
{
	Check(ObjectNetworkId > 0);
	DBitStream OutStreamHeader;
	OutStreamHeader.AddByte((uint8_t)ENetCommand::RPC);
	OutStreamHeader.AddPackedUInt(ObjectNetworkId);

	for (SServerClientData& ClientData : ClientConnections)
	{
		if (ClientData.RelevantNetObjects.Contains(ObjectNetworkId))
		{
			DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStreamHeader.GetByteCount() + (uint32_t)OutStreamPayload.GetByteCount());
			memcpy(Buffer->Buffer, OutStreamHeader.GetData(), OutStreamHeader.GetByteCount());
			memcpy(Buffer->Buffer + OutStreamHeader.GetByteCount(), OutStreamPayload.GetData(), OutStreamPayload.GetByteCount());

			ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
		}
	}
}

void DServer::SendRPC(DVector<uint64_t> Targets, uint64_t ObjectNetworkId, DBitStream&& OutStreamPayload)
{
	Check(ObjectNetworkId > 0);
	DBitStream OutStreamHeader;
	OutStreamHeader.AddByte((uint8_t)ENetCommand::RPC);
	OutStreamHeader.AddPackedUInt(ObjectNetworkId);

	for (SServerClientData& ClientData : ClientConnections)
	{
		if (Targets.Contains(ClientData.ClientId) && ClientData.RelevantNetObjects.Contains(ObjectNetworkId))
		{
			DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStreamHeader.GetByteCount() + (uint32_t)OutStreamPayload.GetByteCount());
			memcpy(Buffer->Buffer, OutStreamHeader.GetData(), OutStreamHeader.GetByteCount());
			memcpy(Buffer->Buffer + OutStreamHeader.GetByteCount(), OutStreamPayload.GetData(), OutStreamPayload.GetByteCount());

			ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
		}
	}
}

void DServer::SetNetOwnership(DVector<uint64_t> Targets, uint64_t ObjectNetworkId, bool bOwned)
{
	Check(ObjectNetworkId > 0);
	DBitStream OutStreamHeader;
	OutStreamHeader.AddByte((uint8_t)ENetCommand::NetOwnership);
	OutStreamHeader.AddPackedUInt(ObjectNetworkId);
	OutStreamHeader.AddBool(bOwned);

	for (SServerClientData& ClientData : ClientConnections)
	{
		if (Targets.Contains(ClientData.ClientId) && ClientData.RelevantNetObjects.Contains(ObjectNetworkId))
		{
			DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStreamHeader.GetByteCount());
			memcpy(Buffer->Buffer, OutStreamHeader.GetData(), OutStreamHeader.GetByteCount());

			ClientData.TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
		}
	}
}

void DServer::EnsureSubObjectRelevancy()
{
	for (SServerClientData& ClientData : ClientConnections)
	{
		EnsureSubObjectRelevancyForClient(ClientData);
	}
}

void DServer::EnsureSubObjectRelevancyForClient(SServerClientData& ClientData)
{
	// Collect all relevant objects
	for (auto& Pair : ClientData.RelevantNetObjects)
	{
		if (!Pair.second.IsValid())
		{
			continue;
		}

		EnsureSubObjectRelevancyForObject(ClientData, Pair.second);
		//LOG(DString::Format("Ensuring relevancy for primary object %s", *Pair.second->GetDisplayName()));
	}
}

void DServer::EnsureSubObjectRelevancyForObject(SServerClientData& ClientData, DObjectPtr<DObject> Object)
{
	Check(Object.IsValid());
	DRelevantSubObjectContainer RelevantObjects;

	Object->CollectRelevantSubObjects(RelevantObjects);
	RelevantObjects.Add(Object);

	DVector<DObjectPtr<DObject>> AddList;

	for (const std::pair<uint64_t, DObjectPtr<DObject>>& Pair : RelevantObjects.GetRelevantSubObjects())
	{
		if (!ClientData.RelevantNetSubObjects.Contains(Pair.first) && !ClientData.RelevantNetObjects.Contains(Pair.first))
		{
			//LOG(DString::Format("Ensuring relevancy for sub object %s", *Pair.second->GetDisplayName()));
			AddList.Add(Pair.second);
		}
	}

	DVector<uint64_t> RemoveList;

	for (const std::pair<uint64_t, DObjectPtr<DObject>>& Pair : ClientData.RelevantNetSubObjects)
	{
		if (!RelevantObjects.GetRelevantSubObjects().Contains(Pair.first) && !ClientData.RelevantNetObjects.Contains(Pair.first))
		{
			RemoveList.Add(Pair.first);
		}
	}

	for (uint64_t Removal : RemoveList)
	{
		//LOG(DString::Format("Server: Removing %i", Removal));
		//ClientRemoveDObjectSub(ClientData, Removal);
	}

	for (DObjectPtr<DObject>& AddObject : AddList)
	{
		//LOG(DString::Format("Server: Adding %i", AddObject->GetNetworkId()));
		ClientInitializeDObjectSub(ClientData, AddObject);
	}
}

void DServer::EnsureSubObjectRelevancyForObjectAllClients(DObjectPtr<class DObject> Object)
{
	for (SServerClientData& ClientData : ClientConnections)
	{
		EnsureSubObjectRelevancyForObject(ClientData, Object);
	}
}
