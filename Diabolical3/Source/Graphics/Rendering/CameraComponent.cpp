#include "CameraComponent.h"
#include "World/SceneObject.h"
#include "Graphics/Window.h"
#include "Graphics/Rendering/RenderComponent.h"
#include "World/World.h"
#include "Graphics/Window.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/Rendering/MaterialInstance.h"

DRegisteredObject<DCameraComponent> RegisteredObject = DRegisteredObject<DCameraComponent>();
DRegisteredObjectBase* DCameraComponent::GetRegisteredObject() const
{
	return &RegisteredObject;
}

DCameraComponent::DCameraComponent()
{
	bIsNetworked = false;
}

void DCameraComponent::RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars)
{
	DSceneComponent::RegisterSyncVars(SyncVars);
}

void DCameraComponent::RenderScene(DWeakPtr<DWindow> WindowWeak)
{
	DSharedPtr<DWindow> Window = WindowWeak.Lock();
	
	Check(Window.IsValid());
	if (!Window.IsValid())
	{
		return;
	}

	if (!GetParent().IsValid())
	{
		LOG("Parent not valid");
		return;
	}

	if (!GetParent()->GetOwnerWorld().IsValid())
	{
		LOG("World not valid");
		return;
	}


	float AspectRatio = Window->GetAspectRatio();

	uint32_t WindowWidth, WindowHeight;
	Window->GetWidthHeight(WindowWidth, WindowHeight);

	if (!GBuffer.IsValid() || WindowWidth != GBuffer->GetWidth() || WindowHeight != GBuffer->GetHeight())
	{
		GBuffer = std::make_shared<DRenderTargetGBuffer>(WindowWidth, WindowHeight);
	}

	GBuffer->Bind();

	DVector<DObjectPtr<DRenderComponent>> RenderComponents = GetParent()->GetOwnerWorld()->GetAllComponentsOfType<DRenderComponent>();
	for (DObjectPtr<DRenderComponent>& RenderComponent : RenderComponents)
	{
		RenderComponent->Render(GetWeakThis());
	}

	//GBuffer->DrawToScreen();

	if (!LightpassBuffer1.IsValid() || WindowWidth != LightpassBuffer1->GetWidth() || WindowHeight != LightpassBuffer1->GetHeight())
	{
		LightpassBuffer1 = std::make_shared<DRenderTargetAlbedo>(WindowWidth, WindowHeight);
	}

	if (!LightpassBuffer2.IsValid() || WindowWidth != LightpassBuffer2->GetWidth() || WindowHeight != LightpassBuffer2->GetHeight())
	{
		LightpassBuffer2 = std::make_shared<DRenderTargetAlbedo>(WindowWidth, WindowHeight);
	}

	static DSharedPtr<DMaterial> LightMaterial;
	if (!LightMaterial.IsValid())
	{
		DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/QuadCopy.vert")->AsString();
		DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/LightPass.frag")->AsString();

		Check(VertexShader.Length() > 0);
		Check(FragmentShader.Length() > 0);

		LightMaterial = std::make_shared<DMaterial>();
		LightMaterial->BuildShader(VertexShader, FragmentShader);
	}

	LightpassBuffer1->Bind();

	DSharedPtr<DMaterialInstance> LightMaterialInstance = new DMaterialInstance(LightMaterial);
	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetAlbedoTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetPositionTexture());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetNormalTexture());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, Width, Height);

			LightMaterialInstance->SetUniform("albedoTexture", 0);
			LightMaterialInstance->SetUniform("positionTexture", 1);
			LightMaterialInstance->SetUniform("normalTexture", 2);
			LightMaterialInstance->SetUniform("currentAlbedo", 3);
			LightMaterialInstance->SetUniform("lightPos", SVector3f(5, 0, 0));
			LightMaterialInstance->SetUniform("lightColor", SVector3f(1, 1, 1));

			LightMaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));

	LightpassBuffer2->Bind();

	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetAlbedoTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetPositionTexture());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetNormalTexture());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, LightpassBuffer1->GetAlbedoTexture());

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, Width, Height);

			LightMaterialInstance->SetUniform("albedoTexture", 0);
			LightMaterialInstance->SetUniform("positionTexture", 1);
			LightMaterialInstance->SetUniform("normalTexture", 2);
			LightMaterialInstance->SetUniform("currentAlbedo", 3);
			LightMaterialInstance->SetUniform("lightPos", SVector3f(5, 0, 2));
			LightMaterialInstance->SetUniform("lightColor", SVector3f(1, 0, 1));

			LightMaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));

	LightpassBuffer1->Bind();

	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetAlbedoTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetPositionTexture());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, GBuffer->GetNormalTexture());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, LightpassBuffer2->GetAlbedoTexture());

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glViewport(0, 0, Width, Height);

			LightMaterialInstance->SetUniform("albedoTexture", 0);
			LightMaterialInstance->SetUniform("positionTexture", 1);
			LightMaterialInstance->SetUniform("normalTexture", 2);
			LightMaterialInstance->SetUniform("currentAlbedo", 3);
			LightMaterialInstance->SetUniform("lightPos", SVector3f(5, 0, 4));
			LightMaterialInstance->SetUniform("lightColor", SVector3f(0, 1, 0));

			LightMaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));

	LightpassBuffer1->DrawToScreen();
}

void DCameraComponent::PostConstruct()
{
	DSceneComponent::PostConstruct();

	LOG("New camera, finding available window.");

	for (DSharedPtr<DWindow>& Window : DWindow::GetAllWindows())
	{
		Check(Window.IsValid());
		if (!Window.IsValid())
		{
			continue;
		}
		Window->AddCamera(GetWeakThis());
		TargetWindow = Window;
		break;
	}
}

void DCameraComponent::OnReceiveSyncVarUpdate()
{
	DSceneComponent::OnReceiveSyncVarUpdate();
}

SMatrix44f DCameraComponent::GetViewMatrix()
{
	Check(ParentObject.Get().IsValid());
	if (!ParentObject.Get().IsValid())
	{
		return SMatrix44f::Identity;
	}

	STransformf ThisTransform = ParentObject.Get()->GetTransform();

	if (ThisTransform != CachedParentTransform)
	{
		DirtyAllCacheVars();
		CachedParentTransform = ThisTransform;
	}

	if (!bViewMatrixCached)
	{
		SEulerRotationf EulerRotation = ThisTransform.GetEulerRotation();
		//FPS camera:  RotationX(pitch) * RotationY(yaw)
		SQuaternionf qPitch = SQuaternionf::FromAngleAxis(EulerRotation.Pitch, SVector3f(1, 0, 0));
		SQuaternionf qYaw = SQuaternionf::FromAngleAxis(EulerRotation.Yaw, SVector3f(0, 1, 0));
		SQuaternionf qRoll = SQuaternionf::FromAngleAxis(EulerRotation.Roll, SVector3f(0, 0, 1));

		SQuaternionf orientation = qPitch * qYaw * qRoll;
		orientation = orientation.Normalized();
		SMatrix44f rotate = orientation.ToMatrix();

		SMatrix44f translate = SMatrix44f::Identity;
		translate = SMatrix44f::Translate(translate, -ThisTransform.GetPosition());

		CachedViewMatrix = translate * rotate;
		bViewMatrixCached = true;
	}
	return CachedViewMatrix;
}

SMatrix44f DCameraComponent::GetPerspectiveProjectionMatrix(float AspectRatio)
{
	float DegToRad = 0.0174533f;
	return SMatrix44f::Perspective(90.0f * DegToRad, AspectRatio, 0.001f, 1000.f);
}