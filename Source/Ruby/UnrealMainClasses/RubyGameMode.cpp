// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "RubyGameMode.h"
#include "RubyPlayerController.h"
#include "Ruby/Interactables/Characters/RubyCharacter.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

void ARubyGameMode::PostLogin(APlayerController* NewPlayer)
{	
	ARubyPlayerController* PlayerController = Cast<ARubyPlayerController>(NewPlayer);
	if (PlayerController->TeamId == -1)
	{
		const int32 NewPlayerId = (GetGameState<AGameState>()->PlayerArray.Num() - 1);
		TArray<AActor*> Cameras;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraClass, Cameras);
		PlayerController->Server_Init(Cast<ACameraActor>(Cameras[NewPlayerId]), NewPlayerId, GetClassToSpawn(NewPlayerId));
	}
	Super::PostLogin(NewPlayer);
}

