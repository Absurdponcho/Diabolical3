#pragma once
#include "RenderComponent.h"

class DMeshComponent : public DRenderComponent
{
public:
	DMeshComponent();
	virtual ~DMeshComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void Render(DObjectPtr<class DCameraComponent> CameraComponent) override {}
};

class DCubeComponent : public DMeshComponent
{
public:
	DCubeComponent();
	virtual ~DCubeComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void Render(DObjectPtr<class DCameraComponent> CameraComponent) override;

	virtual void PostConstruct() override;

protected:
	DSharedPtr<class DMaterialInstance> MaterialInstance;
};

class DCubeComponentLocal : public DCubeComponent
{
public:
	DCubeComponentLocal()
	{
		bIsNetworked = false;
	}
	virtual ~DCubeComponentLocal() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

};