// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPRecvPacketBase.h"

void UTCPRecvPacketBase::Deserialize(TCPBufferReader& reader)
{
	BufferReader = &reader;
	DeserializeBP();
}

UTCPRecvPacketBase* UTCPRecvPacketBase::CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet)
{
	UTCPRecvPacketBase* newObject = NewObject<UTCPRecvPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
