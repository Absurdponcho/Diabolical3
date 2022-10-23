#pragma once
#include "Quaternion/Quaternion.inl"
#include "EulerRotation/EulerRotation.inl"

template<typename T>
MEulerRotation<T> QuaternionToEuler(const MQuaternion<T>& Quat)
{
	MEulerRotation<T> OutRotation;
	const T SingularityTest = Quat.X * Quat.Y + Quat.W * Quat.Z;
	const T YawY = T(2) * (Quat.W * Quat.Z + Quat.X * Quat.Y);
	const T YawX = (T(1.0) - T(2.0) * ((Quat.Y * Quat.Y) + (Quat.Z * Quat.Z)));

	const T SingularityThreshold = T(0.4999995);
	const T RadToDeg = T(57.2958);
	T Pitch, Yaw, Roll;

	T Pi = T(3.14159);

	// @TODO: Check singularity properly

	//if (SingularityTest < -SingularityThreshold)
	//{
	//	Yaw = 2 * atan2(Quat.X, Quat.W) * RadToDeg;
	//	Pitch = Pi / 2 * RadToDeg;
	//	//Roll = (atan2(YawY, YawX) * RadToDeg);
	//	//Pitch = MEulerRotation<T>::NormalizeAxis(-Roll - (T(2.0) * atan2(Quat.X, Quat.W) * RadToDeg));
	//}
	//else if (SingularityTest > SingularityThreshold)
	//{

	//}
	//else
	//{
		Yaw = atan2(2 * Quat.Y * Quat.W - 2 * Quat.X * Quat.Z, 1 - 2 * (Quat.Y * Quat.Y) - 2 * (Quat.Z * Quat.Z)) * RadToDeg;
		Roll = asin(2 * SingularityTest) * RadToDeg;
		Pitch = atan2(2 * Quat.X * Quat.W - 2 * Quat.Y * Quat.Z, 1 - 2 * (Quat.X * Quat.X) - 2 * (Quat.Z * Quat.Z)) * RadToDeg;
	//}

	OutRotation = { Pitch, Yaw, Roll };

	return OutRotation;
}