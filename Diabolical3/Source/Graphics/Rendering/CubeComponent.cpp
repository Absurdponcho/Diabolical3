#include "MeshComponent.h"
#include "Material.h"
#include "MeshPrimitives.h"
#include "World/SceneObject.h"
#include "CameraComponent.h"
#include "MaterialInstance.h"
#include "Graphics/Rendering/RenderThread.h"

DRegisteredObject<DCubeComponent> RegisteredObject = DRegisteredObject<DCubeComponent>();
DRegisteredObjectBase* DCubeComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DRegisteredObject<DCubeComponentLocal> RegisteredObjectLocal = DRegisteredObject<DCubeComponentLocal>();
DRegisteredObjectBase* DCubeComponentLocal::GetRegisteredObject() const
{
	return &RegisteredObjectLocal;
}

DCubeComponent::DCubeComponent()
{
	bIsNetworked = true;
}

void DCubeComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DMeshComponent::RegisterSyncVars(SyncVars);
}

void DCubeComponent::Render(DObjectPtr<DCameraComponent> CameraComponent)
{
	Check(ParentObject.Get().IsValid());
	if (!ParentObject.Get().IsValid())
	{
		return;
	}
	Check(CameraComponent.IsValid());
	if (!CameraComponent.IsValid())
	{
		return;
	}

	DObjectPtr<DCubeComponent> WeakThis = GetWeakThis();

	SMatrix44f ViewMatrix = CameraComponent->GetViewMatrix();
	SMatrix44f ProjectionMatrix = CameraComponent->GetPerspectiveProjectionMatrix(1.0f);

	SMatrix44f ModelMatrix = GetModelMatrix();

	DEngine::RenderThread->Invoke(DAction<int>([WeakThis, ModelMatrix, ViewMatrix, ProjectionMatrix](int a)
		{
			if (!WeakThis.IsValid())
			{
				Check(false);
				return;
			}

			if (!WeakThis->MaterialInstance.IsValid())
			{
				Check(false);
				return;
			}

			SMatrix44f MVPMatrix = ModelMatrix * ViewMatrix * ProjectionMatrix;

			bool bUniformSuccess = WeakThis->MaterialInstance->SetUniform("MVP", MVPMatrix);

			Check(bUniformSuccess);
			if (!bUniformSuccess)
			{
				LOG_ERR("Failed to set uniform \"MVP\"");
				return;
			}

			WeakThis->MaterialInstance->Bind();

			MeshPrimitives::Cube->Draw();

		}));
	
}

void DCubeComponent::PostConstruct()
{
	DMaterial::GetDefaultMaterial();
	MaterialInstance = new DMaterialInstance(DMaterial::GetDefaultMaterial());
}
