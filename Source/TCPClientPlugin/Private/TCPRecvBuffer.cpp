// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPRecvBuffer.h"

TCPRecvBuffer::TCPRecvBuffer(int32 bufferSize) : BufferSize(bufferSize)
{
	Capacity = bufferSize * BUFFER_COUNT;
	Buffer.AddZeroed(Capacity);
}

TCPRecvBuffer::~TCPRecvBuffer()
{

}

void TCPRecvBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		ReadCursor = WriteCursor = 0;
	}
	else
	{
		if (FreeSize() < BufferSize)
		{
			::memcpy(&Buffer[0], &Buffer[ReadCursor], dataSize);
			ReadCursor = 0;
			WriteCursor = dataSize;
		}
	}
}

bool TCPRecvBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	ReadCursor += numOfBytes;
	return true;
}

bool TCPRecvBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	WriteCursor += numOfBytes;
	return true;
}