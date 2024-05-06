// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPRecvPacketBase.h"
#include "Templates/SubclassOf.h"

void UTCPRecvPacketBase::ConvertFromBytes(TCPBufferReader& reader)
{
	BufferReader = &reader;
	ConvertFromBytesBP();
}

UTCPRecvPacketBase* UTCPRecvPacketBase::CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet)
{
	UTCPRecvPacketBase* newObject = NewObject<UTCPRecvPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
