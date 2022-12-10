// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPClientController.h"

#include "TCPClient.h"
#include "TCPPacketQueue.h"
#include "TCPRecvBuffer.h"
#include "TCPSessionBase.h"
#include "TCPClientError.h"

#define NEW_AYNC_CALLBACK(FuncName) [this](FAsyncResultRef res){ if(this != nullptr) FuncName(res);}

TCPClientController::TCPClientController()
{
	SetReceiveBufferSize(4096);
	SetSendBufferSize(4096);
	//Session = new ITCPServerSession();
	//Session->SetController(this);
	MessageQueue = new TCPPacketQueue();
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
	int error = Client.BeginConnect(ip, port, NEW_AYNC_CALLBACK(ConnectCallback), NEW_AYNC_CALLBACK(DisconnectCallback), nullptr);
	PrintErrorMessage(error);
}

int32 TCPClientController::SetReceiveBufferSize(int32 size)
{
	if (RecvBuff != nullptr)
	{
		delete RecvBuff;
		RecvBuff = nullptr;
	}
	RecvBuff = new TCPRecvBuffer(size);
	return Client.SetReceiveBufferSize(2 * size);
}

int32 TCPClientController::SetSendBufferSize(int32 size)
{
	return Client.SetSendBufferSize(2 * size);
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
	int error = Client.BeginSend(Message, NEW_AYNC_CALLBACK(SendCallback), Message);
	PrintErrorMessage(error);
}

void TCPClientController::StartRecv()
{
	RecvBuff->Clean();
	int error = Client.BeginRecv(RecvBuff->WritePos(), RecvBuff->FreeSize(), NEW_AYNC_CALLBACK(RecvCallback), nullptr);
	PrintErrorMessage(error);
}

void TCPClientController::Disconnect(const FString& cause, bool shutdownNoramlly)
{
	Client.Disconnect(cause, shutdownNoramlly);
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
	if (dataSize < sizeof(TCPPacketHeader))
	{
		return false;
	}

	TCPPacketHeader header = *(reinterpret_cast<TCPPacketHeader*>(buffer));
	sizeOfMessage = header.Size;
	if (dataSize < header.Size)
	{
		return false;
	}
	return true;
}

void TCPClientController::PutMessage(uint8* buffer, int32 sizeOfPacket)
{
	//Assemble Packet
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

void TCPClientController::PrintErrorMessage(int error)
{
	switch (error)
	{
	case None:
		break;
	case Connect_AddressisNotValid:
		UE_LOG(LogTemp, Error, TEXT("Address is Not Valid."));
		break;
	case Connect_Closed:
		UE_LOG(LogTemp, Error, TEXT("This TCP Client was Closed. Please Create New TCP Client"));
		break;
	case Connect_AlreadyConnected:
		UE_LOG(LogTemp, Error, TEXT("This Session Already Connected"));
		break;
	case Connect_CallbackIsNull:
		UE_LOG(LogTemp, Error, TEXT("Callback is null"));
		break;
	case Connect_SockeSubsystemIsNull:
		UE_LOG(LogTemp, Error, TEXT("SocketSubsytem is null"));
		break;
	case Send_NotConnected:
		UE_LOG(LogTemp, Error, TEXT("Not Connected but try to send Message"));
		break;
	case Send_CallbackIsNull:
		UE_LOG(LogTemp, Error, TEXT("BeginSend callback is null"));
		break;
	case Recv_NotConnected:
		UE_LOG(LogTemp, Error, TEXT("Not Connected but try to receive message"));
		break;
	case Recv_CallbackIsNull:
		UE_LOG(LogTemp, Error, TEXT("BeginRecv callback is null"));
		break;
	case Recv_ProhibitDoubleReceiving:
		UE_LOG(LogTemp, Error, TEXT("You had call the BeginRecv before receiving message is finished"));
		break;
	default:
		break;
	}
}