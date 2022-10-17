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
	bIsNetworked = true;
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

	DVector<DObjectPtr<DRenderComponent>> RenderComponents = GetParent()->GetOwnerWorld()->GetAllComponentsOfType<DRenderComponent>();
	//LOG(DString::Format("Comp size %i", RenderComponents.Size()));
	for (DObjectPtr<DRenderComponent>& RenderComponent : RenderComponents)
	{
		RenderComponent->Render(GetWeakThis());
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

	SVector3f EulerRotation = { 0, 0, 0 };
	//FPS camera:  RotationX(pitch) * RotationY(yaw)
	glm::quat qPitch = glm::angleAxis(EulerRotation.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(EulerRotation.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(EulerRotation.z, glm::vec3(0, 0, 1));

	//For a FPS camera we can omit roll
	glm::quat orientation = qPitch * qYaw * qRoll;
	orientation = glm::normalize(orientation);
	glm::mat4 rotate = glm::mat4_cast(orientation);

	glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::translate(translate, -ThisTransform.Position);

	return rotate * translate;
}

SMatrix44f DCameraComponent::GetPerspectiveProjectionMatrix(float AspectRatio)
{
	return glm::perspective(glm::radians(90.f), AspectRatio, 0.001f, 1000.f);
}