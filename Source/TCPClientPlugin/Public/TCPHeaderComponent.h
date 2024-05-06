// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPBufferWriter.h"

#include "TCPHeaderComponent.generated.h"


/**
 * @brief This is the default header provided.
 * To have a header that suits your server, inherit this class and customize it.
 * Next, set the header for the session
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class TCPCLIENTPLUGIN_API UTCPHeaderComponent : public UObject
{
	GENERATED_BODY()

/**
* This is example of custom header
* To interpret the header using reinterpret_cast,
* do not include virtual functions in the header struct.
* This create a virtual table and adds complexity.
* 
* #pragma pack(push, 1)
* struct CustomHeader
* {
*		int32 Size;
*		int32 crc;
*		int16 Id;
* };
* #pragma pack(pop)
*/
public:
	/**
	* @brief Specify the total size of the header.
	* @return Size of header
	*/
	virtual int32 GetHeaderSize() const;
	/**
	* @brief Define the rules for extracting the total size of message from the received packet header.
	* @param Byte array pointer of Packet
	* @return Size of message
	*/
	virtual int32 ReadTotalSize(uint8* buffer) const;
	/**
	* @brief Define the rules for extracting the protocol from the received packet.
	* @param Byte array pointer of Packet
	* @return Protocol Id
	*/
	virtual int32 ReadProtocol(uint8* buffer) const;
	/**
	* @brief Configure the rules for the packet header to be sent.
	* The size specified in GetHeaderSize() has already been allocated in front.
	* @param The Writer where the message is fully serialized into bytes by 'TCPSendPacket'
	* @param Protocol Id defined in TCPSendPacket
	*/
	virtual void WriteHeader(TCPBufferWriter& writer, int32 protocolId) const;
	/** 
	* @brief Please verify your packet here;
	* @param Byte array pointer of Packet
	* @return if false, the connection will be terminated automatically. */
	virtual bool CheckIntegrity(uint8* buffer) const;
};
