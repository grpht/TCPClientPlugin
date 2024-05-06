// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TCPClientCommon.h"
#include "Containers/Queue.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

#include <functional>
#include <memory>
#include <atomic>

/**
 *
 */
class TCPClient
{
public:
    TCPClient();
    ~TCPClient();
    
    int32 BeginConnect(const FString host, int32 port, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state);
    int32 BeginConnect(const FIPv4Endpoint endpoint, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state);
    int32 BeginSend(FByteArrayRef& sendBuffPtr, std::function<void(FAsyncResultRef)> callback, std::any state);
    int32 BeginRecv(uint8* buffer, int32 bufferSize, std::function<void(FAsyncResultRef)> callback, std::any state);
    void Disconnect(const FString& cause, bool shutdownNormally = true);
    bool IsConnected() { return bConnected.load(); }

    int32 SetReceiveBufferSize(int32 size);
    int32 SetSendBufferSize(int32 size);

    FString GetDomainIpAddress(const FString& URL);
private:
    class FSocket* Socket;
    class ISocketSubsystem* SocketSubsystem;
    bool bClosed;

    std::atomic<bool> bConnected;
    std::atomic<bool> bSending;
    std::atomic<bool> bReceiving;
    TQueue<FByteArrayRef> SendingQueue;

    class TCPThreadPool* ThrdPool;

    std::function<void(FAsyncResultRef)> DisconnectCallback;
};

