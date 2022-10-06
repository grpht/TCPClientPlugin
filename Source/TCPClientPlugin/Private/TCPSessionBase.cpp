// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSessionBase.h"

#include "TCPClientController.h"
#include "TCPSendPacketBase.h"

#include "Serialization/BufferArchive.h"
#include "TCPBufferReader.h"
#include "TCPBufferWriter.h"
bool UTCPSessionBase::IsConnected()
{
	return Controller != nullptr && Controller->IsConnected();
}

void UTCPSessionBase::Send(ITCPSendPacket& sendPacket)
{
	if (Controller == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Send Fail. TCPClientController is null"));
		return;
	}
	if (!Controller->IsConnected())
	{
		UE_LOG(LogTemp, Error, TEXT("Send Fail. Session is Disconnected"));
		return;
	}
	TSharedRef<TCPBufferWriter, ESPMode::ThreadSafe> ref(new TCPBufferWriter());
	TCPBufferWriter& writer = ref.Get();

	int indexSize = writer.Reserve(sizeof(PacketHeader::Size));
	int indexAny = writer.Reserve(sizeof(PacketHeader::Any));
	int indexId = writer.Reserve(sizeof(PacketHeader::Id));
	

	sendPacket.Serialize(writer);
	
	int packetSize = writer.Num();
	int any = -1;
	int id = sendPacket.GetId();
	FMemory::Memcpy(&writer.GetData()[indexSize], &packetSize, sizeof(PacketHeader::Size));
	FMemory::Memcpy(&writer.GetData()[indexAny], &any, sizeof(PacketHeader::Any));
	FMemory::Memcpy(&writer.GetData()[indexId], &id, sizeof(PacketHeader::Id));

	FByteArrayRef SendBuffPtr = ref;
	PacketHeader header = *reinterpret_cast<PacketHeader*>(SendBuffPtr->GetData());
	Controller->StartSend(SendBuffPtr);
}


void UTCPSessionBase::Send(UTCPSendPacketBase* sendPacket)
{
	checkf(sendPacket != nullptr, TEXT("Sendpacket is null"))
	Send(*sendPacket);
}


void UTCPSessionBase::ConnectedCallback(bool success)
{
	if (OnConnected.IsBound())
	{
		OnConnected.Execute(SessionName, success);
	}
}

void UTCPSessionBase::DisconnectedCallback(bool normalShutdown)
{
	if (OnDisconnected.IsBound())
	{
		OnDisconnected.Execute(SessionName, normalShutdown);
	}
}

void UTCPSessionBase::RecvMessageCallback(FByteArrayRef& MessageByte)
{
	uint8* buffer = MessageByte->GetData();
	int headerSize = sizeof(PacketHeader);
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);
	int32 pureMessageSize = header.Size - headerSize;
	if (pureMessageSize == 0)
	{
		OnRecv(header.Id, pureMessageSize, nullptr);
	}
	else
	{
		OnRecv(header.Id, pureMessageSize, &buffer[headerSize]);
	}
	
}

void UTCPSessionBase::SendMessageCallback(FByteArrayRef& MessageByte)
{
	uint8* buffer = MessageByte->GetData();
	int headerSize = sizeof(PacketHeader);
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);
	int32 pureMessageSize = header.Size - headerSize;
	if (pureMessageSize == 0)
	{
		OnSend(header.Id, pureMessageSize, nullptr);
	}
	else
	{
		OnSend(header.Id, pureMessageSize, &buffer[headerSize]);
	}
}
