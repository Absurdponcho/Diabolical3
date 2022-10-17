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

	STransformf Transform = ParentObject.Get()->GetTransform();

	DMaterial::GetDefaultMaterial();
	DSharedPtr<DMaterialInstance> TempMaterialInstance = new DMaterialInstance(DMaterial::GetDefaultMaterial());

	SMatrix44f ViewMatrix = CameraComponent->GetViewMatrix();
	SMatrix44f ProjectionMatrix = CameraComponent->GetPerspectiveProjectionMatrix(1.0f);

	SMatrix44f ModelMatrix = Transform.GetModelMatrix();

	SMatrix44f MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
;
	DEngine::RenderThread->Invoke(DAction<int>([TempMaterialInstance, MVPMatrix](int a)
		{
			if (!TempMaterialInstance.IsValid())
			{
				return;
			}

			bool bUniformSuccess = TempMaterialInstance->SetUniform("MVP", MVPMatrix);
			Check(bUniformSuccess);
			if (!bUniformSuccess)
			{
				LOG_ERR("Failed to set uniform \"MVP\"");
				return;
			}

			TempMaterialInstance->Bind();

			MeshPrimitives::Cube->Draw();

		}));
}
