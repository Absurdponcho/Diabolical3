#pragma once
#include <World/Object.h>
#include <functional>
#include <tuple>
#include "Types/BitStream.h"
#include "Online/NetworkManager.h"
#include "Online/Client.h"
#include "Online/Server.h"

#ifndef SILENCE_OFFLINE_WARNINGS
#define SILENCE_OFFLINE_WARNINGS 0
#endif

class DNetRPCBase
{
public:
	virtual ~DNetRPCBase() {}
protected:
	virtual void Receive(DBitStream& BitStream) = 0;
	friend class DObjectManager;

	uint64_t RegisterThis(DObject* Parent)
	{
		return Parent->RegisterRPC(this);
	}
};

template <typename... Args>
class DNetRPC : public DNetRPCBase
{
public:
	template <typename T>
	DNetRPC(T* Owner, void (T::* MemberFunc)(Args...))
		: BoundFunc([Owner, MemberFunc](Args... args) { (Owner->*MemberFunc)(args...); }), Parent(Owner)
	{
		RpcIndex = RegisterThis(Parent);
	}

	void Send(Args... Params)
	{
		Send(std::tuple<Args...>(Params...));
	}

	void Send(const std::tuple<Args...>& InArgs)
	{
		Check(Parent);
		if (Parent)
		{
			Check(Parent->IsNetworked());
			Check(Parent->GetNetworkId() > 0);
		}
		if (!Parent || !Parent->IsNetworked() || Parent->GetNetworkId() == 0)
		{
			LOG_ERR("Tried to send RPC but parent DObject is not suitable for networking!");
			return;
		}
		if (!Parent->HasNetOwnership())
		{
			LOG_WARN("Tried to send RPC but do not have net ownership!");
			return;
		}
		DBitStream BitStream;
		BitStream.AddPackedUInt(RpcIndex);
		SerializeTuple(InArgs, BitStream);
		SendImpl(std::move(BitStream));
	}

protected:
	virtual void SendImpl(DBitStream&& BitStream)
	{
		Checkf(false, "Base class DNetRPC should not be used. Try DNetRPCServer or DNetRPCClient");
	}

	virtual void Receive(DBitStream& BitStream) override
	{
		std::tuple<Args...> args;
		DeserializeTuple(args, BitStream);
		std::apply(BoundFunc, args);
	}

	uint64_t RpcIndex;
	class DObject* Parent;


private:
	std::function<void(Args...)> BoundFunc;

};

// RPC from client to server. Client must have ownership of the object.
template <typename... Args>
class DNetRPCServer : public DNetRPC<Args...>
{
public:
	template <typename T>
	DNetRPCServer(T* Owner, void (T::* MemberFunc)(Args...))
		: DNetRPC<Args...>(Owner, MemberFunc)
	{}
protected:
	virtual void SendImpl(DBitStream&& BitStream) override
	{
		DClient* Client = DNetworkManager::GetClient();
		if (!Client)
		{
#if !SILENCE_OFFLINE_WARNINGS
			LOG_WARN("Sending DNetRPCServer when Client is nullptr! If the game is running in offline mode, define \"SILENCE_OFFLINE_WARNINGS true\", or ensure Send() is not called.");
#endif
			return;
		}
		Client->SendRPC(DNetRPC<Args...>::Parent->GetNetworkId(), std::move(BitStream));
	}
};

// RPC from server to client. Receiving client is whom owns the RPC's parent object.
template <typename... Args>
class DNetRPCClient : public DNetRPC<Args...>
{
public:
	template <typename T>
	DNetRPCClient(T* Owner, void (T::* MemberFunc)(Args...))
		: DNetRPC<Args...>(Owner, MemberFunc)
	{}
protected:
	virtual void SendImpl(DBitStream&& BitStream) override
	{
		DServer* Server = DNetworkManager::GetServer();
		if (!Server)
		{
#if !SILENCE_OFFLINE_WARNINGS
			LOG_WARN("Sending DNetRPCClient when Server is nullptr! If the game is running in offline mode, define \"SILENCE_OFFLINE_WARNINGS true\", or ensure Send() is not called.");
#endif
			return;
		}

		DVector<uint64_t> OwnerClients;
		if (DNetRPC<Args...>::Parent->GetOwnerClients(OwnerClients))
		{
			Server->SendRPC(OwnerClients, DNetRPC<Args...>::Parent->GetNetworkId(), std::move(BitStream));
		}
	}
};

// RPC from server to all clients whom have relevancy of the RPC's parent object
template <typename... Args>
class DNetRPCBroadcast : public DNetRPC<Args...>
{
public:
	template <typename T>
	DNetRPCBroadcast(T* Owner, void (T::* MemberFunc)(Args...))
		: DNetRPC<Args...>(Owner, MemberFunc)
	{}
protected:
	virtual void SendImpl(DBitStream&& BitStream) override
	{
		DServer* Server = DNetworkManager::GetServer();
		if (!Server)
		{
#if !SILENCE_OFFLINE_WARNINGS
			LOG_WARN("Sending DNetRPCBroadcast when Server is nullptr! If the game is running in offline mode, define \"SILENCE_OFFLINE_WARNINGS true\", or ensure Send() is not called.");
#endif
			return;
		}
		Server->SendBroadcastRPC(DNetRPC<Args...>::Parent->GetNetworkId(), std::move(BitStream));
	}
};