// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "Tickable.h"
#include "TCPSessionBase.h"

#include "TCPClientSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FConnectedSessionMulticastDelegate,const FString&, SessionName, bool, Success, UTCPSessionBase*, ReturnSession);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDisconnectedSessionMulticastDelegate,const FString&, SessionName, bool, NormalShutdown);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FConnectedSessionDelegate, const FString&, SessionName, bool, Success, UTCPSessionBase*, ReturnSession);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDisconnectedSessionDelegate, const FString&, SessionName, bool, NormalShutdown);
/**
 * 
 */
UCLASS()
class TCPCLIENTPLUGIN_API UTCPClientSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UTCPSessionBase* ConnectSession(TSubclassOf<UTCPSessionBase> session);

	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	UTCPSessionBase* ConnectSession(TSubclassOf<UTCPSessionBase> session, const FConnectedSessionDelegate& connectDelegate, const FDisconnectedSessionDelegate& disconnectDelegate);
	
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void DisconnectSession(UTCPSessionBase* session);

	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void DisconnectSessionByName(const FString& sessionName);
	
	UFUNCTION(BlueprintPure, Category = "TCPClientSubsystem")
	UTCPSessionBase* GetSession(const FString& sessionName);
	
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void GetSessionLazy(const FString& sessionName, const FConnectedSessionDelegate& connectDelegate);
	/**
	* FTickableGameObject override methods
	*/
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableInEditor() const override { return false; }
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsAllowedToTick() const override final;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

private:
	void DeleteController(class TCPClientController* controller);
	UFUNCTION()
	void ConnectedCallback(const FString& sessionName, bool success);
	UFUNCTION()
	void DisConnectedCallback(const FString& sessionName, bool normalShutdown);

public:
	UPROPERTY(BlueprintAssignable, Category = "TCPSession")
	FConnectedSessionMulticastDelegate OnConnected;

	UPROPERTY(BlueprintAssignable, Category = "TCPSession")
	FDisconnectedSessionMulticastDelegate OnDisconnected;

private:
	UPROPERTY()
	TMap<FString, UTCPSessionBase*> Sessions;
	UPROPERTY()
	TArray<UClass*> Handlers;
};