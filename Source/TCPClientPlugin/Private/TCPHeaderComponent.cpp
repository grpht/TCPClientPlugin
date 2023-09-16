// Copyright 2022. Elogen Co. All Rights Reserved.


#include "TCPHeaderComponent.h"
#include "TCPPacketHeader.h"

int32 UTCPHeaderComponent::GetHeaderSize() const
{
	return sizeof(TCPPacketHeader);
}

int32 UTCPHeaderComponent::ReadTotalSize(uint8* buffer) const
{	
	const TCPPacketHeader header = *(reinterpret_cast<TCPPacketHeader*>(buffer));
	return header.Size;
}

int32 UTCPHeaderComponent::ReadProtocol(uint8* buffer) const
{
	const TCPPacketHeader header = *(reinterpret_cast<TCPPacketHeader*>(buffer));
	return header.Id;
}

void UTCPHeaderComponent::WriteHeader(TCPBufferWriter& writer, int32 protocolId) const
{
	int totalPacketSize = writer.Num();

	int skipByte = 0;
	FMemory::Memcpy(&writer.GetData()[skipByte], &totalPacketSize, sizeof(TCPPacketHeader::Size));
	skipByte += sizeof(TCPPacketHeader::Size);
	FMemory::Memcpy(&writer.GetData()[skipByte], &protocolId, sizeof(TCPPacketHeader::Id));
}

