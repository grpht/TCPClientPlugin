// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Class.h"
#include "TCPBufferReader.h"


#include "TCPRecvPacketBase.generated.h"

class ITCPRecvPacket
{
public:
	virtual int32 GetPacketId() const = 0;
	virtual void ConvertFromBytes(TCPBufferReader& reader) = 0;
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
	virtual void ConvertFromBytes(TCPBufferReader& reader) override;

	UFUNCTION(BlueprintPure, Category = "TCPRecvPacket",
		meta = (DisplayName = "CreateRecvPacket"))
	static UTCPRecvPacketBase* CreateRecvPacketBP(TSubclassOf<UTCPRecvPacketBase> packet);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPRecvPacket", meta = (DisplayName = "Deserialize"))
	void ConvertFromBytesBP();

	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	uint8 ReadByte() { return BufferReader->ReadByte(); }
	
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket")
	TArray<uint8> ReadByteArray(int32 length) { return BufferReader->ReadBytes(length); }

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
	
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket",
		meta = (Tooltip = "It uses the Length-Prefixed method for reading strings.\n It first reads a 2-byte length of message, and then reads a string of that length."))
	FString ReadStringUTF8() { return BufferReader->ReadStringUTF8(); }
	
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket",
		meta = (Tooltip = "It uses the Length-Prefixed method that specifies the length for reading strings."))
	FString ReadStringUTF8Length(int32 length) { return BufferReader->ReadStringUTF8(length); }
	
	UFUNCTION(BlueprintCallable, Category = "TCPRecvPacket",
		meta=(Tooltip="It uses the Null-Terminate method for reading strings."))
	FString ReadStringUTF8NT() { return BufferReader->ReadStringUTF8NT(); }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "TCPRecvPacket")
	int32 PacketId = -1;
	TCPBufferReader* BufferReader = nullptr;
};
