// Copyright 2022. Elogen Co. All Rights Reserved.

#include "TCPClientSubsystem.h"
#include "TCPClientController.h"
#include "IPAddressAsyncResolve.h"
#include "SocketSubsystem.h"
#include "Async/Async.h"
#include "Kismet/GameplayStatics.h"

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

UTCPSessionBase* UTCPClientSubsystem::ConnectSession(TSubclassOf<UTCPSessionBase> session)
{
    if (session == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TCPClientSubSystem[StartSession Fail] : Input session is null.."));
    }

    UTCPSessionBase* newSession = NewObject<UTCPSessionBase>(this, session, TEXT("TCPSession"));
    DisconnectSessionByName(newSession->GetName());
    Sessions.Add(newSession->GetName(), newSession);

    TCPClientController* controller = new TCPClientController();
    controller->SetSession(newSession);
    newSession->SetController(controller);
    newSession->OnConnected.BindUFunction(this, FName("ConnectedCallback"));
    newSession->OnDisconnected.BindUFunction(this, FName("DisConnectedCallback"));
    newSession->OnStart();
    
    if (newSession->DNS)
    {
        GetDomainIpAddress(newSession->GetIp(),
            [this, newSession](FString ip, bool success)
            {
                if (success)
                    newSession->GetController()->StartConnect(ip, newSession->GetPort());
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to resolve the DNS address"))
                    ConnectedCallback(newSession->GetName(), false);
                }
            }
        );
    }
    else
        newSession->GetController()->StartConnect(newSession->GetIp(), newSession->GetPort());
   
    return newSession;
}

UTCPSessionBase* UTCPClientSubsystem::ConnectSession(TSubclassOf<UTCPSessionBase> session, const FConnectedSessionDelegate& connectDelegate, const FDisconnectedSessionDelegate& disconnectDelegate)
{
    if (session == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TCPClientSubSystem[StartSession Fail] : Input session is null.."));
    }

    const FString& sessionName = session.GetDefaultObject()->GetName();
    if (connectDelegate.IsBound())
    {
        OnConnected.Add(connectDelegate);
        RegisteredConnectDelegates.Add(sessionName, &connectDelegate);
    }
    if (disconnectDelegate.IsBound())
    {
        OnDisconnected.Add(disconnectDelegate);
        RegisteredDisconnectDelegates.Add(sessionName, &disconnectDelegate);
    }

    return ConnectSession(session);
}

void UTCPClientSubsystem::DisconnectSession(UTCPSessionBase* session)
{
    if (session != nullptr)
    {
        const FString& sessionName = session->GetName();
        DisconnectSessionByName(sessionName);
    }
}

void UTCPClientSubsystem::DisconnectSessionByName(const FString& sessionName)
{
    if (Sessions.Contains(sessionName))
    {
        UTCPSessionBase* Session = Sessions[sessionName];
        TCPClientController* controller = Session->GetController();
        
        if (Session->IsConnected())
        {
            controller->Disconnect(FString("Shutdown Manually"), true);
        }
        
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
    if (!Sessions.Contains(sessionName))
        return;

    if (OnConnected.IsBound())
    {
        OnConnected.Broadcast(sessionName, success, Sessions[sessionName]);
        if (RegisteredConnectDelegates.Find(sessionName))
        {
            auto& registedDelegate = RegisteredConnectDelegates[sessionName];
            OnConnected.Remove(*registedDelegate);
            RegisteredConnectDelegates.Remove(sessionName);
        }
    }
}

void UTCPClientSubsystem::DisConnectedCallback(const FString& sessionName, bool normalShutdown)
{
    if (!Sessions.Contains(sessionName))
        return;

    auto session = Sessions[sessionName];
    
    if (OnDisconnected.IsBound())
    {
        OnDisconnected.Broadcast(sessionName, normalShutdown);
        if (RegisteredDisconnectDelegates.Find(sessionName))
        {
            auto& registedDelegate = RegisteredDisconnectDelegates[sessionName];
            OnDisconnected.Remove(*registedDelegate);
            RegisteredDisconnectDelegates.Remove(sessionName);
        }
    }
}

void UTCPClientSubsystem::GetDomainIpAddress(const FString& URL, TFunction<void(FString, bool)> OnComplete)
{
    /* code by -Kmack-
    * https://forums.unrealengine.com/t/how-to-get-host-by-name/296044/8
    */
    float timeOut = 10.0f;
    Async(EAsyncExecution::ThreadPool,
        [this, URL, timeOut, OnComplete]()
        {
            ISocketSubsystem* const SocketSubSystem = ISocketSubsystem::Get();
            auto ResolveInfo = SocketSubSystem->GetHostByName(TCHAR_TO_ANSI(*URL));
            if (ensure(SocketSubSystem))
            {
                float elapsedTime = 0;
                while (!ResolveInfo->IsComplete() && elapsedTime < timeOut)
                {
                    FPlatformProcess::Sleep(0.05);
                    elapsedTime += 0.05f;
                }

                if (ResolveInfo->IsComplete() && ResolveInfo->GetErrorCode() == 0)
                {
                    const FInternetAddr* Addr = &ResolveInfo->GetResolvedAddress();
                    uint32 OutIP = 0;
                    Addr->GetIp(OutIP);

                    UE_LOG(LogTemp, Warning, TEXT("Found IP address for URL <%s>: %d.%d.%d.%d")
                        , *URL, 0xff & (OutIP >> 24), 0xff & (OutIP >> 16), 0xff & (OutIP >> 8), 0xff & OutIP);

                    FString IpString = Addr->ToString(false);
                    AsyncTask(ENamedThreads::GameThread,
                        [IpString, OnComplete]()
                        {

                            OnComplete(IpString, true);
                        }
                    );
                }
                else
                {
                    AsyncTask(ENamedThreads::GameThread,
                        [OnComplete]()
                        {
                            OnComplete(FString(), false);
                        }
                    );
                }
            }
        }
    );
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
        break;
        
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