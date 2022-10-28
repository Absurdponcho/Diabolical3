#include "PointLightComponent.h"
#include "DiabolicalEngine.h"
#include "Graphics/Rendering/RenderThread.h"
#include "AssetManager/AssetManager.h"
#include "World/SceneObject.h"

DRegisteredObject<DPointLightComponent> RegisteredObject = DRegisteredObject<DPointLightComponent>();
DRegisteredObjectBase* DPointLightComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DPointLightComponent::DPointLightComponent()
{
	bIsNetworked = true;
}

void DPointLightComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DLightComponent::RegisterSyncVars(SyncVars);
	SyncVars.Add(&LightColor);
	SyncVars.Add(&LightIntensity);
}

void DPointLightComponent::PerformLightPass(DSharedPtr<DRenderTargetGBuffer>& GBuffer, DSharedPtr<DRenderTargetAlbedo>& CurrentLightPassBuffer, DSharedPtr<DRenderTargetAlbedo>& DestinationLightPassBuffer)
{
	Check(GBuffer.IsValid() && CurrentLightPassBuffer.IsValid() && DestinationLightPassBuffer.IsValid() && GBuffer->IsValid() && CurrentLightPassBuffer->IsValid() && DestinationLightPassBuffer->IsValid() && GetParent().IsValid());


	static DSharedPtr<DMaterial> PointLightMaterial;
	if (!PointLightMaterial.IsValid())
	{
		DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/QuadCopy.vert")->AsString();
		DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/LightPass.frag")->AsString();

		Check(VertexShader.Length() > 0);
		Check(FragmentShader.Length() > 0);

		PointLightMaterial = std::make_shared<DMaterial>();
		PointLightMaterial->BuildShader(VertexShader, FragmentShader);
	}
	DSharedPtr<DMaterialInstance> LightMaterialInstance = new DMaterialInstance(PointLightMaterial);

	SVector3f LightPosition = GetParent()->GetTransform().GetPosition();
	SVector3f LightColorValue = LightColor.Get();
	uint32_t CurrentLightPassBufferTexture = CurrentLightPassBuffer->GetAlbedoTexture();

	DestinationLightPassBuffer->Bind(false);
	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetAlbedoTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetPositionTexture());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetNormalTexture());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, CurrentLightPassBufferTexture);

			LightMaterialInstance->SetUniform("albedoTexture", 0);
			LightMaterialInstance->SetUniform("positionTexture", 1);
			LightMaterialInstance->SetUniform("normalTexture", 2);
			LightMaterialInstance->SetUniform("currentAlbedo", 3);
			LightMaterialInstance->SetUniform("lightPos", LightPosition);
			LightMaterialInstance->SetUniform("lightColor", LightColorValue);

			LightMaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));

}
