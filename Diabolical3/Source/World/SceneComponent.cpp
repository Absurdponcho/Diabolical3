#include "SceneComponent.h"
#include "SceneObject.h"

DRegisteredObject<DSceneComponent> RegisteredObject = DRegisteredObject<DSceneComponent>();
DRegisteredObjectBase* DSceneComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DSceneComponent::DSceneComponent()
{
	bIsNetworked = true;
}

void DSceneComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DObject::RegisterSyncVars(SyncVars);
	SyncVars.Add(&ParentObject);
}

void DSceneComponent::SetParent(DObjectPtr<DSceneObject> Parent)
{
	if (ParentObject.Get().IsValid())
	{
		ParentObject.Get()->RemoveComponent(GetWeakThis());
	}

	ParentObject.Set(Parent);
}

DObjectPtr<DSceneObject> DSceneComponent::GetParent()
{
	return ParentObject.Get();
}
