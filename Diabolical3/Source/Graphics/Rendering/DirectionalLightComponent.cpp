#include "DirectionalLightComponent.h"
#include "DiabolicalEngine.h"
#include "Graphics/Rendering/RenderThread.h"
#include "AssetManager/AssetManager.h"
#include "World/SceneObject.h"

DRegisteredObject<DDirectionalLightComponent> RegisteredObject = DRegisteredObject<DDirectionalLightComponent>();
DRegisteredObjectBase* DDirectionalLightComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DDirectionalLightComponent::DDirectionalLightComponent()
{
	bIsNetworked = true;
}

void DDirectionalLightComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DLightComponent::RegisterSyncVars(SyncVars);
	SyncVars.Add(&LightColor);
	SyncVars.Add(&LightIntensity);
}

void DDirectionalLightComponent::PerformLightPass(DSharedPtr<DRenderTargetGBuffer>& GBuffer, DSharedPtr<DRenderTargetAlbedo>& CurrentLightPassBuffer, DSharedPtr<DRenderTargetAlbedo>& DestinationLightPassBuffer)
{
	Check(GBuffer.IsValid() && CurrentLightPassBuffer.IsValid() && DestinationLightPassBuffer.IsValid() && GBuffer->IsValid() && CurrentLightPassBuffer->IsValid() && DestinationLightPassBuffer->IsValid() && GetParent().IsValid());

	if (!LightMaterialInstance.IsValid())
	{
		static DSharedPtr<DMaterial> DirectionalLightMaterial;
		if (!DirectionalLightMaterial.IsValid())
		{
			DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/QuadCopy.vert")->AsString();
			DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/DirectionalLightPass.frag")->AsString();

			Check(VertexShader.Length() > 0);
			Check(FragmentShader.Length() > 0);

			DirectionalLightMaterial = std::make_shared<DMaterial>();
			DirectionalLightMaterial->BuildShader(VertexShader, FragmentShader);
		}
		LightMaterialInstance = new DMaterialInstance(DirectionalLightMaterial);
	}
	SVector3f LightPosition = GetParent()->GetTransform().GetPosition();
	SVector3f LightColorValue = LightColor.Get();
	SVector3f LightDir = GetParent()->GetTransform().GetRotation().RotateVector(SVector3f(0, 0, 1));
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
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetMaterialPropertiesTexture());

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, CurrentLightPassBufferTexture);

			LightMaterialInstance->SetUniform("albedoTexture", 0);
			LightMaterialInstance->SetUniform("positionTexture", 1);
			LightMaterialInstance->SetUniform("normalTexture", 2);
			LightMaterialInstance->SetUniform("materialProperties", 3);
			LightMaterialInstance->SetUniform("currentAlbedo", 4);
			LightMaterialInstance->SetUniform("lightDir", LightDir);
			LightMaterialInstance->SetUniform("lightColor", LightColorValue);

			LightMaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));

}
