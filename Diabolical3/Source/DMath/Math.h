#pragma once
#include <stdint.h>
#include <cmath>

namespace DMath
{
	#include "Vector/Vector.inl"
	#include "Matrix/Matrix.inl"
	#include "Quaternion/Quaternion.inl"
	#include "Transform/Transform.inl"
	#include "EulerRotation/EulerRotation.inl"
	#include "MathFuncs.inl"

	typedef MVector4<float> SVector4f;
	typedef MVector4<double> SVector4d; 
	typedef MVector4<int64_t> SVector4i; 
	
	typedef MVector3<float> SVector3f;
	typedef MVector3<double> SVector3d;
	typedef MVector3<int64_t> SVector3i;

	typedef MVector2<float> SVector2f;
	typedef MVector2<double> SVector2d;
	typedef MVector2<int64_t> SVector2i;

	typedef MMatrix44<float> SMatrix44f;
	typedef MMatrix44<double> SMatrix44d;

	typedef MQuaternion<float> SQuaternionf;
	typedef MQuaternion<double> SQuaterniond;

	typedef MEulerRotation<float> SEulerRotationf;
	typedef MEulerRotation<double> SEulerRotationd;

	typedef MTransform<float> STransformf;
	typedef MTransform<double> STransformd;

}

using namespace DMath;

#include "MathSerialize.inl"