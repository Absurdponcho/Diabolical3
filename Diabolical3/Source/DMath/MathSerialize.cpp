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
	SerializeTuple( std::tuple<float, float, float>( Value->X, Value->Y, Value->Z ), BitStream);
	return true;
}

template<>
bool DBitSerializer<SVector3f>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<float, float, float> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->X = std::get<0>(Tuple);
	Value->Y = std::get<1>(Tuple);
	Value->Z = std::get<2>(Tuple);
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
	SerializeTuple(std::tuple<float, float, float, float>(Value->X, Value->Y, Value->Z, Value->W), BitStream);
	return true;
}

template<>
bool DBitSerializer<SQuaternionf>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<float, float, float, float> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->X = std::get<0>(Tuple);
	Value->Y = std::get<1>(Tuple);
	Value->Z = std::get<2>(Tuple);
	Value->W = std::get<3>(Tuple);
	return true;
}

// ======= SEulerRotationf =======
template<>
bool DBitSerializer<SEulerRotationf>::HasSerializeOverride()
{
	return true;
}

template<>
bool DBitSerializer<SEulerRotationf>::SerializeOverride(DBitStream& BitStream)
{
	SerializeTuple(std::tuple<float, float, float>(Value->Pitch, Value->Yaw, Value->Roll), BitStream);
	return true;
}

template<>
bool DBitSerializer<SEulerRotationf>::DeserializeOverride(DBitStream& BitStream)
{
	std::tuple<float, float, float> Tuple;
	DeserializeTuple(Tuple, BitStream);
	Value->Pitch = std::get<0>(Tuple);
	Value->Yaw = std::get<1>(Tuple);
	Value->Roll = std::get<2>(Tuple);
	return true;
}