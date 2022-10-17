#pragma once
#include "Types/BitStream.h"
#include "Types/DVector.h"

class DSyncVarBase
{
public:
	virtual bool IsDirty() const = 0;
	virtual bool IsDirtyForConnection(uint64_t ClientId) = 0;
	virtual bool Serialize(DBitStream& BitStream) = 0;
	virtual bool Deserialize(DBitStream& BitStream) = 0;
	virtual void SetNotDirtyForConnection(uint64_t ClientId) = 0;

protected:
	virtual void MarkNotDirty() = 0;

	friend class DServer;
};

template <typename T>
class DSyncVar : public DSyncVarBase
{
public:
	DSyncVar() {}
	DSyncVar(T&& Other)
	{
		Value = std::move(Other);
		bDirty = true;
	}
	DSyncVar(const T& Other)
	{
		Value = Other;
		bDirty = true;
	}

	virtual bool IsDirtyForConnection(uint64_t ClientId) override
	{
		return !SentConnections.Contains(ClientId);
	}

	virtual bool IsDirty() const override
	{
		return bDirty;
	}

	virtual bool Serialize(DBitStream& BitStream) override
	{
		return DBitSerializer<T>(&Value).Serialize(BitStream);
	}

	virtual bool Deserialize(DBitStream& BitStream) override
	{
		return DBitSerializer<T>(&Value).Deserialize(BitStream);
	}

	virtual void SetNotDirtyForConnection(uint64_t ClientId) override
	{
		if (!SentConnections.Contains(ClientId))
		{
			SentConnections.Add(ClientId);
		}
	}

	virtual T Get() const
	{
		return Value;
	}

	virtual const T* GetPtr() const
	{
		return &Value;
	}

	virtual void Set(T&& NewValue)
	{
		Value = std::move(NewValue);
		bDirty = true;
		SentConnections.Clear();
	}

	virtual void Set(const T& NewValue)
	{
		Value = NewValue;
		bDirty = true;
		SentConnections.Clear();
	}

protected:
	T Value = T();
	bool bDirty = false;
	DVector<uint64_t> SentConnections;

	virtual void MarkNotDirty() override
	{
		Check(bDirty);
		bDirty = false;
	}
};

template <typename T>
class DSyncVector : public DSyncVarBase
{
public:
	DSyncVector() {}
	DSyncVector(T&& Other)
	{
		Value = std::move(Other);
		bDirty = true;
	}
	DSyncVector(const T& Other)
	{
		Value = Other;
		bDirty = true;
	}

	virtual bool IsDirtyForConnection(uint64_t ClientId) override
	{
		return !SentConnections.Contains(ClientId);
	}

	virtual bool IsDirty() const override
	{
		return bDirty;
	}

	virtual bool Serialize(DBitStream& BitStream) override
	{
		if (!BitStream.AddPackedUInt(Value.Size()))
		{
			return false;
		}

		for (T& Element : Value)
		{
			if (!DBitSerializer<T>(&Element).Serialize(BitStream))
			{
				return false;
			}
		}
		return true;
	}

	virtual bool Deserialize(DBitStream& BitStream) override
	{
		uint64_t Size = 0;
		if (!BitStream.ReadPackedUInt(Size))
		{
			return false;
		}


		Value.Clear();
		Value.Reserve(Size);
		for (uint64_t Index = 0; Index < Size; Index++)
		{
			T NewElement;
			if (!DBitSerializer<T>(&NewElement).Deserialize(BitStream))
			{
				return false;
			}
			Value.Add(NewElement);
		}
		return true;
	}

	virtual void SetNotDirtyForConnection(uint64_t ClientId) override
	{
		if (!SentConnections.Contains(ClientId))
		{
			SentConnections.Add(ClientId);
		}
	}

	virtual const T& Get(int Index) const
	{
		return Value[Index];
	}

	virtual void Add(T&& NewValue)
	{
		Value.Add(std::move(NewValue));
		bDirty = true;
		SentConnections.Clear();
	}

	virtual void Add(const T& NewValue)
	{
		Value.Add(NewValue);
		bDirty = true;
		SentConnections.Clear();
	}

	virtual void RemoveAt(int Index)
	{
		Value.RemoveAt(Index);
		bDirty = true;
		SentConnections.Clear();
	}

	virtual void SetAt(int Index, T&& NewValue)
	{
		Value[Index] = std::move(NewValue);
		bDirty = true;
		SentConnections.Clear();
	}

	virtual void SetAt(int Index, const T& NewValue)
	{
		Value[Index] = NewValue;
		bDirty = true;
		SentConnections.Clear();
	}

	virtual uint64_t Size()
	{
		return Value.Size();
	}

	virtual const T& operator[](int Index)
	{
		return Get(Index);
	}

	const DVector<T>& GetVector() const
	{
		return Value;
	}

	// Warning: Changing values via this vector will not update the vector on the network.
	DVector<T>& GetVectorMutable()
	{
		return Value;
	}

protected:
	DVector<T> Value = DVector<T>();
	bool bDirty = false;
	DVector<uint64_t> SentConnections;

	virtual void MarkNotDirty() override
	{
		Check(bDirty);
		bDirty = false;
	}
};