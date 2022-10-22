#pragma once
#include "World/SceneComponent.h"
#include "Math/Math.h"
#include "Input/Input.h"

class DCameraComponent : public DSceneComponent
{
public:
	DCameraComponent();
	virtual ~DCameraComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void PostConstruct() override;

	virtual void RenderScene(DWeakPtr<class DWindow> Window);

	virtual void OnReceiveSyncVarUpdate() override;

	SMatrix44f GetViewMatrix();
	SMatrix44f GetPerspectiveProjectionMatrix(float AspectRatio);

	DWeakPtr<class DWindow> GetTargetWindow() const
	{
		return TargetWindow;
	}
protected:
	DWeakPtr<class DWindow> TargetWindow;
	bool bViewMatrixCached = false;
	SMatrix44f CachedViewMatrix;
	STransformf CachedParentTransform;


	void DirtyAllCacheVars()
	{
		bViewMatrixCached = false;
	}

};