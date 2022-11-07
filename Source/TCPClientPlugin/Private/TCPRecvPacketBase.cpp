// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPRecvPacketBase.h"

UTCPRecvPacketBase* UTCPRecvPacketBase::CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet)
{
	UTCPRecvPacketBase* newObject = NewObject<UTCPRecvPacketBase>(GetTransientPackage(), packet);
	return newObject;
}
