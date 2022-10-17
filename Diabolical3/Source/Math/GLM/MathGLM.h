#pragma once

#if USING_GLM

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SVector2f : public glm::vec2
{
public:
	SVector2f() {}
	SVector2f(glm::vec2 In) : glm::vec2(In) {}
	SVector2f(glm::ivec2 In) : glm::vec2(In) {}

	template <typename X, typename Y>
	SVector2f(X x, Y y) : glm::vec2(x, y) {}

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

	static const SVector3f ZeroVector;
	static const SVector3f OneVector;
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

	static const SQuaternionf Identity;
};

struct STransformf
{
public:
	STransformf() {}

	STransformf(SVector3f InPosition, SVector3f InScale)
		: Position(InPosition), Scale(InScale) {}

	SVector3f Position = SVector3f::ZeroVector;
	SVector3f Scale = SVector3f::OneVector;
	SQuaternionf Rotation = SQuaternionf::Identity;

	SMatrix44f GetModelMatrix()
	{
		SMatrix44f ModelMatrix = SMatrix44f::Identity;
		ModelMatrix = glm::translate(ModelMatrix, Position);
		ModelMatrix = glm::scale(ModelMatrix, Scale);
		ModelMatrix *= glm::mat4_cast(Rotation);
		return ModelMatrix;
	}

	static const STransformf Identity;
};

#endif