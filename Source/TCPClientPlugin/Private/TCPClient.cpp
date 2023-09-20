// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPClient.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UObject/UnrealNames.h"
#include "TCPThreadPool.h"
#include "TCPClientError.h"

TCPClient::TCPClient()
    : Socket(nullptr)
    , SocketSubsystem(nullptr)
    , bClosed(false)
    , bConnected(false)
    , bSending(false)
    , bReceiving(false)
    , ThrdPool(nullptr)
{
    ThrdPool = new TCPThreadPool(5);

    SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TCPClientSocket"), false);
}

TCPClient::~TCPClient()
{
    if (bConnected)
    {
        Disconnect("by TCPClient Destructor");
    }

    if (ThrdPool != nullptr)
    {

        delete ThrdPool;
        ThrdPool = nullptr;
    }

    if (Socket != nullptr)
    {
        SocketSubsystem->DestroySocket(Socket);
        Socket = nullptr;
    }
}

int32 TCPClient::BeginConnect(const FString host, int32 port, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state)
{
    FIPv4Address ip;
    bool isValid = FIPv4Address::Parse(host, ip);
    FIPv4Endpoint endpoint(ip, port);

    if (!isValid)
    {
        return Connect_AddressisNotValid;
    }
    BeginConnect(endpoint, connectCallback, disconnectCallback, state);
    return 0;
}

int32 TCPClient::BeginConnect(const FIPv4Endpoint endpoint, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state)
{
#pragma region VALIDATE_REGION
    if (bConnected.load())
    {
        return TCPClientError::Connect_AlreadyConnected;
    }
    if (bClosed)
    {
        return TCPClientError::Connect_Closed;
    }
    if (connectCallback == nullptr || disconnectCallback == nullptr)
    {
        return TCPClientError::Connect_CallbackIsNull;
    }
    if (SocketSubsystem == nullptr)
    {
        return TCPClientError::Connect_SockeSubsystemIsNull;
    }
#pragma endregion

    DisconnectCallback = disconnectCallback;

    TSharedPtr<FInternetAddr> RemoteAddress = SocketSubsystem->CreateInternetAddr();
    FIPv4Address ip;

    RemoteAddress->SetIp(endpoint.Address.Value);
    RemoteAddress->SetPort(endpoint.Port);

    ThrdPool->EnqueueJob([=, this]()
        {
            if (Socket == nullptr) return;
            bool connected = Socket->Connect(*RemoteAddress);
            bConnected.store(connected);
            auto Ret = MakeShared<TCPAsyncResult, ESPMode::ThreadSafe>(connected, 0, state);
            connectCallback(Ret);
        }
    );

    return 0;
}


int32 TCPClient::BeginSend(FByteArrayRef& sendBuffPtr, std::function<void(FAsyncResultRef)> callback, std::any state)
{
#pragma region VALIDATE_REGION
    if (!bConnected)
    {
        return Send_NotConnected;
    }

    if (callback == nullptr)
    {
        return Send_CallbackIsNull;
    }
#pragma endregion
    SendingQueue.Enqueue(sendBuffPtr);
    if (bSending.exchange(true) == true)
    {
        return 0;
    }

    ThrdPool->EnqueueJob([=, this]()
        {
            while (!SendingQueue.IsEmpty() && bConnected)
            {
                FByteArrayRef SendingMessage;
                SendingQueue.Dequeue(SendingMessage);
                TArray<uint8>* buffptr = SendingMessage.Get();
                int size = buffptr->Num();

                int32 sent = 0;
                int32 processLen = 0;
                bool sendSuccess = true;
                while (processLen < size)
                {
                    if (Socket == nullptr) return;
                    sendSuccess = Socket->Send(&buffptr->GetData()[processLen], size - processLen, sent);
                    if (sendSuccess == false)
                    {
                        Disconnect("Send Failed", false);
                        break;
                    }
                    processLen += sent;
                }

                auto Ret = MakeShared<TCPAsyncResult, ESPMode::ThreadSafe>(sendSuccess, processLen, state);
                callback(Ret);
            }
            bSending.store(false);
        }
    );
    return 0;
}

int32 TCPClient::BeginRecv(uint8* buffer, int32 bufferSize, std::function<void(FAsyncResultRef)> callback, std::any state)
{
#pragma region VALIDATE_REGION
    if (!bConnected)
    {
        return Recv_NotConnected;
    }

    if (callback == nullptr)
    {
        return Recv_CallbackIsNull;
    }

    if (bReceiving.exchange(true) == true)
    {
        return Recv_ProhibitDoubleReceiving;
    }
#pragma endregion

    ThrdPool->EnqueueJob([=, this]()
        {
            int32 byteRead = 0;
            if (Socket == nullptr) return;
            bool recvSuccess = Socket->Recv(buffer, bufferSize, byteRead);

            bReceiving.store(false);
            if (!bConnected)
            {
                return;
            }
            if (!recvSuccess)
            {
                // If socket Send Buffer is overFlowed, 
                // it cause that Recv() is awaken and return false
                // solution: Enlarge Send Buffer Size
                Disconnect("Recv Failed", false);
                return;
            }

            auto Ret = MakeShared<TCPAsyncResult, ESPMode::ThreadSafe >(recvSuccess, byteRead, state);
            callback(Ret);
        }
    );
    return 0;
}

void TCPClient::Disconnect(const FString& cause, bool shutdownNormally)
{
    if (bConnected.exchange(false) == false)
        return;
    bClosed = true;
    UE_LOG(LogTemp, Warning, TEXT("TCPClient Disconnected : %s"), *cause);
    Socket->Shutdown(ESocketShutdownMode::ReadWrite);
    Socket->Close();

    if (DisconnectCallback != nullptr)
    {
        auto Ret = MakeShared<TCPAsyncResult, ESPMode::ThreadSafe >(false, 0, shutdownNormally);
        DisconnectCallback(Ret);
    }
}

int32 TCPClient::SetReceiveBufferSize(int32 size)
{
    if (size < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ReceiveBuffer size must be greater than 0"));
        return -1;
    }
    int32 RecvBufferSize = 0;
    Socket->SetReceiveBufferSize(size, OUT RecvBufferSize);
    return RecvBufferSize;
}

int32 TCPClient::SetSendBufferSize(int32 size)
{
    if (size < 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SendBuffer size must be greater than 0"));
    }
    int32 SendBufferSize = 0;
    Socket->SetSendBufferSize(size, OUT SendBufferSize);
    return SendBufferSize;
}

