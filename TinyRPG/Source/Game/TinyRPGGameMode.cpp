#include "TinyRPGGameMode.h"
#include "World/Object.h"
#include "TinyRPGPlayer.h"

DRegisteredObject<DTinyRPGGameMode> RegisteredObject = DRegisteredObject<DTinyRPGGameMode>();
DRegisteredObjectBase* DTinyRPGGameMode::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DObjectPtr<DPlayer> DTinyRPGGameMode::GenerateNewPlayerObject() const
{
	return DObject::CreateNew<DTinyRPGPlayer>("Tiny RPG Player");
}
