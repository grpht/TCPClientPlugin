// Fill out your copyright notice in the Description page of Project Settings.

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
    void WriteBoolean(bool value);
    void WriteInt8(int8 value);
    void WriteInt16(int16 value);
    void WriteInt32(int32 value);
    void WriteInt64(int64 value);
};