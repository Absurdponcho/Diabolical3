#pragma once
#include "Game/Player.h"
#include "Input/Input.h"
#include "Online/RPC.h"
#include "Math/Math.h"

class DTinyRPGPlayer : public DPlayer
{
public:
	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void PostConstruct() override;

	SInputHandleResult KeyListener(const SKeyEvent& KeyEvent);
	virtual void Tick(const STickInfo& TickInfo) override;
	virtual void OnControlChanged(bool bIsControlled) override;

protected:
	SVector3f InputVector;

	DObjectPtr<class DCubeComponentLocal> RemoteCube;
	DWeakPtr<class DWindow> WindowWeak;
	DNetRPCServer<SVector3f> ServerRequestMove = { this, &DTinyRPGPlayer::ServerRequestMove_Receive };
	void ServerRequestMove_Receive(SVector3f Move);
};