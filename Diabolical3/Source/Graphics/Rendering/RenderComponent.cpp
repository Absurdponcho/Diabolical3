#include "RenderComponent.h"
#include "World/SceneObject.h"

DRegisteredObject<DRenderComponent> RegisteredObject = DRegisteredObject<DRenderComponent>();
DRegisteredObjectBase* DRenderComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DRenderComponent::DRenderComponent()
{
	bIsNetworked = false;
}

void DRenderComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DSceneComponent::RegisterSyncVars(SyncVars);
}

SMatrix44f DRenderComponent::GetModelMatrix()
{
	Check(ParentObject.Get().IsValid());
	if (!ParentObject.Get().IsValid())
	{
		return SMatrix44f();
	}

	const STransformf& ParentTransform = ParentObject.Get()->GetTransform();
	if (ParentTransform != CachedTransform)
	{
		DirtyAllCacheVars();
		CachedTransform = ParentTransform;
	}

	if (!bHasCachedModelMatrix)
	{
		CachedModelMatrix = ParentTransform.GetModelMatrix();
		bHasCachedModelMatrix = true;
	}

	return CachedModelMatrix;
}