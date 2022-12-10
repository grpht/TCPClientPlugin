// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TCPClientCommon.h"
#include "Containers/Queue.h"
#include <mutex>
/**
 *
 */
class TCPPacketQueue
{
public:
    TCPPacketQueue();
    ~TCPPacketQueue();

    bool                    IsQueueEmpty();
    void                    Push(const FByteArrayRef& BufferPtr);
    FByteArrayRef           Pop();

    TArray<FByteArrayRef>   PopAll();

private:
    TQueue<FByteArrayRef>   Queue;
    std::mutex              lock;
};
