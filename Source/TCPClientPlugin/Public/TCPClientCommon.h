#pragma once

#include "CoreMinimal.h"
#include <any>
typedef TSharedPtr<TArray<uint8>> FByteArrayRef;

struct TCPAsyncResult : TSharedFromThis<TCPAsyncResult, ESPMode::ThreadSafe>
{
	TCPAsyncResult(bool success, int32 bytesTransferred, std::any state)
		: Success(success)
		, BytesTransferred(bytesTransferred)
		, State(state)
	{}
	~TCPAsyncResult() = default;

	bool Success{ false };
	int32 BytesTransferred{ 0 };
	std::any State{ nullptr };
};

typedef TSharedRef<TCPAsyncResult, ESPMode::ThreadSafe> FAsyncResultRef;

//change member as you want, But don't use virtual 
struct PacketHeader
{
	int32 Size; //*Don't Rename, but allow changing byte size 
	int16 Any;
	int16 Id;
};