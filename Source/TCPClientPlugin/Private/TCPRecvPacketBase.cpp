// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPRecvPacketBase.h"

UTCPRecvPacketBase* UTCPRecvPacketBase::CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet)
{
	UTCPRecvPacketBase* newObject = NewObject<UTCPRecvPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
