#pragma once
#include "Types/DMemory.h"
#include "ObjectManager.h"
#include "Object.h"
#include "Types/DString.h"
#include "Online/RPC.h"
#include "SceneObject.h"
#include "SceneComponent.h"

class DWorld : public DObject
{
public:
	DWorld();
	virtual ~DWorld() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;
	virtual void PostConstruct() override;	

	virtual void AddSceneObject(DObjectPtr<class DSceneObject> SceneObject);
	virtual bool RemoveSceneObject(DObjectPtr<class DSceneObject> SceneObject);
	virtual bool ContainsSceneObject(DObjectPtr<class DSceneObject> SceneObject);

	virtual void CollectRelevantSubObjects(DRelevantSubObjectContainer& RelevantSubObjects) override;

	virtual void OnReceiveSyncVarUpdate() override;

	template <typename T>
	DVector<DObjectPtr<T>> GetAllObjectsOfType()
	{
		DVector<DObjectPtr<T>> OutArray;
		OutArray.Reserve(32);
		for (DObjectPtr<class DSceneObject>& SceneObject : SceneObjects)
		{
			if (dynamic_cast<T*>(SceneObject.Get()))
			{
				OutArray.Add(SceneObject);
			}
		}
		return OutArray;
	}

	template <typename T>
	DVector<DObjectPtr<T>> GetAllComponentsOfType()
	{
		DVector<DObjectPtr<T>> OutArray;
		OutArray.Reserve(32);
		DVector<DObjectPtr<DSceneObject>>& SceneObjectsVector = SceneObjects.GetVectorMutable();
		for (int Index = 0; Index < SceneObjectsVector.Size(); Index++)
		{
			DObjectPtr<DSceneObject>& SceneObject = SceneObjectsVector[Index];
			if (!SceneObject.IsValid())
			{
				continue;
			}

			DVector<DObjectPtr<DSceneComponent>>& SceneComponents = SceneObject->GetComponentsMutable();
			for (int ComponentIndex = 0; ComponentIndex < SceneComponents.Size(); ComponentIndex++)
			{
				DObjectPtr<DSceneComponent>& SceneComponent = SceneComponents[ComponentIndex];
				if (dynamic_cast<T*>(SceneComponent.Get()))
				{
					OutArray.Add(DObjectPtr<T>(SceneComponent));
				}
			}
		}
		return OutArray;
	}

protected:
	DSyncVector<DObjectPtr<class DSceneObject>> SceneObjects;
};