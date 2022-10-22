#pragma once
#include "World/SceneComponent.h"
#include "Math/Math.h"

class DRenderComponent : public DSceneComponent
{
public:
	DRenderComponent();
	virtual ~DRenderComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void Render(DObjectPtr<class DCameraComponent> CameraComponent) {} 

protected:
	bool bHasCachedModelMatrix = false;
	SMatrix44f CachedModelMatrix;
	STransformf CachedTransform;

	SMatrix44f GetModelMatrix();

	void DirtyAllCacheVars()
	{
		bHasCachedModelMatrix = false;
	}
};