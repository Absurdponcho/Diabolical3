#pragma once
#include "World/SceneComponent.h"
#include "DMath/Math.h"
#include "RenderTargetGBuffer.h"
#include "RenderTargetAlbedo.h"

class DLightComponent : public DSceneComponent
{
public:
	DLightComponent();
	virtual ~DLightComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void PerformLightPass(DSharedPtr<DRenderTargetGBuffer>& GBuffer, DSharedPtr<DRenderTargetAlbedo>& CurrentLightPassBuffer, DSharedPtr<DRenderTargetAlbedo>& DestinationLightPassBuffer) {}

protected:


};