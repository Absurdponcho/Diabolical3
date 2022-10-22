#include "Player.h"
#include "Online/NetworkManager.h"
#include "Graphics/Rendering/CameraComponent.h"

DRegisteredObject<DPlayer> RegisteredObject = DRegisteredObject<DPlayer>();
DRegisteredObjectBase* DPlayer::GetRegisteredObject() const
{
	return &RegisteredObject;
}

bool DPlayer::IsControlled() const
{
	if (DNetworkManager::IsDedicatedServer())
	{
		return false;
	}
	if (DNetworkManager::GetClient())
	{
		return bHasLocalNetOwnership;
	}
	if (DNetworkManager::GetServer())
	{
		DVector<uint64_t> ClientIds;
		GetOwnerClients(ClientIds);
		return ClientIds.Size() == 0;
	}
	return true;
}

void DPlayer::Tick(const STickInfo& TickInfo)
{
	DObject::Tick(TickInfo);

	CheckCamera();

	if (IsControlled())
	{
		if (!bCurrentControl)
		{
			bCurrentControl = true;
			bSentControlCall = true;
			OnControlChanged(true);
		}
	}
	else
	{
		if (bCurrentControl)
		{
			bCurrentControl = false;
			bSentControlCall = true;
			OnControlChanged(false);
		}
	}

	if (!bSentControlCall)
	{
		OnControlChanged(bCurrentControl);
		bSentControlCall = true;
	}
}

void DPlayer::CheckCamera()
{
	if (ShouldHaveCamera())
	{
		if (!CameraComponent.IsValid())
		{
			CameraComponent = DObject::CreateNew<DCameraComponent>("Local Camera");
			RegisterComponent(CameraComponent);
		}
	}
	else
	{
		if (CameraComponent.IsValid())
		{
			CameraComponent->Destroy();
		}
	}
}

bool DPlayer::ShouldHaveCamera() const
{
	return IsControlled();
}
