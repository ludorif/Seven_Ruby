// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RubyGameMode.generated.h"

class ARubyCharacter;

UCLASS(minimalapi)
class ARubyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void  PostLogin(APlayerController* NewPlayer) override;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ACameraActor> CameraClass;
	UFUNCTION(BlueprintImplementableEvent)
		TSubclassOf<ARubyCharacter> GetClassToSpawn(int32 LocalId);
};

