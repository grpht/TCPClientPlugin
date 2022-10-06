// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TCPCLIENTPLUGIN_API TCPBufferReader
{
public:
	TCPBufferReader();
	TCPBufferReader(uint8* buffer, uint32 size, uint32 pos = 0);
	~TCPBufferReader();

	uint8*			Buffer() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			ReadSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len);

	int64			ReadInt64();
	int32			ReadInt32();
	int16			ReadInt16();
	int8			ReadInt8();
	bool			ReadBoolean();
	
	FString			ReadStringUTF8(int length);

	template<typename T>
	TCPBufferReader& operator>>(OUT T& dest);
	
private:
	uint8*			_buffer = nullptr;
	uint32			_size = 0;
	uint32			_pos = 0;
};

template<typename T>
inline TCPBufferReader& TCPBufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_pos]);
	_pos += sizeof(T);
	return *this;
}