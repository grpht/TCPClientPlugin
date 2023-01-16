// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPClientCommon.h"
#include "TCPSendPacketBase.h"
#include "TCPRecvPacketBase.h"
#include "TCPSessionBase.generated.h"

class UTCPRecvPacketBase;
class TCPClientController;

DECLARE_DELEGATE_TwoParams(FOnConnectedDelegate, const FString&, bool);
DECLARE_DELEGATE_TwoParams(FOnDisconnectedDelegate, const FString&, bool);

/**
 * 
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

	void SendPacket(ITCPSendPacket& sendPacket);

	UFUNCTION(BlueprintCallable, Category = "TCPSession", meta = (DisplayName = "Send Packet"))
	void SendPacketBP(UTCPSendPacketBase* sendPacket);

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const FString& GetName() const { return SessionName; }
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const FString& GetIp() const { return Ip; }
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	const int32 GetPort() const { return Port; }
protected:
	virtual void OnStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnStart"))
	void OnStartBP();

	virtual void OnDestroy();
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnDestroy"))
	void OnDestroyBP();

	virtual void OnRecv(int32 id, TCPBufferReader& reader);
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnRecvPacket"))
	void OnRecvBP(UTCPRecvPacketBase* packet);

	virtual void OnSend(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray);
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnSendPacket"))
	void OnSendBP(int32 id, int32 contentsByteSize, const TArray<uint8>& fullByteArray);

	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnConnectedSession"))
	void OnConnectedBP(bool success);
	UFUNCTION(BlueprintImplementableEvent, Category = "TCPSession", meta = (DisplayName = "OnDisconnectedSession"))
	void OnDisconnectedBP(bool normalShutdown);
	
	virtual void ConnectedCallback(bool success);
	virtual void DisconnectedCallback(bool normalShutdown);
	virtual void RecvMessageCallback(FByteArrayRef& MessageByte);
	virtual void SendMessageCallback(FByteArrayRef& MessageByte);

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	void RegisterRecvPacket(TSubclassOf<UTCPRecvPacketBase> recvPacket);
protected:
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString SessionName;
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString Ip;
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	int32 Port;
private:
	void SetController(TCPClientController* controller);
	TCPClientController* GetController(); 
private:
	TCPClientController* Controller{ nullptr };
	FOnConnectedDelegate OnConnected;
	FOnDisconnectedDelegate OnDisconnected;
	TMap<int32, UClass*> RecvPacketMap;
};
