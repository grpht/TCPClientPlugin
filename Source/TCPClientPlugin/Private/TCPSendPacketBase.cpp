// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPSendPacketBase.h"

void UTCPSendPacketBase::ConvertToBytes(TCPBufferWriter& writer)
{
	BufferWriter = &writer;
	ConvertToBytesBP();
}

UTCPSendPacketBase* UTCPSendPacketBase::CreateSendPacketBP(TSubclassOf<UTCPSendPacketBase> packet)
{
	UTCPSendPacketBase* newObject = NewObject<UTCPSendPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
