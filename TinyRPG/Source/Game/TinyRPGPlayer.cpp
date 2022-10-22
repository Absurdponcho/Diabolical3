#include "TinyRPGPlayer.h"
#include "Logging/Logging.h"
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/MeshComponent.h"
#include "Graphics/Window.h"
#include "Time/Time.h"

DRegisteredObject<DTinyRPGPlayer> RegisteredObject = DRegisteredObject<DTinyRPGPlayer>();
DRegisteredObjectBase* DTinyRPGPlayer::GetRegisteredObject() const
{
	return &RegisteredObject;
}

void DTinyRPGPlayer::PostConstruct()
{
	DPlayer::PostConstruct();
}

void DTinyRPGPlayer::Tick(const STickInfo& TickInfo)
{
	DPlayer::Tick(TickInfo);

	if (InputVector != SVector3f::ZeroVector)
	{
		if (IsRemoteClient())
		{
			SVector3f NormalizedInputVector = InputVector.Normalized();
			NormalizedInputVector.x = NormalizedInputVector.x * TickInfo.DeltaTime * 10.0f;
			NormalizedInputVector.y = NormalizedInputVector.y * TickInfo.DeltaTime * 10.0f;
			NormalizedInputVector.z = NormalizedInputVector.z * TickInfo.DeltaTime * 10.0f;
			ServerRequestMove.Send(NormalizedInputVector);
		}
		else
		{
			SVector3f NormalizedInputVector = InputVector.Normalized();
			STransformf Transform = GetTransform();
			Transform.Position.x += NormalizedInputVector.x * TickInfo.DeltaTime * 10.0f;
			Transform.Position.y += NormalizedInputVector.y * TickInfo.DeltaTime * 10.0f;
			Transform.Position.z += NormalizedInputVector.z * TickInfo.DeltaTime * 10.0f;
			SetTransform(Transform);
		}
	}
}

void DTinyRPGPlayer::ServerRequestMove_Receive(SVector3f Move)
{
	STransformf Transform = GetTransform();
	Transform.Position.x += Move.x;
	Transform.Position.y += Move.y;
	Transform.Position.z += Move.z;
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
		InputVector.z += KeyEvent.bReleased ? 1 : -1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_S:
	case EKeyCode::KC_Down:
	{
		InputVector.z += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_A:
	case EKeyCode::KC_Left:
	{
		InputVector.x += KeyEvent.bReleased ? 1 : -1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_D:
	case EKeyCode::KC_Right:
	{
		InputVector.x += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_Space:
	{
		InputVector.y += KeyEvent.bReleased ? -1 : 1;

		SInputHandleResult HandleResult;
		HandleResult.bConsumeInput = true;
		return HandleResult;
	}
	case EKeyCode::KC_LeftCtrl:
	{
		InputVector.y += KeyEvent.bReleased ? 1 : -1;

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
		EulerRotation.y += AxisEvent.MotionX * 0.15f;
		EulerRotation.x += AxisEvent.MotionY * 0.15f;
		Transform.SetEulerRotation(EulerRotation);
		SetTransform(Transform);
		break;
	}
	return SInputHandleResult();

}
