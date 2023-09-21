// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPBufferWriter.h"

#include "TCPHeaderComponent.generated.h"


/**
 * Please, make your own PacketHeader and HeaderComponent in order to match with your server.
 * override the functions by inheriting this class,
 * then register that to the session.
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class TCPCLIENTPLUGIN_API UTCPHeaderComponent : public UObject
{
	GENERATED_BODY()
public:
	virtual int32 GetHeaderSize() const;
	virtual int32 ReadTotalSize(uint8* buffer) const;
	virtual int32 ReadProtocol(uint8* buffer) const;
	virtual void WriteHeader(TCPBufferWriter& writer, int32 protocolId) const;
};
