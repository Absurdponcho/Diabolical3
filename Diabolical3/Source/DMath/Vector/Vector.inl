#pragma once

template <typename T>
struct MVector4
{
public:
	MVector4()
		: X(0), Y(0), Z(0), W(0) {}

	template <typename XType, typename YType>
	MVector4(XType InX, YType InY)
		: X((T)InX), Y((T)InY), Z(0), W(0) {}

	template <typename XType, typename YType, typename ZType>
	MVector4(XType InX, YType InY, ZType InZ)
		: X((T)InX), Y((T)InY), Z((T)InZ), W(0) {}

	template <typename XType, typename YType, typename ZType, typename WType>
	MVector4(XType InX, YType InY, ZType InZ, WType InW)
		: X((T)InX), Y((T)InY), Z((T)InZ), W((T)InW) {}

	template <typename OtherType>
	MVector4(const MVector4<OtherType>& Other)
		: X((T)Other.X), Y((T)Other.Y), Z((T)Other.Z), W((T)Other.W) {}

	MVector4(const MVector4<T>& Other)
		: X((T)Other.X), Y((T)Other.Y), Z((T)Other.Z), W((T)Other.W) {}

	template <typename OtherType>
	bool operator==(const MVector4<OtherType>& Other) const
	{
		return Other.X == X && Other.Y == Y && Other.Z == Z && Other.W = W;
	}

	template <typename OtherType>
	MVector4<T> operator*(const MVector4<OtherType>& Other) const
	{
		return { X * (T)Other.X, Y * (T)Other.Y, Z * (T)Other.Z, W * (T)Other.W };
	}

	template <typename OtherType>
	MVector4<T> operator*(OtherType Other) const
	{
		return { X * (T)Other, Y * (T)Other, Z * (T)Other, W * (T)Other };
	}

	template <typename OtherType>
	MVector4<T> operator+(const MVector4<OtherType>& Other) const
	{
		return { X + (T)Other.X, Y + (T)Other.Y, Z + (T)Other.Z, W + (T)Other.W };
	}

	inline T Length() const
	{
		return sqrt(X * X + Y * Y + Z * Z + W * W);
	}

	inline MVector4<T> Normalized() const
	{
		float ThisLength = Length();
		return MVector4<T>(X / ThisLength, Y / ThisLength, Z / ThisLength, W / ThisLength);
	}

	static const MVector4<T> OneVector;
	static const MVector4<T> ZeroVector;

	T X;
	T Y;
	T Z;
	T W;
};

template <typename T>
struct MVector3
{
public:
	MVector3()
		: X(0), Y(0), Z(0) {}

	template <typename XType, typename YType>
	MVector3(XType InX, YType InY)
		: X((T)InX), Y((T)InY), Z(0) {}

	template <typename XType, typename YType, typename ZType>
	MVector3(XType InX, YType InY, ZType InZ)
		: X((T)InX), Y((T)InY), Z((T)InZ) {}

	template <typename OtherType>
	MVector3(const MVector3<OtherType>& Other)
		: X((T)Other.X), Y((T)Other.Y), Z((T)Other.Z) {}

	template <typename OtherType>
	bool operator==(const MVector3<OtherType>& Other) const
	{
		return Other.X == X && Other.Y == Y && Other.Z == Z;
	}

	template <typename OtherType>
	MVector3<T> operator*(const MVector3<OtherType>& Other) const
	{
		return { X * (T)Other.X, Y * (T)Other.Y, Z * (T)Other.Z };
	}

	template <typename OtherType>
	MVector3<T> operator*(OtherType Other) const
	{
		return { X * (T)Other, Y * (T)Other, Z * (T)Other };
	}

	template <typename OtherType>
	MVector3<T> operator+(const MVector3<OtherType>& Other) const
	{
		return { X + (T)Other.X, Y + (T)Other.Y, Z + (T)Other.Z };
	}

	inline T Length() const
	{
		return sqrt(X * X + Y * Y + Z * Z);
	}

	inline MVector3<T> Normalized() const
	{
		float ThisLength = Length();
		return MVector3<T>(X / ThisLength, Y / ThisLength, Z / ThisLength);
	}

	MVector3<T> operator-() const
	{
		return { -X, -Y, -Z };
	}

	static const MVector3<T> OneVector;
	static const MVector3<T> ZeroVector;

	T X;
	T Y;
	T Z;
};

template <typename T>
struct MVector2
{
public:
	MVector2()
		: X(0), Y(0) {}

	template <typename XType, typename YType>
	MVector2(XType InX, YType InY)
		: X((T)InX), Y((T)InY) {}

	template <typename OtherType>
	MVector2(const MVector2<OtherType>& Other)
		: X((T)Other.X), Y((T)Other.Y) {}

	template <typename OtherType>
	bool operator==(const MVector2<OtherType>& Other) const
	{
		return Other.X == X && Other.Y == Y;
	}

	template <typename OtherType>
	MVector2<T> operator*(const MVector2<OtherType>& Other) const
	{
		return { X * (T)Other.X, Y * (T)Other.Y };
	}

	template <typename OtherType>
	MVector2<T> operator*(OtherType Other) const
	{
		return { X * (T)Other, Y * (T)Other };
	}

	template <typename OtherType>
	MVector2<T> operator+(const MVector2<OtherType>& Other) const
	{
		return { X + (T)Other.X, Y + (T)Other.Y };
	}

	inline T Length() const
	{
		return sqrt(X * X + Y * Y);
	}

	inline MVector2<T> Normalized() const
	{
		float ThisLength = Length();
		return MVector2<T>(X / ThisLength, Y / ThisLength);
	}

	static const MVector2<T> OneVector;
	static const MVector2<T> ZeroVector;

	T X;
	T Y;
};