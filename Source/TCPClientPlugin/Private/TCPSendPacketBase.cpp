// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSendPacketBase.h"

UTCPSendPacketBase* UTCPSendPacketBase::CreateSendPacketBP(TSubclassOf<UTCPSendPacketBase> packet)
{
	UTCPSendPacketBase* newObject = NewObject<UTCPSendPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
