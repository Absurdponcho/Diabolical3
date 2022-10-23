#pragma once
#include "../Vector/Vector.inl"
#include "../Quaternion/Quaternion.inl"
#include "../Matrix/Matrix.inl"
#include "../EulerRotation/EulerRotation.inl"

template <typename T>
struct MTransform
{
public:

	MTransform()
		: Position({ 0, 0, 0 }), Scale({ 1, 1, 1 }), Rotation({ 0, 0, 0, 1 }) {}

	template <typename PType, typename SType, typename RType>
	MTransform(const MVector3<PType>& InPosition, const MVector3<SType>& InScale, const MQuaternion<RType>& InRotation)
		: Position(InPosition), Scale(InScale), Rotation(InRotation) {}

	template <typename OtherType>
	bool operator==(const MTransform<OtherType>& Other) const
	{
		return Other.Position == Position && Other.Scale == Scale && Other.Rotation == Rotation;
	}

	MMatrix44<T> GetModelMatrix() const
	{
		MMatrix44<T> ModelMatrix = MMatrix44<T>::Identity;
		ModelMatrix = MMatrix44<T>::Translate(ModelMatrix, Position);
		ModelMatrix = MMatrix44<T>::Scale(ModelMatrix, Scale);
		ModelMatrix = ModelMatrix * Rotation.ToMatrix();
		return ModelMatrix;
	}

	const MVector3<T>& GetPosition() const
	{
		return Position;
	}

	const MQuaternion<T>& GetRotation() const
	{
		return Rotation;
	}

	MEulerRotation<T> GetEulerRotation() const
	{
		MEulerRotation<T> Euler;
		Euler = QuaternionToEuler(Rotation.Normalized());
		return Euler;
	}

	const MVector3<T>& GetScale() const
	{
		return Scale;
	}

	void SetPosition(const MVector3<T>& InPosition)
	{
		Position = InPosition;
	}

	void SetRotation(const MQuaternion<T>& InRotation)
	{
		Rotation = InRotation.Normalized();
	}

	void SetScale(const MVector3<T>& InScale)
	{
		Scale = InScale;
	}

	void SetEulerRotation(MEulerRotation<T> InRotation)
	{
		MQuaternion<T> Quat;
		Quat = InRotation.ToQuat();
		SetRotation(Quat);
	}

	MVector3<T> Position;
	MVector3<T> Scale;
	MQuaternion<T> Rotation;
};