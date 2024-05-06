// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/BufferArchive.h"
/**
 * 
 */
class TCPCLIENTPLUGIN_API TCPBufferWriter : public FBufferArchive
{
public:
    int32 Reserve(uint32 len);
    void* Memcpy(int32 index, const void* src, uint32 len);

    void Write(void* src, uint32 len);

    void WriteByte(uint8 value);
    void WriteBytes(TArray<uint8>& byteArray);
    void WriteByteArray(TArray<uint8>& byteArray);

    void WriteBoolean(bool value);
    void WriteInt8(int8 value);
    void WriteInt16(int16 value);
    void WriteInt32(int32 value);
    void WriteInt64(int64 value);
    void WriteUInt8(uint8 value);
    void WriteUInt16(uint16 value);
    void WriteUInt32(uint32 value);
    void WriteUInt64(uint64 value);

    void WriteSingle(float value);
    void WriteDouble(double value);

    /**
    * @brief It uses the Length-Prefixed method for Write strings.
    * Serialize the 2-byte number representing the byte length of the string, followed by the string itself.
    */
    void WriteStringUTF8(const FString& message);
    /**
    * @brief It uses the Length-Prefixed method for Write strings.
    * Serialize the string as is.
    */
    void WriteStringUTF8Raw(const FString& message);
    /**
    * @brief It uses the Null-Terminated method for Write strings.
    * Append a null character at the end of the string.
    */
    void WriteStringUTF8NT(const FString& message);
    
};