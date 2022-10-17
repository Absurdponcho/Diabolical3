#include "SceneObject.h"
#include "World.h"
#include "SceneComponent.h"

DRegisteredObject<DSceneObject> RegisteredObject = DRegisteredObject<DSceneObject>();
DRegisteredObjectBase* DSceneObject::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DSceneObject::DSceneObject()
{
	bIsNetworked = true;
}

void DSceneObject::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DObject::RegisterSyncVars(SyncVars);	
	SyncVars.Add(&OwnerWorld);
	SyncVars.Add(&Components);
	SyncVars.Add(&Transform);
}

void DSceneObject::OnReceiveSyncVarUpdate()
{
	DObject::OnReceiveSyncVarUpdate();
}

void DSceneObject::AddToWorld(DObjectPtr<DWorld> World)
{
	Check(World.IsValid());
	if (!World.IsValid())
	{
		LOG_ERR("World is nullptr");
		return;
	}

	World->AddSceneObject(GetWeakThis());
	OwnerWorld.Set(World);
}

bool DSceneObject::RemoveFromWorld(DObjectPtr<DWorld> World)
{
	Check(World.IsValid());
	if (!World.IsValid())
	{
		LOG_ERR("World is nullptr");
		return false;
	}

	OwnerWorld.Set(DObjectPtr<DWorld>());
	return World->RemoveSceneObject(GetWeakThis());
}

bool DSceneObject::IsInWorld(DObjectPtr<DWorld> World)
{
	Check(World.IsValid());
	if (!World.IsValid())
	{
		LOG_ERR("World is nullptr");
		return false;
	}

	return World->ContainsSceneObject(GetWeakThis());
}

void DSceneObject::RegisterComponent(DObjectPtr<DSceneComponent> Component)
{
	Check(!HasComponent(Component));
	if (HasComponent(Component))
	{
		return;
	}

	Components.Add(Component);
	Component->SetParent(GetWeakThis());
	SetDirtyRelevantSubObjects();
}

void DSceneObject::RemoveComponent(DObjectPtr<DSceneComponent> Component)
{
	for (int Index = 0; Index < Components.Size(); Index++)
	{
		if (Component.Get() == Components[Index].Get())
		{
			Component->SetParent(DObjectPtr<DSceneObject>());
			Components.RemoveAt(Index);
			SetDirtyRelevantSubObjects();
			return;
		}
	}
}

bool DSceneObject::HasComponent(DObjectPtr<DSceneComponent> InComponent)
{
	if (!InComponent.IsValid())
	{
		return false;
	}

	for (int Index = 0; Index < Components.Size(); Index++)
	{
		DObjectPtr<DSceneComponent> Component = Components[Index];
		if (Component.Get() == InComponent.Get())
		{
			return true;
		}
	}
	return false;
}

void DSceneObject::CollectRelevantSubObjects(DRelevantSubObjectContainer& RelevantSubObjects)
{
	for (int Index = 0; Index < Components.Size(); Index++)
	{
		DObjectPtr<DSceneComponent> SceneComponent = Components[Index];
		RelevantSubObjects.Add(DObjectPtr<DObject>(std::move(SceneComponent)));
	}
}
