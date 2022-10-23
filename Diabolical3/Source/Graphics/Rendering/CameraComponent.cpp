#include "CameraComponent.h"
#include "World/SceneObject.h"
#include "Graphics/Window.h"
#include "Graphics/Rendering/RenderComponent.h"
#include "World/World.h"
#include "Graphics/Window.h"

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

	if (!RenderTarget.IsValid() || WindowWidth != RenderTarget->GetWidth() || WindowHeight != RenderTarget->GetHeight())
	{
		RenderTarget = std::make_shared<DRenderTarget>(WindowWidth, WindowHeight);
	}

	RenderTarget->Bind();

	DVector<DObjectPtr<DRenderComponent>> RenderComponents = GetParent()->GetOwnerWorld()->GetAllComponentsOfType<DRenderComponent>();
	for (DObjectPtr<DRenderComponent>& RenderComponent : RenderComponents)
	{
		RenderComponent->Render(GetWeakThis());
	}

	RenderTarget->DrawToScreen();
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