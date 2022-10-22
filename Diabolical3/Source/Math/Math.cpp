#include "Math.h"
#include <glm/gtx/rotate_vector.hpp>
#include "Logging/Logging.h"

const SVector2f SVector2f::ZeroVector = { 0, 0 };
const SVector3f SVector3f::ZeroVector = { 0, 0, 0 };
const SVector4f SVector4f::ZeroVector = { 0, 0, 0, 0 };
const SVector2i SVector2i::ZeroVector = { 0, 0 };
const SVector3i SVector3i::ZeroVector = { 0, 0, 0 };

const SVector2f SVector2f::OneVector = { 1, 1 };
const SVector3f SVector3f::OneVector = { 1, 1, 1 };
const SVector4f SVector4f::OneVector = { 1, 1, 1, 1};
const SVector2i SVector2i::OneVector = { 1, 1 };
const SVector3i SVector3i::OneVector = { 1, 1, 1 };

const STransformf STransformf::Identity = {{ 0, 0, 0 }, { 1, 1, 1 }};
const SMatrix44f SMatrix44f::Identity = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }};
const SMatrix33f SMatrix33f::Identity = {{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }};

const SQuaternionf SQuaternionf::Identity = SQuaternionf(glm::vec3(0.f, 0.f, 0.f));
const SEulerRotationf SEulerRotationf::Identity = { 0, 0, 0 };

void SEulerRotationf::ToQuat(SQuaternionf& OutQuat) const
{
	const float DegToRad = 3.14159f / 180.f;
	const float HalfDegToRad = DegToRad / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	const float PitchNoWinding = fmod(x, 360.0f);
	const float YawNoWinding = fmod(y, 360.0f);
	const float RollNoWinding = fmod(z, 360.0f);

	SP = sin(PitchNoWinding * HalfDegToRad);
	CP = cos(PitchNoWinding * HalfDegToRad);
	SY = sin(YawNoWinding * HalfDegToRad);
	CY = cos(YawNoWinding * HalfDegToRad);
	SR = sin(RollNoWinding * HalfDegToRad);
	CR = cos(RollNoWinding * HalfDegToRad);

	OutQuat.x = CR * SP * SY - SR * CP * CY;
	OutQuat.y = -CR * SP * CY - SR * CP * SY;
	OutQuat.z = CR * CP * SY - SR * SP * CY;
	OutQuat.w = CR * CP * CY + SR * SP * SY;
}

void SQuaternionf::ToEuler(SEulerRotationf& OutRotation) const
{
	const float SingularityTest = z * x - w * y;
	const float YawY = 2.0f * (w * z + x * y);
	const float YawX = (1.0f - 2.0f * ((y * y) + (z * z)));

	const float SingularityThreshold = 0.4999995f;
	const float RadToDeg = 57.2958f;
	float Pitch, Yaw, Roll;

	if (SingularityTest < -SingularityThreshold)
	{
		Pitch = -90.0f;
		Yaw = (atan2(YawY, YawX) * RadToDeg);
		Roll = SEulerRotationf::NormalizeAxis(-Yaw - (2.0f * atan2(x, w) * RadToDeg));
	}
	else if (SingularityTest > SingularityThreshold)
	{
		Pitch = 90.0f;
		Yaw = (atan2(YawY, YawX) * RadToDeg);
		Roll = SEulerRotationf::NormalizeAxis(Yaw - (2.0f * atan2(x, w) * RadToDeg));
	}
	else
	{
		Pitch = (asin(2.0f * SingularityTest) * RadToDeg);
		Yaw = (atan2(YawY, YawX) * RadToDeg);
		Roll = (atan2(-2.0f * (w * x + y * z), (1.0f - 2.0f * ((x * x) + (y * y)))) * RadToDeg);
	}

	OutRotation = { Pitch, Yaw, Roll };
}
