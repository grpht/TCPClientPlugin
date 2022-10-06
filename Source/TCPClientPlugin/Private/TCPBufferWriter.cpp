// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPBufferWriter.h"



int32 TCPBufferWriter::Reserve(uint32 len)
{
    return AddUninitialized(len);
}

void* TCPBufferWriter::Memcpy(int32 index, const void* src, uint32 len)
{
    return FMemory::Memcpy(&GetData()[index], src, len);
}

void TCPBufferWriter::Write(void* src, uint32 len)
{
    int32 index = AddUninitialized(len);
    FMemory::Memcpy(&GetData()[index], src, len);
}

void TCPBufferWriter::WriteBoolean(bool value)
{
    Write(&value, sizeof(bool));
}

void TCPBufferWriter::WriteInt8(int8 value)
{
    Write(&value, sizeof(int8));
}

void TCPBufferWriter::WriteInt16(int16 value)
{
    Write(&value, sizeof(int16));
}
void TCPBufferWriter::WriteInt32(int32 value)
{
    Write(&value, sizeof(int32));
}
void TCPBufferWriter::WriteInt64(int64 value)
{
    Write(&value, sizeof(int64));
}