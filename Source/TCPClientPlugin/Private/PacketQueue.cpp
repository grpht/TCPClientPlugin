// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketQueue.h"

PacketQueue::PacketQueue()
{
}

PacketQueue::~PacketQueue()
{
}

bool PacketQueue::IsQueueEmpty()
{
    return Queue.IsEmpty();
}

void PacketQueue::Push(const FByteArrayRef& BufferPtr)
{
    std::lock_guard<std::mutex> lockGuard(lock);
    Queue.Enqueue(BufferPtr);
}

FByteArrayRef PacketQueue::Pop()
{
    std::lock_guard<std::mutex> lockGuard(lock);
    TSharedPtr<TArray<uint8>> item;
    if (Queue.Peek(item))
    {
        Queue.Pop();
    }

    return item;
}

TArray<FByteArrayRef> PacketQueue::PopAll()
{
    TArray<FByteArrayRef> retList;

    std::lock_guard<std::mutex> lockGuard(lock);

    while (!Queue.IsEmpty())
    {
        FByteArrayRef item;
        if (Queue.Peek(item))
        {
            retList.Add(item);
            Queue.Pop();
        }
    }

    return retList;
}
