#pragma once
#include "Types/DMemory.h"
#include "Types/DVector.h"
#include "Types/Map.h"

class DObjectManager
{
public:
	virtual ~DObjectManager() = default;

	void SendAllNetRelevantObjectsForNewConnection(uint64_t ClientId);

	void ProcessCreateObjectPacket(class DBitStream& BitStream);
	void ProcessRemoveObjectPacket(class DBitStream& BitStream);
	void ProcessSetSyncVarPacket(class DBitStream& BitStream);
	void ProcessRPCPacket(class DBitStream& BitStream);
	void ProcessOwnershipPacket(class DBitStream& BitStream);

	void ProcessRPCPacketServer(uint64_t ClientId, class DBitStream& BitStream);

	template <typename T>
	DVector<DObjectPtr<T>> GetObjectsOfType()
	{
		DVector<DObjectPtr<T>> OutArray;
		OutArray.Reserve(32);
		for (DSharedPtr<class DObject>& Object : ManagedObjects)
		{
			if (dynamic_cast<T*>(Object.GetRaw()))
			{
				OutArray.Add(Object.Get());
			}
		}
		return OutArray;
	}

	bool GetObjectFromNetworkId(uint64_t NetworkId, DObjectPtr<class DObject>& OutObject);

protected:
	DVector<DSharedPtr<class DObject>> ManagedObjects;
	DMap<uint64_t, DObjectPtr<class DObject>> NetworkedObjects;
	void RegisterObject(DSharedPtr<class DObject>& Object);

private:
	void NetRegisterObject(DSharedPtr<class DObject>& Object);
	friend class DObject;

};