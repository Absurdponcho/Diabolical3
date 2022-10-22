#pragma once
#include "Types/DMemory.h"
#include "Check.h"
#include "DiabolicalEngine.h"
#include "ObjectManager.h"
#include "Types/DString.h"
#include "ObjectRegistry.h"
#include "Online/SyncVar.h"

struct STickInfo
{
	float DeltaTime;
};

namespace internal
{
	static const unsigned int FRONT_SIZE = sizeof("internal::GetTypeNameHelper<") - 1u;
	static const unsigned int BACK_SIZE = sizeof(">::GetTypeName") - 1u;

	template <typename T>
	struct GetTypeNameHelper
	{
		static const char* GetTypeName(void)
		{
			static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
			static char typeName[size] = {};
			memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);

			return typeName;
		}
	};
}

class DRegisteredObjectBase
{
public:
	virtual ~DRegisteredObjectBase() {}
	virtual uint64_t GetObjectSize() const = 0;
	virtual DString GetObjectName() const = 0;
	virtual DObjectPtr<class DObject> CreateNew(bool bDeferPostConstruct = false) const = 0;

protected:
	void RegisterThis()
	{
		DObjectRegistry::RegisterObjectBase(this);
	}
};

class DRelevantSubObjectContainer
{
public:
	void Add(DObjectPtr<class DObject> RelevantSubObject);
	const DMap<uint64_t, DObjectPtr<class DObject>>& GetRelevantSubObjects()
	{
		return RelevantSubObjects;
	}

protected:
	DMap<uint64_t, DObjectPtr<class DObject>> RelevantSubObjects;
};

class DObject
{
public:
	template <typename T>
	static DObjectPtr<T> CreateNew(DString Name = "Diabolical Object", bool bDeferPostConstruct = false)
	{
		Check(DEngine::ObjectManager);
		DSharedPtr<DObject> NewObject = new T();
		Check(NewObject->GetRegisteredObject()->GetObjectName() == DString(internal::GetTypeNameHelper<T>::GetTypeName()));
		NewObject->DisplayName = Name;

		DEngine::ObjectManager->RegisterObject(NewObject);
		if (!bDeferPostConstruct)
		{
			NewObject->PostConstruct();
		}

		DSharedPtr<T> NewTObject = std::static_pointer_cast<T>(NewObject.AsSTLType());

		return NewTObject.Get();
	}
	
	DObject() = default;
	virtual ~DObject() {}

	// Called after the object's constructor has been called.
	// Any SyncVars from a remote object will be de-serialized into the object
	// before this is called.
	virtual void PostConstruct() {}

	virtual void OnReceiveSyncVarUpdate() {}

	virtual void Tick(const STickInfo& TickInfo) {};

	virtual void Destroy()
	{
		Check(false);
	}

	bool IsNetworked() const
	{
		return bIsNetworked;
	}

	uint64_t GetNetworkId() const
	{
		return NetworkID;
	}

	const DString& GetDisplayName() const
	{
		return *DisplayName.GetPtr();
	}

	virtual DRegisteredObjectBase* GetRegisteredObject() const;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars);

	bool HasNetOwnership() const;
	bool ClientHasNetOwnership(uint64_t ClientId) const;
	void SetOwnershipOnClient(uint64_t ClientId);
	bool IsNetRelevantFor(uint64_t ClientId) const;
	bool IsRemoteClient() const;

	virtual void CollectRelevantSubObjects(DRelevantSubObjectContainer& RelevantSubObjects) {}

	void SetDirtyRelevantSubObjects();

	// Gets a list of clients who have ownership of this object.
	// true if server & returns 1 or more Id.
	// false if client or no clients own this.
	bool GetOwnerClients(DVector<uint64_t>& OutClientIds) const;

	DObjectPtr<DObject> GetWeakThis();

protected:
	bool bIsNetworked = false;
	bool bHasLocalNetOwnership = false;

	// If true, this object will always be relevant for all connections.
	bool bAlwaysNetRelevant = false;

	// for internal use:
	// Registers an RPC and returns the index of the RPC for the RPC's own use
	uint64_t RegisterRPC(class DNetRPCBase* Rpc);
	DVector<class DNetRPCBase*> RegisteredRpcs;

private:
	friend class DObjectManager;
	friend class DNetRPCBase;

	DSyncVar<DString> DisplayName;
	uint64_t NetworkID = -1;
};

template <typename T>
class DRegisteredObject : public DRegisteredObjectBase
{
public:
	DRegisteredObject()
	{
		RegisterThis();
	}
	virtual ~DRegisteredObject() {}
	virtual uint64_t GetObjectSize() const override
	{
		return sizeof(T);
	}
	virtual DString GetObjectName() const override
	{
		return internal::GetTypeNameHelper<T>::GetTypeName();
	}
	virtual DObjectPtr<class DObject> CreateNew(bool bDeferPostConstruct = false) const override
	{
		/*DObjectPtr<T> DynamicObject =
		DSharedPtr<T> SharedDynamic = DynamicObject.Lock();
		DSharedPtr<DObject> SharedDynamicBase = std::static_pointer_cast<DObject>(SharedDynamic.AsSTLType());*/
		return DObject::CreateNew<T>("DynamicObject", bDeferPostConstruct);
	}

};