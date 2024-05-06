// Copyright 2022. Elogen Co. All Rights Reserved.

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
	TCPBufferReader(const TArray<uint8>& bufferArray, uint32 pos = 0);
	~TCPBufferReader();

	void			Init(uint8* buffer, uint32 size, uint32 pos = 0);
	void			Init(const TArray<uint8>& bufferArray, uint32 pos = 0);
	uint8*			Get() { return _buffer; }
	uint32			Size() { return _size; }
	uint32			ReadSize() { return _pos; }
	uint32			FreeSize() { return _size - _pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len);

	uint8           ReadByte();
	TArray<uint8>   ReadBytes(size_t length);

	bool			ReadBoolean();
	int8			ReadInt8();
	int16			ReadInt16();
	int32			ReadInt32();
	int64			ReadInt64();
	uint8           ReadUInt8();
	uint16			ReadUint16();
	uint32			ReadUint32();
	uint64			ReadUint64();
	
	float			ReadHalf();
	float			ReadSingle();
	double			ReadDouble();
	
	/**
	* @brief It uses the Length-Prefixed method for reading strings.
	* It first reads a 2-byte number, and then reads a string of that length.
	*/
	FString			ReadStringUTF8();
	/**
	* @brief It uses the Length-Prefixed method that specifies the length for reading strings.
	*/
	FString			ReadStringUTF8(int32 length);
	/**
	* @brief It uses the Null-Terminate method for reading strings.
	* Reads a string with a null character at the end.
	*/
	FString			ReadStringUTF8NT();

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