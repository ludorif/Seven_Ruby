// Fill out your copyright notice in the Description page of Project Settings.
#include "RubyLibrary.h"
#include "Misc/AssertionMacros.h"
#include "GameFramework/PlayerController.h"
#include "Ruby/UnrealMainClasses/RubyGameMode.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "GameFramework/GameStateBase.h"

int URubyLibrary::CurrentPlayersCount(UObject * WorldContextObject)
{  
	UWorld * World = WorldContextObject->GetWorld();
	AGameStateBase * GameState = World->GetGameState();
	if (ensureMsgf(GameState != nullptr, TEXT("Game state is null")))
	{
		return GameState->PlayerArray.Num();
	}
	return 0;
}
ARubyGameMode* URubyLibrary::GetGameMode(UObject * WorldContextObject)
{
	UWorld * World = WorldContextObject->GetWorld();
	AGameModeBase * GameMode = UGameplayStatics::GetGameMode(World);
	if (ensureMsgf(GameMode != nullptr, TEXT("Game instance is null")))
	{
		return Cast<ARubyGameMode>(GameMode);
	}
	return nullptr;
}
UPackageMap* URubyLibrary::GetPackageMap(APlayerController * PlayerController)
{
	if (PlayerController->GetNetConnection() == nullptr)
		return nullptr;
	return PlayerController->GetNetConnection()->PackageMap;
}
FString URubyLibrary::GetBuildVersion(UObject * WorldContextObject)
{
	const FString BuildVersionPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + "BuildVersion";
	FString BuildVersion = "-1";
	if (FPaths::FileExists(BuildVersionPath))
	{
		FFileHelper::LoadFileToString(BuildVersion, *BuildVersionPath);
	}
	return BuildVersion;
}

