#pragma once
#include "Object.h"
#include "Types/DMemory.h"
#include "Online/SyncVar.h"
#include "Math/Math.h"

// An object than can exist within a DWorld
class DSceneObject : public DObject
{
public:
	DSceneObject();
	virtual ~DSceneObject() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void AddToWorld(DObjectPtr<class DWorld> World);
	virtual bool RemoveFromWorld(DObjectPtr<class DWorld> World);
	virtual bool IsInWorld(DObjectPtr<class DWorld> World);
	DObjectPtr<class DWorld> GetOwnerWorld()
	{
		return OwnerWorld.Get();
	}

	virtual void RegisterComponent(DObjectPtr<class DSceneComponent> Component);
	virtual void RemoveComponent(DObjectPtr<class DSceneComponent> Component);
	virtual bool HasComponent(DObjectPtr<class DSceneComponent> Component);

	virtual void CollectRelevantSubObjects(DRelevantSubObjectContainer& RelevantSubObjects) override;

	virtual void OnReceiveSyncVarUpdate() override;

	const DVector<DObjectPtr<class DSceneComponent>>& GetComponents() const
	{
		return Components.GetVector();
	}

	DVector<DObjectPtr<class DSceneComponent>>& GetComponentsMutable()
	{
		return Components.GetVectorMutable();
	}

	STransformf GetTransform()
	{
		return { Position.Get(), Scale.Get(), Rotation.Get() };
	}

	void SetTransform(const STransformf& InTransform)
	{
		if (Position.Get() != InTransform.Position)
		{
			Position.Set(InTransform.Position);
		}

		if (Scale.Get() != InTransform.Scale)
		{
			Scale.Set(InTransform.Scale);
		}

		if (Rotation.Get() != InTransform.Rotation)
		{
			Rotation.Set(InTransform.Rotation);
		}
	}

protected:
	DSyncVar<SVector3f> Position;
	DSyncVar<SVector3f> Scale = SVector3f::OneVector;
	DSyncVar<SQuaternionf> Rotation;

	DSyncVar<DObjectPtr<class DWorld>> OwnerWorld;
	DSyncVector<DObjectPtr<class DSceneComponent>> Components;
	DVector<DObjectPtr<class DSceneComponent>> NonNetworkedComponents;

	friend class DWorld;
};