// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <any>

typedef TSharedPtr<TArray<uint8>> FByteArrayRef;

struct TCPAsyncResult : TSharedFromThis<TCPAsyncResult, ESPMode::ThreadSafe>
{
	TCPAsyncResult(bool success, int32 bytesTransferred, std::any state = 0)
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

