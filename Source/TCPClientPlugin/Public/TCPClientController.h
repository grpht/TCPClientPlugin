// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TCPClient.h"

class UTCPSessionBase;
/**
 * 
 */
class TCPCLIENTPLUGIN_API TCPClientController
{
public:
    TCPClientController();
    ~TCPClientController();

	/**/
	void StartConnect(const FString& ip, int32 port);
	void CheckMessage();
	int32 SetReceiveBufferSize(int32 size);
	int32 SetSendBufferSize(int32 size);
	inline void SetSession(UTCPSessionBase* session) { Session = session; }
	inline UTCPSessionBase* GetSession() { return Session; }
	inline bool IsConnected() { return Client.IsConnected(); }
	void Disconnect(const FString& cause, bool shutdownNoramlly = true);

	void StartSend(FByteArrayRef& Message);
private:
	void StartRecv();
	void ConnectCallback(FAsyncResultRef result);
	void SendCallback(FAsyncResultRef result);
	void RecvCallback(FAsyncResultRef result);
	void DisconnectCallback(FAsyncResultRef result);

	bool IsOneMessage(uint8* buffer, int32 dataSize, OUT int32& sizeOfPacket);
	void PutMessage(uint8* buffer, int32 sizeOfMessage);
	void PrintErrorMessage(int error);
private:
	TCPClient Client;
	UTCPSessionBase* Session{ nullptr };

	class TCPRecvBuffer* RecvBuff { nullptr };
	class TCPPacketQueue* MessageQueue { nullptr };
};

