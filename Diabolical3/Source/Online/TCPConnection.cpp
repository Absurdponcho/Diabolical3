#include "TCPConnection.h"
#include "Online/Socket/DSocket.h"
#include "Thread/Thread.h"

DTCPConnection::DTCPConnection(DUniquePtr<DSocket>& NewSocket, DString IncomingIP)
	: RemoteIP(IncomingIP),
	TCPSendThread(std::make_unique<DThread>([=]() { TCPSend(); })),
	TCPReceiveThread(std::make_unique<DThread>([=]() { TCPReceive(); }))
{
	Check(NewSocket.Get());
	TCPSocket.Swap(NewSocket);
}

DTCPConnection::~DTCPConnection()
{
	bMustClose = true;
	TCPSendThread->Join();
	TCPReceiveThread->Join();
	TCPSocket->Close();
}

void DTCPConnection::TCPSend()
{
	DUniquePtr<DNetBuffer> Buffer;

	while (!bMustClose)
	{
		while (PopTCPSendBuffer(Buffer))
		{
			uint32_t PacketLength = Buffer->Size;
			if (!TCPSocket->Send((const char*)&PacketLength, sizeof(uint32_t)))
			{
				LOG_ERR("Failed to send packet length");
			}
			if (!TCPSocket->Send(Buffer->Buffer, Buffer->Size))
			{
				LOG_ERR("Failed to send bytes");
			}
		}
		DThread::Sleep(5);
	}
}

void DTCPConnection::TCPReceive()
{
	char RecvBuffer[4096 * 4];

	while (!bMustClose)
	{
		while (true)
		{
			int ReceivedBytes = 0;
			if (TCPSocket->Receive(RecvBuffer, 4096 * 4, ReceivedBytes))
			{
				if (ReceivedBytes > 0)
				{
					ReceiveBuffer.Append((uint8_t*)RecvBuffer, ReceivedBytes);
				}
			}
			else
			{
				break;
			}
			while (ReceiveBuffer.Size() >= sizeof(uint32_t))
			{
				uint32_t PacketLength = *(uint32_t*)ReceiveBuffer.GetData();
				if (ReceiveBuffer.Size() < PacketLength + sizeof(uint32_t))
				{
					break;
				}
				PopCompletePacket(PacketLength);
			}
		}
		DThread::Sleep(5);
	}
}

void DTCPConnection::PopCompletePacket(uint32_t Length)
{
	DVector<uint8_t> Packet;
	Packet.Append(ReceiveBuffer.GetData() + sizeof(uint32_t), Length);
	ReceiveBuffer.RemoveAt(0, Length + sizeof(uint32_t));
	if (OnCompletePacketReceived.IsBound())
	{
		OnCompletePacketReceived.Invoke(std::move(Packet));
	}
}

void DTCPConnection::QueueTCPSendBuffer(DUniquePtr<DNetBuffer>&& Buffer)
{
	auto BuffersValue = TCPSendBuffers.Retrieve();
	BuffersValue->PushBack(DUniquePtr<DNetBuffer>(Buffer.Release()));
}

bool DTCPConnection::PopTCPSendBuffer(DUniquePtr<DNetBuffer>& Buffer)
{
	auto BuffersValue = TCPSendBuffers.Retrieve();
	if (BuffersValue->Size() == 0) return false;
	Buffer.Swap(BuffersValue.Get()[0]);
	BuffersValue->RemoveAt(0);
	return true;
}

void DTCPConnection::SendPing()
{
	DUniquePtr<DNetBuffer> Buffer = std::make_unique<DNetBuffer>(5);
	const char* TestMessage = "Ping";
	strcpy_s(Buffer->Buffer, 5, TestMessage);
	QueueTCPSendBuffer(std::move(Buffer));
}
