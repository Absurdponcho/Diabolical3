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
	SyncVars.Add(&Position);
	SyncVars.Add(&Scale);
	SyncVars.Add(&Rotation);
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

	if (Component->IsNetworked())
	{
		Components.Add(Component);
		SetDirtyRelevantSubObjects();
	}
	else
	{
		NonNetworkedComponents.Add(Component);
	}
	Component->SetParent(GetWeakThis());
}

void DSceneObject::RemoveComponent(DObjectPtr<DSceneComponent> Component)
{
	if (Component->IsNetworked())
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
	else
	{
		for (int Index = 0; Index < NonNetworkedComponents.Size(); Index++)
		{
			if (Component.Get() == NonNetworkedComponents[Index].Get())
			{
				Component->SetParent(DObjectPtr<DSceneObject>());
				NonNetworkedComponents.RemoveAt(Index);
				return;
			}
		}
	}
}

bool DSceneObject::HasComponent(DObjectPtr<DSceneComponent> InComponent)
{
	if (!InComponent.IsValid())
	{
		return false;
	}

	if (InComponent->IsNetworked())
	{
		for (int Index = 0; Index < Components.Size(); Index++)
		{
			DObjectPtr<DSceneComponent> Component = Components[Index];
			if (Component.Get() == InComponent.Get())
			{
				return true;
			}
		}
	}
	else
	{
		for (int Index = 0; Index < NonNetworkedComponents.Size(); Index++)
		{
			DObjectPtr<DSceneComponent> Component = NonNetworkedComponents[Index];
			if (Component.Get() == InComponent.Get())
			{
				return true;
			}
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
