#pragma once
#include "Types/DVector.h"
#include "Types/Map.h"

class DObjectRegistry
{
public:
	static const DMap<uint64_t, class DRegisteredObjectBase*>& GetRegisteredObjects();

private:
	static void RegisterObjectBase(class DRegisteredObjectBase*);

	friend class DRegisteredObjectBase;
};