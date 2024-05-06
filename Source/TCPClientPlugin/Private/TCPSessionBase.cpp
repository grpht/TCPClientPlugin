// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPSessionBase.h"

#include "TCPClientController.h"
#include "TCPSendPacketBase.h"
#include "TCPRecvPacketBase.h"
#include "TCPHeaderComponent.h"

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

	writer.Reserve(Header->GetHeaderSize());
	sendPacket.ConvertToBytes(writer);
	int id = sendPacket.GetPacketId();
	Header->WriteHeader(writer, id);

	FByteArrayRef SendBuffPtr = ref;
	Controller->StartSend(SendBuffPtr);
}

void UTCPSessionBase::SendPacketBP(UTCPSendPacketBase* sendPacket)
{
	if (sendPacket == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sendpacket is null"))
		check(sendPacket != nullptr)
		return;
	}
	SendPacket(*sendPacket);
}

void UTCPSessionBase::RegisterRecvPacket(TSubclassOf<UTCPRecvPacketBase> recvPacket)
{
	auto c = *recvPacket;
	auto cdo = recvPacket.GetDefaultObject();
	RecvPacketMap.Add(cdo->GetPacketId(), *recvPacket);
}

void UTCPSessionBase::OnStart()
{
	if (CustomHeader)
	{
		Header = NewObject<UTCPHeaderComponent>(this, CustomHeader);
	}
	else
	{
		Header = NewObject<UTCPHeaderComponent>();
	}
	Controller->SetHeader(Header);

	for (auto recvPacket : PacketToReceive)
	{
		RegisterRecvPacket(recvPacket);
	}

	OnStartBP();
}

void UTCPSessionBase::OnRecv(int32 id, TCPBufferReader& reader)
{
	auto item = RecvPacketMap.Find(id);
	if (item != nullptr)
	{
		auto packet = UTCPRecvPacketBase::CreateRecvPacketBP(*item);
		packet->ConvertFromBytes(reader);
		OnRecvBP(packet);
	}
}

void UTCPSessionBase::OnSend(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray)
{
	OnSendBP(id, contentsByteSize, fullByteArray);
}

void UTCPSessionBase::OnDestroy()
{
	OnDestroyBP();
	RecvPacketMap.Empty();
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
	int32 contentsSize = Header->ReadTotalSize(buffer) - Header->GetHeaderSize();
	if (!Header->CheckIntegrity(buffer))
	{
		Controller->Disconnect("Invalid data received. Integrity check unsuccessful.", false);
	}
	if (contentsSize == 0)
	{
		TCPBufferReader reader;
		OnRecv(Header->ReadProtocol(buffer), reader);
	}
	else
	{
		TCPBufferReader reader(&buffer[Header->GetHeaderSize()], contentsSize);
		OnRecv(Header->ReadProtocol(buffer), reader);
	}
}

void UTCPSessionBase::SendMessageCallback(FByteArrayRef& messageByte)
{
	uint8* buffer = messageByte->GetData();
	int32 contentsSize = Header->ReadTotalSize(buffer) - Header->GetHeaderSize();
	OnSend(Header->ReadProtocol(buffer), contentsSize, *messageByte);
}

void UTCPSessionBase::SetController(TCPClientController* controller)
{
	Controller = controller;
}

TCPClientController* UTCPSessionBase::GetController()
{
	return Controller;
}
