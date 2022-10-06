// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClientSubsystem.h"
#include "TCPClientController.h"

void UTCPClientSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    
}

void UTCPClientSubsystem::Deinitialize()
{
    TMap<FString, UTCPSessionBase*> copyList = Sessions;
    for (auto& kvp : copyList)
    {
        DisconnectSessionByName(kvp.Key);
    }
}
UTCPSessionBase* UTCPClientSubsystem::ConnectSession(TSubclassOf<UTCPSessionBase> session, const FConnectedSessionDelegate& connectDelegate, const FDisconnectedSessionDelegate& disconnectDelegate)
{
    if (session == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TCPClientSubSystem[StartSession Fail] : Input session is null.."));
    }
    UTCPSessionBase* newSession = NewObject<UTCPSessionBase>(this, session, TEXT("TCPSession"));
    DisconnectSessionByName(newSession->GetName());

    if (connectDelegate.IsBound())
    {
        OnConnected.Add(connectDelegate);
    }
    if (disconnectDelegate.IsBound())
    {
        OnDisconnected.Add(disconnectDelegate);
    }
    newSession->OnStart();
    newSession->OnConnected.BindUFunction(this, FName("ConnectedCallback"));
    newSession->OnDisconnected.BindUFunction(this, FName("DisConnectedCallback"));

    TCPClientController* controller = new TCPClientController();
    controller->SetSession(newSession);
    newSession->SetController(controller);

    Sessions.Add(newSession->GetName(), newSession);

    controller->StartConnect(newSession->GetIp(), newSession->GetPort());
    return newSession;
}

void UTCPClientSubsystem::DisconnectSession(UTCPSessionBase* session)
{
    if (session != nullptr)
    {
        FString& sessionName = session->GetName();
        DisconnectSessionByName(sessionName);
    }
}

void UTCPClientSubsystem::DisconnectSessionByName(const FString& sessionName)
{
    if (Sessions.Contains(sessionName))
    {
        UTCPSessionBase* Session = Sessions[sessionName];
        TCPClientController* controller = Session->GetController();

        Sessions.Remove(sessionName);
        DeleteController(controller);
        Session->OnConnected.Unbind();
        Session->OnDisconnected.Unbind();
        Session->OnDestroy();
    }
}

UTCPSessionBase* UTCPClientSubsystem::GetSession(const FString& sessionName)
{
    if (Sessions.Contains(sessionName))
    {
        return Sessions[sessionName];
    }
    return nullptr;
}

void UTCPClientSubsystem::GetSessionLazy(const FString& sessionName, const FConnectedSessionDelegate& afterGetEvent)
{
    auto session = Sessions.Find(sessionName);
    if (session != nullptr && (*session)->IsConnected())
    {
        afterGetEvent.ExecuteIfBound(sessionName, (*session)->IsConnected(), (*session));
    }
    else
    {
        OnConnected.Add(afterGetEvent);
    }
}

void UTCPClientSubsystem::DeleteController(TCPClientController* controller)
{
    if (controller != nullptr)
    {
        delete controller;
        controller = nullptr;
    }
}

void UTCPClientSubsystem::ConnectedCallback(const FString& sessionName, bool success)
{
    if (OnConnected.IsBound())
    {
        OnConnected.Broadcast(sessionName, success, Sessions[sessionName]);
    }
}

void UTCPClientSubsystem::DisConnectedCallback(const FString& sessionName, bool normalShutdown)
{
    if (OnDisconnected.IsBound())
    {
        OnDisconnected.Broadcast(sessionName, normalShutdown);
    }
}

void UTCPClientSubsystem::Tick(float DeltaTime)
{
    for (auto& kvp : Sessions)
    {
        UTCPSessionBase* session = kvp.Value;
        TCPClientController* controller = session->GetController();
        if (controller == nullptr)
            continue;

        controller->CheckMessage();
    }
}

bool UTCPClientSubsystem::IsAllowedToTick() const
{
    return !IsTemplate();
}

ETickableTickType UTCPClientSubsystem::GetTickableTickType() const
{
    //{ return ETickableTickType::Always; }
    return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType();
}

TStatId UTCPClientSubsystem::GetStatId() const
{
    return UObject::GetStatID();
}

bool UTCPClientSubsystem::IsTickable() const
{
    if (IsTemplate())
        return false;
    
    return true;
}