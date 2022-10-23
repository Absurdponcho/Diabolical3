#pragma once
#include "../Quaternion/Quaternion.inl"

template<typename T>
struct MEulerRotation
{
public:
	MEulerRotation()
		: Pitch(0), Yaw(0), Roll(0) {}

	template <typename XType, typename YType, typename ZType>
	MEulerRotation(XType InPitch, YType InYaw, ZType InRoll)
		: Pitch((T)InPitch), Yaw((T)InYaw), Roll((T)InRoll) {}

	MQuaternion<T> ToQuat() const
	{
		// From
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// Yaw Roll & Pitch needed to be moved around.

		MQuaternion<T> OutQuat;
		const float DegToRad = 0.0174533f;
		const float HalfDegToRad = DegToRad / 2.f;

		const float PitchNoWinding = fmod(Yaw, 360.0f);
		const float YawNoWinding = fmod(Roll , 360.0f);
		const float RollNoWinding = fmod(Pitch, 360.0f);

		T SP = sin(PitchNoWinding * HalfDegToRad);
		T CP = cos(PitchNoWinding * HalfDegToRad);
		T SY = sin(YawNoWinding * HalfDegToRad);
		T CY = cos(YawNoWinding * HalfDegToRad);
		T SR = sin(RollNoWinding * HalfDegToRad);
		T CR = cos(RollNoWinding * HalfDegToRad);

		OutQuat.W = CR * CP * CY + SR * SP * SY;
		OutQuat.X = SR * CP * CY - CR * SP * SY;
		OutQuat.Y = CR * SP * CY + SR * CP * SY;
		OutQuat.Z = CR * CP * SY - SR * SP * CY;
		return OutQuat;
	}

	MEulerRotation<T> operator*(T Other)
	{
		return { Pitch * Other, Yaw * Other, Roll * Other };
	}

	static T ClampAxis(T InAxis)
	{
		InAxis = fmod(InAxis, (T)360.0);

		if (InAxis < (T)0)
		{
			InAxis += (T)360.0;
		}

		return InAxis;
	}

	static T NormalizeAxis(T InAxis)
	{
		InAxis = ClampAxis(InAxis);

		if (InAxis > (T)180.0)
		{
			InAxis -= (T)360.0;
		}

		return InAxis;
	}

	T Pitch;
	T Yaw;
	T Roll;
};
