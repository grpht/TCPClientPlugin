// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "TCPClientCommon.h"
#include "TCPSendPacketBase.h"
#include "TCPRecvPacketBase.h"
#include "TCPHeaderComponent.h"

#include "TCPSessionBase.generated.h"

class UTCPRecvPacketBase;
class TCPClientController;

DECLARE_DELEGATE_TwoParams(FOnConnectedDelegate, const FString&, bool);
DECLARE_DELEGATE_TwoParams(FOnDisconnectedDelegate, const FString&, bool);

/**
 * @brief The Session used for communicating with the server.\n
	After requesting a connection through the TCPClientSubsystem, once the connection is established,
	you can send and receive messages with the server through this instance.
 */
UCLASS(Blueprintable, BlueprintType, notplaceable)
class TCPCLIENTPLUGIN_API UTCPSessionBase : public UObject
{
	GENERATED_BODY()
public:
	friend class TCPClientController;
	friend class UTCPClientSubsystem;
public:
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	bool IsConnected();

	/**
	* @brief Send message to the server
	* @param SendPacket 
	*/
	void SendPacket(ITCPSendPacket& sendPacket);

	UFUNCTION(BlueprintCallable, Category = "TCPSession", meta = (DisplayName = "Send Packet",
		ToolTip="Send message to the server"))
	void SendPacketBP(UTCPSendPacketBase* sendPacket);

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const FString& GetName() const { return SessionName; }

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const FString& GetIp() const { return Ip; }

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const int32 GetPort() const { return Port; }
	
	void SetHeader(TSubclassOf<UTCPHeaderComponent>& header) { CustomHeader = header; }
protected:
	UFUNCTION(BlueprintCallable, Category = "TCPSession",
		meta = (Tooltip = "Register the packets to receive from the server."))
	void RegisterRecvPacket(TSubclassOf<UTCPRecvPacketBase> recvPacket);

	/**
	* @brief It is called when you attempt to connect to the server through the subsystem
	*/
	virtual void OnStart();
	/**
	* @brief Callback when the session has reicieved message from the server
	*/
	virtual void OnRecv(int32 id, TCPBufferReader& reader);
	/**
	* @brief Callback when the session start to send the message the the server.
	*/
	virtual void OnSend(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray);
	/**
	* @brief It is called at the final stage when the session is disconnected from the server
	*/
	virtual void OnDestroy();

#pragma region Blueprint Implementable Function
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnStart",
			ToolTip="It is called when you attempt to connect to the server through the subsystem"))
	void OnStartBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnDestroy",
		Tooltip="It is called at the final stage when the session is disconnected from the server"))
	void OnDestroyBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnRecvPacket",
		Tooltip="Callback when the session has reicieved message from the server"))
	void OnRecvBP(UTCPRecvPacketBase* packet);

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnSendPacket",
		ToolTip = "Callback when the session start to send the message the the server."))
	void OnSendBP(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray);

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnConnectedSession",
		ToolTip = "Callback when the session connects to the server"))
	void OnConnectedBP(bool success);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnDisconnectedSession",
		ToolTip = "Callback when the session disconnects to the server"))
	void OnDisconnectedBP(bool normalShutdown);
#pragma endregion

#pragma region Callbacks by Controller
	/**
	* @brief Callback called by the controller when the session connects to the server
	*/
	virtual void ConnectedCallback(bool success);
	/**
	* @brief Callback called by the controller when the session disconnects to the server
	*/
	virtual void DisconnectedCallback(bool normalShutdown);
	/**
	* @brief Callback called by the controller when the session has reicieved message from the server
	*/
	virtual void RecvMessageCallback(FByteArrayRef& MessageByte);
	/**
	* @brief Callback called by the controller when the session start to send the message the the server.
	*/
	virtual void SendMessageCallback(FByteArrayRef& MessageByte);
#pragma endregion

private:
	void SetController(TCPClientController* controller);
	TCPClientController* GetController();

protected:
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString SessionName;
	
	UPROPERTY(EditAnyWhere, Category = "Session Settings",
		meta = (Tooltip = "If you check this, the address written in the IP will be treated as a DNS."))
	bool DNS = false;

	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString Ip;

	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	int32 Port;

	UPROPERTY(EditDefaultsOnly, Category = "Packet Header")
	TSubclassOf<UTCPHeaderComponent> CustomHeader;
	
	UPROPERTY(EditDefaultsOnly, Category="Register Receive Packets")
	TArray<TSubclassOf<UTCPRecvPacketBase>> PacketToReceive;

private:
	TCPClientController* Controller{ nullptr };
	
	FOnConnectedDelegate OnConnected;
	
	FOnDisconnectedDelegate OnDisconnected;
	
	UPROPERTY()
	TMap<int32, UClass*> RecvPacketMap;
	
	UPROPERTY()
	UTCPHeaderComponent* Header;
};
