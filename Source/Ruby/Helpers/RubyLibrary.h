// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <typeinfo>
#include "Ruby/UnrealMainClasses/Ruby.h"
#include "RubyLibrary.generated.h"

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

//Delegates shared between all classes 
DECLARE_MULTICAST_DELEGATE(FMulticastDelegate);
DECLARE_DELEGATE_OneParam(FBoolDelegate, bool);
DECLARE_DELEGATE_OneParam(FByteDelegate, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FMulticastBoolDelegate, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FMulticastByteDelegate, uint8);
/**
 * This class contains usefull functions to allow easy access to unreal objects  
 */
UCLASS()
class RUBY_API URubyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = Gameplay, meta = (WorldContext = WorldContextObject))
		static int CurrentPlayersCount(UObject * WorldContextObject);
	UFUNCTION(BlueprintPure, Category = Gameplay, meta = (WorldContext = WorldContextObject))
		static class ARubyGameMode* GetGameMode(UObject * WorldContextObject);
	/// <summary>
	/// Get package map to get net id for pointers
	/// </summary>
	UFUNCTION(BlueprintPure, Category = Gameplay)
		static UPackageMap* GetPackageMap(APlayerController * PlayerController);
	UFUNCTION(BlueprintPure, Category = Gameplay)
		static FString GetBuildVersion(UObject * WorldContextObject);
	/// <summary>
	/// Function to call a lamdba with a delay. 
	/// Manage 0 delay instead of TimerManager base solution
	/// Example : URubyLibrary::DelayedFunction(delay, GetWorld(), [this] { FunctionName(); });
	/// </summary>
	template <typename F>
	static FORCEINLINE void DelayedFunction(float Delay, UObject * WorldContextObject, F&& Lambda)
	{
		FTimerHandle Handle;
		if (Delay == 0)
		{
			Lambda();
		}
		else
		{
			FTimerDelegate TimerCallback;
			UWorld * World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
			TimerCallback.BindLambda(Lambda);
			World->GetTimerManager().SetTimer(Handle, TimerCallback, Delay, false);
		}
	}
	template <typename F>
	static void LoopFunction(float Delay, UObject * WorldContextObject, F&& Lambda, float LoopDuration, FTimerHandle& TimerHandle)
	{
		FTimerDelegate TimerCallback;
		UWorld * World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		TimerCallback.BindLambda(Lambda);
		World->GetTimerManager().SetTimer(TimerHandle, TimerCallback, Delay, true);
		//Clear the timer handle at the end of the loop duration to stop it
		URubyLibrary::DelayedFunction(
			LoopDuration,
			World,
			[&TimerHandle, World]
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		});
	}
	 template <class T>
	 static T GetEnumFromString(FString EnumStr)
	 {		 
		 FString ClassName = FString(ANSI_TO_TCHAR(typeid(T).name()));
		 //Enum class name contains "enum " text
		 ClassName = ClassName.Replace(TEXT("enum "), TEXT(""));
		 const UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *ClassName, true);
		 int32 Index = Enum->GetIndexByName(FName(*EnumStr));
		 return T(static_cast<uint8>(Index));
	 }
	 FORCEINLINE static FString GetBuildingPath(TCHAR* BuildingName)
	 {
		 return BUILDING_PATH.Replace(TEXT("*"), BuildingName);
	 }
	 FORCEINLINE static FString GetTeamMaterialPath(const TCHAR* TeamId)
	 {
		 return TEAM_MATERIAL_PATH.Replace(TEXT("*"), TeamId);
	 }

	 FORCEINLINE static void Print(const FString& LogString) { UE_LOG(LogTemp, Log, TEXT("%s"), *LogString) };
	 FORCEINLINE static void Print(const float LogFloat) { Print(FString::SanitizeFloat(LogFloat)); };
};
