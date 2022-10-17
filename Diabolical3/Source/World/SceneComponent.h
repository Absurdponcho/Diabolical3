#pragma once
#include "Object.h"
#include "Online/RPC.h"

class DSceneComponent : public DObject
{
public:
	DSceneComponent();
	virtual ~DSceneComponent() {};

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

	virtual void RegisterSyncVars(DVector<DSyncVarBase*>& SyncVars) override;

	DObjectPtr<class DSceneObject> GetParent();

protected:
	void SetParent(DObjectPtr<class DSceneObject> Parent);

	DSyncVar<DObjectPtr<class DSceneObject>> ParentObject;

	friend class DSceneObject;
};