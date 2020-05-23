// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RubyUserWidget.generated.h"
/**
 * 
 */

class ARubyPlayerController;
UCLASS()
class RUBY_API URubyUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		class UButton* SpawnButton;
	UFUNCTION(BlueprintNativeEvent)
		void Init(TSubclassOf<AActor> NewClassToSpawn, const FVector& NewSpawnLocation, ARubyPlayerController* NewRubyPlayerController, const uint8 NewPrice);
private:
	TSubclassOf<AActor> ClassToSpawn;
	FVector SpawnLocation;
	uint8 Price;
	class ARubyPlayerController* RubyPlayerController;
	UFUNCTION()
		void Spawn();
};
