#pragma once
#include "../Vector/Vector.inl"
#include <memory.h>
#include <cmath>

template <typename T>
struct MMatrix44
{
public:

	MMatrix44()
	{
		memset(&Values, 0, sizeof(T) * 16);
	}

	MMatrix44(const MVector4<T>& InX, const MVector4<T>& InY, const MVector4<T>& InZ, const MVector4<T>& InW)
	{
		Values[0][0] = InX.X;
		Values[0][1] = InX.Y;
		Values[0][2] = InX.Z;
		Values[0][3] = InX.W;
		Values[1][0] = InY.X;
		Values[1][1] = InY.Y;
		Values[1][2] = InY.Z;
		Values[1][3] = InY.W;
		Values[2][0] = InZ.X;
		Values[2][1] = InZ.Y;
		Values[2][2] = InZ.Z;
		Values[2][3] = InZ.W;
		Values[3][0] = InW.X;
		Values[3][1] = InW.Y;
		Values[3][2] = InW.Z;
		Values[3][3] = InW.W;
	}

	template <typename Type1, typename Type2, typename Type3, typename Type4>
	MMatrix44(const MVector4<Type1>& InX, const MVector4<Type2>& InY, const MVector4<Type3>& InZ, const MVector4<Type4>& InW)
	{
		Values[0][0] = (T)InX.X;
		Values[0][1] = (T)InX.Y;
		Values[0][2] = (T)InX.Z;
		Values[0][3] = (T)InX.W;
		Values[1][0] = (T)InY.X;
		Values[1][1] = (T)InY.Y;
		Values[1][2] = (T)InY.Z;
		Values[1][3] = (T)InY.W;
		Values[2][0] = (T)InZ.X;
		Values[2][1] = (T)InZ.Y;
		Values[2][2] = (T)InZ.Z;
		Values[2][3] = (T)InZ.W;
		Values[3][0] = (T)InW.X;
		Values[3][1] = (T)InW.Y;
		Values[3][2] = (T)InW.Z;
		Values[3][3] = (T)InW.W;
	}

	static MMatrix44<T> Translate(const MMatrix44<T>& Matrix, const MVector3<T>& Translation)
	{
		MMatrix44<T> OutMat = Matrix;
		const MVector4<T>& Column0 = *(const MVector4<T>*)& Matrix.Values[0][0];
		const MVector4<T>& Column1 = *(const MVector4<T>*)& Matrix.Values[1][0];
		const MVector4<T>& Column2 = *(const MVector4<T>*)& Matrix.Values[2][0];
		const MVector4<T>& Column3 = *(const MVector4<T>*)& Matrix.Values[3][0];

		MVector4<T>& OutColumn3 = *(MVector4<T>*)&OutMat.Values[3];
		OutColumn3 = Column0 * Translation.X + Column1 * Translation.Y + Column2 * Translation.Z + Column3;
		return OutMat;
	}
	
	static MMatrix44<T> Scale(const MMatrix44<T>& Matrix, const MVector3<T>& Scale)
	{
		MMatrix44<T> OutMat = Matrix;
		const MVector4<T>& Column0 = *(const MVector4<T>*) & Matrix.Values[0][0];
		const MVector4<T>& Column1 = *(const MVector4<T>*) & Matrix.Values[1][0];
		const MVector4<T>& Column2 = *(const MVector4<T>*) & Matrix.Values[2][0];
		const MVector4<T>& Column3 = *(const MVector4<T>*) & Matrix.Values[3][0];

		MVector4<T>& OutColumn0 = *(MVector4<T>*) & OutMat.Values[0][0];
		MVector4<T>& OutColumn1 = *(MVector4<T>*) & OutMat.Values[1][0];
		MVector4<T>& OutColumn2 = *(MVector4<T>*) & OutMat.Values[2][0];
		MVector4<T>& OutColumn3 = *(MVector4<T>*) & OutMat.Values[3][0];
		OutColumn0 = Column0 * Scale.X;
		OutColumn1 = Column1 * Scale.Y;
		OutColumn2 = Column2 * Scale.Z;
		OutColumn3 = Column3;
		return OutMat;
	}

	static MMatrix44<T>	Perspective(T Fov, T Aspect, T Near, T Far)
	{
		MMatrix44<T> Result = MMatrix44<T>::Identity;

		T frustumDepth = Far - Near;
		T oneOverDepth = T(1.0) / frustumDepth;
		bool leftHanded = true;

		Result.Values[1][1] = T(1.0) / std::tan(T(0.5) * Fov);
		Result.Values[0][0] = (leftHanded ? T(1.0) : T(-1.0)) * Result.Values[1][1] / Aspect;
		Result.Values[2][2] = Far * oneOverDepth;
		Result.Values[3][2] = (-Far * Near) * oneOverDepth;
		Result.Values[2][3] = 1;
		Result.Values[3][3] = 0;

		return Result;
	}

	MMatrix44<T> operator*(const MMatrix44<T>& Other) const
	{
		const int N = 4;

		MMatrix44<T> Result;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				T num = 0;
				for (int k = 0; k < N; k++) {
					num += Values[i][k] * Other.Values[k][j];
				}
				Result.Values[i][j] = num;
			}
		}

		return Result;
	}

	static const MMatrix44<T> Identity;

	T Values[4][4];
};