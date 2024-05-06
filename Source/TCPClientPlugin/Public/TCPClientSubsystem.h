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
 * @brief It is a globally accessible class for managing TCP connections.
 */
UCLASS()
class TCPCLIENTPLUGIN_API UTCPClientSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	/**
	* @brief Attempt to connect to the server.
	* @param class of session
	* @return instance of session
	*/
	UTCPSessionBase* ConnectSession(TSubclassOf<UTCPSessionBase> session);

	/**
	* @brief Attempt to connect to the server. using in blueprint
	* @param class of session
	* @param connect delegate
	* @param disconnect delegate
	* @return instance of session
	*/
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	UTCPSessionBase* ConnectSession(TSubclassOf<UTCPSessionBase> session, const FConnectedSessionDelegate& connectDelegate, const FDisconnectedSessionDelegate& disconnectDelegate);
	
	/**
	* @brief Terminate the connection with the server connected with the session
	* @param instance of session
	*/
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void DisconnectSession(UTCPSessionBase* session);

	/**
	* @brief Terminate the connection with the server connected with the session
	* @param session name
	*/
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void DisconnectSessionByName(const FString& sessionName);
	
	/**
	* @brief Get the session instance by name
	* @param session name
	* @return instance of session
	*/
	UFUNCTION(BlueprintPure, Category = "TCPClientSubsystem")
	UTCPSessionBase* GetSession(const FString& sessionName);

	/**
	* @brief Perform the function to fetch a delayed session.
	* This is used when the connection status of the session is uncertain
	* @param session name
	* @param connect delegate
	*/
	UFUNCTION(BlueprintCallable, Category = "TCPClientSubsystem")
	void GetSessionLazy(const FString& sessionName, const FConnectedSessionDelegate& connectDelegate);

#pragma region FTickableGameObject override methods
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableInEditor() const override { return false; }
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsAllowedToTick() const override final;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
#pragma endregion

private:
	void DeleteController(class TCPClientController* controller);
	UFUNCTION()
	void ConnectedCallback(const FString& sessionName, bool success);
	UFUNCTION()
	void DisConnectedCallback(const FString& sessionName, bool normalShutdown);

	void GetDomainIpAddress(const FString& URL, TFunction<void(FString, bool)> OnComplete);
public:
	/**
	* @brief It receives all connection event messages. You must filter by Session name.
	*/
	UPROPERTY(BlueprintAssignable, Category = "TCPSession")
	FConnectedSessionMulticastDelegate OnConnected;
	/**
	* @brief It receives all connection event messages. You must filter by Session name.
	*/
	UPROPERTY(BlueprintAssignable, Category = "TCPSession")
	FDisconnectedSessionMulticastDelegate OnDisconnected;

private:
	UPROPERTY()
	TMap<FString, UTCPSessionBase*> Sessions;
	UPROPERTY()
	TArray<UClass*> Handlers;

	TMap<FString, const FConnectedSessionDelegate*> RegisteredConnectDelegates;
	TMap<FString, const FDisconnectedSessionDelegate*> RegisteredDisconnectDelegates;
};