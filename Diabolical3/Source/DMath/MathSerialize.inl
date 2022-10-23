#pragma once
#include "Types/BitStream.h"
#include "Math.h"

// ======= STransformf =======
template<>
bool DBitSerializer<STransformf>::HasSerializeOverride();
template<>
bool DBitSerializer<STransformf>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<STransformf>::DeserializeOverride(DBitStream& BitStream);

// ======= SVector3f =======
template<>
bool DBitSerializer<SVector3f>::HasSerializeOverride();
template<>
bool DBitSerializer<SVector3f>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<SVector3f>::DeserializeOverride(DBitStream& BitStream);

// ======= SQuaternionf =======
template<>
bool DBitSerializer<SQuaternionf>::HasSerializeOverride();
template<>
bool DBitSerializer<SQuaternionf>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<SQuaternionf>::DeserializeOverride(DBitStream& BitStream);