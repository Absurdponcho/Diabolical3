#include "GameMode.h"
#include "Player.h"
#include "Online/Server.h"
#include "DiabolicalEngine.h"
#include "World/ObjectManager.h"
#include "World/World.h"

DRegisteredObject<DGameMode> RegisteredObject = DRegisteredObject<DGameMode>();
DRegisteredObjectBase* DGameMode::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DGameMode::DGameMode()
{
	bIsNetworked = true;
	bAlwaysNetRelevant = true;
}

void DGameMode::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DObject::RegisterSyncVars(SyncVars);
	SyncVars.Add(&GameWorld);
}

DObjectPtr<DPlayer> DGameMode::CreateNewPlayerObject() const
{
	Checkf(GameWorld.Get().IsValid(), "GameMode must have a world!");
	if (!GameWorld.Get().IsValid())
	{
		return DObjectPtr<DPlayer>();
	}

	DObjectPtr<DPlayer> Player = GenerateNewPlayerObject();
	GameWorld.Get()->AddSceneObject(Player);
	return Player;
}

DObjectPtr<DPlayer> DGameMode::GenerateNewPlayerObject() const
{
	return DObject::CreateNew<DPlayer>("Standard Player");
}

void DGameMode::BroadcastNewClient(uint64_t ClientId)
{
	for (DObjectPtr<DGameMode> GameMode : DEngine::ObjectManager->GetObjectsOfType<DGameMode>())
	{
		if (!GameMode.IsValid())
		{
			continue;
		}
		GameMode->InitializeNewClient(ClientId);
	}
}

void DGameMode::InitializeNewClient(uint64_t ClientId)
{
	DObjectPtr<DPlayer> NewPlayerObject = CreateNewPlayerObject();
	Check(NewPlayerObject.IsValid());
	
	NewPlayerObject->SetOwnershipOnClient(ClientId);
}

void DGameMode::SetGameWorld(DObjectPtr<DWorld>& World)
{
	GameWorld.Set(World);
}

DObjectPtr<DWorld> DGameMode::GetGameWorld()
{
	return GameWorld.Get();
}