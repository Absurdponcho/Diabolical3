#pragma once
#include "World/Object.h"

class DPlayer : public DObject
{
public:
	virtual ~DPlayer() {}

	virtual DRegisteredObjectBase* GetRegisteredObject() const override;

};