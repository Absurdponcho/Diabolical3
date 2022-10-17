#pragma once
#include "World/SceneComponent.h"
#include "Math/Math.h"

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

};