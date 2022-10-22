#pragma once
#include "World/SceneObject.h"

class DPlayer : public DSceneObject
{
public:
	DPlayer()
	{
		bIsNetworked = true;
	}

	virtual ~DPlayer() {}

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void Tick(const STickInfo& TickInfo) override;

	bool IsControlled() const;

	virtual void OnControlChanged(bool bIsControlled) {}

protected:
	void CheckCamera();
	bool ShouldHaveCamera() const;
	bool bCurrentControl = false;
	bool bSentControlCall = false;
	DObjectPtr<class DCameraComponent> CameraComponent;
};