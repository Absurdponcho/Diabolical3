#include "DMemory.h"
#include "World/Object.h"
#include "Types/BitStream.h"

bool DObjectPtrBase::SetFromNetworkId()
{
	DObjectPtr<DObject> NetObject;
	if (!DEngine::ObjectManager->GetObjectFromNetworkId(NetworkId, NetObject))
	{
		//LOG(DString::Format("object not on net %llu", NetworkId));
		return false;
	}

	NetObject.CopyTo(this);
	return true;
}

bool DObjectPtrBase::Serialize(class DBitStream& BitStream)
{
	if (GetPtrInternal())
	{
		if (!BitStream.AddBool(true))
		{
			return false;
		}
		DObject* Internal = static_cast<DObject*>(GetPtrInternal());
		if (Internal->IsNetworked())
		{
			//LOG(DString::Format("Serialize DObjectptr %s", *Internal->GetDisplayName()));
			return BitStream.AddPackedUInt(Internal->GetNetworkId());
		}
		else
		{
			Check(false); // not implemented			
			return false;
		}
	}
	else
	{
		return BitStream.AddBool(false);
	}
}

bool DObjectPtrBase::Deserialize(class DBitStream& BitStream)
{
	DObject* Internal = static_cast<DObject*>(GetPtrInternal());

	bool bValid = false;
	if (!BitStream.ReadBool(bValid))
	{
		LOG("no bool");
		return false;
	}

	if (!bValid)
	{
		LOG("not valid");
		*this = DObjectPtr<DObject>();
		return true;
	}

	uint64_t InNetworkId = 0;
	if (!BitStream.ReadPackedUInt(InNetworkId))
	{
		LOG("no net id");
		return false;
	}

	NetworkId = InNetworkId;
	bHasNetworkId = true;
	if (!SetFromNetworkId())
	{
		// return true even though it failed, because we might receive this networked object later. Then we can resolve the pointer using the network id.
		return true;
	}
	return true;
}