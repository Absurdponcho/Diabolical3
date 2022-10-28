#include "LightComponent.h"

DRegisteredObject<DLightComponent> RegisteredObject = DRegisteredObject<DLightComponent>();
DRegisteredObjectBase* DLightComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DLightComponent::DLightComponent()
{
	bIsNetworked = true;
}
