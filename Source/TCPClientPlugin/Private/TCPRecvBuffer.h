// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class TCPRecvBuffer
{
    enum { BUFFER_COUNT = 2 };

public:
    TCPRecvBuffer(int32 bufferSize);
    ~TCPRecvBuffer();

    void			Clean();
    bool			OnRead(int32 numOfBytes);
    bool			OnWrite(int32 numOfBytes);

    uint8*          ReadPos() { return &Buffer[ReadCursor]; }
    uint8*          WritePos() { return &Buffer[WriteCursor]; }
    int32			DataSize() { return WriteCursor - ReadCursor; }
    int32			FreeSize() { return Capacity - WriteCursor; }

private:
    int32				Capacity = 0;
    int32				BufferSize = 0;
    int32				ReadCursor = 0;
    int32				WriteCursor = 0;
    TArray<uint8>		Buffer;
};

