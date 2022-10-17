#include "ObjectRegistry.h"
#include "Object.h"
#include "Logging/Logging.h"

void DObjectRegistry::RegisterObjectBase(DRegisteredObjectBase* BaseObject)
{
	const_cast<DMap<uint64_t, class DRegisteredObjectBase*>& >(GetRegisteredObjects())[std::hash<DString>()(BaseObject->GetObjectName())] = BaseObject;
}

const DMap<uint64_t, class DRegisteredObjectBase*>& DObjectRegistry::GetRegisteredObjects()
{
	static DMap<uint64_t, class DRegisteredObjectBase*> RegisteredObjects;

	return RegisteredObjects;
}
