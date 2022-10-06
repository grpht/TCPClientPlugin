// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TCPClientCommon.h"
#include "TCPSendPacketBase.h"
#include "TCPSessionBase.generated.h"

DECLARE_DELEGATE_TwoParams(FOnConnectedDelegate, const FString&, bool);
DECLARE_DELEGATE_TwoParams(FOnDisconnectedDelegate, const FString&, bool);


#define REGISTER_(FuncName) [this](uint16 id, int32 size ){ FuncName(id, size);}
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class TCPCLIENTPLUGIN_API UTCPSessionBase : public UObject
{
	GENERATED_BODY()
public:
	friend class TCPClientController;
	friend class UTCPClientSubsystem;
public:
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	bool IsConnected();

	
	void Send(ITCPSendPacket& sendPacket);

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	void Send(UTCPSendPacketBase* sendPacket);

	void SetController(TCPClientController* controller) { Controller = controller; }
	TCPClientController* GetController() { return Controller; }

	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	FString& GetName() { return SessionName; }
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	FString& GetIp() { return Ip; }
	UFUNCTION(BlueprintCallable, Category = "TCPSession")
	int32 GetPort() { return Port; }


protected:
	virtual void OnStart() {};

	virtual void OnRecv(int16 Id, int32 size, uint8* message)
	{ checkf(false, TEXT("You must implement OnRecv method in child of UTCPSessionBase")); }

	virtual void OnSend(int16 Id, int32 size, uint8* message) 
	{ checkf(false, TEXT("You must implement OnSend method in child of UTCPSessionBase")); }

	virtual void OnDestroy() {};

	virtual void ConnectedCallback(bool success);
	virtual void DisconnectedCallback(bool normalShutdown);
	virtual void RecvMessageCallback(FByteArrayRef& MessageByte);
	virtual void SendMessageCallback(FByteArrayRef& MessageByte);
protected:
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString SessionName;
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	FString Ip;
	UPROPERTY(EditAnyWhere, Category = "Session Settings")
	int32 Port;
private:
	TCPClientController* Controller{ nullptr };
	FOnConnectedDelegate OnConnected;
	FOnDisconnectedDelegate OnDisconnected;
	
};
