// Copyright (C) Ludovic Riffiod 2020. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ActionRequest.generated.h"

/// <summary>
/// This structure is used to contains serialized request data 
/// It will be send through light RPC client->server
/// </summary>
USTRUCT()
struct FActionRequest { 
	GENERATED_BODY()
public :
	/// <summary>
	/// int value corresponding to the ComparisonIndex of the request FName 
	/// </summary>
	UPROPERTY()
		uint32 RequestType;
	
	/// <summary>
	/// Request data serialized (could be anything)
	/// </summary>
	UPROPERTY()
	TArray<uint8> SerializedRequestData;
	
	///Default constructor
	FActionRequest(): RequestType(0)
	{
	}
	
	/// <summary>
	/// Special constructor 
	/// </summary>
	/// <param name="NewRequestType">unsigned int value of the corresponding enum</param>
	explicit FActionRequest(const uint32 NewRequestType)
	{
		RequestType = NewRequestType;
	}

	FORCEINLINE const bool IsSameType(const FName OtherRequestType) const { return RequestType == GetTypeHash(OtherRequestType); }
};

