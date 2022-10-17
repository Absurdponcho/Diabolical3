#pragma once


class DNetBuffer
{
public:
	DNetBuffer(uint32_t NewSize) { Buffer = new char[NewSize]; Size = NewSize; }
	~DNetBuffer() { delete[] Buffer; }
	uint32_t Size = 0;
	char* Buffer = nullptr;
};

enum class ENetCommand : uint8_t
{
	CreateObject,
	RemoveObject,
	SetSyncVar,
	RPC,
	NetOwnership,
};