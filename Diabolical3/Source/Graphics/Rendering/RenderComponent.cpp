#include "RenderComponent.h"

DRegisteredObject<DRenderComponent> RegisteredObject = DRegisteredObject<DRenderComponent>();
DRegisteredObjectBase* DRenderComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DRenderComponent::DRenderComponent()
{
	bIsNetworked = true;
}

void DRenderComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DSceneComponent::RegisterSyncVars(SyncVars);
}