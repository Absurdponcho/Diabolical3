#pragma once
#include "World/SceneComponent.h"

class DRenderComponent : public DSceneComponent
{
public:
	DRenderComponent();
	virtual ~DRenderComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void Render(DObjectPtr<class DCameraComponent> CameraComponent) {} 
};