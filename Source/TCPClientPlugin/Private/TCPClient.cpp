// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClient.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "UObject/UnrealNames.h"
#include "ThreadPool.h"

TCPClient::TCPClient()
{
    ThrdPool = new ThreadPool(10);

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

void TCPClient::BeginConnect(const FString host, int32 port, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state)
{
    FIPv4Address ip;
    bool isValid = FIPv4Address::Parse(host, ip);
    FIPv4Endpoint endpoint(ip, port);

    if (!isValid)
    {
        throw std::runtime_error("Address is Not Valid.");
    }
    BeginConnect(endpoint, connectCallback, disconnectCallback, state);
}

void TCPClient::BeginConnect(const FIPv4Endpoint endpoint, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state)
{

#pragma region VALIDATE_REGION
    if (bConnected.load())
    {
        throw std::logic_error("This TCP Client Already Connected");
    }
    if (bClosed)
    {
        throw std::logic_error("This TCP Client was Closed. Please Create New TCP Client");
    }
    if (connectCallback == nullptr || disconnectCallback == nullptr)
    {
        throw std::runtime_error("Callback is null");
    }
    if (SocketSubsystem == nullptr)
    {
        throw std::runtime_error("SocketSubsytem is null");
    }
#pragma endregion

    DisconnectCallback = disconnectCallback;

    TSharedPtr<FInternetAddr> RemoteAddress = SocketSubsystem->CreateInternetAddr();
    FIPv4Address ip;

    RemoteAddress->SetIp(endpoint.Address.Value);
    RemoteAddress->SetPort(endpoint.Port);

    ThrdPool->EnqueueJob([=]()
        {
            if (Socket == nullptr) return;
            bool connected = Socket->Connect(*RemoteAddress);
            bConnected.store(connected);
            auto Ret = MakeShared<TCPAsyncResult, ESPMode::ThreadSafe>(connected, 0, state);
            connectCallback(Ret);
        }
    );
}


void TCPClient::BeginSend(FByteArrayRef& sendBuffPtr, std::function<void(FAsyncResultRef)> callback, std::any state)
{
#pragma region VALIDATE_REGION
    if (!bConnected)
    {
        throw std::logic_error("Not Connected but try to send Message");
    }

    if (callback == nullptr)
    {
        throw std::runtime_error("BeginSend callback is null");
    }
#pragma endregion
    SendingQueue.Enqueue(sendBuffPtr);
    if (bSending.exchange(true) == true)
    {
        return;
    }

    ThrdPool->EnqueueJob([=]()
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
}

void TCPClient::BeginRecv(uint8* buffer, int32 bufferSize, std::function<void(FAsyncResultRef)> callback, std::any state)
{
#pragma region VALIDATE_REGION
    if (!bConnected)
    {
        throw std::logic_error("Not Connected but try to receive message");
    }

    if (callback == nullptr)
    {
        throw std::runtime_error("BeginRecv callback is null");
    }

    if (bReceiving.exchange(true) == true)
    {
        throw std::logic_error("You had call the BeginRecv before receiving message is finished");
    }
#pragma endregion

    ThrdPool->EnqueueJob([=]()
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
        throw std::invalid_argument("ReceiveBuffer size must be greater than 0");
    }
    int32 RecvBufferSize = 0;
    Socket->SetReceiveBufferSize(size, OUT RecvBufferSize);
    return RecvBufferSize;
}

int32 TCPClient::SetSendBufferSize(int32 size)
{
    if (size < 0)
    {
        throw std::invalid_argument("SendBuffer size must be greater than 0");
    }
    int32 SendBufferSize = 0;
    Socket->SetSendBufferSize(size, OUT SendBufferSize);
    return SendBufferSize;
}

