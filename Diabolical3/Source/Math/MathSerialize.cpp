#include "Math.h"

// ======= STransformf =======
template<>
bool DBitSerializer<STransformf>::HasSerializeOverride()
{
	return true;
}

template<>
bool DBitSerializer<STransformf>::SerializeOverride(DBitStream& BitStream)
{
	SerializeTuple( std::tuple<SVector3f, SQuaternionf, SVector3f>( Value->GetPosition(), Value->GetRotation(), Value->GetScale()), BitStream);
	return true;
}

template<>
bool DBitSerializer<STransformf>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<SVector3f, SQuaternionf, SVector3f> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->SetPosition(std::get<0>(Tuple));
	Value->SetRotation(std::get<1>(Tuple));
	Value->SetScale(std::get<2>(Tuple));
	return true;
}

// ======= SVector3f =======
template<>
bool DBitSerializer<SVector3f>::HasSerializeOverride()
{
	return true;
}

template<>
bool DBitSerializer<SVector3f>::SerializeOverride(DBitStream& BitStream)
{
	SerializeTuple( std::tuple<float, float, float>( Value->x, Value->y, Value->z ), BitStream);
	return true;
}

template<>
bool DBitSerializer<SVector3f>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<float, float, float> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->x = std::get<0>(Tuple);
	Value->y = std::get<1>(Tuple);
	Value->z = std::get<2>(Tuple);
	return true;
}

// ======= SQuaternionf =======
template<>
bool DBitSerializer<SQuaternionf>::HasSerializeOverride()
{
	return true;
}

template<>
bool DBitSerializer<SQuaternionf>::SerializeOverride(DBitStream& BitStream)
{
	SerializeTuple(std::tuple<float, float, float, float>(Value->x, Value->y, Value->z, Value->w), BitStream);
	return true;
}

template<>
bool DBitSerializer<SQuaternionf>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<float, float, float, float> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->x = std::get<0>(Tuple);
	Value->y = std::get<1>(Tuple);
	Value->z = std::get<2>(Tuple);
	Value->w = std::get<3>(Tuple);
	return true;
}