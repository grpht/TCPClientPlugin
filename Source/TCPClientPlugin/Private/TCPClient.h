// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClientCommon.h"
#include <functional>
#include <memory>
#include <atomic>
#include <any>


#include "Interfaces/IPv4/IPv4Endpoint.h"
/**
 *
 */
class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    int BeginConnect(const FString host, int32 port, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state);
    int BeginConnect(const FIPv4Endpoint endpoint, std::function<void(FAsyncResultRef)> connectCallback, std::function<void(FAsyncResultRef)> disconnectCallback, std::any state);
    int BeginSend(FByteArrayRef& sendBuffPtr, std::function<void(FAsyncResultRef)> callback, std::any state);
    int BeginRecv(uint8* buffer, int32 bufferSize, std::function<void(FAsyncResultRef)> callback, std::any state);
    void Disconnect(const FString& cause, bool shutdownNormally = true);
    bool IsConnected() { return bConnected.load(); }

    int32 SetReceiveBufferSize(int32 size);
    int32 SetSendBufferSize(int32 size);

private:
    class FSocket* Socket{ nullptr };
    class ISocketSubsystem* SocketSubsystem{ nullptr };

    std::atomic<bool> bConnected{ false };
    std::atomic<bool> bSending{ false };
    std::atomic<bool> bReceiving{ false };
    bool bClosed{ false };
    TQueue<FByteArrayRef> SendingQueue;

    class ThreadPool* ThrdPool { nullptr };

    std::function<void(FAsyncResultRef)> DisconnectCallback;
};

