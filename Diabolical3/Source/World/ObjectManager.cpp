#include "ObjectManager.h"
#include "Logging/Logging.h"
#include "Object.h"
#include "Online/NetworkManager.h"
#include "Online/Server.h"
#include "Types/BitStream.h"
#include "Online/RPC.h"
#include "Types/DString.h"

void DObjectManager::RegisterObject(DSharedPtr<class DObject>& Object)
{
	if (bIsTicking)
	{
		DeferredRegister.Add(Object);
	}
	else
	{
		ManagedObjects.Add(Object);

		if (Object->IsNetworked() && DNetworkManager::GetServer())
		{
			NetRegisterObject(Object);
		}
	}
}

void DObjectManager::NetRegisterObject(DSharedPtr<class DObject>& Object)
{
	static uint64_t NetworkIds = 1;
	Object->NetworkID = NetworkIds++;

	NetworkedObjects[Object->GetNetworkId()] = Object;

	for (SServerClientData& ClientData : DNetworkManager::GetServer()->GetClientData())
	{
		if (Object->IsNetRelevantFor(ClientData.ClientId))
		{
			DNetworkManager::GetServer()->ClientInitializeDObject(ClientData, Object);
		}
	}
}

void DObjectManager::SendAllNetRelevantObjectsForNewConnection(uint64_t ClientId)
{
	Check(DNetworkManager::GetServer());
	for (SServerClientData& ClientData : DNetworkManager::GetServer()->GetClientData())
	{
		if (ClientData.ClientId == ClientId)
		{
			for (DSharedPtr<DObject>& Object : ManagedObjects)
			{
				if (Object->bIsNetworked && Object->IsNetRelevantFor(ClientId))
				{
					DNetworkManager::GetServer()->ClientInitializeDObject(ClientData, Object);
				}
			}
			DNetworkManager::GetServer()->EnsureSubObjectRelevancyForClient(ClientData);
			return;
		}
	}
}

bool DObjectManager::GetObjectFromNetworkId(uint64_t NetworkId, DObjectPtr<DObject>& OutObject)
{
	DObjectPtr<DObject>* Object = NetworkedObjects.Find(NetworkId);
	if (Object)
	{
		OutObject = *Object;
		return true;
	}
	return false;
}

void DObjectManager::ProcessCreateObjectPacket(DBitStream& BitStream)
{
	uint64_t ObjectTypeNameHash;
	uint64_t NetworkId;
	BitStream.ReadPackedUInt(ObjectTypeNameHash);
	BitStream.ReadPackedUInt(NetworkId);

	Check(DObjectRegistry::GetRegisteredObjects().Contains(ObjectTypeNameHash));
	DObjectPtr<DObject> NewObject = DObjectRegistry::GetRegisteredObjects().Get(ObjectTypeNameHash)->CreateNew(true);
	NewObject->NetworkID = NetworkId;

	DVector<DSyncVarBase*> SyncVars;
	NewObject->RegisterSyncVars(SyncVars);
	for (DSyncVarBase* SyncVar : SyncVars)
	{
		SyncVar->Deserialize(BitStream);
	}

	NetworkedObjects[NetworkId] = NewObject;

	NewObject->PostConstruct();

	LOG(DString::Format("Received Net Object %s", *NewObject->GetDisplayName()));

	if (SyncVars.Size() > 0)
	{
		NewObject->OnReceiveSyncVarUpdate();
	}
}

void DObjectManager::ProcessRemoveObjectPacket(DBitStream& BitStream)
{
	uint64_t NetworkId;
	BitStream.ReadPackedUInt(NetworkId);


	LOG(DString::Format("Removed Net Object Id %llu", NetworkId));

}

void DObjectManager::ProcessSetSyncVarPacket(DBitStream& BitStream)
{
	uint64_t ObjectCount = 0;
	if (!BitStream.ReadPackedUInt(ObjectCount))
	{
		return;
	}

	DVector<uint64_t> SyncVarCounts;
	uint64_t TotalSyncVarCount = 0;
	for (uint64_t Index = 0; Index < ObjectCount; Index++)
	{
		uint64_t SyncVarCount = 0;
		if (!BitStream.ReadPackedUInt(SyncVarCount))
		{
			return;
		}
		SyncVarCounts.Add(SyncVarCount);
		TotalSyncVarCount += SyncVarCount;
	}

	//LOG(DString::Format("Received SetSyncvar. %llu Objects, %llu Syncvars", ObjectCount, TotalSyncVarCount));

	for (uint64_t SyncVarCount : SyncVarCounts)
	{
		uint64_t ObjectNetworkId = 0;
		if (!BitStream.ReadPackedUInt(ObjectNetworkId))
		{
			return;
		}

		DObjectPtr<DObject> Object;
		if (!GetObjectFromNetworkId(ObjectNetworkId, Object))
		{
			Checkf(false, "Could not find object with network Id %llu", ObjectNetworkId);
			return;
		}

		DVector<DSyncVarBase*> SyncVars;
		Object->RegisterSyncVars(SyncVars);

		for (uint64_t Index = 0; Index < SyncVarCount; Index++)
		{
			uint64_t SyncVarOffset = 0;
			if (!BitStream.ReadPackedUInt(SyncVarOffset))
			{
				return;
			}
			bool bDeserializeSuccess = SyncVars[SyncVarOffset]->Deserialize(BitStream);
			Check(bDeserializeSuccess);
			if (!bDeserializeSuccess)
			{
				return;
			}
		}

		Object->OnReceiveSyncVarUpdate();
	}
}

void DObjectManager::ProcessRPCPacket(DBitStream& BitStream)
{
	uint64_t ObjectNetworkId = 0;
	if (!BitStream.ReadPackedUInt(ObjectNetworkId))
	{
		return;
	}

	DObjectPtr<DObject> Object;
	if (!GetObjectFromNetworkId(ObjectNetworkId, Object))
	{
		Checkf(false, "Could not find object with network Id %llu", ObjectNetworkId);
		return;
	}

	uint64_t RpcIndex = 0;
	if (!BitStream.ReadPackedUInt(RpcIndex))
	{
		return;
	}

	Check(Object->RegisteredRpcs.Size() >= RpcIndex);
	if (Object->RegisteredRpcs.Size() < RpcIndex)
	{
		LOG_ERR(DString::Format("Object does not have Rpc at index %llu", RpcIndex));
		return;
	}

	Object->RegisteredRpcs[RpcIndex]->Receive(BitStream);
}

void DObjectManager::ProcessRPCPacketServer(uint64_t ClientId, DBitStream& BitStream)
{
	uint64_t ObjectNetworkId = 0;
	if (!BitStream.ReadPackedUInt(ObjectNetworkId))
	{
		return;
	}

	DObjectPtr<DObject> Object;
	if (!GetObjectFromNetworkId(ObjectNetworkId, Object))
	{
		Checkf(false, "Could not find object with network Id %llu", ObjectNetworkId);
		return;
	}

	if (!Object->ClientHasNetOwnership(ClientId))
	{
		DNetworkManager::GetServer()->Disconnect(ClientId);
		return;
	}

	uint64_t RpcIndex = 0;
	if (!BitStream.ReadPackedUInt(RpcIndex))
	{
		return;
	}

	Check(Object->RegisteredRpcs.Size() >= RpcIndex);
	if (Object->RegisteredRpcs.Size() < RpcIndex)
	{
		LOG_ERR(DString::Format("Object does not have Rpc at index %llu", RpcIndex));
		return;
	}

	Object->RegisteredRpcs[RpcIndex]->Receive(BitStream);
}

void DObjectManager::ProcessOwnershipPacket(class DBitStream& BitStream)
{
	uint64_t ObjectNetworkId = 0;
	if (!BitStream.ReadPackedUInt(ObjectNetworkId))
	{
		return;
	}

	DObjectPtr<DObject> Object;
	if (!GetObjectFromNetworkId(ObjectNetworkId, Object))
	{
		Checkf(false, "Could not find object with network Id %llu", ObjectNetworkId);
		return;
	}

	bool bOwned = false;
	if (!BitStream.ReadBool(bOwned))
	{
		return;
	}

	LOG(DString::Format("Received net ownership for object network Id %llu", ObjectNetworkId));
	Object->bHasLocalNetOwnership = bOwned;
}
