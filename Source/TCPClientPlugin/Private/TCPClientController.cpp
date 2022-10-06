// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClientController.h"

#include "RecvBuffer.h"
#include "TCPClient.h"
#include "PacketQueue.h"
#include "RecvBuffer.h"
#include "TCPSessionBase.h"
#define NEW_AYNC_CALLBACK(FuncName) [this](FAsyncResultRef res){ if(this != nullptr) FuncName(res);}

TCPClientController::TCPClientController()
{
	SetReceiveBufferSize(4096);
	SetSendBufferSize(4096);
	//Session = new ITCPServerSession();
	//Session->SetController(this);
	MessageQueue = new PacketQueue();
}

TCPClientController::~TCPClientController()
{
	if (RecvBuff != nullptr)
	{
		delete RecvBuff;
		RecvBuff = nullptr;
	}

	if (MessageQueue != nullptr)
	{
		delete MessageQueue;
		MessageQueue = nullptr;
	}
}


void TCPClientController::StartConnect(const FString& ip, int32 port)
{
	try
	{
		client.BeginConnect(ip, port, NEW_AYNC_CALLBACK(ConnectCallback), NEW_AYNC_CALLBACK(DisconnectCallback), nullptr);
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("error : %s"), e.what());
	}
}

int32 TCPClientController::SetReceiveBufferSize(int32 size)
{
	if (RecvBuff != nullptr)
	{
		delete RecvBuff;
		RecvBuff = nullptr;
	}
	RecvBuff = new RecvBuffer(size);
	return client.SetReceiveBufferSize(2 * size);
}

int32 TCPClientController::SetSendBufferSize(int32 size)
{
	return client.SetSendBufferSize(2 * size);
}

void TCPClientController::CheckMessage()
{
	if (IsConnected() && !MessageQueue->IsQueueEmpty())
	{
		TArray<FByteArrayRef> messages = MessageQueue->PopAll();
		for (auto& message : messages)
		{
			Session->RecvMessageCallback(message);
		}
	}
}

void TCPClientController::StartSend(FByteArrayRef& Message)
{
	try
	{
		client.BeginSend(Message, NEW_AYNC_CALLBACK(SendCallback), Message);
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("error : %s"), e.what());
	}
}

void TCPClientController::StartRecv()
{
	RecvBuff->Clean();
	try
	{
		client.BeginRecv(RecvBuff->WritePos(), RecvBuff->FreeSize(), NEW_AYNC_CALLBACK(RecvCallback), nullptr);
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Warning, TEXT("error : %s"), e.what());
	}
}

void TCPClientController::Disconnect(const FString& cause, bool shutdownNoramlly)
{
	client.Disconnect(cause, shutdownNoramlly);
}


void TCPClientController::ConnectCallback(FAsyncResultRef result)
{
	if (!result->Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect Failed"));
		return;
	}

	Session->ConnectedCallback(result->Success);
	StartRecv();
}

void TCPClientController::SendCallback(FAsyncResultRef result)
{
	FByteArrayRef& sendMessage = std::any_cast<FByteArrayRef&>(result->State);
	if (sendMessage != nullptr)
	{
		Session->SendMessageCallback(sendMessage);
	}
}

void TCPClientController::RecvCallback(FAsyncResultRef result)
{
	int32 bytesTransferred = result->BytesTransferred;

	if (bytesTransferred == 0)//todo : ErrorEnum
	{
		Disconnect("by Server", false);
		return;
	}

	if (bytesTransferred > 0)
	{
		if (false == RecvBuff->OnWrite(bytesTransferred)) //Wirte Byte 앞으로 이동
		{
			Disconnect("OnWrite Overflow", false);
			return;
		}

		int32 recvSize = RecvBuff->DataSize();
		uint8* buffer = RecvBuff->ReadPos();

		int32 processLen = 0;
		while (true)
		{
			int32 dataSize = recvSize - processLen;

			int32 sizeOfMessage = 0;
			if (!IsOneMessage(&buffer[processLen], dataSize, OUT sizeOfMessage))
			{
				break;
			}
			PutMessage(&buffer[processLen], sizeOfMessage);

			processLen += sizeOfMessage;
		}

		if (processLen < 0 || recvSize < processLen || false == RecvBuff->OnRead(processLen))
		{
			Disconnect("OnRead Overflow", false);
			return;
		}
		StartRecv();
	}
}

bool TCPClientController::IsOneMessage(uint8* buffer, int32 dataSize, OUT int32& sizeOfMessage)
{
	if (dataSize < sizeof(PacketHeader))
	{
		return false;
	}

	PacketHeader header = *(reinterpret_cast<PacketHeader*>(buffer));
	sizeOfMessage = header.Size;
	if (dataSize < header.Size)
	{
		return false;
	}
	return true;
}

void TCPClientController::PutMessage(uint8* buffer, int32 sizeOfPacket)
{
	//패킷조립
	FByteArrayRef buffArrPtr = MakeShared<TArray<uint8>, ESPMode::ThreadSafe>(TArray<uint8>());
	buffArrPtr.Get()->AddUninitialized(sizeOfPacket);
	FMemory::Memcpy(buffArrPtr.Get()->GetData(), buffer, sizeOfPacket);

	MessageQueue->Push(buffArrPtr);
}



void TCPClientController::DisconnectCallback(FAsyncResultRef result)
{
	bool shutdownNormally = std::any_cast<bool>(result->State);
	Session->DisconnectedCallback(shutdownNormally);
}

