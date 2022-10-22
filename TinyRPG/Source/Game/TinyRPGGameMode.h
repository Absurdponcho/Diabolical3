#pragma once
#include "Game/GameMode.h"

class DTinyRPGGameMode : public DGameMode
{
public:
	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual DObjectPtr<class DPlayer> GenerateNewPlayerObject() const override;
};
