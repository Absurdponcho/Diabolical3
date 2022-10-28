#include "TinyRPGPlayer.h"
#include "Logging/Logging.h"
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/MeshComponent.h"
#include "Graphics/Window.h"
#include "Time/Time.h"
#include "DMath/Math.h"
#include "Graphics/Rendering/PointLightComponent.h"


DRegisteredObject<DTinyRPGPlayer> RegisteredObject = DRegisteredObject<DTinyRPGPlayer>();
DRegisteredObjectBase* DTinyRPGPlayer::GetRegisteredObject() const
{
	return &RegisteredObject;
}

void DTinyRPGPlayer::PostConstruct()
{
	DPlayer::PostConstruct();

	auto LightComponent = DObject::CreateNew<DPointLightComponent>("Player Light");
	RegisterComponent(LightComponent);
}

void DTinyRPGPlayer::Tick(const STickInfo& TickInfo)
{
	DPlayer::Tick(TickInfo);

	if (InputVector != SVector3f::ZeroVector)
	{
		STransformf Transform = GetTransform();
		if (IsRemoteClient())
		{
			SVector3f NormalizedInputVector = InputVector.Normalized();
			NormalizedInputVector = Transform.Rotation.RotateVector(NormalizedInputVector);
			NormalizedInputVector.X = NormalizedInputVector.X * TickInfo.DeltaTime * 10.0f;
			NormalizedInputVector.Y = NormalizedInputVector.Y * TickInfo.DeltaTime * 10.0f;
			NormalizedInputVector.Z = NormalizedInputVector.Z * TickInfo.DeltaTime * 10.0f;
			ServerRequestMove.Send(NormalizedInputVector);
		}
		else
		{
			SVector3f NormalizedInputVector = InputVector.Normalized();
			NormalizedInputVector = Transform.Rotation.RotateVector(NormalizedInputVector);
			Transform.Position.X += NormalizedInputVector.X * TickInfo.DeltaTime * 10.0f;
			Transform.Position.Y += NormalizedInputVector.Y * TickInfo.DeltaTime * 10.0f;
			Transform.Position.Z += NormalizedInputVector.Z * TickInfo.DeltaTime * 10.0f;
			SetTransform(Transform);
		}
	}
}

void DTinyRPGPlayer::ServerRequestMove_Receive(SVector3f Move)
{
	STransformf Transform = GetTransform();
	Transform.Position.X += Move.X;
	Transform.Position.Y += Move.Y;
	Transform.Position.Z += Move.Z;
	SetTransform(Transform);
}

void DTinyRPGPlayer::OnControlChanged(bool bIsControlled)
{
	DPlayer::OnControlChanged(bIsControlled);

	if (bIsControlled)
	{
		Check(CameraComponent.IsValid());
		if (!CameraComponent.IsValid())
		{
			return;
		}

		WindowWeak = CameraComponent->GetTargetWindow();
		DSharedPtr<DWindow> WindowShared = CameraComponent->GetTargetWindow().Lock();
		Check(WindowShared.IsValid());
		if (WindowShared.IsValid())
		{
			WindowShared->InputStack.KeyListeners.Add({ GetWeakThis(), &DTinyRPGPlayer::KeyListener });
			WindowShared->InputStack.AxisListeners.Add({ GetWeakThis(), &DTinyRPGPlayer::AxisListener });
		}

		if (RemoteCube.IsValid())
		{
			RemoteCube->Destroy();
		}
	}
	else
	{
		DSharedPtr<DWindow> WindowShared = WindowWeak.Lock();
		if (WindowShared.IsValid())
		{
			WindowShared->InputStack.KeyListeners.Remove({ GetWeakThis(), &DTinyRPGPlayer::KeyListener });
			WindowShared->InputStack.AxisListeners.Remove({ GetWeakThis(), &DTinyRPGPlayer::AxisListener });
		}

		if (!RemoteCube.IsValid())
		{
			RemoteCube = DObject::CreateNew<DCubeComponent>("Remote Cube");
			RegisterComponent(RemoteCube);
		}
	}
}

SInputHandleResult DTinyRPGPlayer::KeyListener(const SKeyEvent& KeyEvent)
{
	Check(IsControlled());

	switch (KeyEvent.KeyCode)
	{
	case EKeyCode::KC_W:
	case EKeyCode::KC_Up:
	{
		InputVector.Z += KeyEvent.bReleased ? 1 : -1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_S:
	case EKeyCode::KC_Down:
	{
		InputVector.Z += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_A:
	case EKeyCode::KC_Left:
	{
		InputVector.X += KeyEvent.bReleased ? 1 : -1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_D:
	case EKeyCode::KC_Right:
	{
		InputVector.X += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_Space:
	{
		InputVector.Y += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_LeftCtrl:
	{
		InputVector.Y += KeyEvent.bReleased ? 1 : -1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	}
	return SInputHandleResult();
}

SInputHandleResult DTinyRPGPlayer::AxisListener(const SAxisEvent& AxisEvent)
{
	Check(IsControlled());

	switch (AxisEvent.AxisCode)
	{
	case EAxisCode::AC_Mouse:
		STransformf Transform = GetTransform();
		SEulerRotationf EulerRotation = Transform.GetEulerRotation();
		EulerRotation.Yaw -= AxisEvent.MotionX * 0.15f;
		EulerRotation.Pitch -= AxisEvent.MotionY * 0.15f;
		Transform.SetEulerRotation(EulerRotation);
		SetTransform(Transform);
		break;
	}
	return SInputHandleResult();

}
