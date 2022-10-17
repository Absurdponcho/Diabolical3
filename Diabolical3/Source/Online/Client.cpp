#include "Client.h"
#include "Online/Socket/DSocket.h"
#include "Check.h"
#include "TCPConnection.h"
#include "World/ObjectManager.h"
#include "Types/BitStream.h"
#include "Thread/GameThread.h"
#include "Types/Action.h"

DClient::DClient()
	:	TCPSocket(DSocket::CreateSocket()),
		TCPThread(std::make_unique<DThread>([=]() { TCPRun(); }))
{
	TCPSocket->CreateTCP();
}

DClient::~DClient()
{
	bMustClose = true;
	TCPThread->Join();
	TCPSocket->Close();
}

void DClient::AsyncConnect(const DString& Address, int Port)
{
	Check (!bIsConnected);
	Check (!bPendingConnect);
	Check(TCPSocket.Get());
	Check(TCPThread.Get());

	PendingAddress = Address;
	PendingPort = Port;
	bPendingConnect = true;
}

void DClient::TCPRun()
{
	while (!bIsConnected)
	{
		if (bPendingConnect)
		{
			LOG("Pending Connect");
			if (TCPSocket->Connect(PendingAddress, PendingPort))
			{
				bPendingConnect = false;
				bIsConnected = true;
				TcpConnection = std::make_unique<DTCPConnection>(TCPSocket, PendingAddress);
				TcpConnection->OnCompletePacketReceived = [this](DVector<uint8_t> Payload)
				{
					this->HandleTCPPacket(Payload);
				};
				OnConnect.Invoke(true);
			}
			else
			{
				bPendingConnect = false;
				OnConnect.Invoke(false);
				LOG(DString::Format("Failed to connect to %s:%i", *PendingAddress, PendingPort));
			}
		}
		else
		{
			LOG("Not Pending Connect");
		}

		if (!bIsConnected)
		{
			// wait for connection
			DThread::Sleep(100);
		}
	}
	
}

void DClient::HandleTCPPacket(DVector<uint8_t> Payload)
{
	DBitStream PayloadStream = DBitStream(std::move(Payload));

	DGameThread::Invoke(DAction<DBitStream>([](DBitStream BitStream)
		{
			uint8_t ReadCommand = 0;
			if (!BitStream.ReadByte(ReadCommand))
			{
				return;
			}
			ENetCommand Command = (ENetCommand)ReadCommand;

			switch (Command)
			{
			case ENetCommand::CreateObject:
				DEngine::ObjectManager->ProcessCreateObjectPacket(BitStream);
				break;
			case ENetCommand::RemoveObject:
				DEngine::ObjectManager->ProcessRemoveObjectPacket(BitStream);
				break;
			case ENetCommand::SetSyncVar:
				DEngine::ObjectManager->ProcessSetSyncVarPacket(BitStream);
				break;
			case ENetCommand::RPC:
				DEngine::ObjectManager->ProcessRPCPacket(BitStream);
				break;
			case ENetCommand::NetOwnership:
				DEngine::ObjectManager->ProcessOwnershipPacket(BitStream);
				break;
			default:
				Check(false);
			}
		}, PayloadStream));
	
}

void DClient::SendRPC(uint64_t ObjectNetworkId, DBitStream&& OutStreamPayload)
{
	Check(ObjectNetworkId > 0);
	DBitStream OutStreamHeader;
	OutStreamHeader.AddByte((uint8_t)ENetCommand::RPC);
	OutStreamHeader.AddPackedUInt(ObjectNetworkId);

	DUniquePtr<DNetBuffer> Buffer = new DNetBuffer((uint32_t)OutStreamHeader.GetByteCount() + (uint32_t)OutStreamPayload.GetByteCount());
	memcpy(Buffer->Buffer, OutStreamHeader.GetData(), OutStreamHeader.GetByteCount());
	memcpy(Buffer->Buffer + OutStreamHeader.GetByteCount(), OutStreamPayload.GetData(), OutStreamPayload.GetByteCount());

	TcpConnection->QueueTCPSendBuffer(std::move(Buffer));
}
