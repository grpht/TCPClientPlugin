// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPBufferReader.h"
#include "TCPRecvPacketBase.generated.h"

class ITCPRecvPacket
{
public:
	virtual int32 GetPacketId() const = 0;
	virtual void Deserialize(TCPBufferReader& reader) = 0;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class TCPCLIENTPLUGIN_API UTCPRecvPacketBase : public UObject, public ITCPRecvPacket
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "TCPRecvPacket")
	virtual int32 GetPacketId() const override
	{
		return PacketId;
	};

	virtual void Deserialize(TCPBufferReader& reader) override
	{
		BufferReader = &reader;
		DeserializeBP();
	}

	UFUNCTION(BlueprintPure, Category = "TCPRecvPacket", meta = (DisplayName = "CreateSendPacket"))
	static UTCPRecvPacketBase* CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPRecvPacket", meta = (DisplayName = "Deserialize"))
	void DeserializeBP();

	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	bool ReadBoolean() { return BufferReader->ReadBoolean(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	int32 ReadInt8() { return BufferReader->ReadInt8(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	int32 ReadInt16() { return BufferReader->ReadInt16(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	int32 ReadInt32() { return BufferReader->ReadInt32(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	int64 ReadInt64() { return BufferReader->ReadInt64(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	float ReadFloat() { return BufferReader->ReadSingle(); }
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	FString ReadStringUTF8() { return BufferReader->ReadStringUTF8(); }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TCPRecvPacket")
	int32 PacketId { -1 };
	TCPBufferReader* BufferReader;
};
