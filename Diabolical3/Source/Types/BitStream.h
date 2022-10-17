#pragma once
#include "Types/DVector.h"
#include "Logging/Logging.h"
#include "Types/DMemory.h"

class DBitStream
{
public:
	DBitStream() {}
	DBitStream(DVector<uint8_t>&& TargetBuffer)
	{
		Buffer = std::move(TargetBuffer);
		BitCount = Buffer.SizeBytes() * 8;
	}

	virtual bool AddBits(const uint8_t* Source, size_t Amount);
	virtual bool AddByte(uint8_t Value);
	virtual bool AddBytes(const uint8_t* Value, size_t Amount);
	virtual bool AddBit(uint8_t Value, size_t Offset);
	virtual bool AddString(const class DString& Source);
	virtual bool AddBool(bool Value);
	virtual bool AddPackedUInt(uint64_t Value);
	virtual bool AddPackedInt(int64_t Value);
	virtual bool AddPackedDouble(double Value, uint32_t Precision = 100);
	virtual bool AddPackedFloat(float Value, uint32_t Precision = 100);

	virtual bool ReadBit(uint8_t& Destination, size_t Offset);
	virtual bool ReadBits(uint8_t& Destination, size_t Amount);
	virtual bool ReadByte(uint8_t& Destination);
	virtual bool ReadBytes(uint8_t* Destination, size_t Amount);
	virtual bool ReadString(DString& Destination);
	virtual bool ReadBool(bool& Destination);
	virtual bool ReadPackedUInt(uint64_t& Destination);
	virtual bool ReadPackedInt(int64_t& Destination);
	virtual bool ReadPackedDouble(double& Destination, uint32_t Precision = 100);
	virtual bool ReadPackedFloat(float& Destination, uint32_t Precision = 100);

	uint8_t* GetData()
	{
		return Buffer.GetData();
	}

	size_t GetBitCount() const
	{
		return BitCount;
	}

	size_t GetByteCount() const
	{
		return Buffer.SizeBytes();
	}

private:
	size_t BitCount = 0;
	size_t ReadPos = 0;
	DVector<uint8_t> Buffer;
};


class DBitSerializerBase
{
public:
	virtual size_t TypeSize() const = 0;
	virtual uint8_t* GetValuePtr() = 0;

	virtual bool Serialize(DBitStream& BitStream)
	{
		if (CheckSerializeOverride())
		{
			return UseSerializeOverride(BitStream);
		}
		ShowInefficientWarning(); 
		return BitStream.AddBytes(GetValuePtr(), TypeSize());
	}

	virtual bool Deserialize(DBitStream& BitStream)
	{
		if (CheckSerializeOverride())
		{
			return UseDeserializeOverride(BitStream);
		}
		ShowInefficientWarning();
		return BitStream.ReadBytes(GetValuePtr(), TypeSize());
	}

protected:
	virtual bool CheckSerializeOverride()
	{
		return false;
	}

	virtual bool UseSerializeOverride(DBitStream&) = 0;
	virtual bool UseDeserializeOverride(DBitStream&) = 0;

	// Let the inherited class throw the warning so we can have a hint on the type used in the log message.
	virtual void ShowInefficientWarning() = 0;
};

// NOTE: To implement custom serialization, create template specialization for HasSerializeOverride and return true.
// Then implement template specializations for BOTH SerializeOverride and DeserializeOverride.
template<typename T>
class DBitSerializer : public DBitSerializerBase
{
public:
	DBitSerializer(T* ValuePtr)
	{
		Value = ValuePtr;
	}

	virtual size_t TypeSize() const override
	{
		return sizeof(T);
	}

	virtual uint8_t* GetValuePtr() override
	{
		Check(Value);
		return (uint8_t*)Value;
	}

	bool HasSerializeOverride()
	{
		return false;
	}

	bool SerializeOverride(DBitStream& BitStream)
	{
		Check(false);
		return false;
	}

	bool DeserializeOverride(DBitStream& BitStream)
	{
		Check(false);
		return false;
	}

	virtual void ShowInefficientWarning() override
	{
		LOG_WARN("Inefficient serialize used");
	}

protected:
	virtual bool CheckSerializeOverride() override
	{
		if (std::is_base_of<DObjectPtrBase, T>::value)
		{
			return true;
		}
		return HasSerializeOverride();
	}

	virtual bool UseSerializeOverride(DBitStream& BitStream)
	{
		if (std::is_base_of<DObjectPtrBase, T>::value)
		{
			return reinterpret_cast<DObjectPtrBase*>(Value)->Serialize(BitStream);
		}
		return SerializeOverride(BitStream);
	}

	virtual bool UseDeserializeOverride(DBitStream& BitStream)
	{
		if (std::is_base_of<DObjectPtrBase, T>::value)
		{
			return reinterpret_cast<DObjectPtrBase*>(Value)->Deserialize(BitStream);
		}
		return DeserializeOverride(BitStream);
	}

private:
	T* Value = nullptr;
};


// ======= uint64_t =======
template<>
bool DBitSerializer<uint64_t>::HasSerializeOverride();
template<>
bool DBitSerializer<uint64_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<uint64_t>::DeserializeOverride(DBitStream& BitStream);

// ======= int64_t =======
template<>
bool DBitSerializer<int64_t>::HasSerializeOverride();
template<>
bool DBitSerializer<int64_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<int64_t>::DeserializeOverride(DBitStream& BitStream);

// ======= uint32_t =======
template<>
bool DBitSerializer<uint32_t>::HasSerializeOverride();
template<>
bool DBitSerializer<uint32_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<uint32_t>::DeserializeOverride(DBitStream& BitStream);

// ======= int32_t =======
template<>
bool DBitSerializer<int32_t>::HasSerializeOverride();
template<>
bool DBitSerializer<int32_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<int32_t>::DeserializeOverride(DBitStream& BitStream);

// ======= uint16_t =======
template<>
bool DBitSerializer<uint16_t>::HasSerializeOverride();
template<>
bool DBitSerializer<uint16_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<uint16_t>::DeserializeOverride(DBitStream& BitStream);

// ======= int16_t =======
template<>
bool DBitSerializer<int16_t>::HasSerializeOverride();
template<>
bool DBitSerializer<int16_t>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<int16_t>::DeserializeOverride(DBitStream& BitStream);

// ======= DString =======
template<>
bool DBitSerializer<DString>::HasSerializeOverride();
template<>
bool DBitSerializer<DString>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<DString>::DeserializeOverride(DBitStream& BitStream);

// ======= bool =======
template<>
bool DBitSerializer<bool>::HasSerializeOverride();
template<>
bool DBitSerializer<bool>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<bool>::DeserializeOverride(DBitStream& BitStream);

// ======= float =======
template<>
bool DBitSerializer<float>::HasSerializeOverride();
template<>
bool DBitSerializer<float>::SerializeOverride(DBitStream& BitStream);
template<>
bool DBitSerializer<float>::DeserializeOverride(DBitStream& BitStream);

// ======= Tuple =======

template <typename... Args>
void SerializeTuple(const std::tuple<Args...>& Tuple, DBitStream& BitStream) {
	std::apply([&](auto&... tupleArgs) mutable
		{
			auto SerializeElement = [&](auto x) mutable
			{
				if (!DBitSerializer(&x).Serialize(BitStream))
				{
					Check(false);
				}
			};

			(SerializeElement(tupleArgs), ...);
		}, Tuple
	);
}

template <typename... Args>
void DeserializeTuple(std::tuple<Args...>& Tuple, DBitStream& BitStream) {
	std::apply([&](auto&... tupleArgs) mutable
		{
			auto SerializeElement = [&](auto& x) mutable
			{
				if (!DBitSerializer(&x).Deserialize(BitStream))
				{
					Check(false);
				}
			};

			(SerializeElement(tupleArgs), ...);
		}, Tuple
	);
}
