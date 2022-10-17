#include "World.h"
#include "Logging/Logging.h"
#include "SceneObject.h"

DRegisteredObject<DWorld> RegisteredObject = DRegisteredObject<DWorld>();
DRegisteredObjectBase* DWorld::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DWorld::DWorld()
{
	bIsNetworked = true;
	bAlwaysNetRelevant = true;
}

void DWorld::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DObject::RegisterSyncVars(SyncVars);
	SyncVars.Add(&SceneObjects);
}

void DWorld::PostConstruct()
{
	
}

void DWorld::OnReceiveSyncVarUpdate()
{
	DObject::OnReceiveSyncVarUpdate();

	LOG(DString::Format("Sync var. size of scene objects = %llu", SceneObjects.Size()));

	for (int Index = 0; Index < SceneObjects.Size(); Index++)
	{
		if (SceneObjects[Index].IsValid())
		{
			LOG(DString::Format("valid %s", *SceneObjects[Index]->GetDisplayName()));
		}
		else
		{
			LOG("not valid");
		}
	}
}

void DWorld::AddSceneObject(DObjectPtr<DSceneObject> SceneObject)
{
	Check(SceneObject.IsValid());
	if (!SceneObject.IsValid())
	{
		LOG_ERR("SceneObject is nullptr");
		return;
	}

	if (!ContainsSceneObject(SceneObject))
	{
		SceneObjects.Add(SceneObject);
		SceneObject->AddToWorld(GetWeakThis());
		SetDirtyRelevantSubObjects();
	}
}

bool DWorld::RemoveSceneObject(DObjectPtr<DSceneObject> SceneObject)
{
	Check(SceneObject.IsValid());
	if (!SceneObject.IsValid())
	{
		LOG_ERR("SceneObject is nullptr");
		return false;
	}

	for (int Index = 0; Index < SceneObjects.Size(); Index++)
	{
		if (SceneObjects[Index].Get() == SceneObject.Get())
		{
			SceneObject->RemoveFromWorld(GetWeakThis());
			SceneObjects.RemoveAt(Index);
			SetDirtyRelevantSubObjects();
			return true;
		}
	}	
	return false;
}

bool DWorld::ContainsSceneObject(DObjectPtr<DSceneObject> SceneObject)
{
	Check(SceneObject.IsValid());
	if (!SceneObject.IsValid())
	{
		LOG_ERR("SceneObject is nullptr");
		return false;
	}

	for (int Index = 0; Index < SceneObjects.Size(); Index++)
	{
		if (SceneObjects[Index].Get() == SceneObject.Get())
		{
			return true;
		}
	}
	return false;
}

void DWorld::CollectRelevantSubObjects(DRelevantSubObjectContainer& RelevantSubObjects)
{
	for (int Index = 0; Index < SceneObjects.Size(); Index++)
	{
		DObjectPtr<DSceneObject> SceneObject = SceneObjects[Index];
		RelevantSubObjects.Add(DObjectPtr<DObject>(std::move(SceneObject)));
	}
}
