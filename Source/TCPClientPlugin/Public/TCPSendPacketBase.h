// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPSendPacketBase.generated.h"


class ITCPSendPacket
{
public:
	virtual int16 GetId() const = 0;
	virtual void Serialize(class TCPBufferWriter& writer) = 0;
};
/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class TCPCLIENTPLUGIN_API UTCPSendPacketBase : public UObject, public ITCPSendPacket
{
	GENERATED_BODY()
public:
	virtual int16 GetId() const override
	{
		checkf(false, TEXT("You must implement GetId method in child of TCPSendPacket"));
		return 0;
	};

	virtual void Serialize(class TCPBufferWriter& writer) override
	{
		checkf(false, TEXT("You must implement ToByteArray method in child of TCPSendPacket"));
	}
};
