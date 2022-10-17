#include "MeshComponent.h"

DRegisteredObject<DMeshComponent> RegisteredObject = DRegisteredObject<DMeshComponent>();
DRegisteredObjectBase* DMeshComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DMeshComponent::DMeshComponent()
{
	bIsNetworked = true;
}

void DMeshComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DRenderComponent::RegisterSyncVars(SyncVars);
}