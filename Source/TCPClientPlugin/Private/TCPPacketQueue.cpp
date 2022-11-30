// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPPacketQueue.h"

TCPPacketQueue::TCPPacketQueue()
{
}

TCPPacketQueue::~TCPPacketQueue()
{
}

bool TCPPacketQueue::IsQueueEmpty()
{
    return Queue.IsEmpty();
}

void TCPPacketQueue::Push(const FByteArrayRef& BufferPtr)
{
    std::lock_guard<std::mutex> lockGuard(lock);
    Queue.Enqueue(BufferPtr);
}

FByteArrayRef TCPPacketQueue::Pop()
{
    std::lock_guard<std::mutex> lockGuard(lock);
    TSharedPtr<TArray<uint8>> item;
    if (Queue.Peek(item))
    {
        Queue.Pop();
    }

    return item;
}

TArray<FByteArrayRef> TCPPacketQueue::PopAll()
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
