// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPBufferWriter.h"

#include "TCPSendPacketBase.generated.h"


class ITCPSendPacket
{
public:
	
	virtual int32 GetPacketId() const = 0;
	virtual void ConvertToBytes(TCPBufferWriter& writer) = 0;
};
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class TCPCLIENTPLUGIN_API UTCPSendPacketBase : public UObject, public ITCPSendPacket
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "TCPSendPacket")
	virtual int32 GetPacketId() const override
	{
		return PacketId;
	};
	virtual void ConvertToBytes(TCPBufferWriter& writer) override;

	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket", meta = (DisplayName = "CreateSendPacket"))
	static UTCPSendPacketBase* CreateSendPacketBP(TSubclassOf<UTCPSendPacketBase> packet);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSendPacket", meta = (DisplayName = "Serialize"))
	void ConvertToBytesBP();

	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteBoolean(bool value) { BufferWriter->WriteBoolean(value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WiretInt8(int32 value) { BufferWriter->WriteInt8((int8)value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteInt16(int32 value) { BufferWriter->WriteInt16((int16)value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteInt32(int32 value) { BufferWriter->WriteInt32(value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteInt64(int64 value) { BufferWriter->WriteInt64(value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteFloat(float value) { BufferWriter->WriteSingle(value); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteStringUTF8(const FString& message) { BufferWriter->WriteStringUTF8(message); }
	UFUNCTION(BlueprintCallable, Category = "TCPSendPacket")
	void WriteByteArray(UPARAM(ref) TArray<uint8>& byteArray) { BufferWriter->WriteByteArray(byteArray); }
protected:
	UPROPERTY(EditDefaultsOnly, Category = "TCPSendPacket")
	int32 PacketId = -1;
	TCPBufferWriter* BufferWriter = nullptr;
};
