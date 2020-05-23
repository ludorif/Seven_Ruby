// Copyright (C) Ludovic Riffiod 2020. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "ActionRequest.h"
#include "ActionRequestData.generated.h"

/// <summary>
/// This structure will contains request data 
/// Macro allows to pre-compile struct with all types we want 
/// The macro will also manage the data serialization
/// </summary>
USTRUCT()
struct FActionRequestData
{
	GENERATED_BODY();
public:
	/// <summary>
	/// int value corresponding to the ComparisonIndex of the request FName 
	/// </summary>
	uint32 RequestType;
	
	///Default constructor
	FActionRequestData(): RequestType(0){}
	
	/// <summary>
	/// Special constructor used by macros
	/// </summary>
	/// <param name="NewRequestType">FName created in macro declaration</param>
	explicit FActionRequestData(const FName NewRequestType);

	/// <summary>
	/// Convert any structure into bytes array
	/// </summary>
	/// <param name="Requestor">The actor sending the request</param>
	/// <returns>FActionRequest with serialized data</returns>
	const FActionRequest SerializeRequestData(AActor* Requestor);
	
	/// <summary>
	/// Serialize / deserialize specific structure with ref, special implementation is in macro
	/// </summary>
	/// <param name="Ar">Archive to keep data (bytes array)</param>
	/// <param name="Requestor">The actor sending the request</param>
	virtual void SaveData(FArchive& Ar, AActor* Requestor) {};

	/// <param name="Requestor">The actor sending the request</param>
	/// <returns>Map containing net id</returns>
	UPackageMap* GetPackageMap(AActor* Requestor)
	{
		return Requestor->GetNetConnection()->PackageMap;
	}

	/// <summary>
	/// Serialize specific structure with pointer, special implementation is in macro
	/// </summary>
	/// <param name="Ar">Archive to keep data (bytes array)</param>
	/// <param name="PackageMap">Map containing net id</param>
	/// <param name="Pointer">Pointer to UObject</param>
	template<class T>
	static void SavePointer(FArchive& Ar, UPackageMap* PackageMap, T Pointer) 
	{
		FNetworkGUID NetGuid = PackageMap->GetNetGUIDFromObject(Pointer);
		Ar << NetGuid;		
	};

	/// <summary>
	/// Deserialize specific structure with pointer, special implementation is in macro
	/// </summary>
	/// <param name="Ar">Archive to keep data (bytes array)</param>
	/// <param name="PackageMap">Map containing net id</param>
	/// <returns>Pointer to UObject</returns>
	template<class T>
	static T LoadPointer(FArchive& Ar, UPackageMap* PackageMap)
	{
		FNetworkGUID NetGuid;
		Ar << NetGuid;
		
		if(NetGuid.IsValid()) 
		{
			return static_cast<T>(PackageMap->GetObjectFromNetGUID(NetGuid, true));
		}

		//NetGuid is invalid if pointer was null or pointed to a not replicated actor
		return nullptr;
	};
};

/// <summary>
/// static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));
/// 
/// Create a new global FName used to recognize type of request easily
/// To prevent problems with Unreal FName we use value greater than MAX_NETWORKED_HARDCODED_NAME
/// </summary>

/// <summary>
/// Each macro below define a constructor and a way to serialize / deserialize its data depending of its type arguments
/// </summary>

#pragma region REQUEST_ACTION_Pointer
#define DECLARE_REQUEST_ACTION_POINTER_ONE_PARAM(Name, TypeVar1, NameVar1) \
static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));\
USTRUCT()\
struct F##Name : public FActionRequestData { \
public :\
UPROPERTY()\
TypeVar1 NameVar1; \
UFUNCTION()\
F##Name(TypeVar1 Var1) : FActionRequestData(NAME_##Name)\
{ \
NameVar1 = Var1;\
} \
F##Name(const FActionRequest& ActionRequest, AActor* Requestor)\
{\
	FMemoryReader FromBinary = FMemoryReader(ActionRequest.SerializedRequestData);\
    NameVar1 = LoadPointer<TypeVar1>(FromBinary, GetPackageMap(Requestor));\
}\
void SaveData(FArchive& Ar, AActor* Requestor) override\
{\
    SavePointer<TypeVar1>(Ar, GetPackageMap(Requestor), NameVar1);\
}\
};
#pragma endregion 

#pragma region REQUEST_ACTION_No_Pointer

#define DECLARE_REQUEST_ACTION(Name) \
static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));\
USTRUCT()\
struct F##Name : public FActionRequestData { \
public :\
UFUNCTION()\
F##Name(): FActionRequestData(NAME_##Name){};\
};

#define DECLARE_REQUEST_ACTION_ONE_PARAM(Name, TypeVar1, NameVar1) \
static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));\
USTRUCT()\
struct F##Name : public FActionRequestData { \
public :\
UPROPERTY()\
TypeVar1 NameVar1; \
UFUNCTION()\
F##Name(TypeVar1 Var1) : FActionRequestData(NAME_##Name) \
{ \
NameVar1 = Var1;\
} \
F##Name(const FActionRequest& ActionRequest, AActor* Requestor)\
{\
	FMemoryReader FromBinary = FMemoryReader(ActionRequest.SerializedRequestData);\
	SaveData(FromBinary, Requestor);\
}\
void SaveData(FArchive& Ar, AActor* Requestor) override\
{\
	Ar << NameVar1;\
}\
};

#define DECLARE_REQUEST_ACTION_TWO_PARAMS(Name, TypeVar1, NameVar1, TypeVar2, NameVar2) \
static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));\
USTRUCT()\
struct F##Name : public FActionRequestData { \
public :\
UPROPERTY()\
TypeVar1 NameVar1; \
UPROPERTY()\
TypeVar2 NameVar2; \
UFUNCTION()\
F##Name(TypeVar1 Var1, TypeVar2 Var2) : FActionRequestData(NAME_##Name) \
{ \
NameVar1 = Var1;\
NameVar2 = Var2;\
} \
F##Name(const FActionRequest& ActionRequest, AActor* Requestor)\
{\
	FMemoryReader FromBinary = FMemoryReader(ActionRequest.SerializedRequestData);\
	SaveData(FromBinary, Requestor);\
}\
void SaveData(FArchive& Ar, AActor* Requestor) override\
{\
	Ar << NameVar1;\
	Ar << NameVar2;\
}\
};

#define DECLARE_REQUEST_ACTION_THREE_PARAMS(Name, TypeVar1, NameVar1, TypeVar2, NameVar2, TypeVar3, NameVar3 ) \
static FName NAME_##Name(EName(MAX_NETWORKED_HARDCODED_NAME + __COUNTER__ + 1), TEXT(#Name));\
USTRUCT()\
struct F##Name : public FActionRequestData { \
public :\
UPROPERTY()\
TypeVar1 NameVar1; \
UPROPERTY()\
TypeVar2 NameVar2; \
UPROPERTY()\
TypeVar3 NameVar3; \
UFUNCTION()\
F##Name(TypeVar1 Var1, TypeVar2 Var2, TypeVar3 Var3) : FActionRequestData(NAME_##Name) \
{ \
NameVar1 = Var1;\
NameVar2 = Var2;\
NameVar3 = Var3;\
} \
F##Name(const FActionRequest& ActionRequest, AActor* Requestor)\
{\
	FMemoryReader FromBinary = FMemoryReader(ActionRequest.SerializedRequestData);\
	SaveData(FromBinary, Requestor);\
}\
void SaveData(FArchive& Ar, AActor* Requestor) override\
{\
	Ar << NameVar1;\
	Ar << NameVar2;\
	Ar << NameVar3;\
}\
};
#pragma endregion 
