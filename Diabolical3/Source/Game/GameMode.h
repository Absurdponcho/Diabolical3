#pragma once
#include "World/Object.h"
#include "Types/DMemory.h"
#include "Online/SyncVar.h"

class DGameMode : public DObject
{
public:
	DGameMode();

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;
	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	DObjectPtr<class DPlayer> CreateNewPlayerObject() const;

	virtual void SetGameWorld(DObjectPtr<class DWorld>& World);
	virtual DObjectPtr<class DWorld> GetGameWorld();

protected:
	virtual DObjectPtr<class DPlayer> GenerateNewPlayerObject() const;
	DSyncVar<DObjectPtr<class DWorld>> GameWorld;

	static void BroadcastNewClient(uint64_t ClientId);
	void InitializeNewClient(uint64_t ClientId);

	friend class DServer;
	friend class DNetworkManager;
};