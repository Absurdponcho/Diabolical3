#pragma once
#include "LightComponent.h"
#include "Online/SyncVar.h"

class DPointLightComponent : public DLightComponent
{
public:
	DPointLightComponent();
	virtual ~DPointLightComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	virtual void PerformLightPass(DSharedPtr<DRenderTargetGBuffer>& GBuffer, DSharedPtr<DRenderTargetAlbedo>& CurrentLightPassBuffer, DSharedPtr<DRenderTargetAlbedo>& DestinationLightPassBuffer) override;

	DSyncVar<SVector3f> LightColor = SVector3f(1, 1, 1);
	DSyncVar<float> LightIntensity = 1;

};