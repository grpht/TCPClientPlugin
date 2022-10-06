// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class RecvBuffer
{
    enum { BUFFER_COUNT = 2 };

public:
    RecvBuffer(int32 bufferSize);
    ~RecvBuffer();

    void			Clean();
    bool			OnRead(int32 numOfBytes);
    bool			OnWrite(int32 numOfBytes);

    uint8* ReadPos() { return &_buffer[_readCursor]; }
    uint8* WritePos() { return &_buffer[_writeCursor]; }
    int32			DataSize() { return _writeCursor - _readCursor; }
    int32			FreeSize() { return _capacity - _writeCursor; }

private:
    int32				_capacity = 0;
    int32				_bufferSize = 0;
    int32				_readCursor = 0;
    int32				_writeCursor = 0;
    TArray<uint8>		_buffer;
};

