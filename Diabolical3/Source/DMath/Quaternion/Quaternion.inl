#pragma once
#include "../Vector/Vector.inl"
#include "../Matrix/Matrix.inl"

template <typename T>
struct MQuaternion
{
public:
	MQuaternion()
		: X(0), Y(0), Z(0), W(1) {}

	template <typename XType, typename YType, typename ZType, typename WType>
	MQuaternion(XType InX, YType InY, ZType InZ, WType InW)
		: X((T)InX), Y((T)InY), Z((T)InZ), W((T)InW) {}

	template <typename AngleType>
	static MQuaternion<T> FromAngleAxis(AngleType Angle, MVector3<T> Axis)
	{
		const AngleType DegToRad = (AngleType)0.0174533;
		Angle *= DegToRad;
		return { Axis.X * sin(Angle / 2), Axis.Y * sin(Angle / 2), Axis.Z * sin(Angle / 2), cos(Angle / 2) };
	}

	template <typename OtherType>
	bool operator==(const MQuaternion<OtherType>& Other) const
	{
		return Other.X == X && Other.Y == Y && Other.Z == Z && Other.W == W;
	}

	template <typename OtherType>
	MQuaternion<T> operator*(const MQuaternion<OtherType>& Other) const
	{
		return {
			W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z,  // 1
			W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,  // i
			W * Other.Y - X * Other.Z + Y * Other.W + Z * Other.X,  // j
			W * Other.Z + X * Other.Y - Y * Other.X + Z * Other.W   // k
		};
	}

	template <typename Type1, typename Type2>
	static T Dot(const MQuaternion<Type1>& Left, const MQuaternion<Type2>& Right)
	{
		return Left.X * Right.X + Left.Y * Right.Y + Left.Z * Right.Z + Left.W * Right.W;
	}

	template <typename OtherType>
	T Dot(const MQuaternion<OtherType>& Other) const
	{
		return X * Other.X + Y * Other.Y + Z * Other.Z + W * Other.W;
	}

	T Length() const
	{
		return sqrt(Dot(*this));
	}

	MQuaternion<T> Normalized() const
	{
		T ThisLength = Length();
		T OneOverLen = ((T)1) / ThisLength;

		return { X * OneOverLen, Y * OneOverLen, Z * OneOverLen, W * OneOverLen };
	}

	MMatrix44<T> ToMatrix() const
	{
		MMatrix44<T> Result;

		T XX(X * X);
		T YY(Y * Y);
		T ZZ(Z * Z);
		T XZ(X * Z);
		T XY(X * Y);
		T YZ(Y * Z);
		T WX(W * X);
		T WY(W * Y);
		T WZ(W * Z);

		Result.Values[0][0] = T(1) - T(2) * (YY + ZZ);
		Result.Values[0][1] = T(2) * (XY + WZ);
		Result.Values[0][2] = T(2) * (XZ - WY);
		Result.Values[0][3] = T(0);

		Result.Values[1][0] = T(2) * (XY - WZ);
		Result.Values[1][1] = T(1) - T(2) * (XX + ZZ);
		Result.Values[1][2] = T(2) * (YZ + WX);
		Result.Values[1][3] = T(0);

		Result.Values[2][0] = T(2) * (XZ + WY);
		Result.Values[2][1] = T(2) * (YZ - WX);
		Result.Values[2][2] = T(1) - T(2) * (XX + YY);
		Result.Values[2][3] = T(0);

		Result.Values[3][0] = T(0);
		Result.Values[3][1] = T(0);
		Result.Values[3][2] = T(0);
		Result.Values[3][3] = T(1);

		return Result;
	}

	MVector3<T> RotateVector(const MVector3<T>& InVec) const
	{
		MVector3<T> Result;
		
		const MVector3<T> Q(X, Y, Z);
		const MVector3<T> TT = Q.Cross(InVec) * T(2.0);
		Result = InVec + (TT * W) + Q.Cross(TT);

		return Result;
	}

	static const MQuaternion<T> Identity;

	T X;
	T Y;
	T Z;
	T W;
};
