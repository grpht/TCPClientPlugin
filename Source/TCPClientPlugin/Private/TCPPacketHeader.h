// Copyright 2022. Elogen Co. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//change member as you want, But don't use virtual 
#pragma pack(push, 1)
struct TCPPacketHeader
{
	int32 Size; //*Don't Rename, but allow changing byte size 
	//int16 Any; //add or remove any header
	int16 Id;
};
#pragma pack(pop)