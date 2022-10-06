// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPRecvPacketBase.generated.h"

class ITCPRecvPacket
{
public:
	virtual int16 GetId() const = 0;
	virtual void AssemblePacket(int size, uint8* message) = 0;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class TCPCLIENTPLUGIN_API UTCPRecvPacketBase : public UObject, public ITCPRecvPacket
{
	GENERATED_BODY()
public:
	virtual int16 GetId() const override
	{
		checkf(false, TEXT("You must implement GetId method in child of TCPRecvPacket"));
		return 0;
	};

	virtual void AssemblePacket(int size, uint8* message) override
	{
		checkf(false, TEXT("You must implement FromByteArray method in child of TCPRecvPacket"));
	}
};
