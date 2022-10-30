#include "CameraComponent.h"
#include "World/SceneObject.h"
#include "Graphics/Window.h"
#include "Graphics/Rendering/RenderComponent.h"
#include "World/World.h"
#include "Graphics/Window.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/Rendering/MaterialInstance.h"
#include "LightComponent.h"

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

	{	// Disable blending
		DEngine::RenderThread->Invoke(DAction<int>([=](int a)
			{
				glDisable(GL_BLEND);
			}));
	}

	{	// Skybox
		static DSharedPtr<DMaterial> SkyBoxMaterial;
		if (!SkyBoxMaterial.IsValid())
		{
			DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/SkyBox.vert")->AsString();
			DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/SkyBox.frag")->AsString();

			Check(VertexShader.Length() > 0);
			Check(FragmentShader.Length() > 0);

			SkyBoxMaterial = std::make_shared<DMaterial>();
			SkyBoxMaterial->BuildShader(VertexShader, FragmentShader);
		}
		DSharedPtr<DMaterialInstance> SkyBoxMaterialInstance = new DMaterialInstance(SkyBoxMaterial);

		SMatrix44f SkyboxTransformMatrix = GetParent()->GetTransform().GetRotation().ToMatrix();

		DEngine::RenderThread->Invoke(DAction<int>([=](int a)
			{
				SkyBoxMaterialInstance->SetUniform("TransformMatrix", SkyboxTransformMatrix);
				SkyBoxMaterialInstance->SetUniform("UpColor", SVector3f(0.53, 0.81, 0.92));
				SkyBoxMaterialInstance->SetUniform("DownColor", SVector3f(0.16, 0.32, 0.50));
				SkyBoxMaterialInstance->SetUniform("HorizonColor", SVector3f(0.93, 0.92, 0.97));
				SkyBoxMaterialInstance->Bind();

				glCullFace(GL_FRONT);
				MeshPrimitives::Cube->Draw();
				glCullFace(GL_BACK);

				// Clear the depth buffer after drawing the sky box, so we can draw things on top of it!
				// Alternatively can draw the sky box at z=0.99999999, which will draw behind most things but this is easy.
				glClear(GL_DEPTH_BUFFER_BIT);
			}));
	}

	DVector<DObjectPtr<DRenderComponent>> RenderComponents = GetParent()->GetOwnerWorld()->GetAllComponentsOfType<DRenderComponent>();
	for (DObjectPtr<DRenderComponent>& RenderComponent : RenderComponents)
	{
		RenderComponent->Render(GetWeakThis());
	}

	if (!LightpassBuffer1.IsValid() || WindowWidth != LightpassBuffer1->GetWidth() || WindowHeight != LightpassBuffer1->GetHeight())
	{
		LightpassBuffer1 = std::make_shared<DRenderTargetAlbedo>(WindowWidth, WindowHeight);
	}

	if (!LightpassBuffer2.IsValid() || WindowWidth != LightpassBuffer2->GetWidth() || WindowHeight != LightpassBuffer2->GetHeight())
	{
		LightpassBuffer2 = std::make_shared<DRenderTargetAlbedo>(WindowWidth, WindowHeight);
	}

	LightpassBuffer2->Bind(true);
	LightpassBuffer1->Bind(true);

	bool bBufferRotation = true;
	DVector<DObjectPtr<DLightComponent>> LightComponents = GetParent()->GetOwnerWorld()->GetAllComponentsOfType<DLightComponent>();
	for (DObjectPtr<DLightComponent>& LightComponent : LightComponents)
	{
		LightComponent->PerformLightPass(GBuffer, bBufferRotation ? LightpassBuffer2 : LightpassBuffer1, bBufferRotation ? LightpassBuffer1 : LightpassBuffer2);
		bBufferRotation = !bBufferRotation;
	}

	if (bBufferRotation)
	{
		LightpassBuffer2->DrawToScreen();
	}
	else
	{
		LightpassBuffer1->DrawToScreen();
	}
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
	return SMatrix44f::Perspective(90.0f * DegToRad, AspectRatio, 0.1f, 1000.f);
}