#pragma once

#if USING_GLM

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

struct SVector2f : public glm::vec2
{
public:
	SVector2f() {}
	SVector2f(glm::vec2 In) : glm::vec2(In) {}
	SVector2f(glm::ivec2 In) : glm::vec2(In) {}

	template <typename X, typename Y>
	SVector2f(X x, Y y) : glm::vec2(x, y) {}

	inline float Length() const
	{
		return glm::length((glm::vec2)*this);
	}

	inline SVector2f Normalized() const
	{
		float ThisLength = Length();
		return SVector2f(x / ThisLength, y / ThisLength);
	}

	static const SVector2f ZeroVector;
	static const SVector2f OneVector;
};

struct SVector3f : public glm::vec3
{
public:
	SVector3f() {}

	SVector3f(glm::vec3 In) 
		: glm::vec3(In) {}

	SVector3f(glm::ivec3 In) 
		: glm::vec3(In) {}

	template <typename X, typename Y, typename Z>
	SVector3f(X x, Y y, Z z) 
		: glm::vec3(x, y, z) {}

	inline float Length() const
	{
		return glm::length((glm::vec3)*this);
	}

	inline SVector3f Normalized() const
	{
		float ThisLength = Length();
		return SVector3f(x / ThisLength, y / ThisLength, z / ThisLength);
	}

	static const SVector3f ZeroVector;
	static const SVector3f OneVector;
};

struct SEulerRotationf : public SVector3f
{
public:
	using SVector3f::SVector3f;

	void ToQuat(struct SQuaternionf& OutQuat) const;

	static float ClampAxis(float InAxis)
	{
		InAxis = fmod(InAxis, 360.0f);

		if (InAxis < 0.0f)
		{
			InAxis += 360.0f;
		}

		return InAxis;
	}

	static float NormalizeAxis(float InAxis)
	{
		InAxis = ClampAxis(InAxis);

		if (InAxis > 180.0f)
		{
			InAxis -= 360.0f;
		}

		return InAxis;
	}

	static const SEulerRotationf Identity;

};

struct SVector4f : public glm::vec4
{
public:
	SVector4f() {}

	SVector4f(glm::vec4 In) 
		: glm::vec4(In) {}

	SVector4f(glm::ivec4 In) 
		: glm::vec4(In) {}

	template <typename X, typename Y, typename Z, typename W>
	SVector4f(X x, Y y, Z z, W w) : glm::vec4(x, y, z, w) {}

	inline float Length() const
	{
		return glm::length((glm::vec4)*this);
	}

	inline SVector4f Normalized() const
	{
		float ThisLength = Length();
		return SVector4f(x / ThisLength, y / ThisLength, z / ThisLength, w / ThisLength);
	}

	static const SVector4f ZeroVector;
	static const SVector4f OneVector;
};

struct SVector2i : public glm::ivec2
{
public:
	SVector2i() {}

	SVector2i(glm::ivec2 In) 
		: glm::ivec2(In) {}

	template <typename X, typename Y>
	SVector2i(X x, Y y) : glm::ivec2(x, y) {}

	static const SVector2i ZeroVector;
	static const SVector2i OneVector;
};

struct SVector3i : public glm::ivec3
{
public:
	SVector3i() {}
	SVector3i(glm::ivec3 In) 
		: glm::ivec3(In) {}

	template <typename X, typename Y, typename Z>
	SVector3i(X x, Y y, Z z) : glm::ivec3(x, y, z) {}

	static const SVector3i ZeroVector;
	static const SVector3i OneVector;
};

struct SMatrix44f : public glm::mat4x4
{
public:
	SMatrix44f() {}

	SMatrix44f(glm::mat4x4 In) 
		: glm::mat4x4(In) {}
	SMatrix44f(SVector4f X, SVector4f Y, SVector4f Z, SVector4f W) 
		: glm::mat4x4(X, Y, Z, W) {}

	static const SMatrix44f Identity;
};

struct SMatrix33f : public glm::mat3x3
{
public:
	SMatrix33f() {}

	SMatrix33f(glm::mat3x3 In)
		: glm::mat3x3(In) {}
	SMatrix33f(SVector3f X, SVector3f Y, SVector3f Z)
		: glm::mat3x3(X, Y, Z) {}

	static const SMatrix33f Identity;
};

struct SQuaternionf : public glm::quat
{
public:
	SQuaternionf() {}

	SQuaternionf(glm::quat In)
		: glm::quat(In) {}

	void ToEuler(struct SEulerRotationf& OutRotation) const;
	SQuaternionf Normalized() const
	{
		return glm::normalize(*this);
	}
	SQuaternionf Conjugate() const
	{
		return glm::conjugate(*this);
	}

	static const SQuaternionf Identity;
};

struct STransformf
{
public:
	STransformf() {}

	STransformf(SVector3f InPosition, SVector3f InScale)
		: Position(InPosition), Scale(InScale) {}

	STransformf(SVector3f InPosition, SVector3f InScale, SQuaternionf InRotation)
		: Position(InPosition), Scale(InScale), Rotation(InRotation) {}

	SMatrix44f GetModelMatrix() const
	{
		SMatrix44f ModelMatrix = SMatrix44f::Identity;
		ModelMatrix = glm::translate(ModelMatrix, Position);
		ModelMatrix = glm::scale(ModelMatrix, Scale);
		ModelMatrix *= glm::mat4_cast(Rotation);
		return ModelMatrix;
	}

	const SVector3f& GetPosition() const
	{
		return Position;
	}

	const SQuaternionf& GetRotation() const
	{
		return Rotation;
	}

	SEulerRotationf GetEulerRotation() const
	{
		SEulerRotationf Euler;
		Rotation.Normalized().ToEuler(Euler);
		return Euler;
	}

	const SVector3f& GetScale() const
	{
		return Scale;
	}

	void SetPosition(const SVector3f& InPosition)
	{
		Position = InPosition;
	}

	void SetRotation(const SQuaternionf& InRotation)
	{
		Rotation = InRotation.Normalized();
	}

	void SetScale(const SVector3f& InScale)
	{
		Scale = InScale;
	}

	void SetEulerRotation(SEulerRotationf InRotation)
	{
		SQuaternionf Quat;
		InRotation.ToQuat(Quat);
		SetRotation(Quat);
	}

	SVector3f RotateVector(SVector3f InVector)
	{
		glm::vec3 v = InVector;
		glm::quat q = Rotation;
		glm::mat4 m = glm::mat4(q);
		glm::vec4 v4 = m * glm::vec4(v, 1.0f);
		return glm::vec3(v4.x, v4.y, v4.z);
	}

	bool Equals(const STransformf& Other) const
	{
		return Other.Position == Position && Other.Scale == Scale && Other.Rotation == Rotation;
	}

	bool operator==(const STransformf& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const STransformf& Other) const
	{
		return !Equals(Other);
	}

	static const STransformf Identity;

	SVector3f Position = SVector3f::ZeroVector;
	SVector3f Scale = SVector3f::OneVector;
	SQuaternionf Rotation = SQuaternionf::Identity;
};

#endif