#include "Player.h"

DRegisteredObject<DPlayer> RegisteredObject = DRegisteredObject<DPlayer>();
DRegisteredObjectBase* DPlayer::GetRegisteredObject() const
{
	return &RegisteredObject;
}
