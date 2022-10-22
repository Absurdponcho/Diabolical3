#include "Object.h"
#include "World.h"
#include "ObjectManager.h"
#include "Check.h"

DRegisteredObject<DObject> RegisteredObject = DRegisteredObject<DObject>();
DRegisteredObjectBase* DObject::GetRegisteredObject() const
{
	return &RegisteredObject;
}

void DObject::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	SyncVars.Add(&DisplayName);
}

uint64_t DObject::RegisterRPC(DNetRPCBase* Rpc)
{
	RegisteredRpcs.Add(Rpc);
	return RegisteredRpcs.Size() - 1;
}

bool DObject::IsRemoteClient() const
{
	return DNetworkManager::GetClient();
}

bool DObject::HasNetOwnership() const
{
	if (DNetworkManager::GetServer())
	{
		return true;
	}
	else if (DNetworkManager::GetClient())
	{
		return bHasLocalNetOwnership;
	}
	else
	{
		return true;
	}
}

bool DObject::ClientHasNetOwnership(uint64_t ClientId) const
{
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{
		for (SServerClientData& ServerClientData : Server->GetClientData())
		{
			if (ServerClientData.ClientId == ClientId)
			{
				return ServerClientData.OwnershipNetObjects.Contains(NetworkID);
			}
		}
		return false;
	}

	Checkf(false, "ClientHasNetOwnership called on non-server!");
	return false;
}

bool DObject::GetOwnerClients(DVector<uint64_t>& OutClientIds) const
{
	OutClientIds.Clear();
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{
		for (SServerClientData& ServerClientData : Server->GetClientData())
		{
			if (ServerClientData.OwnershipNetObjects.Contains(NetworkID))
			{
				OutClientIds.Add(ServerClientData.ClientId);
			}
		}

		return OutClientIds.Size() > 0;
	}

	Checkf(false, "GetOwnerClients called on non-server!");
	return false;
}

void DObject::SetOwnershipOnClient(uint64_t ClientId)
{
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{
		for (SServerClientData& ServerClientData : Server->GetClientData())
		{
			if (ServerClientData.ClientId == ClientId)
			{
				if (!ServerClientData.OwnershipNetObjects.Contains(NetworkID))
				{
					if (!ServerClientData.RelevantNetObjects.Contains(NetworkID))
					{
						// Client has no relevancy of this object, so add relevancy.
						Server->ClientInitializeDObject(ServerClientData, GetWeakThis());
					}
					ServerClientData.OwnershipNetObjects.Add(NetworkID);
					Server->SetNetOwnership({ ServerClientData.ClientId }, NetworkID, true);
				}
				return;
			}
		}
		Checkf(false, "No client with Id %llu", ClientId);
	}

	Checkf(false, "SetOwnershipOnClient called on non-server!");
	return;
}

DObjectPtr<DObject> DObject::GetWeakThis()
{
	if (IsNetworked())
	{
		if (DObjectPtr<DObject>* Ptr = DEngine::ObjectManager->NetworkedObjects.Find(NetworkID))
		{
			return *Ptr;
		}
	}

	for (DSharedPtr<DObject>& Object : DEngine::ObjectManager->ManagedObjects)
	{
		if (Object.GetRaw() == this)
		{
			return Object;
		}
	}

	for (DSharedPtr<DObject>& Object : DEngine::ObjectManager->DeferredRegister)
	{
		if (Object.GetRaw() == this)
		{
			return Object;
		}
	}

	return DObjectPtr<DObject>();
}

bool DObject::IsNetRelevantFor(uint64_t ClientId) const
{
	// This does not return if the object is currently net relevant.
	// It returns true if the object SHOULD become relevant on the client.
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{
		if (bAlwaysNetRelevant)
		{
			return true;
		}
		for (SServerClientData& ServerClientData : Server->GetClientData())
		{
			if (ServerClientData.ClientId == ClientId)
			{
				return ServerClientData.OwnershipNetObjects.Contains(NetworkID);
			}
		}
		Checkf(false, "No client with Id %llu", ClientId);
	}

	Checkf(false, "IsNetRelevantFor called on non-server!");
	return false;
}

void DObject::SetDirtyRelevantSubObjects()
{
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{
		Server->EnsureSubObjectRelevancyForObjectAllClients(GetWeakThis());
	}
}

void DRelevantSubObjectContainer::Add(DObjectPtr<DObject> RelevantSubObject)
{
	Check(RelevantSubObject.IsValid());
	if (!RelevantSubObject.IsValid())
	{
		LOG_ERR("null RelevantSubObject");
		return;
	}
	Check(RelevantSubObject->IsNetworked());
	if (!RelevantSubObject->IsNetworked())
	{
		LOG_ERR("RelevantSubObject is not networked");
		return;
	}

	bool bAlreadyContained = RelevantSubObjects.Contains(RelevantSubObject->GetNetworkId());
	RelevantSubObjects[RelevantSubObject->GetNetworkId()] = RelevantSubObject;
	if (!bAlreadyContained)
	{
		RelevantSubObject->CollectRelevantSubObjects(*this);
	}
}
