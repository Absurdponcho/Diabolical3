#include "BitStream.h"
#include "Check.h"
#include "Types/DString.h"
#include "Logging/Logging.h"

bool DBitStream::AddBits(const uint8_t* Source, size_t Amount)
{
	int BitOffset = 0;
	while (Amount > 0)
	{
		if (!AddBit(*Source, BitOffset))
		{
			return false;
		}
		BitOffset++;
		Amount--;
		if (BitOffset == 8)
		{
			BitOffset = 0;
			Source++;
		}
	}
	return true;
}

bool DBitStream::AddByte(uint8_t Value)
{
	return AddBits(&Value, 8);
}

bool DBitStream::ReadByte(uint8_t& Destination)
{
	Destination = 0;
	int Remainder = ReadPos % 8;
	if (Remainder == 0)
	{
		Destination = Buffer[ReadPos / 8];
		ReadPos += 8;
		return true;
	}
	else
	{
		for (int Index = 0; Index < 8; Index++)
		{
			if (!ReadBit(Destination, Index))
			{
				return false;
			}
		}
	}
	return true;
}

bool DBitStream::AddBytes(const uint8_t* Value, size_t Amount)
{
	for (int Index = 0; Index < Amount; Index++)
	{
		if (!AddByte(*Value))
		{
			return false;
		}
		Value++;
	}
	return true;
}


bool DBitStream::ReadBytes(uint8_t* Destination, size_t Amount)
{
	for (int Index = 0; Index < Amount; Index++)
	{
		if (!ReadByte(*Destination))
		{
			return false;
		}
		Destination++;
	}
	return true;
}

bool DBitStream::AddBit(uint8_t Value, size_t Offset)
{
	Check(Offset < 8);
	if (Offset >= 8)
	{
		return false;
	}

	int BitOffset = BitCount % 8;
	uint8_t BitValue = Value & ((uint8_t)1 << Offset);
	if (BitOffset == 0)
	{
		Buffer.Add(BitValue >> Offset);
	}
	else
	{
		Buffer[Buffer.Size() - 1] |= (BitValue >> Offset << BitOffset);
	}

	BitCount++;

	return true;
}

bool DBitStream::ReadBit(uint8_t& Destination, size_t Offset)
{
	Check(Offset < 8);
	if (Offset >= 8)
	{
		return false;
	}
	Checkf(ReadPos < BitCount, "ReadPos (%i) is not less than BitCount (%i)", ReadPos, BitCount);
	if (ReadPos >= BitCount)
	{
		return false;
	}
	int BitOffset = ReadPos % 8;
	uint8_t BitValue = Buffer[ReadPos / 8] & ((uint8_t)1 << BitOffset);
	BitValue >>= BitOffset;
	Destination |= BitValue << Offset;
	ReadPos++;
	return true;
}

bool DBitStream::ReadBits(uint8_t& Destination, size_t Amount)
{
	Destination = 0;
	for (int Index = 0; Index < Amount; Index++)
	{
		if (!ReadBit(Destination, Index))
		{
			return false;
		}
	}
	return true;
}

bool DBitStream::AddString(const DString& Source)
{
	if (!AddBytes((uint8_t*)*Source, Source.Length()))
	{
		return false;
	}
	if (!AddByte(0))
	{
		return false;
	}
	return true;
}

bool DBitStream::ReadString(DString& Destination)
{
	Destination = "";
	uint8_t Read = 0;
	if (!ReadByte(Read))
	{
		return false;
	}
	while (Read != 0)
	{
		Destination.Append((char*)&Read, 1);
		Read = 0;
		if (!ReadByte(Read))
		{
			return false;
		}
	}
	return true;
}

bool DBitStream::AddBool(bool Value)
{
	if (Value)
	{
		return AddBit(1, 0);
	}
	else
	{
		return AddBit(0, 0);
	}
}

bool DBitStream::ReadBool(bool& Destination)
{
	Destination = false;
	return ReadBit((uint8_t&)Destination, 0);
}

bool DBitStream::AddPackedUInt(uint64_t Value)
{
	if (Value == 0) // Zero, so add 0 and false
	{
		if (!AddBits((uint8_t*)&Value, 7))
		{
			return false;
		}
		if (!AddBool(false)) // No more data
		{
			return false;
		}
		return true;
	}

	while (Value > 0)
	{
		if (!AddBits((uint8_t*)&Value, 7))
		{
			return false;
		}
		Value >>= 7;
		if (Value > 0)
		{
			if (!AddBool(true))// Yes there is more data
			{
				return false;
			}
		}
		else
		{
			if (!AddBool(false)) // No more data
			{
				return false;
			}
		}
	}
	return true;
}

bool DBitStream::ReadPackedUInt(uint64_t& Destination)
{
	Destination = 0;
	int Offset = 0;
	while (true)
	{
		uint8_t Read;
		if (!ReadByte(Read))
		{
			return false;
		}
		uint64_t OffsettedRead = Read & 0b01111111;
		OffsettedRead <<= Offset;
		Destination |= OffsettedRead;

		bool bMore = Read & 0b10000000;
		if (!bMore)
		{
			break;
		}

		Offset += 7;
	}

	return true;
}

bool DBitStream::AddPackedInt(int64_t SignedValue)
{
	uint64_t Value = 0;
	if (SignedValue >= 0)
	{
		Value = SignedValue;
		if (!AddBool(true))// positive
		{
			return false;
		}
	}
	else
	{
		Value = -SignedValue;
		if (!AddBool(false))// negative
		{
			return false;
		}
	}

	int DataSize = 6;

	if (Value == 0) // Zero, so add 0 and false
	{
		if (!AddBits((uint8_t*)&Value, DataSize))
		{
			return false;
		}
		if (!AddBool(false)) // No more data
		{
			return false;
		}
		return true;
	}

	while (Value > 0)
	{
		if (!AddBits((uint8_t*)&Value, DataSize))
		{
			return false;
		}
		Value >>= DataSize;
		if (Value > 0)
		{
			if (!AddBool(true))// Yes there is more data
			{
				return false;
			}
		}
		else
		{
			if (!AddBool(false)) // No more data
			{
				return false;
			}
		}
		DataSize = 7;
	}
	return true;
}

bool DBitStream::ReadPackedInt(int64_t& Destination)
{
	uint8_t FirstByte;
	if (!ReadByte(FirstByte))
	{
		return false;
	}
	
	bool bPositive = FirstByte & 1;
	uint64_t UnsignedValue = 0;
	UnsignedValue |= ((FirstByte >> 1) & 0b00111111);
	bool bMore = FirstByte & 0b10000000;
	if (bMore)
	{
		int Offset = 6;
		while (true)
		{
			uint8_t Read;
			if (!ReadByte(Read))
			{
				return false;
			}
			uint64_t OffsettedRead = Read & 0b01111111;
			OffsettedRead <<= Offset;
			UnsignedValue |= OffsettedRead;

			bool bMore = Read & 0b10000000;
			if (!bMore)
			{
				break;
			}

			Offset += 7;
		}
	}

	if (bPositive)
	{
		Destination = (int64_t)UnsignedValue;
	}
	else
	{
		Destination = -(int64_t)UnsignedValue;
	}

	return true;
}

bool DBitStream::AddPackedDouble(double Value, uint32_t Precision)
{
	int64_t IntValue = (int64_t)(Value * (double)Precision);
	return AddPackedInt(IntValue);
}

bool DBitStream::ReadPackedDouble(double& Destination, uint32_t Precision)
{
	int64_t IntValue = 0;
	if (!ReadPackedInt(IntValue))
	{
		return false;
	}
	Destination = (double)IntValue;
	Destination /= (double)Precision;
	return true;
}


bool DBitStream::AddPackedFloat(float Value, uint32_t Precision)
{
	int32_t IntValue = (int32_t)(Value * (float)Precision);
	return AddPackedInt(IntValue);
}

bool DBitStream::ReadPackedFloat(float& Destination, uint32_t Precision)
{
	int64_t IntValue = 0;
	if (!ReadPackedInt(IntValue))
	{
		return false;
	}
	Destination = (float)IntValue;
	Destination /= (float)Precision;
	return true;
}


// ======= uint64_t =======
template<>
bool DBitSerializer<uint64_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<uint64_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedUInt(*Value); }
template<>
bool DBitSerializer<uint64_t>::DeserializeOverride(DBitStream& BitStream) { return BitStream.ReadPackedUInt(*Value); }

// ======= int64_t =======
template<>
bool DBitSerializer<int64_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<int64_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedInt(*Value); }
template<>
bool DBitSerializer<int64_t>::DeserializeOverride(DBitStream& BitStream) { return BitStream.ReadPackedInt(*Value); }

// ======= uint32_t =======
template<>
bool DBitSerializer<uint32_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<uint32_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedUInt(*Value); }
template<>
bool DBitSerializer<uint32_t>::DeserializeOverride(DBitStream& BitStream) 
{ 
	uint64_t Read;
	if (!BitStream.ReadPackedUInt(Read))
	{
		return false;
	}
	*Value = (uint32_t)Read;
	return true;
}

// ======= int32_t =======
template<>
bool DBitSerializer<int32_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<int32_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedInt(*Value); }
template<>
bool DBitSerializer<int32_t>::DeserializeOverride(DBitStream& BitStream)
{
	int64_t Read;
	if (!BitStream.ReadPackedInt(Read))
	{
		return false;
	}
	*Value = (int32_t)Read;
	return true;
}

// ======= uint16_t =======
template<>
bool DBitSerializer<uint16_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<uint16_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedUInt(*Value); }
template<>
bool DBitSerializer<uint16_t>::DeserializeOverride(DBitStream& BitStream)
{ 
	uint64_t Read;
	if (!BitStream.ReadPackedUInt(Read))
	{
		return false;
	}
	*Value = (uint16_t)Read;
	return true;
}

// ======= int16_t =======
template<>
bool DBitSerializer<int16_t>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<int16_t>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddPackedInt(*Value); }
template<>
bool DBitSerializer<int16_t>::DeserializeOverride(DBitStream& BitStream)
{
	int64_t Read;
	if (!BitStream.ReadPackedInt(Read))
	{
		return false;
	}
	*Value = (int16_t)Read;
	return true;
}

// ======= DString =======
template<>
bool DBitSerializer<DString>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<DString>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddString(*Value); }
template<>
bool DBitSerializer<DString>::DeserializeOverride(DBitStream& BitStream) { return BitStream.ReadString(*Value); }

// ======= bool =======
template<>
bool DBitSerializer<bool>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<bool>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddBool(*Value); }
template<>
bool DBitSerializer<bool>::DeserializeOverride(DBitStream& BitStream) { return BitStream.ReadBool(*Value); }

// ======= float =======
template<>
bool DBitSerializer<float>::HasSerializeOverride() { return true; }
template<>
bool DBitSerializer<float>::SerializeOverride(DBitStream& BitStream) { return BitStream.AddBytes((uint8_t*)Value, 4); }
template<>
bool DBitSerializer<float>::DeserializeOverride(DBitStream& BitStream) { return BitStream.ReadBytes((uint8_t*)Value, 4); }
