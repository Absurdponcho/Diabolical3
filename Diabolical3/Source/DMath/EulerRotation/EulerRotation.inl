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
		MQuaternion<T> OutQuat;
		const float DegToRad = 0.0174533f;
		const float HalfDegToRad = DegToRad / 2.f;
		float SP, SY, SR;
		float CP, CY, CR;

		const float PitchNoWinding = fmod(Pitch, 360.0f);
		const float YawNoWinding = fmod(Yaw, 360.0f);
		const float RollNoWinding = fmod(Roll, 360.0f);

		SP = sin(PitchNoWinding * HalfDegToRad);
		CP = cos(PitchNoWinding * HalfDegToRad);
		SY = sin(YawNoWinding * HalfDegToRad);
		CY = cos(YawNoWinding * HalfDegToRad);
		SR = sin(RollNoWinding * HalfDegToRad);
		CR = cos(RollNoWinding * HalfDegToRad);

		OutQuat.X = CR * SP * SY - SR * CP * CY;
		OutQuat.Y = -CR * SP * CY - SR * CP * SY;
		OutQuat.Z = CR * CP * SY - SR * SP * CY;
		OutQuat.W = CR * CP * CY + SR * SP * SY;
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
