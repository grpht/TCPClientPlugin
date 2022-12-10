// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPSessionBase.h"

#include "TCPClientController.h"
#include "TCPSendPacketBase.h"
#include "TCPRecvPacketBase.h"

#include "Serialization/BufferArchive.h"
#include "TCPBufferReader.h"
#include "TCPBufferWriter.h"

bool UTCPSessionBase::IsConnected()
{
	return Controller != nullptr && Controller->IsConnected();
}

void UTCPSessionBase::SendPacket(ITCPSendPacket& sendPacket)
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

	int indexSize = writer.Reserve(sizeof(TCPPacketHeader::Size));
	//int indexAny = writer.Reserve(sizeof(TCPPacketHeader::Any));
	int indexId = writer.Reserve(sizeof(TCPPacketHeader::Id));
	

	sendPacket.Serialize(writer);
	
	int packetSize = writer.Num();
	//int any = -1;
	int id = sendPacket.GetPacketId();
	FMemory::Memcpy(&writer.GetData()[indexSize], &packetSize, sizeof(TCPPacketHeader::Size));
	//FMemory::Memcpy(&writer.GetData()[indexAny], &any, sizeof(TCPPacketHeader::Any));
	FMemory::Memcpy(&writer.GetData()[indexId], &id, sizeof(TCPPacketHeader::Id));

	FByteArrayRef SendBuffPtr = ref;
	TCPPacketHeader header = *reinterpret_cast<TCPPacketHeader*>(SendBuffPtr->GetData());
	Controller->StartSend(SendBuffPtr);
}

void UTCPSessionBase::OnStart()
{
	OnStartBP();
}

void UTCPSessionBase::OnDestroy()
{
	OnDestroyBP();
	RecvPacketMap.Empty();
}

void UTCPSessionBase::OnRecv(int32 id, TCPBufferReader& reader)
{
	auto item = RecvPacketMap.Find(id);
	if (item != nullptr)
	{
		auto packet = UTCPRecvPacketBase::CreateRecvPacketBP(*item);
		packet->Deserialize(reader);
		OnRecvBP(packet);
	}
}

void UTCPSessionBase::OnSend(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray)
{
	OnSendBP(id, contentsByteSize, fullByteArray);
}


void UTCPSessionBase::SendPacketBP(UTCPSendPacketBase* sendPacket)
{
	checkf(sendPacket != nullptr, TEXT("Sendpacket is null"))
	SendPacket(*sendPacket);
}

void UTCPSessionBase::ConnectedCallback(bool success)
{
	OnConnectedBP(success);
	if (OnConnected.IsBound())
	{
		OnConnected.Execute(SessionName, success);
	}
}

void UTCPSessionBase::DisconnectedCallback(bool normalShutdown)
{
	OnDisconnectedBP(normalShutdown);
	if (OnDisconnected.IsBound())
	{
		OnDisconnected.Execute(SessionName, normalShutdown);
	}
}

void UTCPSessionBase::RecvMessageCallback(FByteArrayRef& messageByte)
{
	uint8* buffer = messageByte->GetData();
	int headerSize = sizeof(TCPPacketHeader);
	TCPPacketHeader header = *reinterpret_cast<TCPPacketHeader*>(buffer);
	//int32 any = header.Any;
	int32 contentsSize = header.Size - headerSize;
	
	if (contentsSize == 0)
	{
		TCPBufferReader reader;
		OnRecv(header.Id, reader);
	}
	else
	{
		TCPBufferReader reader(&buffer[headerSize], contentsSize);
		OnRecv(header.Id, reader);
	}
}

void UTCPSessionBase::SendMessageCallback(FByteArrayRef& messageByte)
{
	uint8* buffer = messageByte->GetData();
	int headerSize = sizeof(TCPPacketHeader);
	TCPPacketHeader header = *reinterpret_cast<TCPPacketHeader*>(buffer);
	//int32 any = header.Any;
	int32 contentsSize = header.Size - headerSize;
	if (contentsSize == 0)
	{
		OnSend(header.Id, contentsSize, *messageByte);
	}
	else
	{
		OnSend(header.Id, contentsSize, *messageByte);
	}
}

void UTCPSessionBase::RegisterRecvPacket(TSubclassOf<UTCPRecvPacketBase> recvPacket)
{
	auto c = *recvPacket;
	auto cdo = recvPacket.GetDefaultObject();
	RecvPacketMap.Add(cdo->GetPacketId(), *recvPacket);
}
