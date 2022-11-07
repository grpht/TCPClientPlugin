// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPBufferReader.h"

TCPBufferReader::TCPBufferReader()
{
}

TCPBufferReader::TCPBufferReader(uint8* buffer, uint32 size, uint32 pos)
    : _buffer(buffer), _size(size), _pos(pos)
{
}

TCPBufferReader::TCPBufferReader(const TArray<uint8>& bufferArray, uint32 pos)
{
    Init(bufferArray, pos);
}

TCPBufferReader::~TCPBufferReader()
{
}

void TCPBufferReader::Init(uint8* buffer, uint32 size, uint32 pos)
{
    _buffer = buffer;
    _size = size;
    _pos = pos;
}

void TCPBufferReader::Init(const TArray<uint8>& bufferArray, uint32 pos)
{
    TArray<uint8>& arr = const_cast<TArray<uint8>&>(bufferArray);
    Init(arr.GetData(), arr.Num(), pos);
}

bool TCPBufferReader::Peek(void* dest, uint32 len)
{
    if (FreeSize() < len)
        return false;

    ::memcpy(dest, &_buffer[_pos], len);
    return true;
}

bool TCPBufferReader::Read(void* dest, uint32 len)
{
    if (Peek(dest, len) == false)
        return false;

    _pos += len;
    return true;
}

int64 TCPBufferReader::ReadInt64()
{
    int64 ret;
    Read(&ret, sizeof(ret));
    return ret;
}

int32 TCPBufferReader::ReadInt32()
{
    int32 ret;
    Read(&ret, sizeof(ret));
    return ret;
}

int16 TCPBufferReader::ReadInt16()
{
    int16 ret;
    Read(&ret, sizeof(ret));
    return ret;
}

int8 TCPBufferReader::ReadInt8()
{
    int8 ret;
    Read(&ret, sizeof(ret));
    return ret;
}

bool TCPBufferReader::ReadBoolean()
{
    bool ret;
    Read(&ret, sizeof(ret));
    return ret;
}

float TCPBufferReader::ReadHalf()
{
    float ret;
    Read(&ret, 2);
    return ret;
}

float TCPBufferReader::ReadSingle()
{
    float ret;
    Read(&ret, sizeof(ret));
    return ret;
}

double TCPBufferReader::ReadDouble()
{
    double ret;
    Read(&ret, sizeof(ret));
    return ret;
}

TArray<uint8> TCPBufferReader::ReadBytes(uint32 length)
{
    TArray<uint8> dest;
    dest.AddUninitialized(length);
    Read(dest.GetData(), length);
    return dest;
}

FString TCPBufferReader::ReadStringUTF8()
{
    int16 length = ReadInt16();
    TArray<uint8> dest;
    dest.AddUninitialized(length);
    Read(dest.GetData(), length);
    dest.Add(0);
    return UTF8_TO_TCHAR(dest.GetData());
}